/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : 定时器例子
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH58x_common.h"

__attribute__((aligned(4))) uint32_t CapBuf[100];
__attribute__((aligned(4))) uint32_t PwmBuf[100];

volatile uint8_t capFlag = 0;

void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

int main()
{
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    /* 配置串口调试 */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

    GPIOB_SetBits(GPIO_Pin_13);
    GPIOB_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA);

    TMR0_TimerInit(FREQ_SYS / 1);         // 设置定时时间 100ms
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END); // 开启中断
    PFIC_EnableIRQ(TMR0_IRQn);

    while(1);
}


__INTERRUPT
__HIGH_CODE
void TMR0_IRQHandler(void) // TMR0 定时中断
{
    if(TMR0_GetITFlag(TMR0_3_IT_CYC_END))
    {
        TMR0_ClearITFlag(TMR0_3_IT_CYC_END); // 清除中断标志
        GPIOB_InverseBits(GPIO_Pin_13);
    }
}
