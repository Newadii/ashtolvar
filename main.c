// zadanie3.c -- Tomáš Gloznek, 22.11.2018 17:34

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

char gn, gm;

typedef struct NODE {
    char up;
    char down;
    char left;
    char right;
    char dirs[4];
    char path;
    int visited;
    int cost;
} node;

void heap_insert(node* dmap, int *cost_heap, int index)
{
    int last = cost_heap[0];
    cost_heap[last] = index;

    while ( dmap[cost_heap[last]].cost < dmap[cost_heap[last/2]].cost && last > 1)
    {
           cost_heap[last] = cost_heap[last/2];
           cost_heap[last/2] = index;
           last = last/2;
    }
    cost_heap[0]++;
}

char node_cost(char in)
{
    switch(in) {
        case 'H':
            return 2;
        case 'N':
            return -1;
        default:
            return 1;
    }
}

int dest(int start, char dir)
{
    switch(dir)
    {
        case 0:
            return start - gm;
        case 1:
            return start + gm;
        case 2:
            return start - 1;
        case 3:
            return start + 1;
    }
}



void jixtra(node *dmap, int start, int end)
{
    dmap[start].cost = 0;
    while(dmap[end].visited)
    {
        dmap[start].visited = 0;
        for(int dir=0; dir < 4; dir++)
        {
            if(dmap[start].dirs[dir] != -1)
                if(dmap[dest(start, dir)].visited &&
                        (dmap[dest(start, dir)].cost == -1 || dmap[start].cost + dmap[start].dirs[dir] < dmap[dest(start, dir)].cost))
                    dmap[dest(start, dir)].cost = dmap[start].cost + dmap[start].dirs[dir];

        }

    }
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty)
{
    gn = n, gm = m;
    node *dmap = malloc(n*m * sizeof(node));
    int nm = n*m;

    for(int i=0, index=0; i<n; i++)
    {
        for(int k=0; k<m; k++, index++)
        {
            dmap[index].dirs[0] = i>0 ? node_cost(mapa[i-1][k]) : -1;
            dmap[index].dirs[1] = i+1<n ? node_cost(mapa[i+1][k]) : -1;
            dmap[index].dirs[2] = k>0 ? node_cost(mapa[i][k-1]) : -1;
            dmap[index].dirs[3] = k+1<m ? node_cost(mapa[i][k+1]) : -1;
            dmap[index].cost = -1;
            dmap[index].visited = -1;
            dmap[index].path = -1;

            if(k + 1 == m)
                printf("%c", mapa[i][k]);
            else
                printf("%c-", mapa[i][k]);
        }
        printf("\n");
    }
    for(int i=0; i<nm; i++)
    {
        printf("%d %d %d %d\n", dmap[i].up, dmap[i].down, dmap[i].left, dmap[i].right);
    }
}

int main()
{
    int x,y;
    x = (getchar() - '0') * 10;
    x += getchar() - '0';
    y = (getchar() - '0') * 10;
    y += getchar() - '0';
    printf("%d, %d\n", x, y);
    char **mapa = malloc(x * sizeof(int *));
    for(int i = 0; i < x; ++i)
        mapa[i] = malloc(y * sizeof(int));

    for(int j = 0; j < x; ++j)
    {
        for(int i = 0; i < y; ++i)
        {
            mapa[j][i] = (char)getchar();
        }
    }
    zachran_princezne(mapa, x, y, 0, NULL);
}