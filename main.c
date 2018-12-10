// zadanie3.c -- Tomáš Gloznek, 22.11.2018 04:20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRINCESS 5
#define GEN_OFF 0
#define GEN_ON 1

int ct;
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
    int path;
    int visited;
    int cost;
    char port;
} node;
struct GENERATED_PATHS {
    path *generator;
    path *dragon;
    path *gen_on_dragon;
    path **princess;
    path **gen_on_princess;
};
struct START_POINTS {
    struct GENERATED_PATHS start;
    struct GENERATED_PATHS generator;
    struct GENERATED_PATHS dragon;
    struct GENERATED_PATHS *princess;
} *start_points;

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

node *dmap_init(char **mapa)
{
    node *dmap = (node *)malloc(gn*gm * sizeof(node));
    int index=0;

    for(int i=0; i<MAX_PRINCESS; i++)
        special.princess[i] = -1;
    special.dragon = -1;
    special.generator = -1;

    for(int i=0; i<gn; i++)
    {
        for(int k=0; k<gm; k++, index++)
        {
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
//        printf("\n");
    }
    return dmap;
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
    result->cost = dmap[index].cost;
    result->last = malloc(sizeof(struct NODE_LIST));
    struct NODE_LIST *now = result->last;

    while(1)
    {
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
//// JIXTRA
path *jixtra(node *dmap_init, int start, int end, char g_on)
{
    if(dmap_init == NULL || start < 0 || end < 0)
        return NULL;
    node *dmap = (node *) malloc(gn*gm * sizeof(node));
    memcpy(dmap, dmap_init, gn*gm * sizeof(node));

    int *cost_heap = malloc(gn*gm * sizeof(int));
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
        // TELEPORTY
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
    path *result = get_path(dmap, end);
    free(dmap);
    free(cost_heap);
    return result;
}

struct NODE_LIST *get_root(path *p)
{
    struct NODE_LIST *now = p->last;
    while(1)
    {
        if(now->next)
            now = now->next;
        else
            return now;
    }
}

struct NODE_LIST *copy_node_list(struct NODE_LIST *to_be_copied)
{
    if(to_be_copied == NULL)
        return NULL;
    struct NODE_LIST *result = malloc(sizeof(struct NODE_LIST));
    struct NODE_LIST *now = result;

    while(1)
    {
        now->index = to_be_copied->index;
        if(to_be_copied->next == NULL)
        {
            now->next = NULL;
            break;
        }
        now->next = malloc(sizeof(struct NODE_LIST));
        now = now->next;
        to_be_copied = to_be_copied->next;
    }
    free(now->next);
    now->next = NULL;
    return result;
}

path *connect_paths(path *a, path *b)
{
    path *result = malloc(sizeof(path));
    if(a == NULL)
    {
        result->last = copy_node_list(b->last);
        result->cost = b->cost;
        return result;
    }
    if(b == NULL)
    {
        result->last = copy_node_list(a->last);
        result->cost = a->cost;
        return result;
    }
    struct NODE_LIST *a_root, *b_root;
    a_root = get_root(a);
    b_root = get_root(b);

    result->cost = a->cost + b->cost;
    if(a->last->index == b_root->index)
    {
        b_root->next = a->last->next;
        result->last = copy_node_list(b->last);
        b_root->next = NULL;
    }
    else if(b->last->index == a_root->index)
    {
        a_root->next = b->last->next;
        result->last = copy_node_list(a->last);
        a_root->next = NULL;
    }
    else
        return NULL;

    return result;
}

void dealloc_path(path *to_be_free)
{
    if(to_be_free == NULL)
        return;
    struct NODE_LIST *next, *now = to_be_free->last;
    while(now)
    {
        next = now->next;
        free(now);
        now = next;
    }
    free(to_be_free);
}
//// PATH CREATING
void create_paths(node *dmap)
{
    start_points = malloc(sizeof(struct START_POINTS));
    // from start - GEN OFF
    start_points->start.generator = jixtra(dmap, 0, special.generator, GEN_OFF);
    start_points->start.dragon = jixtra(dmap, 0, special.dragon, GEN_OFF);
    // from dragon - GEN OFF/ON
    start_points->dragon.generator = jixtra(dmap, special.dragon, special.generator, GEN_OFF);
    start_points->dragon.princess = malloc(special.princess_count * sizeof(path *));
    start_points->dragon.gen_on_princess = malloc(special.princess_count * sizeof(path *));
    for(int i=0; i < special.princess_count; i++)
    {
        start_points->dragon.princess[i] = jixtra(dmap, special.dragon, special.princess[i], GEN_OFF);
        start_points->dragon.gen_on_princess[i] = jixtra(dmap, special.dragon, special.princess[i], GEN_ON);
    }
    // from princess - GEN OFF/ON
    start_points->princess = malloc(special.princess_count * sizeof(struct GENERATED_PATHS));
    for(int i=0; i < special.princess_count; i++)
    {
        start_points->princess[i].generator = jixtra(dmap, special.princess[i], special.generator, GEN_OFF);
        start_points->princess[i].princess = malloc(special.princess_count * sizeof(path *));
        start_points->princess[i].gen_on_princess = malloc(special.princess_count * sizeof(path *));
        for(int k=0; k < special.princess_count; k++)
        {
            start_points->princess[i].princess[k] = jixtra(dmap, special.princess[i], special.princess[k], GEN_OFF);
            start_points->princess[i].gen_on_princess[k] = jixtra(dmap, special.princess[i], special.princess[k], GEN_ON);
        }
    }
    // from generator - GEN ON
    start_points->generator.gen_on_dragon = jixtra(dmap, special.generator, special.dragon, GEN_ON);
    start_points->generator.gen_on_princess = malloc(special.princess_count * sizeof(path *));
    for(int i=0; i < special.princess_count; i++)
        start_points->generator.gen_on_princess[i] = jixtra(dmap, special.generator, special.princess[i], GEN_ON);
}
//// MAIN PATH FINDER
path *rescue_plan(struct SPECIAL to_be_done)
{
    path *this, *from_rescue;
    if(to_be_done.dragon)
    {
        if(to_be_done.generator)
        {
            to_be_done.dragon = 0;
            this = start_points->start.dragon;
            from_rescue = rescue_plan(to_be_done);
            path *gen_off = this == NULL ? NULL : connect_paths(this, from_rescue);
            dealloc_path(from_rescue);

            to_be_done.dragon = 1;
            to_be_done.generator = 0;
            this = start_points->start.generator;
            from_rescue = rescue_plan(to_be_done);
            path *gen_on = this == NULL ? NULL : connect_paths(this, from_rescue);
            dealloc_path(from_rescue);

            if(gen_off == NULL)
                return gen_on;
            else if (gen_on == NULL)
                return gen_off;

            if(gen_on->cost > gen_off->cost)
            {
                dealloc_path(gen_on);
                return gen_off;
            } else
            {
                dealloc_path(gen_off);
                return gen_on;
            }
        }
        else
        {
            to_be_done.dragon = 0;
            this = start_points->generator.gen_on_dragon;
            from_rescue = rescue_plan(to_be_done);
            this = this == NULL ? NULL : connect_paths(this, from_rescue);
            dealloc_path(from_rescue);
            return this;
        }
    }
    int princess_quantity = 0, all_princess = special.princess_count;
    for(int i=0; i < all_princess; i++)
        if(to_be_done.princess[i] == 1)
            princess_quantity++;

    if(princess_quantity == 0)
        return NULL;

    struct GENERATED_PATHS *princess_pointer;
    if(to_be_done.princess_count == -1)
        princess_pointer = &start_points->dragon;
    else if(to_be_done.princess_count == -2)
        princess_pointer = &start_points->generator;
    else
        princess_pointer = &start_points->princess[to_be_done.princess_count];

    if(to_be_done.generator)
    {
        path **to_compare = malloc((all_princess + 1) * sizeof(path *));
        for(int i=0; i < all_princess; i++)
        {
            if(to_be_done.princess[i] == 0)
            {
                to_compare[i] = NULL;
                continue;
            }
            struct SPECIAL this_request = to_be_done;
            this_request.princess[i] = 0;
            this_request.princess_count = i;

            this = princess_pointer->princess[i];
            from_rescue = rescue_plan(this_request);
            to_compare[i] = this == NULL ? NULL : connect_paths(this, from_rescue);
            dealloc_path(from_rescue);
        }
        to_be_done.generator = 0;
        to_be_done.princess_count = -2;

        this = princess_pointer->generator;
        from_rescue = rescue_plan(to_be_done);
        to_compare[all_princess] = this == NULL ? NULL : connect_paths(this, from_rescue);
        dealloc_path(from_rescue);

        this = to_compare[all_princess];
        for(int i=0; i < all_princess; i++)
            if(this == NULL || (to_compare[i] != NULL && to_compare[i]->cost < this->cost))
                this = to_compare[i];

        for(int i=0; i < all_princess + 1; i++)
            if(to_compare[i] != this)
                dealloc_path(to_compare[i]);
        free(to_compare);
        return this;
    } else
    {
        path **to_compare = malloc((all_princess) * sizeof(path *));
        for(int i=0; i < all_princess; i++)
        {
            if(to_be_done.princess[i] == 0)
            {
                to_compare[i] = NULL;
                continue;
            }
            struct SPECIAL this_request = to_be_done;
            this_request.princess[i] = 0;
            this_request.princess_count = i;

            this = princess_pointer->gen_on_princess[i];
            from_rescue = rescue_plan(this_request);
            to_compare[i] = this == NULL ? NULL : connect_paths(this, from_rescue);
            dealloc_path(from_rescue);
        }
        this = to_compare[0];
        for(int i=1; i < all_princess; i++)
            if(this == NULL || (to_compare[i] != NULL && to_compare[i]->cost < this->cost))
                this = to_compare[i];

        for(int i=0; i < all_princess; i++)
            if(to_compare[i] != this)
                dealloc_path(to_compare[i]);
        free(to_compare);
        return this;
    }
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *path_length)
{
    gn = n, gm = m;
    node *dmap = dmap_init(mapa);

    struct SPECIAL to_be_done;
    to_be_done.generator = 1;
    to_be_done.dragon = 1;
    to_be_done.princess_count = -1;
    for(int i=0; i<special.princess_count; i++)
        to_be_done.princess[i] = 1;

    create_paths(dmap);
    path *result = rescue_plan(to_be_done);

    if(result)
    {
//        printf("\nafter jixtra: \ncena: %d\ncesta: ", result->cost);
        if(path_length)
            *path_length = 0;
        else
            return NULL;
        struct NODE_LIST *now = result->last;
        while(now)
        {
//            printf("%d ", now->index);
            now = now->next;
            (*path_length)++;
        }
        int *final = malloc(*path_length * 2 * sizeof(int));
        now = result->last;
        for(int i=(*path_length*2)-1; i>=0; i--, now = now->next)
        {
            final[i] = now->index/m;
            final[--i] = now->index%m;
        }
        return final;
    }

    free(dmap);
    return NULL;
}

int main()
{
    int x,y, *path_length = malloc(sizeof(int));
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
    int *final = zachran_princezne(mapa, x, y, 0, path_length);
    printf("\n\n");
    for(int i=0; i<*path_length; i++)
        printf("%d,%d  ", final[(i*2)+1], final[i*2]);
    printf("\n");
    return 0;
}
/*
0707HGHPCDCC0CCHPPHHHHHCHHHHHHHC0CCCHCH0CHCH1CCPCCCHH
0530CCHHHHCCHHHCHCCCHHHCHHHHCHPCHCHHPCHCHCHCCHCHCHHCCHHHCCCHCCHHHHCHCCCCHHHHHCCHCCCHHCCCHHHCCHHHHCHCCCHHCCCHDCCCHCHCHCHCCHCCCHHCCCHCHCCHCHCPHCHCHHCCHCCHHC
3030CCCCHHHCCHCCHHCCCCCCCCPCHHCCCCHCCCGH0CCCHCHCHCHCHCCCHHHHCCCHCHCHCHHCCCHCCHHCCHCHCCCCCCHCCCHHHHCCCCHHCHCCCHHCCCCHCCHCCHHCCCHCCCCHCHCCHHHCCHCCCCCCCCCHCHCCHCCCCHCHHCCCCHCHCHHCCCCCCCHCCCCHCHCHCCHCCHCCCCHCCHHHHHHPHCCCCCCC1HCHCHCCHCCHCHCCCCHHHCCCCHHCHHHHCHHHCCHHC1HHHHHCHHCHHHCHHCCCCCCCHCHHCHCCPCHCCHCHHCHHCHCCCHCCCCHCHHCHCCC0CCCCCCCCHCCCCCHCCHHCCCCCCHHHCCCHCCHCCCHCCHCHCHHCHHHCCHCHHCHHHCCHCHCHCCCCHCCCCHCCCCCCHCCCCHHHCCCHCCHHCCCHHHCCHHCCCCCCHCCHHHHCCH0CCHHHCHHHCCDHHHHCHCCHHCCCC0CHHHCCHHHCCCCCCHCHHHCCC1CCCCCCCCCCHCCCHCHCCCCCHCCCHHCHHCHCCCHHCCCHCCCHHHHCCCCCCHCHCHCCHCCCCCHCCCCHHCHCCCCCHCHCCHHCCHCCCCCCHCHCCCCHHHHCCHCCHCHCHCCHHCCCHHCHHCHHHHCHCCHCHCCHCCCHCCHCHHCCHCCCHHCCCHCHCHHCHCCCCCCCCCCHCCCHCCCCCCCCHHHHHHHCHCCCCHCCCCCCCHHCCCHCHHCCCCCPHHCHCCHCHCCCCHHCHCCCHHHHCCCCCCCCCHCHHCCHCCCHHCHHHHCHCCHHHHCHHHCHHCHCHCHHCHHHHCCCHHCCCCCHHHCCHHCCCHHHCHCCCCCCCHHCCCC0HCHCCHCCHHCCCCCCCHPHCCCCHHCCCHCHCCCCCHCCCHCCHCCCHCCC
8020HCCCHCCHHHHHHHCCHCCCCCCCHCHHCHHCHHCHCCHCCCCHCHHHCHCCCHHHCHHHHHCHCC0HCHCHCCHHHCCHHHHHHHCCHHCHCCHHCHHCCHCCCHHHHHHCHHHCCCCDCCCCHCCCCHHHCHCCCCHCHCHCHHHHHCHCHCCCCCHCHCHHHHCHHCCHHHCCCCHHCCCHHCHHHHHCCCCCCCHCHHHCCHHHHHCHHHHCHHCCCHHCCHCCHHHCHHCCCHCCCCHCHCCC0HCCCCHHHCCCHHHHHCHCCHCHHHHHHCCHHCCCHCCHHCHCCCHHHCHHHCCCCHCHCCHCCHHCHCHHCCHCHHCCHCCCHHCCHHCCCCHCCCCCCHHCHCHCCCHCHCHHCCHHHHCHHCHHHHHCCCHCHHCCCHCHCCHHHCHCCCCCHHHHCCCCCPCCHCCCHCHHHCCHHHCCHCCHHHCHCCHHHHCHCCHHCCHCCCCCCHCHCCHHHHCCCCHHCHHCHCHHCGCCHCCHCHCH11HCHHHCHCHCHCHHHCCHCCHCCHHCHCCHCHCHHCHC1CHCHCCHH1CHHHHCHHHCHCCHCCCHHHCCHHHHCHHCCCHHCHCHCCHHHCHCCHCCCHCCHHCCHHCHHCHCC1HHHHHHHHHHCCCHHCHHCCCCCHCCHCHHCCHCCHHHCHHHCCCCHHHHHCHCHCCHCCHCCCCCHCHHCCCCCHCCCHHCHHHCHCHHCCCCCCHCHHCHCC0CHHHCHHCHCCCHHHHHCHHHCCCHCCCCHHHHCHHHCCHHCHCHHHHHCHCCCHHCHHCCHCCCCHHCHCCHCCCCCHHCHHCCHCHCCCHHHHCHCHCHCCHCHCHCHHCCCHHCCHCCHHCHCCCCHHHHHPHHHHHHHHCCHCHCHCCHCCCHCCHCCHCHHCCCHCCCCCCHHHCHHHCHHHHHCH1HHCHCCCHCHCHHCHCHPCCCHHHCCHCHCCHCCHHCHHCHCCCHHCHCHCCHCHCHCHCHHHCCCHHCH0CCH0H10HCHHHCHHCHCHHCHCHHHHHHCCHHHHHHCHHCCCCHHCHHHHHHCCHH0CCHCHCHHCCCHHCHH0HCHHCHHCHHCCCCHCCCHHCHHCHHCCCHHHHCHCCCCHHCHHHCHHHC1CCCHHCHCCCHHCCCCHHCCCHHCHHCCCHHHHCCCCHCHCHHCHCHHCHCCHCCCCHCCCCCCCHHHHCCHHHHCHHHHCHCHHCCCCCHHCCHHCHHHHHCCHHHHCHCCCHPCHCHHCHHCHCCCCHCHCHCHCHCHHHHHCHCHHCHHCHHHHCH0HHHHHHHHHCCCCCCHCCCHCHHCHHHCHHHCCCCCHCCCHCCHC10CCHCHHHCHHHHCHCHHCHCHCCHHCCHCHCHHCHHHHHCCCCHCCCHHHCCCHHCCCHHHCHHHHHHHCCHCHHCCHHCHHHHHHHCCCHCHHCHCHHCCCHHHCCHHCHCHCCHCCHCHCHHHHCHCHCHCHCCCHHHHCHCCHCHCHCCHHHCHCCHHHCCHHHHHCCHHCHHCCCHHHHHCHHCCHCPHCHCHHCHHHCHCHCHHHHHHHHCHCCCCHCHHHHCCCCHHHCHCCHCHCHHHHCCCCCHHCCCHCCCHHHHCCHHCHCHCHCHCCHCHCCHH
*/
