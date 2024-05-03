#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int** graph;
    int n;
    int start;
    int end;
    int* counter;
} ThreadArgs;

void make_edges(int n, int** graph) {
    for (int i = 0; i < n * n; i++) {
        for (int j = 0; j < 4; j++) {
            graph[i][j] = -1;
        }
    }

    if (n < 2) {
        return;
    }

    for (int i = 0; i < n * n; i++) {
        int row = i / n;
        int col = i % n;
        int num_neighbors = 0;

        if (col < n - 1) {
            graph[i][num_neighbors++] = i + 1;  // Right
        }
        if (row < n - 1) {
            graph[i][num_neighbors++] = i + n;  // Down
        }
        if (col > 0) {
            graph[i][num_neighbors++] = i - 1;  // Left
        }
        if (row > 0) {
            graph[i][num_neighbors++] = i - n;  // Up
        }
    }
}

int findAllPaths(int** graph, int src, int dst, bool* visited) {
    int counter = 0;
    visited[src] = true;

    if (src == dst) {
        counter++;
    } else {
        for (int i = 0; i < 4; i++) {
            int neighbor = graph[src][i];
            if (neighbor != -1 && !visited[neighbor]) {
                counter += findAllPaths(graph, neighbor, dst, visited);
            }
        }
    }

    visited[src] = false;
    return counter;
}

void* calculate_paths(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int** graph = args->graph;
    int n = args->n;
    int start = args->start;
    int end = args->end;
    int* counter = args->counter;

    bool* visited = (bool*)calloc(n * n, sizeof(bool));

    for (int src = start; src < end; src++) {
        for (int dst = src + 1; dst < n * n; dst++) {
            int delta = findAllPaths(graph, src, dst, visited);
            *counter += delta;
//            printf("\nFollowing are all different paths from %d to %d: %d", src, dst, delta);
        }
    }

    free(visited);
    return NULL;
}

int main(void) {
    for (int n = 1; n < 8; n++) {
        FILE* fp = fopen("output.txt", "a");
        if (fp == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        int** graph = (int**)malloc(n * n * sizeof(int*));
        for (int i = 0; i < n * n; i++) {
            graph[i] = (int*)malloc(4 * sizeof(int));
        }
        make_edges(n, graph);

        int num_threads = (int)sysconf(_SC_NPROCESSORS_ONLN);
        int chunk_size = (n * n) / num_threads;

        pthread_t* threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
        ThreadArgs* thread_args = (ThreadArgs*)malloc(num_threads * sizeof(ThreadArgs));
        int* counters = (int*)calloc(num_threads, sizeof(int));

        time_t start = time(NULL);

        for (int i = 0; i < num_threads; i++) {
            thread_args[i].graph = graph;
            thread_args[i].n = n;
            thread_args[i].start = i * chunk_size;
            thread_args[i].end = (i == num_threads - 1) ? n * n : (i + 1) * chunk_size;
            thread_args[i].counter = &counters[i];
            pthread_create(&threads[i], NULL, calculate_paths, &thread_args[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }

        int total_counter = 0;
        for (int i = 0; i < num_threads; i++) {
            total_counter += counters[i];
        }

        time_t end = time(NULL);

        total_counter = total_counter * 2 + n * n;
        double time_taken = difftime(end, start);
        char answer[100];
        snprintf(answer, sizeof(answer), "\n%dx%d - %d (time taken: %.2f seconds)", n, n, total_counter, time_taken);
        printf("%s\n", answer);
        fprintf(fp, "%s", answer);

        for (int i = 0; i < n * n; i++) {
            free(graph[i]);
        }
        free(graph);
        free(threads);
        free(thread_args);
        free(counters);
        fclose(fp);
    }

    return 0;
}
