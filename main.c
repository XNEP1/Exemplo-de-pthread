/* Trabalho T3 de Bruno Krügel.
 * GRR20206874
 * 2021.
 * Gerencia todos os processor de vaninação. 
 * Compile usando gcc e as opções -ansi e -pthread.
 * Exemplo: "gcc -ansi *.c -o t3 -pthread"
 */

#include "main.h"

/* Global Vars */
pthread_mutex_t mutex_registro;
pthread_mutex_t mutex_fila;
int qnt_atendentes;

int sorteia(int min, int max) {
    srand(time(NULL));
    return min + (rand() % (max-min+1));
}

/* Retorna um vetor de atendentes de tamanho igual ao parametro. Ou NULL caso não tenha sucesso. */
struct atendente *contratar_atendentes(int quantidade){
    srand(time(NULL));
    int i;
    char nome_do_atendente[6];
    int random;

    int possiveis_expedientes[4] = {
        MEIO_PERIODO,
        ESTAGIARIO,
        TURNO_COMPLETO, 
        TURNO_EMERGENCIAL
    };

    type_pilha *nomes = criar_pilha(6 * sizeof(char));
    empilhar(nomes, &("Paulo\0"));
    empilhar(nomes, &("Bruno\0"));
    empilhar(nomes, &("Vitor\0"));
    empilhar(nomes, &("Marie\0"));
    empilhar(nomes, &("Carla\0"));

    struct atendente *vetor = calloc(quantidade, sizeof(struct atendente));

    for(i=0; i<quantidade; i++){
        if(desempilhar(nomes, &nome_do_atendente))
            memcpy(vetor[i].nome, nome_do_atendente, (6 * sizeof(char)));

        random = sorteia(0, 3);
        vetor[i].expediente = possiveis_expedientes[ random ];
        vetor[i].tipo = possiveis_expedientes[ random ];
        vetor[i].situacao = LIVRE;
        vetor[i].flag_intervalo = 0;
    }
    destruir_pilha(nomes);
    return vetor;
}

void tempo_de_atendimento(t_filaP *fila, t_nodo *paciente, int *status){
    (*status) += (paciente->prioridade_real / qnt_atendentes);
    return;
}

int mau_humor(t_filaP *fila, t_nodo *paciente){
    int expectativa_tempo = 0;
    t_nodo *aux;
    if(paciente->prioridade_real == PRIORITARIO ){
        expectativa_tempo = varrer_inverso_fila(fila, paciente->anterior, &tempo_de_atendimento);
    }

    if(expectativa_tempo >= 60)
        return 1;
    else
        return 0;
}

void bonus_2(t_filaP *fila, t_nodo *paciente, int *status){
    int dado, prioridade;
    if(mau_humor(fila, paciente)){
        remover_item_da_fila(paciente, &dado, &prioridade, fila);
        (*status)++;
    }
}

void preencher(t_filaP *fila, int qnt){
    int i, pr;
    pthread_mutex_lock(&mutex_fila);
    tratar_starvation(fila, ANTI_STARVATION_INCREASE);  /* Bonus 1 */
    for(i=0; i<qnt; i++){
        if(sorteia(1, 10) >= 7) 
            pr = PRIORITARIO;
        else
            pr = NAO_PRIORITARIO;
        inserir_na_fila(fila->tamanho, pr, fila);
    }
    pthread_mutex_unlock(&mutex_fila);
}

bool existe_expediente(struct equipe equipe){
    int i;
    bool status = false;
    for(i=0; i<equipe.tamanho; i++)
        if(equipe.atendentes[i].expediente > 0)
            status = true;

    return status;
}

void *processa_intervalo(void *atendente_arg){
    struct atendente *atendente = (struct atendente *)atendente_arg;
    int minutos = 15;

    atendente->situacao = INTERVALO;
    sleep(minutos/SPEED);
    /* O intervado conta no expediente? */
    /* atendente->expediente = atendente->expediente - minutos; */
    atendente->situacao = LIVRE;

    pthread_exit(NULL);
}

