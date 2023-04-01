/* Trabalho T3 de Bruno Krügel.
 * 2021.
 * Implemente uma fila de prioridade com as operações tradicionais sobre conjuntos dinâmicos 
 * (inicializar fila de prioridades, inserir elemento, remover elemento, verificar se está vazia, mostrar fila)
 */

#include "filap.h"

/* A cada item removido, esse numero é acrescentado a prioridade de todos os itens restantes da fila como forma de previnir starvation. */
#define ANTISTARVATION_PRIORIDADE_MAX 1024

#define SUCESSO 1
#define FRACASSO 0

int inicializa_fila(t_filaP *f){
    if(f == NULL)
        return FRACASSO;

    f->inicio = NULL;
    f->atalhos = NULL;
    f->tamanho = 0;

    return SUCESSO;
}

int fila_vazia(t_filaP *f){
    if((f->inicio == NULL) || (f->tamanho == 0))
        return SUCESSO;

    return FRACASSO;
}

int fila_unitaria(t_filaP *f){
    if((f->tamanho == 1))
        return SUCESSO;

    return FRACASSO;
}

int inserir_inicio_fila(int x, int pr, t_filaP *f){
    t_nodo *novo_nodo = malloc(sizeof(t_nodo));
    if(novo_nodo == NULL)
        return FRACASSO;

    novo_nodo->prox = f->inicio;
    novo_nodo->anterior = NULL;
    novo_nodo->dado = x;
    novo_nodo->prioridade_atual = pr;
    novo_nodo->prioridade_real = pr;
    if( !fila_vazia(f))
        f->inicio->anterior = novo_nodo;
    f->inicio = novo_nodo;
    (f->tamanho)++;

    return SUCESSO;
}

int inserir_na_fila(int x, int pr, t_filaP *f){
 /* Casos que o item vai ser inserido logo no começo precisam ser tratados de forma diferente, já que o f->inicio precisa ser atualizado. */
    if(fila_vazia(f))
        return inserir_inicio_fila(x, pr, f);

    if(pr > f->inicio->prioridade_atual)
        return inserir_inicio_fila(x, pr, f);

    t_nodo *aux = f->inicio;
    t_nodo *novo_nodo = malloc(sizeof(t_nodo));
    struct nodo_atalhos *atalho;

    if(novo_nodo == NULL)
        return FRACASSO;
    novo_nodo->dado = x;
    novo_nodo->prioridade_atual = pr;
    novo_nodo->prioridade_real = pr;

/* Para não precisar varrer todo a fila para colocar um novo elemento, criei uma lista ligada que guarda 
 * um ponteiro para o ultimo elemento de cada prioridade. */
    for(atalho=f->atalhos; atalho!=NULL; atalho=atalho->prox){
        if(atalho->prioridade == pr){
            novo_nodo->prox = atalho->ptr_para_final->prox;
            novo_nodo->anterior = atalho->ptr_para_final;
            if( novo_nodo->prox != NULL ) 
                novo_nodo->prox->anterior = novo_nodo;
            atalho->ptr_para_final->prox = novo_nodo;
            atalho->ptr_para_final = novo_nodo;
            (f->tamanho)++;
            return SUCESSO;
        }
    }

/* Como nenhum ponteiro para essa prioridade foi encontrado na lista, um novo será criado. */
    struct nodo_atalhos *novo_atalho = malloc(sizeof(struct nodo_atalhos));
    if(novo_atalho == NULL)
        return FRACASSO;
    novo_atalho->ptr_para_final = novo_nodo;
    novo_atalho->prioridade = pr;

    novo_atalho->prox = f->atalhos;
    f->atalhos = novo_atalho;

    for(aux=f->inicio; aux->prox!=NULL; aux=aux->prox){
        if((aux->prox->prioridade_atual < pr))
            break;
    }

    novo_nodo->prox = aux->prox;
    novo_nodo->anterior = aux;
    aux->prox = novo_nodo;
    (f->tamanho)++;

    return SUCESSO;   
}

void atualizar_atalho_da_fila(t_nodo *item_removido, t_filaP *f){
    struct nodo_atalhos *atalho, *aux;
    if(f->atalhos != NULL){

        if(f->atalhos->ptr_para_final == item_removido){
            if(item_removido->anterior != NULL)
                f->atalhos->ptr_para_final = item_removido->anterior;
            else{
                aux = f->atalhos;
                f->atalhos = f->atalhos->prox;
                free(aux);
            }
        }
        else{

            for(atalho=f->atalhos; atalho->prox!=NULL; atalho=atalho->prox){
                aux = atalho->prox;
                if(aux->ptr_para_final == item_removido){
                    if(item_removido->anterior != NULL)
                        aux->ptr_para_final = item_removido->anterior;
                    else{
                        atalho->prox = aux->prox;
                        free(aux);
                        break;
                    }
                }
            }
        }
    }
}


