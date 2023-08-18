import time
import numba as nb
import numpy as np


@nb.njit
def findAllPaths(graph, src, dst, visited):
    counter = 0
    visited[src] = True

    if src == dst:
        counter += 1
    else:
        for i in range(graph.shape[1]):
            neighbor = graph[src, i]
            if neighbor != -1 and not visited[neighbor]:
                counter += findAllPaths(graph, neighbor, dst, visited)

    visited[src] = False
    return counter


def make_edges(n):
    graph = np.empty((n ** 2, 4), dtype=np.int8)
    graph.fill(-1)

    if n < 2:
        return graph

    for layer in range(1, n + 1):
        prev_layer = layer - 1
        for i in range(prev_layer ** 2, layer ** 2):
            neighbors = []
            if layer != n:
                if i < prev_layer ** 2 + layer:  # right
                    neighbors.append(i + (2 * layer - 1))
                if i >= prev_layer ** 2 + prev_layer:  # down
                    neighbors.append(i + (2 * layer + 1))

            if i < prev_layer ** 2 + prev_layer:  # left
                neighbors.append(i - (2 * prev_layer - 1))
            elif i > prev_layer ** 2 + prev_layer:  # up
                neighbors.append(i - (2 * prev_layer + 1))

            if i < layer ** 2 - 1:
                neighbors.append(i + 1)
            if i > prev_layer ** 2:
                neighbors.append(i - 1)

            graph[i, :len(neighbors)] = neighbors

    return graph


if __name__ == "__main__":
    for n in range(1, 8):
        graph = make_edges(n)

        counter = delta = 0
        visited = np.zeros(n ** 2, dtype=np.bool_)
        start = time.time()

        for src in range(n ** 2):
            for dst in range(src + 1, n ** 2):
                delta = findAllPaths(graph, src, dst, visited)
                counter += delta
                print(f"Following are all different paths from {src} to {dst}: {delta}")

        end = time.time()

        counter = counter * 2 + n ** 2
        answer = f"\n{n}x{n} - {counter} (time taken: {round(end - start, 2)} seconds)"
        print(answer)

        with open("output.txt", "a") as f:
            f.write(answer)
