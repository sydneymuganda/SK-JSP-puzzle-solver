
#include "./include/server.h"
#include <pthread.h>

int main(int argc, char const *argv[])
{
    pthread_t thread1, thread2;
    // run server thread
    if (pthread_create(&thread1, NULL, server_main, NULL) != 0) {
        die("pthread_create() for thread1");
    }

    // run client thread
    if (pthread_create(&thread2, NULL, client_main, NULL) != 0) {
        die("pthread_create() for thread2");
    }

    // Wait for both threads to finish
    if (pthread_join(thread1, NULL) != 0) {
        die("pthread_join() for thread1");
    }
    if (pthread_join(thread2, NULL) != 0) {
        die("pthread_join() for thread2");
    }
    return 0;
}