#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool timer_done = false;

struct question {
    char *question;
    char *answer;
};
typedef struct question question_t;

struct ask_args {
    int score;
    question_t *question;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
};
typedef struct ask_args ask_args_t;

struct timer_args{
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
};
typedef struct timer_args timer_args_t;

void *timer(void *args) {
    timer_args_t *timerArgs = args;
    sleep(5);
    timer_done = true;
    pthread_cond_signal(timerArgs ->cond);
    pthread_exit(NULL);
}

void *ask(void *args) {
    ask_args_t *ask_args = args;

    printf("%s? ", ask_args->question->question);

    char input[128];

    fgets(input, 128, stdin);

    // strip newline
    input[strcspn(input, "\n")] = '\0';

    if (!strcmp(input, ask_args->question->answer)) {
        printf("Correct!\n");
        ask_args->score++;
    } else {
        printf("Incorrect :-(\n");
    }
    pthread_cond_signal(ask_args->cond);
    pthread_exit(NULL);
}


int main(int argc, char const *argv[]) {
    question_t questions[] = {
        {.question = "3*2", .answer = "6"},
        {.question = "50/10", .answer = "5"},
        {.question = "2+1+1+1", .answer = "5"},
        {.question = "3^3", .answer = "27"},
        {.question = "3+3", .answer = "6"},
        {.question = "4/2", .answer = "2"}};


    pthread_cond_t cond;
    pthread_mutex_t mutex;
    pthread_cond_init(&cond,NULL);
    pthread_mutex_init(&mutex,NULL);

    timer_args_t timerArgs = {.cond = &cond, .mutex = &mutex};
    pthread_t timer_thread;
    if (pthread_create(&timer_thread, NULL, timer, &timerArgs)) {
        printf("Error creating timer thread.\n");
    }
    int score = 0;
    for (int i = 0; i < 6; ++i) {
        if(timer_done){
            break;
        }
        ask_args_t ask_args = {.score = score, .question = &questions[i], &cond, &mutex};

        pthread_t ask_thread;
        if (pthread_create(&ask_thread, NULL, ask, &ask_args)) {
            printf("Error creating asker thread.\n");
        }
        pthread_cond_wait(&cond,&mutex);

        score = ask_args.score;
    }

    printf("End of questions, final score %d\n", score);
    return 0;
}
