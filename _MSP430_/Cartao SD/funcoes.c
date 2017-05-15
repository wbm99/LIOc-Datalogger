/*
 * Universidade Federal do Rio de Janeiro
 * Laboratório de Instrumentação Oceanográfica
 * LIOc/COPPE/UFRJ
 *
 * Projeto: Datalogger para Bioacústica Submarina
 *
 *      Created on: 15/12/2015
 *      Author: William Barbosa de Macedo
 *
 *      funcoes.c - Arquivo com algumas funções necessárias à main
 */
#include "../Cartao SD/funcoes.h"
#include "ff.h"
#include "driverlib.h"
#include "../Cartao SD/ff.h"

unsigned char MST_Data,SLV_Data;
BYTE buffer[32];
int result=1;

/******************************************************************************************************************************************/
void fat_init (void)
{
    errCode = FR_NOT_READY;
    while (errCode != FR_OK)
    {                               //go until f_open returns FR_OK (function successful)
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        //errCode = f_opendir(&dir, "/");				    	//root directory
        errCode = f_open(&file, nomedoarquivo, FA_OPEN_ALWAYS | FA_WRITE);
        errCode = f_sync(&file);
        if(errCode != FR_OK)
            result=0;                                       //used as a debugging flag
    }
}

/******************************************************************************************************************************************/

/* FUNÇÃO QUE INICIALIZA O CLOCK DO SISTEMA. FEITA COM REGISTRADORES. TESTAR COM DRIVERLIB */

void initCLK(void)
{
  volatile unsigned int i;

  // Increase Vcore setting to level3 to support fsystem=25MHz  - UP TO 3.3 V
  // NOTE: Change core voltage one level at a time..
  SetVcoreUp (0x01);
  SetVcoreUp (0x02);
  SetVcoreUp (0x03);

  UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO  /* REFO = Reference for the DCO - Digitally Controled Oscillator */
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO /* ACLK = Auxiliary Clock */

  __bis_SR_register(SCG0);                  // Disable the FLL control loop  /* FLL = Frequency Locked Loop */
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
  UCSCTL2 = FLLD_1 + 762;                   // Set DCO Multiplier for 25MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (762 + 1) * 32768 = 25MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
  __delay_cycles(782000);

  // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);     // Clear XT2,XT1,DCO fault flags   /* XT - eXTernal oscillators */
    SFRIFG1 &= ~OFIFG;                              // Clear fault flags
  }while (SFRIFG1&OFIFG);                           // Test oscillator fault flag
}

/******************************************************************************************************************************************/

/* FUNÇÃO QUE ELEVA O NÍVEL DO VCORE DO SISTEMA */

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write  /* Power Management Module */
  PMMCTL0_H = PMMPW_H;
  // Set SVS/SVM high side new level    /* SVS Means Supervision */
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level   /* Core Voltage */
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}
/******************************************************************************************************************************************/

/*Função que inicia o Timer de 10 segundos da Main*/
void iniciarTimer(void)
{
	Timer_A_initContinuousModeParam initContParam = {0};

	initContParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
	initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_5;
	initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
	initContParam.timerClear = TIMER_A_DO_CLEAR;
	initContParam.startTimer = false;

	Timer_A_initContinuousMode(TIMER_A0_BASE, &initContParam);

	Timer_A_clearTimerInterrupt(TIMER_A0_BASE);

	Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);
}
/******************************************************************************************************************************************/

/* Função que implementa o timer de medição do período de amostragem do ADC */
/*void TimerADC(void)
{
	Timer_A_initContinuousModeParam initContParam = {0};

	initContParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
	initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_5;
	initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
	initContParam.timerClear = TIMER_A_DO_CLEAR;
	initContParam.startTimer = false;

	Timer_A_initContinuousMode(TIMER_A1_BASE, &initContParam);

	Timer_A_initCaptureModeParam initCapParam = {0};

	initCapParam.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
	initCapParam.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
	initCapParam.captureInputSelect =
	initCapParam.synchronizeCaptureSource =
	initCapParam.captureInterruptEnable =
	initCapParam.captureOutputMode =
}
*/
