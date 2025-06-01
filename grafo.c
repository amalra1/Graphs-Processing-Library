#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "grafo.h"

#define MAX_LINHA 2048

typedef struct vertice 
{
    char* nome;           
    int peso;                  
    struct vertice *prox;
} vertice;

struct grafo 
{
    char *nome;
    unsigned int num_vertices;
    vertice **lista_adj;
    unsigned int num_arestas;
};

grafo *inicializa_grafo() 
{
    grafo *g = (grafo*)malloc(sizeof(grafo));

    if (!g) 
    {
        perror("Erro ao alocar grafo");
        return NULL;
    }

    g->nome = malloc(MAX_LINHA);
    g->num_vertices = 0;
    g->lista_adj = NULL;
    g->num_arestas = 0;

    return g;
}

grafo *le_grafo(FILE *f)
{
    if (!f) 
        return NULL;
    
    grafo *g = inicializa_grafo();
    if (!g) 
        return NULL;

    char linha[MAX_LINHA];
    char **nomes = NULL;
    unsigned int total = 0, capacidade = 10;
    int leu_nome = 0;

    nomes = malloc(capacidade * sizeof(char*));
    if (!nomes) return NULL;

    while (fgets(linha, MAX_LINHA, f) != NULL) 
    {
        linha[strcspn(linha, "\n")] = 0;

        if (strlen(linha) == 0 || (linha[0] == '/' && linha[1] == '/'))
            continue;

        if (!leu_nome) 
        {
            strcpy(g->nome, linha);
            leu_nome = 1;
            continue;
        }

        // Checa se é uma aresta 
        char *ponta1 = NULL, *ponta2 = NULL;
        int peso = 0;
        char *sep = strstr(linha, "--");

        if (sep)
        {
            *sep = '\0';
            ponta1 = linha;
            ponta2 = sep + 2;

            while (*ponta2 == ' ') ponta2++;
            while (*(sep - 1) == ' ' && sep > linha) *(--sep) = '\0';

            // Lógica do peso -- FAZER DEPOIS
            // char *peso_str = strrchr(ponta2, ' ');
            // if (peso_str) {
            //     peso = atoi(peso_str + 1);
            //     *peso_str = '\0'; // Remove o peso da string do nome
            // }

            // Adiciona os dois vértices na estrutura
            char *vertices[2] = {ponta1, ponta2};
            for (int i = 0; i < 2; i++) 
            {
                int existe = 0;
                for (unsigned int j = 0; j < total; j++) 
                {
                    if (strcmp(nomes[j], vertices[i]) == 0)
                    {
                        existe = 1;
                        break;
                    }
                }
                if (!existe) 
                {
                    if (total == capacidade) 
                    {
                        capacidade *= 2;
                        nomes = realloc(nomes, capacidade * sizeof(char*));
                    }

                    nomes[total] = malloc(strlen(vertices[i]) + 1);
                    strcpy(nomes[total], vertices[i]);
                    total++;
                }
            }

            g->num_arestas++;
        } 
        else 
        {
            // Vértice isolado 
            int existe = 0;
            for (unsigned int j = 0; j < total; j++) 
            {
                if (strcmp(nomes[j], linha) == 0)
                {
                    existe = 1;
                    break;
                }
            }
            if (!existe) 
            {
                if (total == capacidade) 
                {
                    capacidade *= 2;
                    nomes = realloc(nomes, capacidade * sizeof(char*));
                }

                nomes[total] = malloc(strlen(linha) + 1);
                strcpy(nomes[total], linha);
                total++;
            }
        }
    }

    g->num_vertices = total;

    for (unsigned int i = 0; i < total; i++)
        free(nomes[i]);
    free(nomes);

    return g;
}

char *nome(grafo *g) 
{
    return g->nome;
}

unsigned int n_vertices(grafo *g)
{
    return g->num_vertices;
}

unsigned int n_arestas(grafo *g)
{
    return g->num_arestas;
}