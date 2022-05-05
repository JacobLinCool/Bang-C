#ifndef __UTILS_CATCHER_H
#define __UTILS_CATCHER_H

#include <signal.h>

#include "../third/cimple/all.h"

static u8 terminate_countdown = 3;

void __catch_signal(int sig) {
    switch (terminate_countdown) {
        case 3:
            Console.yellow("\nDon't go!");
            break;

        case 2:
            Console.yellow("\nPlease don't go!");
            break;

        case 1:
            Console.yellow("\nPlease~~");
            break;
        default:
            Console.red("\n:(");
            exit(EXIT_FAILURE);
    }
    terminate_countdown--;
}

void setup_catcher() {
    struct sigaction action;
    action.sa_handler = __catch_signal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &action, NULL);
}

#endif  // __UTILS_CATCHER_H
