#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <conio.h>

#define MAX_PLAYERS 20
#define MAX_THREADS 10

int scores[MAX_PLAYERS];
char names[MAX_PLAYERS][50];
pthread_mutex_t lock;

int num_players, num_threads;
int running = 1;
int paused = 0;
int game_duration;

FILE *log_file;

void log_event(const char *message)
{
    fprintf(log_file, "%s\n", message);
    fflush(log_file);
    
}

void *display_scores(void *arg)
{
    printf("Display thread started!\n");
    fflush(stdout);
    while (running)
    {
        pthread_mutex_lock(&lock);
        printf("[Paused=%d Running=%d]\n", paused, running);
        printf("\n------ Current Scores ------\n");
        for (int i = 0; i < num_players; i++)
        {
            printf("%s: %d\n", names[i], scores[i]);
        }
        pthread_mutex_unlock(&lock);
        sleep(2);
    }
    return NULL;
}

void *update_scores(void *arg)
{
    int id = *((int *)arg);
    srand(time(NULL) + id);

    while (running)
    {
        if (paused)
        {
            sleep(1);
            continue;
        }

        int player = rand() % num_players;
        int points = rand() % 10;

        pthread_mutex_lock(&lock);
        scores[player] += points;
        printf("Updater %d added %d points to %s\n", id, points, names[player]);

        char log_msg[100];
        sprintf(log_msg, "Updater %d added %d points to %s", id, points, names[player]);
        log_event(log_msg);
        pthread_mutex_unlock(&lock);

        sleep(rand() % 3 + 1);
    }

    free(arg);
    return NULL;
}

void *timer_thread(void *arg)
{
    int duration = *((int *)arg);
    for (int t = duration; t > 0 && running; t--)
    {
        sleep(1);
    }
    if (running)
    {
        printf("\n[Time's Up! Game Over]\n");
        log_event("Game ended due to timer completion.");
        running = 0;
    }
    return NULL;
}

void manual_scoring()
{
    pthread_mutex_lock(&lock);
    printf("\n--- Manual Scoring ---\n");
    for (int i = 0; i < num_players; i++)
        printf("%d. %s (Current: %d)\n", i + 1, names[i], scores[i]);
    printf("Enter player number: ");
    int choice;
    scanf("%d", &choice);
    getchar();
    if (choice < 1 || choice > num_players)
    {
        printf("Invalid player number!\n");
        pthread_mutex_unlock(&lock);
        return;
    }
    printf("Enter points to add: ");
    int pts;
    scanf("%d", &pts);
    getchar();
    scores[choice - 1] += pts;
    printf("Added %d points to %s\n", pts, names[choice - 1]);
    char log_msg[100];
    sprintf(log_msg, "Manual: Added %d points to %s", pts, names[choice - 1]);
    log_event(log_msg);
    pthread_mutex_unlock(&lock);
}

void *user_input(void *arg)
{
    while (running)
    {
        if (_kbhit())
        {
            char c = _getch();
            if (c == 'p' || c == 'P')
            {
                paused = 1;
                printf("\n[Game Paused]\n");
                log_event("Game paused by user.");
            }
            else if (c == 'r' || c == 'R')
            {
                paused = 0;
                printf("\n[Game Resumed]\n");
                log_event("Game resumed by user.");
            }
            else if (c == 'm' || c == 'M')
            {
                paused = 1;
                manual_scoring();
                paused = 0;
            }
            else if (c == 'x' || c == 'X')
            {
                running = 0;
                printf("\n[Exiting Game...]\n");
                log_event("Game exited manually.");
            }
        }
        sleep(1);
    }
    return NULL;
}

int main()
{
    printf("Enter number of players: ");
    scanf("%d", &num_players);
    getchar();

    for (int i = 0; i < num_players; i++)
    {
        printf("Enter name of Player %d: ", i + 1);
        fgets(names[i], 50, stdin);
        names[i][strcspn(names[i], "\n")] = 0;
        scores[i] = 0;
    }

    printf("Enter number of updater threads: ");
    scanf("%d", &num_threads);
    printf("Enter game duration (in seconds): ");
    scanf("%d", &game_duration);

    log_file = fopen("score_log.txt", "w");
    if (!log_file)
    {
        printf("Error opening log file.\n");
        return 1;
    }

    pthread_t display_thread, updater_threads[MAX_THREADS], input_thread, timer;
    int ids[MAX_THREADS];
    pthread_mutex_init(&lock, NULL);

    pthread_create(&display_thread, NULL, display_scores, NULL);
    for (int i = 0; i < num_threads; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&updater_threads[i], NULL, update_scores, id);
    }
    pthread_create(&input_thread, NULL, user_input, NULL);
    pthread_create(&timer, NULL, timer_thread, &game_duration);

    pthread_join(input_thread, NULL);
    running = 0;

    pthread_join(display_thread, NULL);
    pthread_join(timer, NULL);
    for (int i = 0; i < num_threads; i++)
        pthread_join(updater_threads[i], NULL);

    pthread_mutex_destroy(&lock);
    fclose(log_file);

    printf("\n--- Final Scores ---\n");
    for (int i = 0; i < num_players; i++)
        printf("%s: %d\n", names[i], scores[i]);
    printf("All updates saved in score_log.txt\n");

    return 0;
}
