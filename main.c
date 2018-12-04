// zadanie3.c -- Tomáš Gloznek, 22.11.2018 17:34

#include <stdio.h>
#include <stdlib.h>

typedef struct NODE {
    char up;
    char down;
    char left;
    char right;
    char visited;
    char cost;
    char path;
} node;

enum direction {Up, Down, Left, Right};

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

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty)
{
    node *dmap = malloc(n*m * sizeof(node));
    int nm = n*m;

    for(int i=0, index=0; i<n; i++)
    {
        for(int k=0; k<m; k++, index++)
        {
            dmap[index].up = i>0 ? node_cost(mapa[i-1][k]) : -1;
            dmap[index].down = i+1<n ? node_cost(mapa[i+1][k]) : -1;
            dmap[index].left = k>0 ? node_cost(mapa[i][k-1]) : -1;
            dmap[index].right = k+1<m ? node_cost(mapa[i][k+1]) : -1;
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

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
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