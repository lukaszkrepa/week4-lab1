#include <pthread.h>
#include <stdio.h>

void *hello_world(void *args) {
    int *n = args;
    printf("Hello from thread %d\n", *n);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
    pthread_t threads[5];
    int values[5];
    for (int i = 1; i <=5; i++){
        values[i-1] = i;
        if (pthread_create(&threads[i-1], NULL, hello_world, &values[i-1])) {
            printf("Error creating thread\n");
        }

    }
    for (int i = 1; i <= 5; i++){
        if (pthread_join(threads[i-1], NULL)) {
            printf("Error joining thread\n");
        }
    }

}
