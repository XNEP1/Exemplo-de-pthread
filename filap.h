/* Trabalho T3 de Bruno Krügel.
 * 2021.
 * Implemente uma fila de prioridade com as operações tradicionais sobre conjuntos dinâmicos 
 * (inicializar fila de prioridades, inserir elemento, remover elemento, verificar se está vazia, mostrar fila)
 */

#ifndef FILAP_H
#define FILAP_H

#include <stdio.h>
#include <stdlib.h>

typedef struct nodo{
    int dado;
    int prioridade_atual;
    int prioridade_real;
    struct nodo *prox;
    struct nodo *anterior;
} t_nodo;


/* Cada item dessa lista guarda o endereço do ultimo item de uma certa prioridade. */
/* Exemplo:                                                                        */
/*  | 2 | 1 |                                                                      */
/*     \_  \____                                                                   */
/*       \___   \____                                                              */
/*           \       \                                                             */
/*  | 2 | 2 | 2 | 1 | 1 |                                                          */
/*                                                                                 */
struct nodo_atalhos{
    t_nodo *ptr_para_final;
    int prioridade;
    struct nodo_atalhos *prox;
};

typedef struct fila{
    t_nodo *inicio;
    struct nodo_atalhos *atalhos;
    int tamanho;
} t_filaP;

/* Cria uma fila vazia, isto é, aponta para NULL e contém tamanho zero; Retorna 1 se a operação foi bem sucedida e zero caso contrário; */
int inicializa_fila(t_filaP *f); 

/* Retorna 1 se a fila está vazia e zero caso contrário; */
int fila_vazia(t_filaP *f); 

/* Retorna 1 se a fila tiver apenas um elemento e zero caso contrário; */
int fila_unitaria(t_filaP *f);

/* Insere o elemento x no inı́cio da fila com prioridade pr, mas apenas se a fila for vazia; Retorna 1 se a operação foi bem sucedida e zero caso contrário; */
int inserir_inicio_fila(int x, int pr, t_filaP *f); 

/* Insere o elemento x na fila de acordo com a prioridade pr; Retorna 1 se a operação foi bem sucedida e zero caso contrário; */
int inserir_na_fila(int x, int pr, t_filaP *f); 

/* Remove um atalho caso ele aponte para o ponteiro item_removido. Caso não exista nenhum, nada acontecerá. */
void atualizar_atalho_da_fila(t_nodo *item_removido, t_filaP *f);

/* Remove o elemento com mais prioidade da fila e o retorna em *item; Retorna 1 se a operação foi bem sucedida e zero caso contrário; */
int remover_proximo_da_fila(int *item, int *item_pr, t_filaP *f); 

/* Remove um nodo da fila e retorna suas propriedades nos ponteiros item e item_pr. A função retorna 1 caso tenha sucesso ou 0 caso contrario. */
int remover_item_da_fila(t_nodo *nodo, int *item, int *item_pr, t_filaP *f);

/* Aumenta a prioridade de todos os itens na lista como forma de previnir starvation. */
void tratar_starvation(t_filaP *f, int aumento);

/* Retorna 0 caso um item esteja na frente de outro item com prioridade(atual) maior e retorna 1 caso a fila esteja de fato em ordem. */
int vereficar_integridade(t_filaP *f);

/* Para cada item da fila f, chama a função callback usando esse item como parametro. */
int varrer_fila(t_filaP *f, t_nodo *inicio, void (*callback)() );

/* Para cada item da fila f, chama a função callback usando esse item como parametro. Porém, varrendo a fila no sentido contrario. */
int varrer_inverso_fila(t_filaP *f, t_nodo *inicio, void (*callback)() );

/* Imprime os elementos da fila, do inı́cio ao final. Se ela for vazia não imprime nada, nem um \n; */
void imprimir_fila(t_filaP *f); 

/* Remove todos os elementos da fila e faz com que ela aponte para NULL; */
void destruir_fila(t_filaP *f); 
/* Usado para destroir a fila usando recursão. Libera o espaço de todos os nodos ligados. */
void destroi_nodos(t_nodo *n);
void destroi_atalhos(struct nodo_atalhos *a);

#endif