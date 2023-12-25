
#include "mlog_.h"


int main(int argc, char* argv[])
{
    SLOG_INFO("- unit test start -\n");

    mlog_test();
    SLOG_SET_LEVEL(M_DEBUG);

    math_test();
    lib_test();
    fifo_test();
    socket_test();

    SLOG_INFO("- unit test done -\n");

    return 0;
}
