#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

#define MAX_LINHA 2048

typedef struct vertice {
    char* nome;
    int peso;
    struct vertice *prox;
} vertice;

struct grafo {
    char *nome;
    unsigned int num_vertices;
    vertice **lista_adj;
    unsigned int num_arestas;
};

static void adiciona_nome(char *nome, char ***nomes, unsigned int *total, unsigned int *capacidade) 
{
    for (unsigned int j = 0; j < *total; j++) 
    {
        if (strcmp((*nomes)[j], nome) == 0)
            return;
    }

    if (*total == *capacidade) 
    {
        *capacidade *= 2;
        *nomes = realloc(*nomes, (*capacidade) * sizeof(char*));
    }

    (*nomes)[*total] = malloc(strlen(nome) + 1);
    strcpy((*nomes)[*total], nome);
    (*total)++;
}

static int indice_vertice(char *nome, vertice **lista_adj, unsigned int total) 
{
    for (unsigned int i = 0; i < total; i++)
        if (strcmp(lista_adj[i]->nome, nome) == 0)
            return i;

    return -1;
}

static void adiciona_aresta(vertice **lista_adj, int origem, int destino) 
{
    vertice *v = malloc(sizeof(vertice));
    v->nome = lista_adj[destino]->nome;
    v->peso = 0;
    v->prox = lista_adj[origem]->prox;
    lista_adj[origem]->prox = v;
}

grafo *inicializa_grafo() 
{
    grafo *g = malloc(sizeof(grafo));

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
    if (!f) return NULL;

    grafo *g = inicializa_grafo();

    if (!g) 
        return NULL;

    char linha[MAX_LINHA];
    char **nomes = malloc(10 * sizeof(char*));
    unsigned int total = 0, capacidade = 10;
    int leu_nome = 0;

    // Coleta nomes do grafo e vértices
    while (fgets(linha, MAX_LINHA, f)) 
    {
        linha[strcspn(linha, "\n")] = 0;

        // Ignora os comentários e linhas vazias
        if (strlen(linha) == 0 || (linha[0] == '/' && linha[1] == '/')) 
            continue;

        if (!leu_nome) 
        {
            strcpy(g->nome, linha);
            leu_nome = 1;
            continue;
        }

        char *sep = strstr(linha, "--");
        if (sep) 
        {
            *sep = '\0';
            char *p1 = linha;
            char *p2 = sep + 2;
            while (*p2 == ' ') p2++;
            while (*(sep - 1) == ' ' && sep > linha) *(--sep) = '\0';

            adiciona_nome(p1, &nomes, &total, &capacidade);
            adiciona_nome(p2, &nomes, &total, &capacidade);
            g->num_arestas++;
        } 
        else
            adiciona_nome(linha, &nomes, &total, &capacidade);
    }

    g->num_vertices = total;
    g->lista_adj = malloc(total * sizeof(vertice*));
    for (unsigned int i = 0; i < total; i++) 
    {
        g->lista_adj[i] = malloc(sizeof(vertice));
        g->lista_adj[i]->nome = nomes[i];
        g->lista_adj[i]->peso = 0;
        g->lista_adj[i]->prox = NULL;
    }

    rewind(f);
    leu_nome = 0;

    // Conecta os vértices pelas arestas na lista de adjacência
    while (fgets(linha, MAX_LINHA, f)) 
    {
        linha[strcspn(linha, "\n")] = 0;


        if (strlen(linha) == 0 || (linha[0] == '/' && linha[1] == '/')) continue;

        if (!leu_nome) 
        {
            leu_nome = 1;
            continue;
        }

        char *sep = strstr(linha, "--");
        if (sep) 
        {
            *sep = '\0';
            char *p1 = linha;
            char *p2 = sep + 2;
            while (*p2 == ' ') p2++;
            while (*(sep - 1) == ' ' && sep > linha) *(--sep) = '\0';

            int idx1 = indice_vertice(p1, g->lista_adj, total);
            int idx2 = indice_vertice(p2, g->lista_adj, total);

            if (idx1 != -1 && idx2 != -1) 
            {
                // Adiciona duas vezes porque é ida e volta
                // No num_arestas está certo, pois conta apenas uma vez
                adiciona_aresta(g->lista_adj, idx1, idx2);
                adiciona_aresta(g->lista_adj, idx2, idx1);
            }
        }
    }

    free(nomes);
    return g;
}

char *nome(grafo *g) {
    return g->nome;
}

unsigned int n_vertices(grafo *g) {
    return g->num_vertices;
}

unsigned int n_arestas(grafo *g) {
    return g->num_arestas;
}

void imprime_lista_adjacencia(grafo *g) 
{
    printf("\nLista de Adjacência:\n");
    for (unsigned int i = 0; i < g->num_vertices; i++) 
    {
        printf("%s: ", g->lista_adj[i]->nome);
        vertice *atual = g->lista_adj[i]->prox;

        while (atual) 
        {
            printf("%s", atual->nome);
            if (atual->prox)
                printf(", ");
            atual = atual->prox;
        }
        printf("\n");
    }
}

