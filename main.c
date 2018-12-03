// zadanie3.c -- Tomáš Gloznek, 22.11.2018 17:34

#include <stdio.h>
#include <stdlib.h>

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty)
{
    for(int i=0; i<n; i++)
    {
        for(int k=0; k<m; k++)
            if(k+1 == m)
                printf("%c",mapa[i][k]);
            else
                printf("%c-",mapa[i][k]);
        printf("\n");
    }
}

// Vlastna funkcia main() je pre vase osobne testovanie. Dolezite: pri testovacich scenaroch sa nebude spustat!
int main()
{
    return 0;
} 