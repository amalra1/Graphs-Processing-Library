#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

//------------------------------------------------------------------------------
int main(void) {

  FILE *arq;
  grafo *g = inicializa_grafo();

  if (!g) 
  {
    fprintf(stderr, "Falha ao inicializar o grafo.\n");
    return EXIT_FAILURE;
  }

  arq = fopen("entradas/grafo1.txt", "r");

  if (!arq) 
  {
    perror("Erro ao abrir o arquivo do grafo"); 
    return EXIT_FAILURE;
  }

  g = le_grafo(arq);

  if (!g) 
  {
    fprintf(stderr, "Erro ao ler o grafo do arquivo.\n");
    fclose(arq); 
    return EXIT_FAILURE;
  }

  printf("Nome do grafo lido: %s\n", g->nome);

  fclose(arq);

  return 0;

  // grafo *g = le_grafo(stdin);
  // char *s;

  // printf("grafo: %s\n", nome(g));
  // printf("%d vertices\n", n_vertices(g));
  // printf("%d arestas\n", n_arestas(g));
  // printf("%d componentes\n", n_componentes(g));

  // printf("%sbipartido\n", bipartido(g) ? "" : "não ");

  // printf("diâmetros: %s\n", s=diametros(g));
  // free(s);

  // printf("vértices de corte: %s\n", s=vertices_corte(g));
  // free(s);

  // printf("arestas de corte: %s\n", s=arestas_corte(g));
  // free(s);

  // return ! destroi_grafo(g);
}
