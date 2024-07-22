#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils_log.h"

double _gauss_noise(double mean, double stddev)
{
    const int n   = 12; // 使用12个随机数的和，根据中心极限定理
    double    sum = 0.0;

    for (int i = 0; i < n; i++) {
        sum += (double)rand() / RAND_MAX;
    }

    // 标准化和调整
    sum -= n / 2.0;
    sum /= sqrt(n / 12.0);

    return stddev * sum + mean;
}


int main(int argc, char* argv[])
{

    // extern int serial_test();
    // serial_test();

    // extern int premake_test();
    // // premake_test();
    // srand(time(NULL));
    // for (int i = 0; i < 10000; i++) {
    //     double noise = _gauss_noise(0, 1);
    //     printf("%f, ", noise);
    //     if (noise > 5 || noise < -5) {
    //         printf("error\n");
    //     }
    // }

    slog_info("- unit test start -\n");

    mlog_test();

    slog_set_level(M_DEBUG);

    math_test();
    lib_test();
    fifo_test();
    socket_test();

    slog_info("- unit test done -\n");

    return 0;
}
