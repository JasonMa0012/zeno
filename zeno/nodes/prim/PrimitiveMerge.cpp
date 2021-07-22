#include <zeno/zeno.h>
#include <zeno/PrimitiveObject.h>
#include <zeno/ListObject.h>
#include <zeno/vec.h>
#include <cstring>
#include <cstdlib>
#include <cassert>

namespace zeno {


struct PrimitiveMerge : zeno::INode {
  virtual void apply() override {
    auto list = get_input<ListObject>("listPrim");
    auto outprim = std::make_shared<PrimitiveObject>();

    size_t len = 0;
    for (auto const &obj: list->arr) {
        auto prim = dynamic_cast<PrimitiveObject *>(obj.get());
        assert(prim);
        for (auto const &[key, varr]: prim->m_attrs) {
            std::visit([&, key_ = key](auto const &arr) {
                using T = std::decay_t<decltype(arr[0])>;
                auto &outarr = outprim->add_attr<T>(key_);
                for (auto const &val: arr) {
                    outarr.push_back(val);
                }
            }, varr);
        }
        for (auto const &idx: prim->points) {
            outprim->points.push_back(idx + len);
        }
        for (auto const &idx: prim->lines) {
            outprim->lines.push_back(idx + len);
        }
        for (auto const &idx: prim->tris) {
            outprim->tris.push_back(idx + len);
        }
        for (auto const &idx: prim->quads) {
            outprim->quads.push_back(idx + len);
        }
        len += prim->size();
    }
    outprim->resize(len);

    set_output("prim", std::move(outprim));
  }
};

ZENDEFNODE(PrimitiveMerge, {
    {"listPrim"},
    {"prim"},
    {},
    {"primitive"},
});


}
