/*
 * Universidade Federal do Rio de Janeiro
 * Laboratório de Instrumentação Oceanográfica
 * LIOc/COPPE/UFRJ
 *
 * Projeto: Datalogger Ambiental Acústico
 *
 * - Módulo ADC - 12 Bits - F5529
 * 	 Aquisição e Conversão Digital de dado analógico da porta 6.0 (porta A0)
 *
 *      Created on: 03/06/2016
 *      Last Update on: 31/05/2017
 *      Author: William Barbosa de Macedo
 */

#include <adc.h>
#include <stdio.h>
#include "funcoes.h"

unsigned short int indiceVetor=0; /* indice para o vetor de resultados */


int adc_init1(void)

{
	GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7);
	/* Setting Pin 6.0  as ADC Input */
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN0);

	// Initialize ADC12 with ADC12’s built-in oscillator
	ADC12_A_init (ADC12_A_BASE,
	ADC12_A_SAMPLEHOLDSOURCE_SC,
	ADC12_A_CLOCKSOURCE_ADC12OSC,
	ADC12_A_CLOCKDIVIDER_1);  /* clock do ADC  */

	//Switch ON ADC12
	ADC12_A_enable(ADC12_A_BASE);

	  /*
	     * Base address of ADC12_A_A Module
	     * For memory buffers 0-7 sample/hold for 8 clock cycles
	     * For memory buffers 8-15 sample/hold for 8 clock cycles (default)
	     * Disable Multiple Sampling
	  */

	ADC12_A_setupSamplingTimer (ADC12_A_BASE,
			ADC12_A_CYCLEHOLD_4_CYCLES,
			ADC12_A_CYCLEHOLD_4_CYCLES,
			ADC12_A_MULTIPLESAMPLESDISABLE);  /* atenção. No modo Single Channel Single Conversion, deve estar setado em DISABLE */

	// Configure the Input to the Memory Buffer with the specified Reference Voltages
	ADC12_A_configureMemoryParam param = {0};
	param.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	param.inputSourceSelect = ADC12_A_INPUT_A0;
	param.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param);

	ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
	ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE0);

	return 0;
}

int adc_init2(void)

{
	GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7);
	/* Setting Pin 6.0  as ADC Input */
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN0);

	// Initialize ADC12 with ADC12’s built-in oscillator
	ADC12_A_init (ADC12_A_BASE,
	ADC12_A_SAMPLEHOLDSOURCE_SC,
	ADC12_A_CLOCKSOURCE_ADC12OSC,
	ADC12_A_CLOCKDIVIDER_1);  /* clock do ADC  */

	//Switch ON ADC12
	ADC12_A_enable(ADC12_A_BASE);

	  /*
	     * Base address of ADC12_A_A Module
	     * For memory buffers 0-7 sample/hold for 8 clock cycles
	     * For memory buffers 8-15 sample/hold for 8 clock cycles (default)
	     * Disable Multiple Sampling
	  */

	ADC12_A_setupSamplingTimer (ADC12_A_BASE,
			ADC12_A_CYCLEHOLD_4_CYCLES,
			ADC12_A_CYCLEHOLD_4_CYCLES,
	ADC12_A_MULTIPLESAMPLESENABLE);  /* atenção */

	// Configure the Input to the Memory Buffer with the specified Reference Voltages
	ADC12_A_configureMemoryParam param = {0};
	param.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	param.inputSourceSelect = ADC12_A_INPUT_A0;
	param.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param);

	ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
	ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE0);

	return 0;
}

int adc_init3(void)

{
	GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN7);
	GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7);
	/* Setting Pin 6.0  as ADC Input */
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6, GPIO_PIN0);

	// Initialize ADC12 with ADC12’s built-in oscillator
	ADC12_A_init (ADC12_A_BASE,
	ADC12_A_SAMPLEHOLDSOURCE_SC,
	ADC12_A_CLOCKSOURCE_SMCLK,
	ADC12_A_CLOCKDIVIDER_2);  /* clock do ADC  */

	//Switch ON ADC12
	ADC12_A_enable(ADC12_A_BASE);

	  /*
	     * Base address of ADC12_A_A Module
	     * For memory buffers 0-7 sample/hold for 8 clock cycles
	     * For memory buffers 8-15 sample/hold for 8 clock cycles (default)
	     * Disable Multiple Sampling
	  */

	ADC12_A_setupSamplingTimer (ADC12_A_BASE,
			ADC12_A_CYCLEHOLD_8_CYCLES,
			ADC12_A_CYCLEHOLD_8_CYCLES,
	ADC12_A_MULTIPLESAMPLESDISABLE);  /* atenção */

	// Configure the Input to the Memory Buffer with the specified Reference Voltages
	ADC12_A_configureMemoryParam param = {0};
	param.memoryBufferControlIndex = ADC12_A_MEMORY_0;
	param.inputSourceSelect = ADC12_A_INPUT_A0;
	param.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
	param.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
	param.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
	ADC12_A_configureMemory(ADC12_A_BASE ,&param);

	ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
	ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE0);

	return 0;
}


