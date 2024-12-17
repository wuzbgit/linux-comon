#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "rf.h"

typedef enum {
    RF_INIT,
    RF_SOFT_RESET,
    RF_HARD_RESET,
    RF_CHECK_MODULE,
    RF_CHECK_SIGNAL,
    RF_CHECK_SIMCARD,
    RF_DIAL,
    RF_NET
}RF_State_e;

static volatile bool running = true;

static bool check_net()
{
    
}
void* rf_monitor_thread(void* arg)
{
    RF_State_e state = RF_INIT;
    while (running) {
        switch (state)
        {
        case RF_INIT:
            if (rf_exist() == false) {
                rf_dev_clean();
                state = RF_HARD_RESET;
            }
            break;
        case RF_HARD_RESET:
            rf_reset();
            break;
        case RF_SOFT_RESET:
            break;
        case RF_CHECK_MODULE:
            break;
        case RF_CHECK_SIMCARD:
            break;
        case RF_DIAL:
            break;
        case RF_NET:
            break;
        default:
            break;
        }
    }
    printf("Log thread exiting...\n");
    return NULL;
}

void handle_signal(int sig)
{
    if (sig == SIGINT) {
        printf("Received SIGINT, ending process...\n");
        running = false;

        exit(0);
    } else if (sig == SIGUSR1) {
        printf("Received SIGUSR1, logging special message...\n");
    }
}

int main(int argc, char *argv[]) {
    pthread_t log_tid;
    struct sigaction sa;

    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL); // 捕获Ctrl+C
    sigaction(SIGUSR1, &sa, NULL); // 捕获SIGUSR1

    if (pthread_create(&log_tid, NULL, log_thread, NULL) != 0) {
        perror("Failed to create log thread");
        exit(EXIT_FAILURE);
    }

    while (running) {
        pause(); 
    }

    pthread_cancel(log_tid);
    pthread_join(log_tid, NULL);

    return 0;
}

