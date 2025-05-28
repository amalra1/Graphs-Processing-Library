#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "grafo.h"

grafo *inicializa_grafo() 
{
    grafo *g = (grafo*)malloc(sizeof(grafo));
    if (!g) {
        perror("Erro ao alocar grafo");
        return NULL;
    }

    g->nome = (char*)malloc(MAX_LINHA);
    g->nomes_vertices = NULL;
    g->num_vertices = 0;
    g->matriz_adj = NULL;
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