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

#if 1 /* 定时器0，设定100ms定时器进行IO口闪灯， PB15-LED */

    GPIOB_SetBits(GPIO_Pin_13);
    GPIOB_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA);

    TMR0_TimerInit(FREQ_SYS / 1);         // 设置定时时间 100ms
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END); // 开启中断
    PFIC_EnableIRQ(TMR0_IRQn);
#endif

#if 0 /* 定时器3，PWM输出 */

    GPIOB_ResetBits(GPIO_Pin_22); // 配置PWM口 PB22
    GPIOB_ModeCfg(GPIO_Pin_22, GPIO_ModeOut_PP_5mA);

    TMR3_PWMInit(High_Level, PWM_Times_1);
    TMR3_PWMCycleCfg(60 * 100); // 周期 100us  最大67108864
    TMR3_PWMActDataWidth(3000); // 占空比 50%, 修改占空比必须暂时关闭定时器
    TMR3_PWMEnable();
    TMR3_Enable();

#endif

#if 0                                      /* 定时器1，CAP捕捉， */
    PWR_UnitModCfg(DISABLE, UNIT_SYS_LSE); // 注意此引脚是LSE晶振引脚，要保证关闭才能使用其他功能
    GPIOA_ResetBits(GPIO_Pin_10);          // 配置PWM口 PA10
    GPIOA_ModeCfg(GPIO_Pin_10, GPIO_ModeIN_PU);

    TMR1_CapInit(Edge_To_Edge);
    TMR1_CAPTimeoutCfg(0xFFFFFFFF); // 设置捕捉超时时间
    TMR1_DMACfg(ENABLE, (uint16_t)(uint32_t)&CapBuf[0], (uint16_t)(uint32_t)&CapBuf[100], Mode_Single);
    TMR1_ITCfg(ENABLE, TMR1_2_IT_DMA_END); // 开启DMA完成中断
    PFIC_EnableIRQ(TMR1_IRQn);

    while(capFlag == 0);
    capFlag = 0;
    for(uint8_t i = 0; i < 100; i++)
    {
        PRINT("%08ld ", CapBuf[i] & 0x1ffffff); // 26bit, 最高位表示 高电平还是低电平
    }
    PRINT("\n");

#endif

#if 0 /* 定时器2，计数器， */
    GPIOB_ModeCfg(GPIO_Pin_11, GPIO_ModeIN_PD);
    GPIOPinRemap(ENABLE, RB_PIN_TMR2);

    TMR2_EXTSingleCounterInit(FallEdge_To_FallEdge);
    TMR2_CountOverflowCfg(1000); // 设置计数上限1000

    /* 开启计数溢出中断，计慢1000个周期进入中断 */
    TMR2_ClearITFlag(TMR0_3_IT_CYC_END);
    PFIC_EnableIRQ(TMR2_IRQn);
    TMR2_ITCfg(ENABLE, TMR0_3_IT_CYC_END);

    do
    {
        /* 1s打印一次当前计数值，如果送入脉冲频率较高，可能很快计数溢出，需要按实际情况修改 */
        mDelaymS(1000);
        PRINT("=%ld \n", TMR2_GetCurrentCount());
    } while(1);

#endif

#if 0 /* 定时器2,DMA PWM.*/
    GPIOB_ModeCfg(GPIO_Pin_11, GPIO_ModeOut_PP_5mA);
    GPIOPinRemap(ENABLE, RB_PIN_TMR2);

    PRINT("TMR2 DMA PWM\n");
    TMR2_PWMCycleCfg(60 * 2000); // 周期 2000us  主频是60Mhz 每秒震荡60M次 震荡60次为1微秒
    for(uint8_t i=0; i<50; i++)
    {
      PwmBuf[i]=2400*i;
    }
    for(i=50; i<100; i++)
    {
      PwmBuf[i]=2400*(100-i);
    }

    TMR2_PWMInit(Low_Level, PWM_Times_16);
    TMR2_DMACfg(ENABLE, (uint16_t)(uint32_t)&PwmBuf[0], (uint16_t)(uint32_t)&PwmBuf[100], Mode_LOOP);
    TMR2_PWMEnable();
    TMR2_Enable();
    /* 开启计数溢出中断，计满100个周期进入中断 */
    TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
    TMR2_ITCfg(ENABLE, TMR1_2_IT_DMA_END);
    PFIC_EnableIRQ(TMR2_IRQn);

#endif

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

__INTERRUPT
__HIGH_CODE
void TMR1_IRQHandler(void) // TMR1 定时中断
{
    if(TMR1_GetITFlag(TMR1_2_IT_DMA_END))
    {
        TMR1_ITCfg(DISABLE, TMR1_2_IT_DMA_END); // 使用单次DMA功能+中断，注意完成后关闭此中断使能，否则会一直上报中断。
        TMR1_ClearITFlag(TMR1_2_IT_DMA_END);    // 清除中断标志
        capFlag = 1;
    }
}

__INTERRUPT
__HIGH_CODE
void TMR2_IRQHandler(void)
{
    if(TMR2_GetITFlag(TMR0_3_IT_CYC_END))
    {
        TMR2_ClearITFlag(TMR0_3_IT_CYC_END);
        /* 计数器计满，硬件自动清零，重新开始计数 */
        /* 用户可自行添加需要的处理 */
    }
    if(TMR2_GetITFlag(TMR1_2_IT_DMA_END))
    {
        TMR2_ClearITFlag(TMR1_2_IT_DMA_END);
        PRINT("DMA end\n");
        /* DMA 结束 */
        /* 用户可自行添加需要的处理 */
    }
}