int remover_proximo_da_fila(int *item, int *item_pr, t_filaP *f){
/* Como todos os nodos já foram inseridos de forma a respeitar a prioridade, */
/* pegar o primeiro item da lista basta. */
    return remover_item_da_fila(f->inicio, item, item_pr, f);
}

int remover_item_da_fila(t_nodo *item_removido, int *item, int *item_pr, t_filaP *f){
    if(fila_vazia(f))
        return FRACASSO;

/* Remove ou atualiza o atalho caso ele aponte para o item removido. */
    atualizar_atalho_da_fila(item_removido, f);

    if(item_removido == f->inicio){ /* Ele é o primeiro item da lista. */
        if(fila_unitaria(f)){
            f->inicio = NULL;
        }
        else{
            f->inicio = item_removido->prox;
            item_removido->prox->anterior = NULL;
        }
    }

    *item = item_removido->dado;
    *item_pr = item_removido->prioridade_real;

    if(item_removido->anterior != NULL)
        item_removido->anterior->prox = item_removido->prox;

    if(item_removido->prox != NULL)
        item_removido->prox->anterior = item_removido->anterior;

    (f->tamanho)--;
    free(item_removido);
    
    return SUCESSO;
}

void tratar_starvation(t_filaP *f, int aumento){
    if(fila_vazia(f))
        return;

    t_nodo *aux;
    struct nodo_atalhos *atalho;

    for(aux=f->inicio; aux!=NULL; aux=aux->prox)
        if( (aux->prioridade_atual + aumento) <= ANTISTARVATION_PRIORIDADE_MAX )
            aux->prioridade_atual += aumento;

    for(atalho=f->atalhos; atalho!=NULL; atalho=atalho->prox)
        for(aux=atalho->ptr_para_final; (aux!=NULL && (aux->prioridade_atual >= atalho->prioridade)); aux=aux->prox)
            atalho->ptr_para_final = aux;
}

int vereficar_integridade(t_filaP *f){
    if(fila_vazia(f))
        return SUCESSO;

    t_nodo *aux, *anterior;
    anterior = f->inicio;
    for(aux=f->inicio; aux->prox!=NULL; aux=aux->prox){
        if(aux->prioridade_atual > anterior->prioridade_atual )
            return FRACASSO;
    }

    return SUCESSO;
}

int varrer_fila(t_filaP *f, t_nodo *inicio, void (*callback)()){
    t_nodo *aux, *proximo;
    int status = FRACASSO;
    
    for(aux=inicio; aux!=NULL; aux=proximo){
        proximo = aux->prox;
        (*callback)(f, aux, &status);
    }

    return status;
}

int varrer_inverso_fila(t_filaP *f, t_nodo *inicio, void (*callback)() ){
    t_nodo *aux, *anterior;
    int status = FRACASSO;
    
    for(aux=inicio; aux!=NULL; aux=anterior){
        anterior = aux->anterior;
        (*callback)(f, aux, &status);
    }

    return status;
}

void imprimir_fila(t_filaP *f){
    if(fila_vazia(f))
        return;
    
    t_nodo *aux;
    printf("tam: %d -> ", f->tamanho);
    for(aux=f->inicio; aux!=NULL; aux=aux->prox)
        printf("\n\tprioridade %d (atual: %d): id %d -> %p", aux->prioridade_real, aux->prioridade_atual, aux->dado, aux);

    struct nodo_atalhos *atalhos;
    printf("\nAtalhos:");
    for(atalhos=f->atalhos; atalhos!=NULL; atalhos=atalhos->prox)
        printf("\n\tatalho para %d: id %d -> %p", atalhos->prioridade, atalhos->ptr_para_final->dado, atalhos->ptr_para_final);
    printf("\n");
    
}

void destruir_fila(t_filaP *f){
    if( !fila_vazia(f))
        destroi_nodos(f->inicio);

    if( f->atalhos != NULL )
        destroi_atalhos(f->atalhos);
}

void destroi_nodos(t_nodo *n){
    if(n->prox != NULL) /* Percorre até o final da lista e retorna liberando todas as memorias dos nodos. */
        destroi_nodos(n->prox);

    free(n);
}

void destroi_atalhos(struct nodo_atalhos *a){
    if(a->prox != NULL)
        destroi_atalhos(a->prox);

    free(a);
}