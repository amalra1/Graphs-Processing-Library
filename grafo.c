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

unsigned int destroi_grafo(grafo *g)
{
    if (!g) 
        return 0;

    // Free da lista de adjacência
    for (unsigned int i = 0; i < g->num_vertices; i++) 
    {
        vertice *atual = g->lista_adj[i];
        
        vertice *prox = atual->prox;
        while (prox) 
        {
            vertice *temp = prox;
            prox = prox->prox;
            free(temp);
        }

        free(atual->nome);
        free(atual);
    }

    free(g->lista_adj);
    free(g->nome);
    free(g);

    return 1;
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

void busca_profundidade(grafo *g, int idx, int *visitado) 
{
    visitado[idx] = 1;
    vertice *v = g->lista_adj[idx]->prox;

    while (v) 
    {
        for (unsigned int i = 0; i < g->num_vertices; i++) 
        {
            if (strcmp(v->nome, g->lista_adj[i]->nome) == 0 && !visitado[i]) 
            {
                busca_profundidade(g, i, visitado);
                break;
            }
        }
        v = v->prox;
    }
}

unsigned int n_componentes(grafo *g) 
{
    if (!g) 
        return 0;

    int *visitado = calloc(g->num_vertices, sizeof(int));
    if (!visitado) 
        return 0;

    unsigned int componentes = 0;

    // Verifica se existe um caminho para todos os vértices a partir de um vértice i
    // Se não houver para algum, quer dizer que ali tem outro componente
    for (unsigned int i = 0; i < g->num_vertices; i++) 
    {
        if (!visitado[i]) 
        {
            componentes++;
            busca_profundidade(g, i, visitado);
        }
    }

    free(visitado);
    return componentes;
}

unsigned int bipartido(grafo *g) 
{
    if (!g) 
        return 0;

    int *cor = malloc(g->num_vertices * sizeof(int));
    if (!cor) 
        return 0;

    // A """cor""" -1 significa não visitado
    for (unsigned int i = 0; i < g->num_vertices; i++)
        cor[i] = -1;

    for (unsigned int i = 0; i < g->num_vertices; i++) 
    {
        if (cor[i] != -1) 
            continue;

        // Faz uma busca em largura a partir de i, para verificar os seus vizinhos
        unsigned int fila_inicio = 0, fila_fim = 0;
        unsigned int *fila = malloc(g->num_vertices * sizeof(unsigned int));

        if (!fila) 
        {
            free(cor);
            return 0;
        }

        fila[fila_fim++] = i;
        cor[i] = 0;

        while (fila_inicio < fila_fim) 
        {
            int atual = fila[fila_inicio++];
            vertice *vizinho = g->lista_adj[atual]->prox;

            while (vizinho) 
            {
                int idx_vizinho = -1;
                for (unsigned int j = 0; j < g->num_vertices; j++) 
                {
                    if (strcmp(g->lista_adj[j]->nome, vizinho->nome) == 0) 
                    {
                        idx_vizinho = j;
                        break;
                    }
                }

                if (idx_vizinho != -1) 
                {
                    if (cor[idx_vizinho] == -1) 
                    {
                        cor[idx_vizinho] = 1 - cor[atual];
                        fila[fila_fim++] = idx_vizinho;
                    }
                    
                    // Se falhar em algum dos casos, já não é bipartido 
                    else if (cor[idx_vizinho] == cor[atual]) 
                    {
                        free(cor);
                        free(fila);
                        return 0;
                    }
                }

                vizinho = vizinho->prox;
            }
        }

        free(fila);
    }

    free(cor);
    return 1;
}