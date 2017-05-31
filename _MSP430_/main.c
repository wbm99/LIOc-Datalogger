/*
 * Universidade Federal do Rio de Janeiro
 * Laboratório de Instrumentação Oceanográfica
 * LIOc/COPPE/UFRJ
 *
 * Projeto: Datalogger para Bioacústica Submarina
 *
 *      Created on: 10/01/2017
 *      Last Update on: 31/05/2017
 *      Author: William Barbosa de Macedo
 *
 *      Obs.: Comentários originais dos códigos da Texas Instruments em inglês. Comentários do autor em português.
 */

/* Includes do Sistema */
#include <stdio.h> 										   /* Biblioteca padrão de I/O */
#include <stdlib.h>                                        /* Biblioteca padrão */
#include <string.h>                                        /* Manipulação de strings */

/* Meus includes */
#include "Conversor AD/adc.h" 						/* Referências do ADC criadas */
#include "driverlib.h"								/* Biblioteca do Microcontrolador MSP430 com funções para mais fácil acesso */
#include "funcoes.h" 					            /* Funções criadas para esse programa */
#include "Cartao SD/funcoes.h" 						/* Redundância criada automaticamente pelo CCS */
#include "ff.h"                                     /* Funções da fatFS para manipulação do cartão SD */
#include "Cartao SD/ff.h"                           /* Redundância criada automaticamente pelo CCS */

/* Declaração das funções principais com base no fluxograma */
int preparacao(void);
int pre_aquisicao(int *novoArquivo);
int aquisicao(void);
int finalizacao(int *cartaoCheio);

/* Funções auxiliares */
int arquivoAtual(char*);  						    	 /* atualiza o nome do arquivo atual. Ex: dados23 */

int main(void)
{
	/* Desativa o watchdog (padrão) */
	WDTCTL = WDTPW+WDTHOLD;                              /* Stop watchdog timer */

	numArquivoDados = 0; 								 /* inicializando variável */
	int novoArquivo = 0, cartaoCheio;                    /* se novoArquivo=0, o software está abrindo o seu primeiro arquivo */

	preparacao();

	while(!(novoArquivo))
	{
		/* se pre_aquisicao retornar 3 (escolhido), significa que o cartão está cheio, logo, não devo realizar aquisição. O parâmetro novoArquivo informa a pre_aquisicao,
		   se o software está abrindo o seu primeiro arquivo, necessitando ou não de configurações iniciais do Conversor AD */
		if((pre_aquisicao(&novoArquivo)) != 3)
				aquisicao();
		else
			cartaoCheio = 1;
		if(finalizacao(&cartaoCheio) == 1)
			novoArquivo = 0;
		else
		novoArquivo = 1; /* se finalizacao retornar 1, significa que não há mais medições a serem feitas, seja porque foi alcançado o objetivo da configuração, ou porque o cartão encheu 100%. */
	}

	return 0;
} /* fim da main */


