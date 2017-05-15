/*
 * Universidade Federal do Rio de Janeiro
 * Laboratório de Instrumentação Oceanográfica
 * LIOc/COPPE/UFRJ
 *
 * Projeto: Datalogger para Bioacústica Submarina
 *
 * - Header - Módulo ADC - 12 Bits - F5529
 *
 *      Created on: 03/06/2016
 *      Last Update on: 27/09/2016
 *      Author: William Barbosa de Macedo
 */

#ifndef CONVERSOR_AD_ADC_H_
#define CONVERSOR_AD_ADC_H_

/* Defina quantas amostras irão ser capturadas antes da transmissão ao Cartão S/D. Observação: Por enquanto só está programado o envio de um único vetor de amostras */
#define COMPRIMENTO_VETOR_RESULTADOS                                2048

#include "driverlib.h"
#include <stdint.h>

short int resultadoGlobal; /* Vetor de resultados. Captura o valor de 0 a 4095 lido da porta analógica. Para passar um dado da ISR para o código normal, a variável deve ser global */
short int vetorResultados[COMPRIMENTO_VETOR_RESULTADOS];    /* ADC: vetor de resultados de 0 a COMPRIMENTO_VETOR_RESULTADOS-1. Variável global. */

/*
 * amostra dividida em inteiro e decimal para evitar float - consumo de memoria
 *  typedef struct amostra{
 *		unsigned char inteiro;
 *		int decimal;
 *  } partes;
 *
 *partes amostra;
*/


int adc_init1(void);   /* faz as configurações iniciais para o ADC - Modo1 */
int adc_init2(void);   /* faz as configurações iniciais para o ADC - Modo2 */
int adc_init3(void);   /* faz as configurações iniciais para o ADC - Modo3 */

int get_adc1(void);    /* realiza a aquisição e guarda na variável global vetorResultados */
int get_adc2(void);    /* realiza a aquisição e guarda na variável global vetorResultados */
int get_adc3(void);    /* realiza a aquisição e guarda na variável global vetorResultados */

unsigned int zerar_vetor(short int vetorResultados[COMPRIMENTO_VETOR_RESULTADOS]);  /* zera o vetor de resultados para uma nova aquisição de 2048 amostragens */


#endif /* CONVERSOR_AD_ADC_H_ */
