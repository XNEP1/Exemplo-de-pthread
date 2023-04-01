/* Trabalho T3 de Bruno Krügel.
 * GRR20206874
 * 2021.
 * Gerencia todos os processor de vaninação. 
 * Compile usando gcc e as opções -ansi e -pthread.
 * Exemplo: "gcc -ansi *.c -o t3 -pthread"
 */

/* ++++++++++ CONFIGURAÇÕES ++++++++++ */

/* Mude para 1 para conseguir ver os valores da fila durante a execução. */
/* Cuidado: Isso limpa a tela do terminal. */
#define DEBUG_MODE 1

/* Imprime as filas no final do relatório. */
#define IMPRIMIR_FILAS 0

/* Define o quão rápido será os processos do programa. Exemplo: Use 1 para ser em tempo real ou 2 para processar duas vezes mais rápido. */
#define SPEED 8

/* Quantidade de minutos que devem passar para começar a vir pessoas da segunda dose. */
/* Caso a primeira fila esvazie, automaticamente a segunda começa a encher. */
#define HORARIO_DA_SEGUNDA_DOSE 200

/* Quantidade de prioridade que vai aumentar para cada 10 minutos que um paciente ficar na fila. */
#define ANTI_STARVATION_INCREASE 3

/* Quantidade de pessoas, com prioridade aleatórias, que entraram na fila a cada 10 minutos. */
#define QNT_PER_10MINUTES 5

/* ------- FIM DAS CONFIGURAÇÕES ----- */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "filap.h"
#include "pilha.h"
#include "display.h"

#define PRIORITARIO 20
#define NAO_PRIORITARIO 10

#define MEIO_PERIODO (4*60)
#define ESTAGIARIO (6*60)
#define TURNO_COMPLETO (8*60)
#define TURNO_EMERGENCIAL (10*60)

#define LIVRE 0
#define OCUPADO 1
#define INTERVALO -1
#define COMPLETO 2

struct dados_vacinacao {
    int qnt;
    int qnt_prioritarios;
    int qnt_nao_prioritarios;
};

struct atendente{
    char nome[6];
    int expediente;
    int tipo;
    int situacao;
    int flag_intervalo; /* Diz se o atendente já fez ou não seu intervalo. */
    struct dados_vacinacao registro_do_atendente;
};

struct equipe{
    struct atendente *atendentes;
    int tamanho;
};

/* Estrutura para argumentos de função assincrona. */
struct processa_fila_arg{
    int dado, prioridade;
    struct dados_vacinacao *registro;
    struct atendente *atendente;
};

/* Estrutura para argumentos de função assincrona. */
struct atualizar_tempo_arg{
    t_filaP *fila_dose_1;
    t_filaP *fila_dose_2;
    int *restantes_dose_1;
    int *restantes_dose_2;
    int *mau_humorados_dose_1;
    int *mau_humorados_dose_2;
};

/* Sorteia um numero aleatório entre min e max. */
int sorteia(int min, int max);

/* Retorna o endereço de um vetor de atendendos já inicializado.*/
/* Não se esqueça de dar free nesse endereço. */
struct atendente *contratar_atendentes(int quantidade);

/* Usado para contar o tempo necessario para ser atendido a partir de alguem da fila. */
void tempo_de_atendimento(t_filaP *fila, t_nodo *paciente, int *status);

/* Retorna 1 se o paciente passado como paremetro cumpre os requesitos para estar de mau-humor. */
int mau_humor(t_filaP *fila, t_nodo *paciente);

/* Verifica e retira aqueles que são prioritários e estão de mau-humor por precisar mais de 1 hora para serem atendidos. */
void bonus_2(t_filaP *fila, t_nodo *paciente, int *status);

/* Coloca uma quantidade de pessoas igual a qnt em uma fila. */
void preencher(t_filaP *fila, int qnt);

/* Retorna true caso algum atendente tenha expediente sobrando e false caso contrário. */
bool existe_expediente(struct equipe equipe);

/* Usado como função assincrona. */
/* Dá um invervalo para o (~escravo~) atendente. */
void *processa_intervalo(void *atendente_arg);

/* Usado como função assincrona. */
/* Cria um loop, fazendo os atendentes vacinarem as pessoas da fila até que não tenha quem mais ser atendido ou quem atender. */
void *processa_fila(void *arg);

/* Executa o sleep para simular a vacinação, contabiliza o atendimento no registro e atualiza o expediente do atendente. */
void atender(t_filaP *fila, struct dados_vacinacao *registro, pthread_t *threads, int *restantes, struct equipe equipe);

/* Usado como função assincrona. */
/* Cria um loop que a cada 10 minutos coloca novas pessoas na fila (até 50 pessoas) e , também, chama a função bonus_2. */
void *atualizar_tempo(void *arg);

#endif
