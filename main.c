#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

        int counter = 0;
        bool* visited = (bool*)calloc(n * n, sizeof(bool));

        clock_t start = clock();
        for (int src = 0; src < n * n; src++) {
            for (int dst = src + 1; dst < n * n; dst++) {
                int delta = findAllPaths(graph, src, dst, visited);
                counter += delta;
                printf("\nFollowing are all different paths from %d to %d: %d", src, dst, delta);
            }
        }
        clock_t end = clock();

        counter = counter * 2 + n * n;
        double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
        char answer[100];
        snprintf(answer, sizeof(answer), "\n%dx%d - %d (time taken: %.2f seconds)", n, n, counter, time_taken);
        printf("%s\n", answer);
        fprintf(fp, "%s", answer);

        for (int i = 0; i < n * n; i++) {
            free(graph[i]);
        }
        free(graph);
        free(visited);
        fclose(fp);
    }
    
    return 0;
}
