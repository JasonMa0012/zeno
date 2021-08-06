#include <zeno/zeno.h>

namespace {

struct LiterialInt : zeno::INode {
    virtual void apply() override {
        auto value = get_param<int>("value");
        set_output2("value", value);
    }
};

ZENDEFNODE(LiterialInt, {
    {},
    {{"int", "value"}},
    {{"int", "value", "0"}},
    {"numeric"},
});

struct LiterialFloat : zeno::INode {
    virtual void apply() override {
        auto value = get_param<float>("value");
        set_output2("value", value);
    }
};

ZENDEFNODE(LiterialFloat, {
    {},
    {{"float", "value"}},
    {{"float", "value", "0"}},
    {"numeric"},
});

}
