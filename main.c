// zadanie3.c -- Tomáš Gloznek, 22.11.2018 17:34

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_PRINCESS 5

char gn, gm;
struct NODE_LIST {
    int index;
    struct NODE_LIST *next;
} *ports[10];
struct SPECIAL {
    int dragon;
    int princess[MAX_PRINCESS];
    int princess_count;
    int generator;
} special;
typedef struct PATH {
    int cost;
    struct NODE_LIST *last;
} path;
typedef struct NODE {
    char dirs[4];
    char path;
    int visited;
    int cost;
    char port;
} node;

void add_special(node *dmap, int index, char type)
{
    if(type < 'A')
    {
        int port_num = type - '0';
        dmap[index].port = port_num;

        if(ports[port_num] == NULL)
        {
            ports[port_num] = malloc(sizeof(struct NODE_LIST));
            ports[port_num]->index = index;
            ports[port_num]->next = NULL;
            return;
        }
        struct NODE_LIST *now = ports[port_num];
        while(now->next != NULL)
            now = now->next;
        now->next = malloc(sizeof(struct NODE_LIST));
        now->next->index = index;
        now->next->next = NULL;
        return;
    }
    switch(type)
    {
        case 'P':
            if(special.princess_count == MAX_PRINCESS)
            { printf("ERROR\n"); return; }
            special.princess[special.princess_count] = index;
            special.princess_count++;
            return;
        case 'D':
            special.dragon = index;
            return;
        case 'G':
            special.generator = index;
            return;
        default:
            return;
    }
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
        default:
            return -1;
    }
}

void heap_insert(node *dmap, int *cost_heap, int index)
{
    int last = cost_heap[0];
    cost_heap[last] = index;

    while (last > 1 && dmap[cost_heap[last]].cost < dmap[cost_heap[last/2]].cost)
    {
           cost_heap[last] = cost_heap[last/2];
           cost_heap[last/2] = index;
           last = last/2;
    }
    cost_heap[0]++;
}

void heap_check(node *dmap, int *cost_heap, int index)
{
    int last = cost_heap[0] - 1;

    while(last > 1 && cost_heap[last] != index)
        last--;

    while (last > 1 && dmap[cost_heap[last]].cost < dmap[cost_heap[last/2]].cost)
    {
        cost_heap[last] = cost_heap[last/2];
        cost_heap[last/2] = index;
        last = last/2;
    }
}

int heap_extract(node *dmap, int *cost_heap)
{
    int result = cost_heap[1];
    cost_heap[0]--;
    cost_heap[1] = cost_heap[ cost_heap[0] ];
    int index, now = 1;

    while(  ( now*2   < cost_heap[0] && dmap[cost_heap[now]].cost > dmap[ cost_heap[now*2]  ].cost) ||
            ( now*2+1 < cost_heap[0] && dmap[cost_heap[now]].cost > dmap[ cost_heap[now*2+1]].cost) )
    {
        index = cost_heap[now];
        if(dmap[ cost_heap[now*2] ].cost < dmap[ cost_heap[now*2+1] ].cost)
        {
            cost_heap[now] = cost_heap[now*2];
            cost_heap[now*2] = index;
            now = now*2;
        } else
        {
            cost_heap[now] = cost_heap[now*2+1];
            cost_heap[now*2+1] = index;
            now = now*2+1;
        }
    }
    return result;
}

path *get_path(node *dmap, int index)
{
    path *result = malloc(sizeof(path));
    result->cost = 0;
    result->last = malloc(sizeof(struct NODE_LIST));
    struct NODE_LIST *now = result->last;

    while(1)
    {
        result->cost++;
        now->index = index;
        index = dmap[index].path;
        if(index == -1)
        {
            now->next = NULL;
            break;
        }
        now->next = malloc(sizeof(struct NODE_LIST));
        now = now->next;
    }
    return result;
}

