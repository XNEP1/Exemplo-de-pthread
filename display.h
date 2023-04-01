/* Trabalho T3 de Bruno Krügel.
 * 2021.
 * Funções relacionadas a mostrar informações na tela. 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <string.h>

#include "main.h"

struct imprimir_infos_arg{
    struct equipe *equipe;
    struct dados_vacinacao *registro_dose_1;
    struct dados_vacinacao *registro_dose_2;
    t_filaP *fila_dose_1;
    t_filaP *fila_dose_2;
    int *restantes_dose_1;
    int *restantes_dose_2;
    int *mau_humorados_dose_1;
    int *mau_humorados_dose_2;
};

/* Usado como função assincrona, caso DEBUG_MODE seja 1. */
/* Use a struct imprimir_infos_arg para passar os argumentos. */
void *imprimir_infos(void *arg);

#endif