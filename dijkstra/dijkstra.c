// 61911650 高野遥斗
#define NNODE 6
#define INF 100 // infinity
#include <stdio.h>
int cost[NNODE][NNODE] = {
{ 0, 2, 5, 1, INF, INF},
{ 2, 0, 3, 2, INF, INF},
{ 5, 3, 0, 3, 1, 5},
{ 1, 2, 3, 0, 1, INF},
{INF, INF, 1, 1, 0, 2},
{INF, INF, 5, INF, 2, 0}
};

void printMinimumCost(int u, int d[NNODE],int p[NNODE]);
void do_dijkstra(int u);
int main() {

    for(int u=0;u<NNODE;u++){
        do_dijkstra(u);
    }
    return 0;
}

void do_dijkstra(int u) {
    // ノード u から他の全てのノード v への経路の計算

    int mincost[NNODE] = {0}; // 始点から終点 v までの反復計算における現在の最小コストの値
    int prev[NNODE] = {0}; // 始点から終点 v への現在の最小コスト経路上における v の一つ前のノード
    int N[NNODE] = {0}; // 最小コスト経路が最終的にわかっているノードの集合
    int Nnum = 1; // 最小コスト経路が最終的にわかっているノードの数
    int v;

    // Initialization
    N[u] = 1;
    for(v=0;v<NNODE;v++){
        if (cost[u][v] < INF) {
            mincost[v] = cost[u][v];
            prev[v] = u;
        } else {
            mincost[v] = INF;
        }
    }

    // Loop
    while(Nnum < NNODE){
        int w = 0;
        // find w not in N such that dest is a minimum
        for(v=0;v<NNODE;v++){
            if (N[w] != 0) {
                w = v;
            } else {
                if (N[v] == 0 && mincost[v] < mincost[w]) {
                    w = v;
                }
            }
        }
        N[w] = 1; // add w to N
        //update mincost for each neighbor v of w and not in N
        for(v=0;v<NNODE;v++){
            if(N[v] == 0 && mincost[v] > mincost[w] + cost[w][v]) {
                mincost[v] = mincost[w] + cost[w][v];
                prev[v] = w;
            }
        }
        Nnum++;
    }
    printMinimumCost(u,mincost,prev);
}

void printMinimumCost(int u, int d[NNODE], int p[NNODE]) {
    int v;

    printf("root node %c:\n  ", 'A'+u);
    for(v=0;v<NNODE;v++){
        if (v>0){
            printf(" ");    
        }
        // [終点ノード,直前ノード,コスト]
        printf("[%c, %c, %d]",'A'+v,'A'+p[v],d[v]);
    }
    printf("\n");
}