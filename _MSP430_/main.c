/*
 * Universidade Federal do Rio de Janeiro
 * Laborat�rio de Instrumenta��o Oceanogr�fica
 * LIOc/COPPE/UFRJ
 *
 * Projeto: Datalogger para Bioac�stica Submarina
 *
 *      Created on: 10/01/2017
 *      Last Update on: 31/05/2017
 *      Author: William Barbosa de Macedo
 *
 *      Obs.: Coment�rios originais dos c�digos da Texas Instruments em ingl�s. Coment�rios do autor em portugu�s.
 */

/* Includes do Sistema */
#include <stdio.h> 										   /* Biblioteca padr�o de I/O */
#include <stdlib.h>                                        /* Biblioteca padr�o */
#include <string.h>                                        /* Manipula��o de strings */

/* Meus includes */
#include "Conversor AD/adc.h" 						/* Refer�ncias do ADC criadas */
#include "driverlib.h"								/* Biblioteca do Microcontrolador MSP430 com fun��es para mais f�cil acesso */
#include "funcoes.h" 					            /* Fun��es criadas para esse programa */
#include "Cartao SD/funcoes.h" 						/* Redund�ncia criada automaticamente pelo CCS */
#include "ff.h"                                     /* Fun��es da fatFS para manipula��o do cart�o SD */
#include "Cartao SD/ff.h"                           /* Redund�ncia criada automaticamente pelo CCS */

/* Declara��o das fun��es principais com base no fluxograma */
int preparacao(void);
int pre_aquisicao(int *novoArquivo);
int aquisicao(void);
int finalizacao(int *cartaoCheio);

/* Fun��es auxiliares */
int arquivoAtual(char*);  						    	 /* atualiza o nome do arquivo atual. Ex: dados23 */

int main(void)
{
	/* Desativa o watchdog (padr�o) */
	WDTCTL = WDTPW+WDTHOLD;                              /* Stop watchdog timer */

	numArquivoDados = 0; 								 /* inicializando vari�vel */
	int novoArquivo = 0, cartaoCheio;                    /* se novoArquivo=0, o software est� abrindo o seu primeiro arquivo */

	preparacao();

	while(!(novoArquivo))
	{
		/* se pre_aquisicao retornar 3 (escolhido), significa que o cart�o est� cheio, logo, n�o devo realizar aquisi��o. O par�metro novoArquivo informa a pre_aquisicao,
		   se o software est� abrindo o seu primeiro arquivo, necessitando ou n�o de configura��es iniciais do Conversor AD */
		if((pre_aquisicao(&novoArquivo)) != 3)
				aquisicao();
		else
			cartaoCheio = 1;
		if(finalizacao(&cartaoCheio) == 1)
			novoArquivo = 0;
		else
		novoArquivo = 1; /* se finalizacao retornar 1, significa que n�o h� mais medi��es a serem feitas, seja porque foi alcan�ado o objetivo da configura��o, ou porque o cart�o encheu 100%. */
	}

	return 0;
} /* fim da main */


