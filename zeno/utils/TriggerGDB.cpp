#ifdef ZENO_FAULTHANDLER
#include <zeno/zeno.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#ifdef __linux__
#include <unistd.h>
#endif

namespace zeno {

void trigger_gdb() {
#ifdef __linux__
    if (!getenv("ZEN_TRIGGDB"))
        return;
    char cmd[1024];
    sprintf(cmd, "sudo gdb -q "
            " -ex 'set confirm off'"
            " -ex 'set pagination off'"
            " -p %d", getpid());
    system(cmd);
#endif
}

}
#else
namespace zeno {
void trigger_gdb() {
}
}
#endif