path *jixtra(node *dmap, int start, int end, char g_on)
{
    int *cost_heap = malloc(gn*gm * sizeof(char));
    cost_heap[0] = 1;

    if(dmap[start].cost == -1)
        dmap[start].cost = 0;
    while(dmap[end].visited)
    {
        dmap[start].visited = 0;
        for(int dir=0; dir < 4; dir++)
        {
            if(dmap[start].dirs[dir] != -1 && dmap[dest(start, dir)].visited)
            {
                if(dmap[dest(start, dir)].cost == -1)
                {
                    dmap[dest(start, dir)].cost = dmap[start].cost + dmap[start].dirs[dir];
                    dmap[dest(start, dir)].path = start;
                    heap_insert(dmap, cost_heap, dest(start, dir));
                } else if(dmap[start].cost + dmap[start].dirs[dir] < dmap[dest(start, dir)].cost)
                {
                    dmap[dest(start, dir)].cost = dmap[start].cost + dmap[start].dirs[dir];
                    dmap[dest(start, dir)].path = start;
                    heap_check(dmap, cost_heap, dest(start, dir));
                }
            }
        }
        if(g_on && dmap[start].port != -1)
        {
            struct NODE_LIST *now = ports[dmap[start].port];
            while(now)
            {
                if(dmap[now->index].visited)
                {
                    if(dmap[now->index].cost == -1)
                    {
                        dmap[now->index].cost = dmap[start].cost;
                        dmap[now->index].path = start;
                        heap_insert(dmap, cost_heap, now->index);
                    } else if(dmap[start].cost < dmap[now->index].cost)
                    {
                        dmap[now->index].cost = dmap[start].cost;
                        dmap[now->index].path = start;
                        heap_check(dmap, cost_heap, now->index);
                    }
                }
                now = now->next;
            }
        }
        start = heap_extract(dmap, cost_heap);
        if(cost_heap[0] == 0)
            return NULL;
    }
    return get_path(dmap, end);
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty)
{
    gn = n, gm = m;
    node *dmap = malloc(n*m * sizeof(node));
    int nm = n*m, index=0;

    for(int i=0; i<MAX_PRINCESS; i++)
        special.princess[i] = -1;
    special.dragon = -1;

    for(int i=0; i<n; i++)
    {
        for(int k=0; k<m; k++, index++)
        {
            if(k + 1 == m)
                printf("%02d:%c", index, mapa[i][k]);
            else
                printf("%02d:%c  ", index, mapa[i][k]);

            if(mapa[i][k] == 'N')
                continue;

            dmap[index].dirs[0] = i>0 ? node_cost(mapa[i-1][k]) : -1;
            dmap[index].dirs[1] = i+1<gn ? node_cost(mapa[i+1][k]) : -1;
            dmap[index].dirs[2] = k>0 ? node_cost(mapa[i][k-1]) : -1;
            dmap[index].dirs[3] = k+1<gm ? node_cost(mapa[i][k+1]) : -1;
            dmap[index].port = -1;
            dmap[index].cost = -1;
            dmap[index].visited = -1;
            dmap[index].path = -1;

            if(mapa[i][k] != 'H' && mapa[i][k] != 'C')
                add_special(dmap, index, mapa[i][k]);
        }
        printf("\n");
    }
//    for(int i=0; i<nm; i++)
//    {
//        printf("(%d) %d %d %d %d  t:%d \t v:%d c:%d p:%d\n", i, dmap[i].dirs[0], dmap[i].dirs[1], dmap[i].dirs[2], dmap[i].dirs[3], dmap[i].port, dmap[i].visited, dmap[i].cost, dmap[i].path);
//    }
/*
    printf("\n");
    for(int i=0; i<10; i++)
    {
        printf("%d: ", i);
        struct NODE_LIST *now = ports[i];
        while(now != NULL)
        {
            printf("%d ", now->index);
            now = now->next;
        }
        printf("\n");
    }
*/
    path *cesta;
    cesta = jixtra(dmap, 0, special.princess[3], 1);
    if(cesta == NULL)
    {
        printf("nemozna cesta\n");
        return NULL;
    }
    printf("\nafter jixtra: \ncena: %d\ncesta: ", cesta->cost);
    struct NODE_LIST *now = cesta->last;
    while(now)
    {
        printf("%d ", now->index);
        now = now->next;
    }
    printf("\n\n");
//    for(int i=0; i<index; i++)
//    {
//        printf("(%d) \t%d %d %d %d \t v:%d c:%d p:%d\n", i, dmap[i].dirs[0], dmap[i].dirs[1], dmap[i].dirs[2], dmap[i].dirs[3], dmap[i].visited, dmap[i].cost, dmap[i].path);
//    }
    return 0;
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