int preparacao(void)
{
	/* inicio da rotina 'inicializacao' */
	int result=1;
	unsigned int cont;                                      /* contador */
    BYTE buffer[50], buffer2[10];  						    /* Buffer para receber os dados do arquivo de configura��o. Buffer2: vari�vel auxiliar na captura de param de varios digitos */
    UINT *bytesRead;    									/* necess�rio em f_read. retorna n�mero de bytes lidos */

    initCLK();                                              /* Inicia os Clocks - Todos no m�ximo. */

    __enable_interrupt(); 					            	/* enabling interrupts */
    errCode = FR_NOT_READY; 								/* Vari�el de resultado para as fun��es fatFS. Setada inicialmente como NOT READY */

    while (errCode != FR_OK)
    {                                                       //go until f_open returns FR_OK (function successful)
    	errCode = f_mount(0, &fatfs);                       //mount drive number 0 - cart�o montado
        errCode = f_open(&file, "cfg.txt", FA_READ);	    //abre arquivo de configura��o. Somente leitura
        errCode = f_sync(&file);							//f_sync no arquivo de configura��o (opcional. garante mais seguran�a na abertura do arquivo)

        for(cont=0;cont<50;cont++)                          /* clearing the buffer vector, avoiding mistakes within old entries */
        {
        	buffer[cont] = '\0';
        }

        for(cont=0;cont<10;cont++)
        {
        	buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
        }

        /* aqui entrar�o as rotinas de carregamento dos dados do arquivo de configura��o. Ler do cfg.txt, armazenar em vari�veis locais ou globais e FECHAR o arquivo posteriormente */
		f_read(&file, buffer, sizeof(buffer), bytesRead);

		/* Nas linhas a seguir, fazemos a transi��o dos dados advindos do arquivo de cfg para as vari�veis do sistema, efetivamente. Fazemos ent�o a convers�o de tipo de char para int */
		tipoAquisicao = buffer[0]-48; 					    /* Convers�o simples de char para int (subtraindo 48 do caractere) */
		unsigned short int i=3,j=0;                         /* indexador do buffer para detectar fim de linha e consequente fim do par�metro de mais de um d�gito */
		char limiar_str[10];

		while(buffer[i] != '\n')                            /* procura pelo \n no buffer que cont�m todos os dados */
		{
			buffer2[j] = buffer[i];                         /* preenche um buffer auxiliar com os d�gitos da vari�vel em quest�o */
			i++;
			j++;
		}

		strncpy(limiar_str, buffer2,j+1); 					/* preenche outro buffer auxiliar com os d�gitos da vari�vel em quest�o */

		limiar = atoi(limiar_str); 							/* converte de string para inteiro */
		i++;
		tipoGravacao = buffer[i]-48;
		modoAmostragem = buffer[i+3]-48;
		modoAmostragem = 1; /* PARA DEBUG. RETIRAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		restricao = buffer[i+6]-48;
		i=15; j=0; 											/* indexador do buffer para detectar fim de linha e consequente fim do par�metro de mais de um d�gito */

		char numeroAquisicoes_str[10], intervaloAquisicoes_str[10], numeroMedicoes_str[10]; /* vari�vel auxiliar para receber par�metro de v�rios d�gitos*/
		char *ptr, *ptr2;                                   /* necess�rio em strtol */

		for(cont=0;cont<10;cont++)
		{
			buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
		}

		while(buffer[i] != '\n')  							/* procura pelo \n no buffer que cont�m todos os dados */
		{
			buffer2[j] = buffer[i]; 						/* preenche um buffer auxiliar com os d�gitos da vari�vel em quest�o */
		    i++;
		    j++;
		}

		strncpy(numeroAquisicoes_str,buffer2,j+1); 					/* preenche outro buffer auxiliar com os d�gitos da vari�vel em quest�o */
		numeroAquisicoes = strtol(numeroAquisicoes_str, &ptr, 10); 	/* converte de string para long */
		i++;
		j=0;
		for(cont=0;cont<10;cont++)
		{
			buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
		}

		while(buffer[i] != '\n') 							/* an�logo � busca acima */
		{
			buffer2[j] = buffer[i];
			i++;
			j++;
		}

		strncpy(intervaloAquisicoes_str,buffer2,j+1);
		intervaloAquisicoes = atoi(intervaloAquisicoes_str);
		i++;
		ganhoAmp = buffer[i]-48;
		freqCorte = buffer[i+3]-48;
		i=i+6;  											/* reposicionando o �ndice do vetor. Para entender melhor: observar a composi��o de buffer */
		j=0;

		for(cont=0;cont<10;cont++)
		{
			buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
		}

		while(buffer[i] != '\n') 							/* an�logo � busca acima */
		{
			buffer2[j] = buffer[i];
			i++;
			j++;
		}
		strncpy(numeroMedicoes_str,buffer2,j+1);
		numeroMedicoes = strtol(numeroMedicoes_str, &ptr2,10);
		//numeroMedicoes = 20;  /* for�ando encher o cart�o */

		/* Os par�metros s�o separados por 2 bytes ENTRE SI. Isto decorre pois s�o separados por \n\c no arquivo .txt. Arquivo Dos\Windows */
		/* Par�metros de mais de um d�gito (ex: 15, 150) passam por uma l�gica de procura ao \n, para ser indexado na vari�vel inteira do c�digo e ser manipulado */

		errCode = f_close(&file);   						/* fechando arquivo de configura��o */

        if(errCode != FR_OK)
        result=0;                                           //used as a debugging flag
    }

  /* fim da inicializacao */

	return 0;
} /* fim da prepara��o */