void *processa_fila(void *arg){
    struct processa_fila_arg *thread_arg = (struct processa_fila_arg *)arg;
    int dado = thread_arg->dado;
    int prioridade = thread_arg->prioridade;
    struct dados_vacinacao *registro = thread_arg->registro;
    struct atendente *atendente = thread_arg->atendente;

    int horas_do_expediente;
    int tempo = prioridade;

    sleep(tempo/SPEED);
    atendente->expediente = atendente->expediente - tempo;


    pthread_mutex_lock(&mutex_registro);
    registro->qnt++;
    atendente->registro_do_atendente.qnt++;
    if(prioridade == PRIORITARIO){
        registro->qnt_prioritarios++;
        atendente->registro_do_atendente.qnt_prioritarios++;
    }
    else{ /* (prioridade == NAO_PRIORITARIO) */
        registro->qnt_nao_prioritarios++;
        atendente->registro_do_atendente.qnt_nao_prioritarios++;
    }
    pthread_mutex_unlock(&mutex_registro);

    if(atendente->tipo > MEIO_PERIODO)
        if(atendente->flag_intervalo == 0)
            if(((double)atendente->expediente / (double)atendente->tipo) <= 0.5)
                atendente->flag_intervalo = 1;

    if(atendente->expediente <= 0)
        atendente->situacao = COMPLETO;
    else
        atendente->situacao = LIVRE;

    free(arg); /* arg é o endereço da uma sessao alocada anteriormente. */
    pthread_exit(NULL);
}

void atender(t_filaP *fila, struct dados_vacinacao *registro, pthread_t *threads, int *restantes, struct equipe equipe){
    int i;
    struct processa_fila_arg *sessao;
    bool atendendo = true;

    while(existe_expediente(equipe) && atendendo){
        for(i=0; i<equipe.tamanho; i++){
            if(fila_vazia(fila)){
                if(!(*restantes)){  /* Não é mais ninguem para atender nessa fila e o atendimento pode acabar. */
                    atendendo = false;
                    break;
                } 
                else /* Apesar de não ter ninguem na fila, novos pacientes ainda viram, então é criado um loop até eles chegarem. */
                    break;
            }
            else if(equipe.atendentes[i].flag_intervalo == 1){
                equipe.atendentes[i].flag_intervalo = 2;
                pthread_create(&threads[i], NULL, &processa_intervalo, &(equipe.atendentes[i]));
            }
            else if(equipe.atendentes[i].situacao == LIVRE){
                sessao = malloc(sizeof(struct processa_fila_arg));
                sessao->atendente = &(equipe.atendentes[i]);
                sessao->registro = registro;
                pthread_mutex_lock(&mutex_fila);
                if( !remover_proximo_da_fila(&(sessao->dado), &(sessao->prioridade), fila) ){
                    printf("Fila vazia não foi tratado antes?\n");
                    exit(1);
                }
                pthread_mutex_unlock(&mutex_fila);

                equipe.atendentes[i].situacao = OCUPADO;
                pthread_create(&threads[i], NULL, &processa_fila, (void*)sessao);
            }
        }
    }

    for(i=0; i<equipe.tamanho; i++)
        pthread_join(threads[i], NULL);
}

void *atualizar_tempo(void *arg){
    struct atualizar_tempo_arg* thread_arg = (struct atualizar_tempo_arg *)arg;
    t_filaP *fila_dose_1 = thread_arg->fila_dose_1;
    t_filaP *fila_dose_2 = thread_arg->fila_dose_2;
    int *restantes_dose_1 = thread_arg->restantes_dose_1;
    int *restantes_dose_2 = thread_arg->restantes_dose_2;   
    int *mau_humorados_dose_1 = thread_arg->mau_humorados_dose_1;
    int *mau_humorados_dose_2 = thread_arg->mau_humorados_dose_2;
    
    int tempo = 0;
    while(true){
        if(*restantes_dose_1 > 0){
            preencher(fila_dose_1, QNT_PER_10MINUTES);
            *restantes_dose_1 = *restantes_dose_1 - QNT_PER_10MINUTES;
            pthread_mutex_lock(&mutex_fila);
            *mau_humorados_dose_1 += varrer_fila(fila_dose_1, fila_dose_1->inicio, &bonus_2);
            pthread_mutex_unlock(&mutex_fila);
        }

        if(( (*restantes_dose_1 == 0) || (tempo >= HORARIO_DA_SEGUNDA_DOSE)) && (*restantes_dose_2 > 0)){
            preencher(fila_dose_2, QNT_PER_10MINUTES);
            *restantes_dose_2 = *restantes_dose_2 - QNT_PER_10MINUTES;
            pthread_mutex_lock(&mutex_fila);
            *mau_humorados_dose_2 += varrer_fila(fila_dose_2, fila_dose_2->inicio, &bonus_2);
            pthread_mutex_unlock(&mutex_fila);
        }

        sleep(10/SPEED);
        tempo = tempo + 10;
    }
}

