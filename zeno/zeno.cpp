#include <zeno/zeno.h>
#include <zeno/ConditionObject.h>
#include <zeno/Visualization.h>
#include <zeno/GlobalState.h>
#include <zeno/safe_at.h>
#include <cassert>

namespace zeno {

ZENAPI IObject::IObject() = default;
ZENAPI IObject::~IObject() = default;

ZENAPI std::shared_ptr<IObject> IObject::clone() const {
    return nullptr;
}

ZENAPI bool IObject::assign(IObject *other) {
    return false;
}

ZENAPI void IObject::dumpfile(std::string const &path) {
}

ZENAPI INode::INode() = default;
ZENAPI INode::~INode() = default;

ZENAPI Context::Context() = default;
ZENAPI Context::~Context() = default;

ZENAPI Context::Context(Context const &other)
    : visited(other.visited)
{
}

ZENAPI Graph::Graph() = default;
ZENAPI Graph::~Graph() = default;

ZENAPI void INode::doComplete() {
    set_output("DST", std::make_shared<ConditionObject>());
    complete();
}

ZENAPI void INode::complete() {}

ZENAPI bool INode::checkApplyCondition() {
    /*if (has_input("COND")) {  // deprecated
        auto cond = get_input<zeno::ConditionObject>("COND");
        if (!cond->get())
            return false;
    }*/

    if (has_option("ONCE")) {
        if (!zeno::state.isFirstSubstep())
            return false;
    }

    if (has_option("PREP")) {
        if (!zeno::state.isOneSubstep())
            return false;
    }

    if (has_option("MUTE")) {
        auto desc = nodeClass->desc.get();
        if (desc->inputs[0].name != "SRC") {
            muted_output = get_input(desc->inputs[0].name);
        } else {
            for (auto const &[ds, bound]: inputBounds) {
                muted_output = get_input(ds);
                break;
            }
        }
        return false;
    }

    return true;
}

ZENAPI void INode::doApply() {
    for (auto const &[ds, bound]: inputBounds) {
        requireInput(ds);
    }

    coreApply();
}

ZENAPI void INode::requireInput(std::string const &ds) {
    auto [sn, ss] = inputBounds.at(ds);
    graph->applyNode(sn);
    auto ref = graph->getNodeOutput(sn, ss);
    inputs[ds] = ref;
}

ZENAPI void INode::coreApply() {
    if (checkApplyCondition()) {
        apply();
    }

    if (has_option("VIEW")) {
        graph->hasAnyView = true;
        if (!state.isOneSubstep())  // no duplicate view when multi-substep used
            return;
        if (!graph->isViewed)  // VIEW subnodes only if subgraph is VIEW'ed
            return;
        auto desc = nodeClass->desc.get();
        auto obj = muted_output ? muted_output
            : safe_at(outputs, desc->outputs[0].name, "output");
        auto path = Visualization::exportPath();
        obj->dumpfile(path);
    }
}

ZENAPI bool INode::has_option(std::string const &id) const {
    return options.find(id) != options.end();
}

ZENAPI bool INode::has_input(std::string const &id) const {
    return inputBounds.find(id) != inputBounds.end();
}

ZENAPI std::shared_ptr<IObject> INode::get_input(std::string const &id) const {
    return safe_at(inputs, id, "input", myname);
}

ZENAPI IValue INode::get_param(std::string const &id) const {
    return safe_at(params, id, "param", myname);
}

ZENAPI void INode::set_output(std::string const &id, std::shared_ptr<IObject> &&obj) {
    outputs[id] = std::move(obj);
}

ZENAPI std::shared_ptr<IObject> const &Graph::getNodeOutput(
    std::string const &sn, std::string const &ss) const {
    auto node = safe_at(nodes, sn, "node");
    if (node->muted_output)
        return node->muted_output;
    return safe_at(node->outputs, ss, "output", node->myname);
}

ZENAPI void Graph::clearNodes() {
    nodes.clear();
}

ZENAPI void Graph::addNode(std::string const &cls, std::string const &id) {
    if (nodes.find(id) != nodes.end())
        return;  // no add twice, to prevent output object invalid
    auto cl = safe_at(sess->nodeClasses, cls, "node class");
    auto node = cl->new_instance();
    node->graph = this;
    node->myname = id;
    node->nodeClass = cl;
    nodes[id] = std::move(node);
}

ZENAPI void Graph::completeNode(std::string const &id) {
    safe_at(nodes, id, "node")->doComplete();
}

ZENAPI void Graph::applyNode(std::string const &id) {
    if (ctx->visited.find(id) != ctx->visited.end()) {
        return;
    }
    ctx->visited.insert(id);
    auto node = safe_at(nodes, id, "node");
    try {
        node->doApply();
    } catch (std::exception const &e) {
        throw zeno::Exception("During evaluation of `"
                + node->myname + "`:\n" + e.what());
    }
}

ZENAPI void Graph::applyNodes(std::vector<std::string> const &ids) {
    try {
        ctx = std::make_unique<Context>();
        for (auto const &id: ids) {
            applyNode(id);
        }
        ctx = nullptr;
    } catch (std::exception const &e) {
        ctx = nullptr;
        throw zeno::Exception(
                (std::string)"ZENO Traceback (most recent call last):\n"
                + e.what());
    }
}

ZENAPI void Graph::bindNodeInput(std::string const &dn, std::string const &ds,
        std::string const &sn, std::string const &ss) {
    safe_at(nodes, dn, "node")->inputBounds[ds] = std::pair(sn, ss);
}

ZENAPI void Graph::setNodeParam(std::string const &id, std::string const &par,
        IValue const &val) {
    safe_at(nodes, id, "node")->params[par] = val;
}

ZENAPI void Graph::setNodeOptions(std::string const &id,
        std::set<std::string> const &opts) {
    safe_at(nodes, id, "node")->options = opts;
}


ZENAPI Session::Session() {
    switchGraph("main");
}

ZENAPI Session::~Session() = default;

ZENAPI void Session::_defNodeClass(std::string const &id, std::unique_ptr<INodeClass> &&cls) {
    nodeClasses[id] = std::move(cls);
}

ZENAPI INodeClass::INodeClass(Descriptor const &desc)
        : desc(std::make_unique<Descriptor>(desc)) {
}

ZENAPI INodeClass::~INodeClass() = default;

ZENAPI void Session::switchGraph(std::string const &name) {
    if (graphs.find(name) == graphs.end()) {
        auto subg = std::make_unique<zeno::Graph>();
        subg->sess = this;
        graphs[name] = std::move(subg);
    }
    currGraph = graphs.at(name).get();
}

ZENAPI Graph &Session::getGraph() const {
    return *currGraph;
}

ZENAPI std::string Session::dumpDescriptors() const {
  std::string res = "";
  for (auto const &[key, cls] : nodeClasses) {
    res += "DESC@" + key + "@" + cls->desc->serialize() + "\n";
  }
  return res;
}


ZENAPI Session &getSession() {
    static std::unique_ptr<Session> ptr;
    if (!ptr) {
        ptr = std::make_unique<Session>();
    }
    return *ptr;
}


SocketDescriptor::SocketDescriptor(std::string const &type,
	  std::string const &name, std::string const &defl)
      : type(type), name(name), defl(defl) {}
SocketDescriptor::~SocketDescriptor() = default;


ParamDescriptor::ParamDescriptor(std::string const &type,
	  std::string const &name, std::string const &defl)
      : type(type), name(name), defl(defl) {}
ParamDescriptor::~ParamDescriptor() = default;

ZENAPI Descriptor::Descriptor() = default;
ZENAPI Descriptor::Descriptor(
  std::vector<SocketDescriptor> const &inputs,
  std::vector<SocketDescriptor> const &outputs,
  std::vector<ParamDescriptor> const &params,
  std::vector<std::string> const &categories)
  : inputs(inputs), outputs(outputs), params(params), categories(categories) {
    this->inputs.push_back("SRC");
    //this->inputs.push_back("COND");  // deprecated
    this->outputs.push_back("DST");
}

ZENAPI std::string Descriptor::serialize() const {
  std::string res = "";
  std::vector<std::string> strs;
  for (auto const &[type, name, defl] : inputs) {
      strs.push_back(type + "@" + name + "@" + defl);
  }
  res += "{" + join_str(strs, "%") + "}";
  strs.clear();
  for (auto const &[type, name, defl] : outputs) {
      strs.push_back(type + "@" + name + "@" + defl);
  }
  res += "{" + join_str(strs, "%") + "}";
  strs.clear();
  for (auto const &[type, name, defl] : params) {
      strs.push_back(type + "@" + name + "@" + defl);
  }
  res += "{" + join_str(strs, "%") + "}";
  res += "{" + join_str(categories, "%") + "}";
  return res;
}


} // namespace zeno