int pre_aquisicao(int *novoArquivo)
{
	unsigned long int resultArmazenamento;
	int result=1;

	maxAquisicoes = 100000;					 				/* a ser determinado! */

	if(!(*novoArquivo))
	{
		initCLK();        									/* Inicia os Clocks - Todos no m�ximo. */
		/* inicio config. aquisicao */

		/* Configurando par�metros de aquisicao, conforme o modo de aquisi��o (taxa de amostragem, tempo de hold, clock, ...) especificado em cfg.txt */
		if(modoAmostragem)
		{
			adc_init1();
		}

		if(modoAmostragem==2)
		{
			adc_init2();
		}

		if(modoAmostragem==3)
		{
			adc_init3();
		}
	}

    /* Aqui dever� entrar o teste de mem�ria no cart�o */
	/* if(naotemmemoria): return 3; propagar os returns at� chegar em set clock normal e fim. */
	/* Usar f_getfree ou fazer a conta? Analisar desempenho */
	resultArmazenamento = numeroAquisicoes * numArquivoDados;
	if((resultArmazenamento) > 1500000) /* Resultado verdadeiro significa que foram preenchidos 6GB. Valor de seguran�a para um cart�o de 8GB. Cada aquisi��o tem 4KB */
		return 3;  /* CART�O CHEIO */

    /* Se tipo2: loop de teste de limiar. Desenvolvimento futuro: implementa��o com interrup��o */
    if(tipoAquisicao==2)
    {
    	if(modoAmostragem==1)
    	{
    		do
    		{
    			get_adc1();
    		} while(vetorResultados[1000] < limiar);  /* Teste de limiar com uma das amostras. Posi��o 1000 das 2048 entradas. */
    	}
    	if(modoAmostragem==1)
    	{
    		do
    	    {
    			get_adc2();
    	    } while(vetorResultados[1000] < limiar);  /* Teste de limiar com uma das amostras. Posi��o 1000 das 2048 entradas. */
    	}
    	if(modoAmostragem==1)
    	{
    		do
    		{
    		    get_adc3();
    		} while(vetorResultados[1000] < limiar);  /* Teste de limiar com uma das amostras. Posi��o 1000 das 2048 entradas. */
    	}
    }

    /* fim config. aquisicao */

    errCode = FR_NOT_READY;
    arquivoAtual(nomedoarquivo);  /* atualizando nome do arquivo de dados atual */


    /* abrindo arquivo de dados */
    while (errCode != FR_OK)
    {                               											//go until f_open returns FR_OK (function successful)
    	errCode = f_open(&file, nomedoarquivo, FA_OPEN_ALWAYS | FA_WRITE);	    //abre arquivo de dados, para escrita
        errCode = f_sync(&file);
        if(errCode != FR_OK)
        	result=0;                                       				    //used as a debugging flag
    }
	return 0;
} // Fim da pr�-aquisi��o


int aquisicao(void)
{
	unsigned long int contador=0;
	unsigned int int_size;                                  /* Vari�vel para guardar tamanho da amostra a ser gravada */
	int_size = sizeof(vetorResultados);

	/* Cada vez que a fun��o get_adcX � executada, ela preenche a vari�vel vetorResultados com 2048 amostras. Logo em seguida, se faz necess�rio o armazenamento no microSD */

	if(modoAmostragem==1)
	{
		while((numeroAquisicoes > contador) && (maxAquisicoes > contador))
		{
			get_adc1();
		    f_write(&file, (void*) vetorResultados, int_size , bytesWritten);
			contador++; /* sinaliza o fim de uma aquisi��o */
		}
	}
	if(modoAmostragem==2)
	{
		while((numeroAquisicoes > contador) && (maxAquisicoes > contador))
		{
			get_adc2();
		    f_write(&file, (void*) vetorResultados, int_size , bytesWritten);
			contador++; /* sinaliza o fim de uma aquisi��o */
		}
	}
	if(modoAmostragem==3)
	{
		while((numeroAquisicoes > contador) && (maxAquisicoes > contador))
		{
			get_adc3();
		    f_write(&file, (void*) vetorResultados, int_size , bytesWritten);
			contador++; /* sinaliza o fim de uma aquisi��o */
		}
	}


		//if(contador==1000 || contador==10000 || contador==30000 || contador ==50000) // TESTE
		//	contador = contador; // debug
	return 0;
} /* Fim da aquisi��o */

int finalizacao(int *cartaoCheio)
{
	if((*cartaoCheio) !=1){
		errCode = f_close(&file);								/* close the file */
		if(numArquivoDados < numeroMedicoes)   /* Retorna para preAquisicao se ainda faltam arquivos a serem gravados. Caso contr�rio, desmonta cart�o. Falta Desenvolvimento abaixo */
			return 1;
	}
	/* aqui dever� entrar a rotina de 'set clock normal' */
	/* in�cio da rotina 'fim' */

	/* check erros */
	/* escreve no arquivo de log de erros. Escrever ao longo do programa ou ao final? */

	errCode = f_mount(0,0);                                	   /* desmonta o cart�o SD */
	return 0;
} /* Fim da finaliza��o */


int arquivoAtual(char nomedoarquivo[20]) /* atualiza o nome do arquivo atual. Ex: dados23 */
{
	numArquivoDados++;
	sprintf(nomedoarquivo, "dados%d",numArquivoDados);
	return 0;
}
