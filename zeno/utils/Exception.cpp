#include <zeno/utils/Exception.h>
#include <spdlog/spdlog.h>

namespace zeno {

void print_traceback(int skip);
void trigger_gdb();

ZENO_API BaseException::BaseException(std::string_view msg) noexcept
    : msg(msg) {
}

ZENO_API Exception::Exception(std::string_view msg) noexcept
    : BaseException(msg) {
    spdlog::error("Exception: {}", msg);
    print_traceback(0);
    trigger_gdb();
}

ZENO_API BaseException::~BaseException() noexcept = default;

ZENO_API char const *BaseException::what() const noexcept {
    return msg.c_str();
}

}
