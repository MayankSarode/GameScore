#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_PLAYERS 5
#define NUM_THREADS 3

int scores[NUM_PLAYERS];
pthread_mutex_t lock;

void* display_scores(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        FILE* file = fopen("scores.txt", "w");
        if (file) {
            for (int i = 0; i < NUM_PLAYERS; i++) {
                fprintf(file, "%d\n", scores[i]);
            }
            fclose(file);
        }
        pthread_mutex_unlock(&lock);
        sleep(1);
    }
    return NULL;
}

void* update_scores(void* arg) {
    int id = *((int*)arg);
    srand(time(NULL) + id);
    while (1) {
        int player = rand() % NUM_PLAYERS;
        int points = rand() % 10;
        pthread_mutex_lock(&lock);
        scores[player] += points;
        printf("Updater %d added %d points to Player %d\n", id, points, player + 1);
        pthread_mutex_unlock(&lock);
        sleep(rand() % 3 + 1);
    }
    return NULL;
}

int main() {
    pthread_t display_thread;
    pthread_t updater_threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < NUM_PLAYERS; i++) scores[i] = 0;

    pthread_create(&display_thread, NULL, display_scores, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&updater_threads[i], NULL, update_scores, &thread_ids[i]);
    }

    pthread_join(display_thread, NULL);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(updater_threads[i], NULL);

    pthread_mutex_destroy(&lock);
    return 0;
}
