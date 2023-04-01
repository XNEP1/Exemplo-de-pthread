/* Trabalho T3 de Bruno Krügel.
 * 2021.
 * Funções relacionadas a mostrar informações na tela. 
 */

#include "display.h"

void *imprimir_infos(void *arg){
    struct imprimir_infos_arg *thread_arg = (struct imprimir_infos_arg *)arg;
    struct equipe *equipe = thread_arg->equipe;
    struct dados_vacinacao *registro_dose_1 = thread_arg->registro_dose_1;
    struct dados_vacinacao *registro_dose_2 = thread_arg->registro_dose_2;
    t_filaP *fila_dose_1 = thread_arg->fila_dose_1;
    t_filaP *fila_dose_2 = thread_arg->fila_dose_2;
    int *restantes_dose_1 = thread_arg->restantes_dose_1;
    int *restantes_dose_2 = thread_arg->restantes_dose_2;
    int *mau_humorados_dose_1 = thread_arg->mau_humorados_dose_1;
    int *mau_humorados_dose_2 = thread_arg->mau_humorados_dose_2;

    int i;
    char status[11], tipo[18];
    do{
        if(DEBUG_MODE) printf("\e[1;1H\e[2J"); /* 'Limpa' a tela em ANSI. */
        printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        /* Quantidade de pessoas nas filas. */
        if((*restantes_dose_1 == 0) && fila_vazia(fila_dose_1))
            printf("Todas as pessoas da fila da primeira dose foram atendidos! (Restantes: %d)\n", (*restantes_dose_1));
        else
            printf("Ainda existem %d pessoas na fila da primeira dose. (Restantes: %d)\n", fila_dose_1->tamanho, (*restantes_dose_1));

        if((*restantes_dose_2 == 0) && fila_vazia(fila_dose_2))
            printf("Todas as pessoas da fila da segunda dose foram atendidos! (Restantes: %d)\n", (*restantes_dose_2));
        else
            printf("Ainda existem %d pessoas na fila da segunda dose. (Restantes: %d)\n", fila_dose_2->tamanho, (*restantes_dose_2));
        printf("\n");

        /* Informações dos atendentes. */
        if(equipe->tamanho > 1)
            printf("%d atendentes foram contratados.\n", equipe->tamanho);
        else
            printf("%d atendente foi contratado.\n", equipe->tamanho);
        for(i=0; i<equipe->tamanho; i++){
            printf("\tAtendente %s:\n", equipe->atendentes[i].nome);

            switch(equipe->atendentes[i].tipo){
                case MEIO_PERIODO:
                    strcpy(tipo, "Meio Período\0");
                    break;
                case ESTAGIARIO:
                    strcpy(tipo, "Estagiário(a)\0");
                    break;
                case TURNO_COMPLETO:
                    strcpy(tipo, "Turno Completo\0");
                    break;
                case TURNO_EMERGENCIAL:
                    strcpy(tipo, "Turno Emergencial\0");
                    break;
                default:
                    strcpy(tipo, "Caso Especial\0");
                    break;
            }
            printf("\t\tTipo de Expediente: %s (%d minutos);\n", tipo, equipe->atendentes[i].tipo);
            printf("\t\tExpediente: %d minutos cumpridos.", (equipe->atendentes[i].tipo - equipe->atendentes[i].expediente));
            if(equipe->atendentes[i].situacao == COMPLETO)
                printf(" (Expediente completo)");
            printf("\n");

            if(DEBUG_MODE){
                switch(equipe->atendentes[i].situacao){
                    case LIVRE:
                        strcpy(status, "LIVRE\0");
                        break;
                    case OCUPADO:
                        strcpy(status, "OCUPADO(a)\0");
                        break;
                    case INTERVALO:
                        strcpy(status, "INTERVALO\0");
                        break;
                    case COMPLETO:
                        strcpy(status, "COMPLETO\0");
                        break;
                }
                printf("\t\tStatus: %s;\n", status);
            }

            printf("\t\tPacientes atendidos: %d\n", equipe->atendentes[i].registro_do_atendente.qnt);
            printf("\t\t\tPrioritários: %d\n", equipe->atendentes[i].registro_do_atendente.qnt_prioritarios);
            printf("\t\t\tNão Prioritários: %d\n", equipe->atendentes[i].registro_do_atendente.qnt_nao_prioritarios);
            printf("\n");
        }
        printf("\n");

        /* Quantidade de pessoas atendidas e vacinadas. */
        printf("Quantidade de pessoas atendidas:\n");
        printf("\tPrimeira dose:\n");
        printf("\t\tPessoas vacinadas: %d\n", registro_dose_1->qnt);
        printf("\t\t\tPrioritários: %d; \n", registro_dose_1->qnt_prioritarios);
        printf("\t\t\tNão Prioritários: %d. \n", registro_dose_1->qnt_nao_prioritarios);
        printf("\n");
        printf("\tSegunda dose:\n");
        printf("\t\tPessoas vacinadas: %d\n", registro_dose_2->qnt);
        printf("\t\t\tPrioritários: %d; \n", registro_dose_1->qnt_prioritarios);
        printf("\t\t\tNão Prioritários: %d. \n", registro_dose_1->qnt_nao_prioritarios);
        printf("\n");

        /* Quantidade de pessoas não atendidas. */
        printf("Logo,\n");
        printf("%d pessoas na fila da primeira dose não foram atendidas!\n", (*restantes_dose_1 + fila_dose_1->tamanho + *mau_humorados_dose_1));
        if(*mau_humorados_dose_1 > 0)
            printf("\tDestes, %d foram embora por mau-humor!\n", *mau_humorados_dose_1);
        else
            printf("\tNinguem ficou mal humorado. :D\n");

        printf("%d pessoas na fila da segunda dose não foram atendidas!\n", (*restantes_dose_2 + fila_dose_2->tamanho + *mau_humorados_dose_2));
        if(*mau_humorados_dose_2 > 0)
            printf("\tDestes, %d foram embora por mau-humor!\n", *mau_humorados_dose_2);
        else
            printf("\tNinguem ficou mal humorado. :D\n");
        
        if(IMPRIMIR_FILAS) printf("Fila da 1a dose:\n");
        if(IMPRIMIR_FILAS) imprimir_fila(fila_dose_1);
        if(IMPRIMIR_FILAS) printf("Fila da 2a dose:\n");
        if(IMPRIMIR_FILAS) imprimir_fila(fila_dose_2);
        printf("---------------------------------------------------------------------------------------------------\n");
        sleep(1);
    } while(DEBUG_MODE);

    printf("++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("                Bom trabalho!\n");
    printf("++++++++++++++++++++++++++++++++++++++++++++\n");
    
    if(DEBUG_MODE) pthread_exit(NULL);
}