int preparacao(void)
{
	/* inicio da rotina 'inicializacao' */
	int result=1;
	unsigned int cont;                                      /* contador */
    BYTE buffer[50], buffer2[10];  						    /* Buffer para receber os dados do arquivo de configuração. Buffer2: variável auxiliar na captura de param de varios digitos */
    UINT *bytesRead;    									/* necessário em f_read. retorna número de bytes lidos */

    initCLK();                                              /* Inicia os Clocks - Todos no máximo. */

    __enable_interrupt(); 					            	/* enabling interrupts */
    errCode = FR_NOT_READY; 								/* Variáel de resultado para as funções fatFS. Setada inicialmente como NOT READY */

    while (errCode != FR_OK)
    {                                                       //go until f_open returns FR_OK (function successful)
    	errCode = f_mount(0, &fatfs);                       //mount drive number 0 - cartão montado
        errCode = f_open(&file, "cfg.txt", FA_READ);	    //abre arquivo de configuração. Somente leitura
        errCode = f_sync(&file);							//f_sync no arquivo de configuração (opcional. garante mais segurança na abertura do arquivo)

        for(cont=0;cont<50;cont++)                          /* clearing the buffer vector, avoiding mistakes within old entries */
        {
        	buffer[cont] = '\0';
        }

        for(cont=0;cont<10;cont++)
        {
        	buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
        }

        /* aqui entrarão as rotinas de carregamento dos dados do arquivo de configuração. Ler do cfg.txt, armazenar em variáveis locais ou globais e FECHAR o arquivo posteriormente */
		f_read(&file, buffer, sizeof(buffer), bytesRead);

		/* Nas linhas a seguir, fazemos a transição dos dados advindos do arquivo de cfg para as variáveis do sistema, efetivamente. Fazemos então a conversão de tipo de char para int */
		tipoAquisicao = buffer[0]-48; 					    /* Conversão simples de char para int (subtraindo 48 do caractere) */
		unsigned short int i=3,j=0;                         /* indexador do buffer para detectar fim de linha e consequente fim do parâmetro de mais de um dígito */
		char limiar_str[10];

		while(buffer[i] != '\n')                            /* procura pelo \n no buffer que contém todos os dados */
		{
			buffer2[j] = buffer[i];                         /* preenche um buffer auxiliar com os dígitos da variável em questão */
			i++;
			j++;
		}

		strncpy(limiar_str, buffer2,j+1); 					/* preenche outro buffer auxiliar com os dígitos da variável em questão */

		limiar = atoi(limiar_str); 							/* converte de string para inteiro */
		i++;
		tipoGravacao = buffer[i]-48;
		modoAmostragem = buffer[i+3]-48;
		modoAmostragem = 1; /* PARA DEBUG. RETIRAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		restricao = buffer[i+6]-48;
		i=15; j=0; 											/* indexador do buffer para detectar fim de linha e consequente fim do parâmetro de mais de um dígito */

		char numeroAquisicoes_str[10], intervaloAquisicoes_str[10], numeroMedicoes_str[10]; /* variável auxiliar para receber parâmetro de vários dígitos*/
		char *ptr, *ptr2;                                   /* necessário em strtol */

		for(cont=0;cont<10;cont++)
		{
			buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
		}

		while(buffer[i] != '\n')  							/* procura pelo \n no buffer que contém todos os dados */
		{
			buffer2[j] = buffer[i]; 						/* preenche um buffer auxiliar com os dígitos da variável em questão */
		    i++;
		    j++;
		}

		strncpy(numeroAquisicoes_str,buffer2,j+1); 					/* preenche outro buffer auxiliar com os dígitos da variável em questão */
		numeroAquisicoes = strtol(numeroAquisicoes_str, &ptr, 10); 	/* converte de string para long */
		i++;
		j=0;
		for(cont=0;cont<10;cont++)
		{
			buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
		}

		while(buffer[i] != '\n') 							/* análogo à busca acima */
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
		i=i+6;  											/* reposicionando o índice do vetor. Para entender melhor: observar a composição de buffer */
		j=0;

		for(cont=0;cont<10;cont++)
		{
			buffer2[cont] = '\0';							/* clearing the auxiliary buffer vector */
		}

		while(buffer[i] != '\n') 							/* análogo à busca acima */
		{
			buffer2[j] = buffer[i];
			i++;
			j++;
		}
		strncpy(numeroMedicoes_str,buffer2,j+1);
		numeroMedicoes = strtol(numeroMedicoes_str, &ptr2,10);
		//numeroMedicoes = 20;  /* forçando encher o cartão */

		/* Os parâmetros são separados por 2 bytes ENTRE SI. Isto decorre pois são separados por \n\c no arquivo .txt. Arquivo Dos\Windows */
		/* Parâmetros de mais de um dígito (ex: 15, 150) passam por uma lógica de procura ao \n, para ser indexado na variável inteira do código e ser manipulado */

		errCode = f_close(&file);   						/* fechando arquivo de configuração */

        if(errCode != FR_OK)
        result=0;                                           //used as a debugging flag
    }

  /* fim da inicializacao */

	return 0;
} /* fim da preparação */


