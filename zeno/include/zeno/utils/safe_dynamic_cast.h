#pragma once

#include <any>
#include <memory>
#include <string>
#include <typeinfo>
#include <zeno/utils/Exception.h>

namespace zeno {

template <class T, class S>
T *safe_dynamic_cast(S *s, std::string const &msg = {}) {
    auto t = dynamic_cast<T *>(s);
    if (!t) {
        throw Exception(msg + "expect `"
                + typeid(T).name() + "`, got `"
                + typeid(*s).name() + "` (dyncast)");
    }
    return t;
}

template <class T, class S>
std::shared_ptr<T> safe_dynamic_cast(
        std::shared_ptr<S> s, std::string const &msg = {}) {
    auto t = std::dynamic_pointer_cast<T>(s);
    if (!t) {
        throw Exception(msg + "expect `"
                + typeid(T).name() + "`, got `"
                + typeid(*s).name() + "` (dyncast)");
    }
    return t;
}

template <class T, class A>
T safe_any_cast(A &&a, std::string const &msg = {}) {
    try {
        return std::any_cast<T>(std::forward<A>(a));
    } catch (std::bad_any_cast const &e) {
        throw Exception(msg + "expect `"
                + typeid(T).name() + "`, got `"
                + a.type().name() + "` (anycast)");
    }
}

}
