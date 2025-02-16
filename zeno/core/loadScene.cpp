#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <spdlog/spdlog.h>
#include <zeno/zeno.h>

namespace zeno {

using namespace rapidjson;

template <class T>
static T generic_get(Value const &x) {
    if (x.IsString()) {
        return (std::string)x.GetString();
    } else if (x.IsInt()) {
        return x.GetInt();
    } else if (x.IsFloat()) {
        return x.GetFloat();
    } else {
        return 0;
    }
}

ZENO_API void Scene::loadScene(const char *json) {
    Document d;
    d.Parse(json);

    for (int i = 0; i < d.Size(); i++) {
        Value const &di = d[i];
        std::string cmd = di[0].GetString();
#ifdef ZENO_FAIL_SILENTLY
        try {
#endif
            if (0) {
            } else if (cmd == "addNode") {
                getGraph().addNode(di[1].GetString(), di[2].GetString());
            } else if (cmd == "completeNode") {
                getGraph().completeNode(di[1].GetString());
            } else if (cmd == "setNodeInput") {
                getGraph().setNodeInput(di[1].GetString(), di[2].GetString(), generic_get<zeno::zany>(di[3]));
            } else if (cmd == "setNodeParam") {
                getGraph().setNodeParam(di[1].GetString(), di[2].GetString(), generic_get<std::variant<int, float, std::string>>(di[3]));
            } else if (cmd == "setNodeOption") {
                getGraph().setNodeOption(di[1].GetString(), di[2].GetString());
            } else if (cmd == "bindNodeInput") {
                getGraph().bindNodeInput(di[1].GetString(), di[2].GetString(), di[3].GetString(), di[4].GetString());
            } else if (cmd == "switchGraph") {
                this->switchGraph(di[1].GetString());
            } else if (cmd == "clearAllState") {
                this->clearAllState();
            }
#ifdef ZENO_FAIL_SILENTLY
        } catch (zeno::BaseException const &e) {
            spdlog::warn("exception executing command {} ({}): {}",
                    i, cmd.c_str(), e.what());
        }
#endif
    }
}

}
