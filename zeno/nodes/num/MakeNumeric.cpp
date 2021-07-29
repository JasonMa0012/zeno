#include <zeno/zeno.h>
#include <zeno/NumericObject.h>

namespace {

struct NumericInt : zeno::INode {
    virtual void apply() override {
        auto obj = std::make_unique<zeno::NumericObject>();
        obj->set(get_param<int>("value"));
        set_output("value", std::move(obj));
    }
};

ZENDEFNODE(NumericInt, {
    {},
    {{"numeric:int", "value"}},
    {{"int", "value", "0"}},
    {"numeric"},
});


struct NumericFloat : zeno::INode {
    virtual void apply() override {
        auto obj = std::make_unique<zeno::NumericObject>();
        obj->set(get_param<float>("value"));
        set_output("value", std::move(obj));
    }
};

ZENDEFNODE(NumericFloat, {
    {},
    {{"numeric:float", "value"}},
    {{"float", "value", "0"}},
    {"numeric"},
});


struct NumericVec2 : zeno::INode {
    virtual void apply() override {
        auto obj = std::make_unique<zeno::NumericObject>();
        auto x = get_param<float>("x");
        auto y = get_param<float>("y");
        obj->set(zeno::vec2f(x, y));
        set_output("vec2", std::move(obj));
    }
};

ZENDEFNODE(NumericVec2, {
    {},
    {{"numeric:vec2f", "vec2"}},
    {{"float", "x", "0"}, {"float", "y", "0"}},
    {"numeric"},
});


struct NumericVec3 : zeno::INode {
    virtual void apply() override {
        auto obj = std::make_unique<zeno::NumericObject>();
        auto x = get_param<float>("x");
        auto y = get_param<float>("y");
        auto z = get_param<float>("z");
        obj->set(zeno::vec3f(x, y, z));
        set_output("vec3", std::move(obj));
    }
};

ZENDEFNODE(NumericVec3, {
    {},
    {{"numeric:vec3f", "vec3"}},
    {{"float", "x", "0"}, {"float", "y", "0"}, {"float", "z", "0"}},
    {"numeric"},
});


struct NumericVec4 : zeno::INode {
    virtual void apply() override {
        auto obj = std::make_unique<zeno::NumericObject>();
        auto x = get_param<float>("x");
        auto y = get_param<float>("y");
        auto z = get_param<float>("z");
        auto w = get_param<float>("w");
        obj->set(zeno::vec4f(x, y, z, w));
        set_output("vec4", std::move(obj));
    }
};

ZENDEFNODE(NumericVec4, {
    {},
    {{"numeric:vec4f", "vec4"}},
    {{"float", "x", "0"}, {"float", "y", "0"},
     {"float", "z", "0"}, {"float", "w", "0"}},
    {"numeric"},
});

}
