/*
 * Universidade Federal do Rio de Janeiro
 * Laborat�rio de Instrumenta��o Oceanogr�fica
 * LIOc/COPPE/UFRJ
 *
 * Projeto: Datalogger para Bioac�stica Submarina
 *
 *      Created on: 15/12/2015
 *      Author: William Barbosa de Macedo
 *
 *      funcoes.h - arquivo header de funcoes.c
 */

#ifndef FUNCOES_H_
#define FUNCOES_H_
#include "../Cartao SD/ff.h"
#include "driverlib.h"

FIL file;                                               /* Opened file object */
FRESULT res;                                            /* Result object      */
UINT bytesRead;                                         /* Bytes read object  */
UINT read;                /* Comentar caso for usar Printf                               Read bytes object  */
DIRS dir;                                               /* Directory object   */
FATFS fatfs;                                            /* File system object */
FRESULT errCode,errCode1,errCode2,errCode3,errCode4;                                        /* Error code objects  */
unsigned int *bytesWritten;                             /* Ponteiro para n�mero de bytes escritos */
char nomedoarquivo[20];                                 /* Vari�vel que cont�m o nome do arquivo de dados */
unsigned int tempo;

/* Declara��o de Vari�veis Globais
 * tipoAquisicao: 1-Cont�nua / 2-Trigger Manual Programado
 * limiar: tensao de disparo do sistema, caso tipoAquisicao=2
 * tipoGravacao: 1-Circular(futuro), 2- Finita, 3 - Infinita (at� encher o cart�o)
 * modoAmostragem: 1-Modo1, 2-Modo2, 3-Modo3. Detalhados no modelo de cfg.txt
 * restricao: 1- Restricao por minutos (futuro), 2 - Restricao por Numero de Aquisicoes
 * numeroAquisicoes: numero de aquisicoes a serem realizadas
 * intervaloAquisicoes: tempo em minutos entre as aquisicoes
 * numeroMedicoes: numero de medicoes a serem realizadas
 * ganhoAmp: desenvolvimento futuro
 * FreqCorte: desenvolvimento futuro
 * numArquivoDados: contagem do n�mero de arquivos criados
 * maxAquisicoes: numero maximo de aquisicoes por arquivo. Depende do modo de amostragem.
 */


unsigned short int tipoAquisicao, limiar, tipoGravacao, modoAmostragem, restricao, intervaloAquisicoes, ganhoAmp, freqCorte, numArquivoDados;
unsigned long int maxAquisicoes, numeroAquisicoes, numeroMedicoes;

void initCLK(void);
void SetVcoreUp (unsigned int level);
FRESULT WriteFile(char*, char*, WORD);
void fat_init(void);
void USBHAL_initClocks(uint32_t mclkFreq);
void iniciarTimer(void);
void pot(void);


#endif /* FUNCOES_H_ */
