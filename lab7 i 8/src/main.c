/* prime_task_manager.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <errno.h>

#define MAX_PROCESSES 10
#define MAX_THREADS 10
#define MAX_LINE 256

// Struktury danych
typedef struct {
    int *numbers;
    int size;
    int *results;
    int *result_index;
    int *current_index;
    pthread_mutex_t *mutex;
    sem_t *sem;
} ThreadData;

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void* worker_thread(void *arg) {
    ThreadData *data = (ThreadData*)arg;
    while (1) {
        sem_wait(data->sem);
        pthread_mutex_lock(data->mutex);
        int idx = (*data->current_index)++;
        pthread_mutex_unlock(data->mutex);

        if (idx >= data->size) break;
        int number = data->numbers[idx];
        if (is_prime(number)) {
            pthread_mutex_lock(data->mutex);
            data->results[(*data->result_index)++] = number;
            pthread_mutex_unlock(data->mutex);
        }
    }
    return NULL;
}

void child_process(int read_fd, int write_fd, int W) {
    while (1) {
        int size;
        if (read(read_fd, &size, sizeof(int)) <= 0) break;
        if (size == -1) exit(0); // sygnał zakończenia

        int *numbers = malloc(size * sizeof(int));
        read(read_fd, numbers, size * sizeof(int));

        int *results = malloc(size * sizeof(int));
        int current_index = 0, result_index = 0;
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        sem_t sem;
        sem_init(&sem, 0, size + W);

        ThreadData td = { numbers, size, results, &result_index, &current_index, &mutex, &sem };
        pthread_t threads[W];

        for (int i = 0; i < W; i++)
            pthread_create(&threads[i], NULL, worker_thread, &td);

        for (int i = 0; i < W; i++)
            pthread_join(threads[i], NULL);

        write(write_fd, &result_index, sizeof(int));
        write(write_fd, results, result_index * sizeof(int));

        free(numbers);
        free(results);
        sem_destroy(&sem);
    }
    exit(0);
}

void distribute_range(int start, int end, int P, int W, int pipes_in[][2], int pipes_out[][2], const char *output_file) {
    int range = end - start + 1;
    int chunk = range / P;
    int leftover = range % P;
    int current = start;

    for (int i = 0; i < P; i++) {
        int part = chunk + (i < leftover ? 1 : 0);
        int *nums = malloc(part * sizeof(int));
        for (int j = 0; j < part; j++) nums[j] = current++;

        write(pipes_in[i][1], &part, sizeof(int));
        write(pipes_in[i][1], nums, part * sizeof(int));

        free(nums);
    }

    FILE *out = fopen(output_file, "w");
    if (!out) {
        perror("fopen output file");
        return;
    }

    for (int i = 0; i < P; i++) {
        int count;
        read(pipes_out[i][0], &count, sizeof(int));
        int *buf = malloc(count * sizeof(int));
        read(pipes_out[i][0], buf, count * sizeof(int));
        for (int j = 0; j < count; j++) fprintf(out, "%d\n", buf[j]);
        free(buf);
    }
    fclose(out);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s P W\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int P = atoi(argv[1]);
    int W = atoi(argv[2]);
    int pipes_in[MAX_PROCESSES][2], pipes_out[MAX_PROCESSES][2];
    pid_t children[MAX_PROCESSES];

    unlink("zadania.in"); // upewnij się że FIFO nie istnieje
    if (mkfifo("zadania.in", 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Otwórz FIFO do odczytu blokująco
    int fifo_fd = open("zadania.in", O_RDONLY);
    if (fifo_fd == -1) {
        perror("open fifo read");
        exit(EXIT_FAILURE);
    }

    // Dummy zapis do FIFO żeby uniknąć EOF przy braku writerów
    int dummy_fd = open("zadania.in", O_WRONLY | O_NONBLOCK);

    FILE *fifo_file = fdopen(fifo_fd, "r");
    if (!fifo_file) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < P; i++) {
        pipe(pipes_in[i]);
        pipe(pipes_out[i]);
        if ((children[i] = fork()) == 0) {
            close(pipes_in[i][1]);
            close(pipes_out[i][0]);
            child_process(pipes_in[i][0], pipes_out[i][1], W);
        }
        close(pipes_in[i][0]);
        close(pipes_out[i][1]);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fifo_file)) {
        if (strncmp(line, "EXIT", 4) == 0) break;

        int start, end;
        char output_file[128];
        if (sscanf(line, "%d %d %s", &start, &end, output_file) == 3) {
            printf("Otrzymano zadanie: %d %d -> %s\n", start, end, output_file);
            distribute_range(start, end, P, W, pipes_in, pipes_out, output_file);
        } else {
            fprintf(stderr, "Niepoprawny format zadania: %s", line);
        }
    }

    // Zakończ procesy potomne
    for (int i = 0; i < P; i++) {
        int end_signal = -1;
        write(pipes_in[i][1], &end_signal, sizeof(int));
    }
    for (int i = 0; i < P; i++) wait(NULL);

    close(fifo_fd);
    if (dummy_fd != -1) close(dummy_fd);
    unlink("zadania.in");

    return 0;
}
