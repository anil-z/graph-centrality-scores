#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

struct queue {
    int data;
    struct queue *next;
};
typedef struct queue Queue;

Queue *q = NULL;

char *nameReturner(char *str) {
    int i;
    static char tempStr[25];
    strcpy(tempStr, str);

    for (i = 0; i < strlen(tempStr); i++)
        if (tempStr[i] == ';')
            tempStr[i] = '\0';

    return tempStr;
}

int whichIndex(char array[][25], int size, char *str) {
    int i;
    for (i = 0; i < size; i++)
        if (strcmp(array[i], str) == 0)
            return i;

    return -1;
}

char *fileOpener(char *filePath) {
    FILE *file = fopen(filePath, "r");
    char str[500];
    static char mainStr[500];
    strcpy(mainStr, " ");

    while (fgets(str, 50, file) != NULL)
        strcat(mainStr, str);

    return mainStr;
}

void graphMaker(char *str, int count, int graph[][count]) {
    char temp[strlen(str)];
    strcpy(temp, str);

    int i, j;


    char names[count][25];

    for (i = 0; i < count; i++)
        for (j = 0; j < count; j++)
            graph[i][j] = 0;

    int index = 0;

    char *line = strtok(str, "\n");
    while (line != NULL) {
        strcpy(names[index], nameReturner(line));
        index++;
        line = strtok(NULL, "\n");
    }

    index = 0;

    line = strtok(temp, "\n");
    while (line != NULL) {

        char *token;
        char *rest = line;

        while ((token = strtok_r(rest, " ", &rest))) {
            if (token[strlen(token) - 1] == ',')
                token[strlen(token) - 1] = '\0';

            if (whichIndex(names, count, token) != -1)
                graph[index][whichIndex(names, count, token)] = 1;
        }

        index++;
        line = strtok(NULL, "\n");
    }

}

void degreeCentrality(int size, int graph[][size], int scores[size], float standardizedScores[size]) {
    int count = 0, i, j;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++)
            if (graph[i][j] == 1)
                count++;

        scores[i] = count;
        standardizedScores[i] = (float) count / (size - 1);
        count = 0;
    }

}

void addQueue(int x) {
    if (q == NULL) {
        q = malloc(sizeof(Queue));
        q->data = x;
        q->next = NULL;
    } else {
        Queue *iter = q;
        while (iter->next != NULL)
            iter = iter->next;
        iter->next = malloc(sizeof(Queue));
        iter->next->data = x;
        iter->next->next = NULL;
    }
}

int removeQueue() {
    Queue *temp = q;
    int x = temp->data;
    q = q->next;
    free(temp);
    return x;
}

int queueEmpty() {
    if (q == NULL)
        return 1;
    return 0;
}

int BFS(int size, int graph[][size], int source) {
    // Returns the source vertex's total distance to other vertices.
    int i, distance[size];
    for (i = 0; i < size; i++)
        distance[i] = 0;

    addQueue(source);

    int x = 0;

    while (!queueEmpty()) {
        x = removeQueue();
        for (i = 0; i < size; i++) {
            if (graph[x][i] == 1) {
                if (distance[i] == 0) {
                    distance[i] = distance[x] + 1;
                    addQueue(i);
                }
            }
        }
    }
    distance[source] = 0;

    int sum = 0;
    for (i = 0; i < size; i++)
        sum += distance[i];

    return sum;

}

void printPath(int currentVertex, int from[], int size, int num, int path[][size]) {
    if (from[currentVertex] == -100)
        return;
    printPath(from[currentVertex], from, size, num, path);
    printf("%d ", currentVertex);
    int i;
    for (i = 0; i < size; i++) {
        if (path[num][i] == -1) {
            path[num][i] = currentVertex;
            break;
        }
    }
}

void dijkstra(int size, int graph[][size], int source, int target, int num, int path[][size]) {
    // Returns the shortest path between source and target

    int i, dist[size], from[size], infinity = 999, undefined = -100, u, temp[size];

    for (i = 0; i < size; i++) {
        dist[i] = infinity;
        from[i] = undefined;
        addQueue(i);
        temp[i] = -1;
    }
    dist[source] = 0;

    while (!queueEmpty()) {
        u = removeQueue();
        for (i = 0; i < size; i++) {
            if (graph[u][i] == 1) {
                if (dist[i] > dist[u] + 1) {
                    dist[i] = dist[u] + 1;
                    from[i] = u;
                }
            }
        }
    }

    path[num][0] = source;
    printf("%d\t\t\t\t%d\t\t\t\t\t\t\t\t%d ", source, target, source);
    printPath(target, from, size, num, path);
    printf("\n");

}

