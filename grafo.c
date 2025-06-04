#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
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

static void adiciona_aresta_com_peso(vertice **lista_adj, int origem, int destino, int peso) 
{
    vertice *v = malloc(sizeof(vertice));
    v->nome = lista_adj[destino]->nome;
    v->peso = peso;
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

// Função auxiliar para extrair nomes dos vértices e peso da linha de aresta
// Exemplo de linha: "um -- dois 4"
// Após a execução:
//   *v1 = "um"
//   *v2 = "dois"
//   *peso = 4 (se não tiver peso, 0)
// A função altera a linha original para inserir '\0' nos lugares certos
static void parse_aresta(char *linha, char **v1, char **v2, int *peso) {
    char *sep = strstr(linha, "--");
    if (!sep) {
        *v1 = linha;
        *v2 = NULL;
        *peso = 0;
        return;
    }

    *sep = '\0';
    *v1 = linha;
    *v2 = sep + 2;

    // Remove espaços extras no início de v2
    while (**v2 == ' ') (*v2)++;

    // Remove espaços à direita de v1
    while (*(sep - 1) == ' ' && sep > linha) *(--sep) = '\0';

    size_t len = strlen(*v2);
    while (len > 0 && (*v2)[len - 1] == ' ') {
        (*v2)[len - 1] = '\0';
        len--;
    }

    // Procura por peso no final de v2
    char *peso_str = strrchr(*v2, ' ');
    *peso = 1;
    if (peso_str) {
        *peso_str = '\0';  // Termina a string antes do peso
        *peso = atoi(peso_str + 1);  // Converte o peso para inteiro
    }
}

// Função principal para ler o grafo
grafo *le_grafo(FILE *f) {
    if (!f) return NULL;

    grafo *g = inicializa_grafo();
    if (!g) return NULL;

    char linha[MAX_LINHA];
    char **nomes = malloc(10 * sizeof(char*));
    unsigned int total = 0, capacidade = 10;
    int leu_nome = 0;

    // Primeiro loop: coleta nomes dos vértices (sem peso)
    while (fgets(linha, MAX_LINHA, f)) {
        linha[strcspn(linha, "\n")] = 0;  // Remove '\n'

        if (strlen(linha) == 0 || (linha[0] == '/' && linha[1] == '/')) 
            continue;

        if (!leu_nome) {
            strcpy(g->nome, linha);
            leu_nome = 1;
            continue;
        }

        char *v1, *v2;
        int peso;
        parse_aresta(linha, &v1, &v2, &peso);

        adiciona_nome(v1, &nomes, &total, &capacidade);
        if (v2) adiciona_nome(v2, &nomes, &total, &capacidade);

        if (v2) g->num_arestas++;  // Só conta se é aresta, não vértice isolado
    }

    g->num_vertices = total;
    g->lista_adj = malloc(total * sizeof(vertice*));
    for (unsigned int i = 0; i < total; i++) {
        g->lista_adj[i] = malloc(sizeof(vertice));
        g->lista_adj[i]->nome = nomes[i];
        g->lista_adj[i]->peso = 0;
        g->lista_adj[i]->prox = NULL;
    }

    rewind(f);
    leu_nome = 0;

    // Segundo loop: conecta vértices pelas arestas (com peso)
    while (fgets(linha, MAX_LINHA, f)) {
        linha[strcspn(linha, "\n")] = 0;

        if (strlen(linha) == 0 || (linha[0] == '/' && linha[1] == '/')) 
            continue;

        if (!leu_nome) {
            leu_nome = 1;
            continue;
        }

        char *v1, *v2;
        int peso;
        parse_aresta(linha, &v1, &v2, &peso);

        if (v2) {
            int idx1 = indice_vertice(v1, g->lista_adj, total);
            int idx2 = indice_vertice(v2, g->lista_adj, total);

            if (idx1 != -1 && idx2 != -1) {
                adiciona_aresta_com_peso(g->lista_adj, idx1, idx2, peso);
                adiciona_aresta_com_peso(g->lista_adj, idx2, idx1, peso);
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

int compara_inteiros(const void *a, const void *b) {
    return (*(int*)a) - (*(int*)b);
}

// Retorna o menor caminho entre dois vértices
int* dijkstra(grafo *g, int origem) {
    int n = g->num_vertices;
    int *dist = malloc(n * sizeof(int));
    int *visitado = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
    }
    dist[origem] = 0;

    for (int count = 0; count < n - 1; count++) {
        // Escolhe o vértice não visitado com menor distância
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (!visitado[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        if (dist[u] == INT_MAX)
            break;  // Não há mais vértices alcançáveis

        visitado[u] = 1;

        vertice *viz = g->lista_adj[u]->prox;
        while (viz) {
            int v = indice_vertice(viz->nome, g->lista_adj, g->num_vertices);
            if (!visitado[v] && dist[u] + viz->peso < dist[v]) {
                dist[v] = dist[u] + viz->peso;
            }
            viz = viz->prox;
        }
    }

    free(visitado);
    return dist;
}

char *diametros(grafo *g) {
    unsigned int n = g->num_vertices;
    int *visitado = calloc(n, sizeof(int));
    int *diametros = malloc(n * sizeof(int));  // No máximo n componentes (todos os vértices isolados)
    int qtd_componentes = 0;

    for (unsigned int i = 0; i < n; i++) {
        if (!visitado[i]) {
            // Marca todos os vértices desse componente como visitado
            busca_profundidade(g, i, visitado);

            // Cria uma lista dos vértices desse componente
            int *vertices_componente = malloc(n * sizeof(int));
            int tamanho = 0;
            for (unsigned int j = 0; j < n; j++) {
                if (visitado[j] == 1) {
                    vertices_componente[tamanho++] = j;
                    // Marca como 2 para não considerar de novo no próximo componente
                    visitado[j] = 2;
                }
            }

            // Calcula o diâmetro desse componente
            int diametro = 0;
            for (int vi = 0; vi < tamanho; vi++) {
                int origem = vertices_componente[vi];
                int *dist = dijkstra(g, origem);  // Calcula as distâncias a partir do vértice origem (testa todos os vértices do componente como origem)

                for (int vj = 0; vj < tamanho; vj++) {
                    int destino = vertices_componente[vj];
                    // Salva a maior distância (menor caminho) encontrada (diâmetro)
                    if (dist[destino] != INT_MAX && dist[destino] > diametro) {
                        diametro = dist[destino];
                    }
                }

                free(dist);
            }

            diametros[qtd_componentes++] = diametro;
            free(vertices_componente);
        }
    }

    // Ordena os diâmetros
    qsort(diametros, qtd_componentes, sizeof(int), compara_inteiros);

    // Monta a string de resposta
    char *resultado = malloc(2048);
    resultado[0] = '\0';
    char buffer[64];

    for (int i = 0; i < qtd_componentes; i++) {
        sprintf(buffer, "%d", diametros[i]);
        strcat(resultado, buffer);
        if (i != qtd_componentes - 1) {
            strcat(resultado, " ");
        }
    }

    free(visitado);
    free(diametros);

    return resultado;
}

static void busca_profundidade_articulacao(grafo *g, int u, int parent, int *tempo, int *visitado, int *desc, int *low, int *articulacao, int *time) 
{
    visitado[u] = 1;
    desc[u] = low[u] = (*time)++;
    int filhos = 0;

    vertice *v = g->lista_adj[u]->prox;
    while (v) 
    {
        int idx_v = -1;
        for (unsigned int i = 0; i < g->num_vertices; i++) 
        {
            if (strcmp(g->lista_adj[i]->nome, v->nome) == 0) 
            {
                idx_v = i;
                break;
            }
        }

        if (idx_v == -1) 
        {
            v = v->prox;
            continue;
        }

        if (!visitado[idx_v]) 
        {
            filhos++;
            busca_profundidade_articulacao(g, idx_v, u, tempo, visitado, desc, low, articulacao, time);
            low[u] = (low[u] < low[idx_v]) ? low[u] : low[idx_v];

            if ((parent == -1 && filhos > 1) || (parent != -1 && low[idx_v] >= desc[u]))
                articulacao[u] = 1;
        } 
        else if (idx_v != parent) 
            low[u] = (low[u] < desc[idx_v]) ? low[u] : desc[idx_v];

        v = v->prox;
    }
}

char *vertices_corte(grafo *g) 
{
    int *visitado = calloc(g->num_vertices, sizeof(int));
    int *desc = malloc(g->num_vertices * sizeof(int));
    int *low = malloc(g->num_vertices * sizeof(int));
    int *articulacao = calloc(g->num_vertices, sizeof(int));
    int tempo = 0;  // Contador, serve pra manter a ordem das visitas nos vértices na busca em profundidade

    for (unsigned int i = 0; i < g->num_vertices; i++)
        if (!visitado[i])
            busca_profundidade_articulacao(g, i, -1, &tempo, visitado, desc, low, articulacao, &tempo);

    // Conta quantos vértices são de corte
    int count = 0;
    for (unsigned int i = 0; i < g->num_vertices; i++)
        if (articulacao[i]) count++;

    if (count == 0) 
    {
        free(visitado);
        free(desc);
        free(low);
        free(articulacao);
        char *vazio = malloc(1);
        vazio[0] = '\0';
        return vazio;
    }

    // Copia os nomes para ordenar
    char **nomes = malloc(count * sizeof(char*));
    int k = 0;
    for (unsigned int i = 0; i < g->num_vertices; i++) 
        if (articulacao[i])
            nomes[k++] = g->lista_adj[i]->nome;

    // Ordena os nomes alfabeticamente
    for (int i = 0; i < count - 1; i++) 
    {
        for (int j = i + 1; j < count; j++) 
        {
            if (strcmp(nomes[i], nomes[j]) > 0) 
            {
                char *tmp = nomes[i];
                nomes[i] = nomes[j];
                nomes[j] = tmp;
            }
        }
    }

    // Concatena os nomes
    size_t tam_total = 0;
    for (int i = 0; i < count; i++)
        tam_total += strlen(nomes[i]) + 1;

    char *res = malloc(tam_total);
    res[0] = '\0';
    for (int i = 0; i < count; i++) 
    {
        strcat(res, nomes[i]);

        if (i < count - 1)
            strcat(res, " ");
    }

    free(visitado);
    free(desc);
    free(low);
    free(articulacao);
    free(nomes);

    return res;
}

static void busca_profundidade_pontes(grafo *g, int u, int parent, int *tempo, int *visitado, int *desc, int *low, char ***pontes, int *count, int *capacity) 
{
    visitado[u] = 1;
    desc[u] = low[u] = (*tempo)++;
    
    vertice *v = g->lista_adj[u]->prox;
    while (v) 
    {
        int idx_v = -1;
        for (unsigned int i = 0; i < g->num_vertices; i++) 
        {
            if (strcmp(g->lista_adj[i]->nome, v->nome) == 0) 
            {
                idx_v = i;
                break;
            }
        }

        if (idx_v == -1) 
        {
            v = v->prox;
            continue;
        }

        if (!visitado[idx_v]) 
        {
            busca_profundidade_pontes(g, idx_v, u, tempo, visitado, desc, low, pontes, count, capacity);
            low[u] = (low[u] < low[idx_v]) ? low[u] : low[idx_v];

            if (low[idx_v] > desc[u]) 
            {
                // Aresta de corte encontrada
                if (*count == *capacity) 
                {
                    *capacity *= 2;
                    *pontes = realloc(*pontes, (*capacity) * sizeof(char*));
                }

                char *ponte = malloc(strlen(g->lista_adj[u]->nome) + strlen(g->lista_adj[idx_v]->nome) + 2);
                if (strcmp(g->lista_adj[u]->nome, g->lista_adj[idx_v]->nome) < 0)
                    sprintf(ponte, "%s %s", g->lista_adj[u]->nome, g->lista_adj[idx_v]->nome);
                else
                    sprintf(ponte, "%s %s", g->lista_adj[idx_v]->nome, g->lista_adj[u]->nome);

                (*pontes)[(*count)++] = ponte;
            }
        } 
        else if (idx_v != parent) 
            low[u] = (low[u] < desc[idx_v]) ? low[u] : desc[idx_v];

        v = v->prox;
    }
}

char *arestas_corte(grafo *g) 
{
    int *visitado = calloc(g->num_vertices, sizeof(int));
    int *desc = malloc(g->num_vertices * sizeof(int));
    int *low = malloc(g->num_vertices * sizeof(int));
    int tempo = 0;

    char **pontes = malloc(10 * sizeof(char*));
    int count = 0, capacity = 10;

    for (unsigned int i = 0; i < g->num_vertices; i++)
        if (!visitado[i])
            busca_profundidade_pontes(g, i, -1, &tempo, visitado, desc, low, &pontes, &count, &capacity);

    if (count == 0) 
    {
        free(visitado);
        free(desc);
        free(low);
        free(pontes);
        char *vazio = malloc(1);
        vazio[0] = '\0';
        return vazio;
    }

    // Ordena as pontes alfabeticamente
    for (int i = 0; i < count - 1; i++) 
    {
        for (int j = i + 1; j < count; j++) 
        {
            if (strcmp(pontes[i], pontes[j]) > 0) 
            {
                char *tmp = pontes[i];
                pontes[i] = pontes[j];
                pontes[j] = tmp;
            }
        }
    }

    // Concatena as pontes
    size_t tam_total = 0;
    for (int i = 0; i < count; i++)
        tam_total += strlen(pontes[i]) + 1;

    char *res = malloc(tam_total);
    res[0] = '\0';
    for (int i = 0; i < count; i++) 
    {
        strcat(res, pontes[i]);

        if (i < count - 1)
            strcat(res, " ");
    }

    // Libera memória
    free(visitado);
    free(desc);
    free(low);
    for (int i = 0; i < count; i++)
        free(pontes[i]);
    free(pontes);

    return res;
}