int get_adc1 (void)
{
	unsigned short int indiceVetor=0; /* indice para o vetor de resultados */
	ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
	zerar_vetor(vetorResultados);
	while (1)
	{
		// Start a single conversion, no repeating or sequences.
		ADC12_A_startConversion (ADC12_A_BASE,
				ADC12_A_MEMORY_0,
				ADC12_A_SINGLECHANNEL);    /* único canal, várias conversões */

		// Wait for the Interrupt Flag to assert at LPM0
        __bis_SR_register(LPM0_bits + GIE);

        if(indiceVetor < COMPRIMENTO_VETOR_RESULTADOS)
        {
        	vetorResultados[indiceVetor] = resultadoGlobal;
    		GPIO_toggleOutputOnPin(GPIO_PORT_P3,GPIO_PIN7);
			indiceVetor++;
			if(indiceVetor==COMPRIMENTO_VETOR_RESULTADOS)
			{
				return 0;
			}
        }
 		ADC12_A_clearInterrupt(ADC12_A_BASE,ADC12IFG0);

	}
}

int get_adc2 (void)
{
	ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
	zerar_vetor(vetorResultados);

	// Start a single conversion, no repeating or sequences.
	ADC12_A_startConversion (ADC12_A_BASE,
	ADC12_A_MEMORY_0,
	ADC12_A_REPEATED_SINGLECHANNEL);    /* único canal, várias conversões */

	// Wait for the Interrupt Flag to assert at LPM0
    __bis_SR_register(LPM0_bits + GIE);

	ADC12_A_clearInterrupt(ADC12_A_BASE,ADC12IFG0);

 	return 0;
}

int get_adc3 (void)
{
	unsigned short int indiceVetor=0; /* indice para o vetor de resultados */
	ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IFG0);
	zerar_vetor(vetorResultados);
	while (1)
	{
		/* colocar zerarvetor aqui? */

		// Start a single conversion, no repeating or sequences.
		ADC12_A_startConversion (ADC12_A_BASE,
				ADC12_A_MEMORY_0,
				ADC12_A_SINGLECHANNEL);    /* único canal, várias conversões */

		// Wait for the Interrupt Flag to assert at LPM0
        __bis_SR_register(LPM0_bits + GIE);

        if(indiceVetor < COMPRIMENTO_VETOR_RESULTADOS)
        {
        	vetorResultados[indiceVetor] = resultadoGlobal;
    		GPIO_toggleOutputOnPin(GPIO_PORT_P3,GPIO_PIN7);
			indiceVetor++;
			if(indiceVetor==COMPRIMENTO_VETOR_RESULTADOS)
			{
				return 0;
			}
        }
 		ADC12_A_clearInterrupt(ADC12_A_BASE,ADC12IFG0);

	}
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_A_ISR(void)                    /* Flag ADC12IFG0 gerou a interrupção, indicando que deve ser executada ISR ADC_A_ISR */
{
		switch(__even_in_range(ADC12IV,34))
	    {
	    case  0: break;       //Vector  0:  No interrupt
	    case  2: break;       //Vector  2:  ADC overflow
	    case  4: break;       //Vector  4:  ADC timing overflow
	    case  6:              //Vector  6:  ADC12IFG0
	    	if(modoAmostragem==2)
	    	{
	    		for(indiceVetor=0;indiceVetor<COMPRIMENTO_VETOR_RESULTADOS-1;indiceVetor++)
	    		{
	    			vetorResultados[indiceVetor] = (ADC12_A_getResults(ADC12_A_BASE,ADC12_A_MEMORY_0));
	    			GPIO_toggleOutputOnPin(GPIO_PORT_P3,GPIO_PIN7);
	    		}
     	 	 //tempo = Timer_A_getCounterValue(TIMER_A0_BASE);
	    		ADC12_A_disableConversions(ADC12_A_BASE, ADC12_A_PREEMPTCONVERSION);
	    		vetorResultados[indiceVetor] = (ADC12_A_getResults(ADC12_A_BASE,ADC12_A_MEMORY_0));
	    	}
	    	if(modoAmostragem == 1 || modoAmostragem == 3)
	    	{
		    	resultadoGlobal = (ADC12_A_getResults(ADC12_A_BASE,ADC12_A_MEMORY_0)); 	/* pegando os resultados e colocando na variável temporária de resultado */
	    	}
	    	//Exit active CPU
	    	__bic_SR_register_on_exit(LPM0_bits);
	    	break;

	    case  8: break;       //Vector  8:  ADC12IFG1
	    case 10: break;       //Vector 10:  ADC12IFG2
	    case 12: break;       //Vector 12:  ADC12IFG3
	    case 14: break;       //Vector 14:  ADC12IFG4
	    case 16: break;       //Vector 16:  ADC12IFG5
	    case 18: break;       //Vector 18:  ADC12IFG6
	    case 20: break;       //Vector 20:  ADC12IFG7
	    case 22: break;       //Vector 22:  ADC12IFG8
	    case 24: break;       //Vector 24:  ADC12IFG9
	    case 26: break;       //Vector 26:  ADC12IFG10
	    case 28: break;       //Vector 28:  ADC12IFG11
	    case 30: break;       //Vector 30:  ADC12IFG12
	    case 32: break;       //Vector 32:  ADC12IFG13
	    case 34: break;       //Vector 34:  ADC12IFG14
	    default: break;
	    }
}

unsigned int zerar_vetor(short int vetorResultados[COMPRIMENTO_VETOR_RESULTADOS])
{
	unsigned short int contador;
	indiceVetor = 0;
	for(contador=0;contador < COMPRIMENTO_VETOR_RESULTADOS;contador++)
	{
		vetorResultados[contador] = 0;
	}
	return 0;
}