void closenessCentrality(int size, int graph[][size], float scores[size], float standardizedScores[size]) {

    int i;
    for (i = 0; i < size; i++) {
        float x = BFS(size, graph, i);
        scores[i] = (float) 1 / x;
        standardizedScores[i] = (float) (size - 1) / x;
    }

}

int midSum(int num, int size, const int array[size]) {
    int i, sum = 0, count = 0;
    for (i = 1; i < size; i++) {
        count++;
        if (array[i] == num) sum++;
        else if (array[i] == -1) break;
    }
    if (array[count - 1] == num) sum--;
    return sum;
}

void betweennessCentrality(int size, int graph[][size], float betweennessCentralityScore[size],
                           float standarizedScores[size]) {
    int i, j, count = 0;

    int w = 0;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (j > i) {
                count++;
                if (i == 0 || j == 0)
                    w++;
            }
        }
    }

    int path[count][size];
    for (i = 0; i < count; i++) {
        for (j = 0; j < size; j++) {
            path[i][j] = -1;
        }
    }

    count = 0;
    for (i = 0; i < size; i++) {
        betweennessCentralityScore[i] = 0;
        for (j = 0; j < size; j++) {
            if (j > i) {
                dijkstra(size, graph, i, j, count, path);
                count++;
            }
        }
    }

    for (i = 0; i < size; i++)
        for (j = 0; j < count; j++)
            betweennessCentralityScore[i] += midSum(i, size, path[j]);

    for (i = 0; i < size; i++)
        betweennessCentralityScore[i] /= (count - w);

    int st = (size - 1) * (size - 2) / 2;

    for (i = 0; i < size; i++)
        standarizedScores[i] = (betweennessCentralityScore[i] / st);

}

void print(int count, int graph[][count], int degreeCentralityScore[count],
           float degreeCentralityStandardizedScore[count], float closenessCentralityScore[count],
           float closenessCentralityStandardizedScore[count], float betweennessCentralityScore[count],
           float betweennessCentralityStandardizedScore[count]) {
    int i, j;
    printf("Graph is in the following: \n");
    for (i = 0; i < count; i++) {
        for (j = 0; j < count; j++)
            printf("%d\t", graph[i][j]);

        printf("\n");
    }

    printf("\n------------------------\n\n");

    for (i = 0; i < count; i++)
        printf("Degree centrality score of %d: %d\t Standardized version:%f\n", i, degreeCentralityScore[i],
               degreeCentralityStandardizedScore[i]);

    printf("\n------------------------\n\n");


    for (i = 0; i < count; i++)
        printf("Closeness centrality score of %d: %f\t Standardized version:%f\n", i, closenessCentralityScore[i],
               closenessCentralityStandardizedScore[i]);

    printf("\n------------------------\n\n");

    for (i = 0; i < count; i++)
        printf("Betweenness centrality score of %d: %f\t Standardized version:%f\n", i, betweennessCentralityScore[i],
               betweennessCentralityStandardizedScore[i]);

}

int main() {
    setlocale(LC_ALL, "Turkish");

    char *comingStr = fileOpener("C:\\Users\\ben\\Desktop\\data project 3\\input.txt");
    char str[500];
    strcpy(str, comingStr);

    if (str[0] == ' ') {
        int idxToDel = 0;
        memmove(&str[idxToDel], &str[idxToDel + 1], strlen(str) - idxToDel);
    }

    int i, count = 1;
    for (i = 0; i < strlen(str); i++)
        if (str[i] == '\n')
            count++;

    int graph[count][count];
    graphMaker(str, count, graph);

    int degreeCentralityScore[count];
    float degreeCentralityStandardizedScore[count];
    degreeCentrality(count, graph, degreeCentralityScore, degreeCentralityStandardizedScore);

    float closenessCentralityScore[count], closenessCentralityStandardizedScore[count];
    closenessCentrality(count, graph, closenessCentralityScore, closenessCentralityStandardizedScore);

    float betwennesCentralityScore[count], betwennesCentralityStandardizedScore[count];
    betweennessCentrality(count, graph, betwennesCentralityScore, betwennesCentralityStandardizedScore);

    print(count, graph, degreeCentralityScore, degreeCentralityStandardizedScore, closenessCentralityScore,
          closenessCentralityStandardizedScore, betwennesCentralityScore, betwennesCentralityStandardizedScore);

    return 0;
}