int pre_aquisicao(int *novoArquivo)
{
	unsigned long int resultArmazenamento;
	int result=1;

	maxAquisicoes = 100000;					 				/* a ser determinado! */

	if(!(*novoArquivo))
	{
		initCLK();        									/* Inicia os Clocks - Todos no máximo. */
		/* inicio config. aquisicao */

		/* Configurando parâmetros de aquisicao, conforme o modo de aquisição (taxa de amostragem, tempo de hold, clock, ...) especificado em cfg.txt */
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

    /* Aqui deverá entrar o teste de memória no cartão */
	/* if(naotemmemoria): return 3; propagar os returns até chegar em set clock normal e fim. */
	/* Usar f_getfree ou fazer a conta? Analisar desempenho */
	resultArmazenamento = numeroAquisicoes * numArquivoDados;
	if((resultArmazenamento) > 1500000) /* Resultado verdadeiro significa que foram preenchidos 6GB. Valor de segurança para um cartão de 8GB. Cada aquisição tem 4KB */
		return 3;  /* CARTÃO CHEIO */

    /* Se tipo2: loop de teste de limiar. Desenvolvimento futuro: implementação com interrupção */
    if(tipoAquisicao==2)
    {
    	if(modoAmostragem==1)
    	{
    		do
    		{
    			get_adc1();
    		} while(vetorResultados[1000] < limiar);  /* Teste de limiar com uma das amostras. Posição 1000 das 2048 entradas. */
    	}
    	if(modoAmostragem==1)
    	{
    		do
    	    {
    			get_adc2();
    	    } while(vetorResultados[1000] < limiar);  /* Teste de limiar com uma das amostras. Posição 1000 das 2048 entradas. */
    	}
    	if(modoAmostragem==1)
    	{
    		do
    		{
    		    get_adc3();
    		} while(vetorResultados[1000] < limiar);  /* Teste de limiar com uma das amostras. Posição 1000 das 2048 entradas. */
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
} // Fim da pré-aquisição


int aquisicao(void)
{
	unsigned long int contador=0;
	unsigned int int_size;                                  /* Variável para guardar tamanho da amostra a ser gravada */
	int_size = sizeof(vetorResultados);

	/* Cada vez que a função get_adcX é executada, ela preenche a variável vetorResultados com 2048 amostras. Logo em seguida, se faz necessário o armazenamento no microSD */

	if(modoAmostragem==1)
	{
		while((numeroAquisicoes > contador) && (maxAquisicoes > contador))
		{
			get_adc1();
		    f_write(&file, (void*) vetorResultados, int_size , bytesWritten);
			contador++; /* sinaliza o fim de uma aquisição */
		}
	}
	if(modoAmostragem==2)
	{
		while((numeroAquisicoes > contador) && (maxAquisicoes > contador))
		{
			get_adc2();
		    f_write(&file, (void*) vetorResultados, int_size , bytesWritten);
			contador++; /* sinaliza o fim de uma aquisição */
		}
	}
	if(modoAmostragem==3)
	{
		while((numeroAquisicoes > contador) && (maxAquisicoes > contador))
		{
			get_adc3();
		    f_write(&file, (void*) vetorResultados, int_size , bytesWritten);
			contador++; /* sinaliza o fim de uma aquisição */
		}
	}


		//if(contador==1000 || contador==10000 || contador==30000 || contador ==50000) // TESTE
		//	contador = contador; // debug
	return 0;
} /* Fim da aquisição */

int finalizacao(int *cartaoCheio)
{
	if((*cartaoCheio) !=1){
		errCode = f_close(&file);								/* close the file */
		if(numArquivoDados < numeroMedicoes)   /* Retorna para preAquisicao se ainda faltam arquivos a serem gravados. Caso contrário, desmonta cartão. Falta Desenvolvimento abaixo */
			return 1;
	}
	/* aqui deverá entrar a rotina de 'set clock normal' */
	/* início da rotina 'fim' */

	/* check erros */
	/* escreve no arquivo de log de erros. Escrever ao longo do programa ou ao final? */

	errCode = f_mount(0,0);                                	   /* desmonta o cartão SD */
	return 0;
} /* Fim da finalização */


int arquivoAtual(char nomedoarquivo[20]) /* atualiza o nome do arquivo atual. Ex: dados23 */
{
	numArquivoDados++;
	sprintf(nomedoarquivo, "dados%d",numArquivoDados);
	return 0;
}