int main(void){
    srand(time(NULL));

    int i;
    t_filaP fila_dose_1;
    t_filaP fila_dose_2;
    struct dados_vacinacao registro_dose_1 = {0,0,0};
    struct dados_vacinacao registro_dose_2 = {0,0,0};
    int mau_humorados_dose_1 = 0;
    int mau_humorados_dose_2 = 0;
    int restantes_dose_1 = 50;
    int restantes_dose_2 = 50;
    struct equipe equipe;
    qnt_atendentes = sorteia(1,5);
    equipe.tamanho = qnt_atendentes;
    char tipo[18];
    pthread_t *threads_equipe = calloc(equipe.tamanho, sizeof(pthread_t));
    pthread_t thread_grafico; /* Esse thread é usado para printar as informações a cada 1 segundo, caso DEBUG_MODE for 1. */
    pthread_t thread_tempo;   /* Nesse thread o tempo é simulado e, com isso, algumas coisa são controladas, como novas pessoas chegando na fila ou outras saindo. */
    
    if(pthread_mutex_init(&mutex_registro, NULL) || pthread_mutex_init(&mutex_fila, NULL)){ /* init retorna 0 caso tenha sucesso. */
        printf("Falha ao iniciar mutex.\n");
        return 0;
    }

    if( !(inicializa_fila(&fila_dose_1)) || !(inicializa_fila(&fila_dose_2))){
        printf("Falha ao criar filas.\n");
        return 0;
    }

    equipe.atendentes = contratar_atendentes(equipe.tamanho);
    if(equipe.atendentes == NULL){
        printf("Falha ao alocar equipe de atendentes!\n");
        return 0;
    }

    struct imprimir_infos_arg imprimir_infos_arg = {&equipe, &registro_dose_1, &registro_dose_2, &fila_dose_1, &fila_dose_2, &restantes_dose_1, &restantes_dose_2, &mau_humorados_dose_1, &mau_humorados_dose_2}; 
    if(DEBUG_MODE) pthread_create(&thread_grafico, NULL, &imprimir_infos, (void*)&imprimir_infos_arg);
    
    struct atualizar_tempo_arg atualizar_tempo_arg = {&fila_dose_1, &fila_dose_2, &restantes_dose_1, &restantes_dose_2, &mau_humorados_dose_1, &mau_humorados_dose_2}; 
    pthread_create(&thread_tempo, NULL, &atualizar_tempo, (void*)&atualizar_tempo_arg);

    /* Atende as pessoas das filas até não ter gente para ser atendida ou gente para atender. */
    atender(&fila_dose_1, &registro_dose_1, threads_equipe, &restantes_dose_1, equipe);
    atender(&fila_dose_2, &registro_dose_2, threads_equipe, &restantes_dose_2, equipe);

    if(!DEBUG_MODE) imprimir_infos((void*)&imprimir_infos_arg);

    if(DEBUG_MODE) pthread_cancel(thread_grafico);
    pthread_cancel(thread_tempo);
    pthread_mutex_destroy(&mutex_registro);
    pthread_mutex_destroy(&mutex_fila);
    free(threads_equipe);
    free(equipe.atendentes);
    destruir_fila(&fila_dose_1);
    destruir_fila(&fila_dose_2);

    return 1;
}