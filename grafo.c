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

    while (fgets(linha, MAX_LINHA, f) != NULL) 
    {
        // Remove '\n'
        linha[strcspn(linha, "\n")] = 0;

        // Pula as linhas que não importam
        if (strlen(linha) == 0 || (linha[0] == '/' && linha[1] == '/'))
            continue;

        // Pega o nome
        strcpy(g->nome, linha);

        break;
    }
    
    return g;
}

char *nome(grafo *g) 
{
    return g->nome;
}