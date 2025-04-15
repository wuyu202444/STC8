//转向灯 = UART + 流水灯
//输入l	左转
//输入r	右转
//输入s	关闭

#include "Config.h"
#include "GPIO.h"
#include "UART.h"
#include "NVIC.h"
#include "Switch.h"
#include "Timer.h"
#include "Delay.h"

#define	LED_SW	P45
#define LED1	P27
#define LED2	P26
#define LED3	P15
#define LED4	P14
#define LED5	P23
#define LED6	P22
#define LED7	P21
#define LED8	P20

int counter = 0;
int led_index = 0;
char current_direction = 's';

void GPIO_config();
void UART_config();
void Timer_config();
void stop();
void turn_left();
void turn_right();
void receive();

void main() {
    //初始化 GPIO UART1 Timer0
    GPIO_config();
    UART_config();
    Timer_config();
    //开启全局中断
    EA = 1;
    //打开LED_SW
    LED_SW = 0;


    while (1) {
		receive();
    }
}


//初始化GPIO
void GPIO_config(void) {
    //所有LED初始化为推挽输出
    P4_MODE_OUT_PP(GPIO_Pin_5);
    P2_MODE_OUT_PP(GPIO_Pin_7);
    P2_MODE_OUT_PP(GPIO_Pin_6);
    P2_MODE_OUT_PP(GPIO_Pin_3);
    P2_MODE_OUT_PP(GPIO_Pin_2);
    P2_MODE_OUT_PP(GPIO_Pin_1);
    P2_MODE_OUT_PP(GPIO_Pin_0);
    P1_MODE_OUT_PP(GPIO_Pin_5);
    P1_MODE_OUT_PP(GPIO_Pin_4);
    //初始化串口P30 P31为准双向
    P3_MODE_IO_PU(GPIO_Pin_0);
    P3_MODE_IO_PU(GPIO_Pin_1);
}

//初始化UART1
void UART_config(void) {
    // >>> 记得添加 NVIC.c, UART.c, UART_Isr.c <<<
    COMx_InitDefine		COMx_InitStructure;					//结构定义
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
    COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4

    NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    UART1_SW(UART1_SW_P30_P31);		// 引脚选择, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

//初始化 Timer0
void Timer_config(void) {
    TIM_InitTypeDef		TIM_InitStructure;						//结构定义
    //定时器0做16位自动重装, 中断频率为1000HZ
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
    TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000UL);		//初值,
    TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

//关闭
void stop() {
    LED1 = 1;
    LED2 = 1;
    LED3 = 1;
    LED4 = 1;
    LED5 = 1;
    LED6 = 1;
    LED7 = 1;
    LED8 = 1;
}
//左转
void turn_left() {
    LED1 = (led_index == 7 ? 0 : 1);
    LED2 = (led_index == 6 ? 0 : 1);
    LED3 = (led_index == 5 ? 0 : 1);
    LED4 = (led_index == 4 ? 0 : 1);
    LED5 = (led_index == 3 ? 0 : 1);
    LED6 = (led_index == 2 ? 0 : 1);
    LED7 = (led_index == 1 ? 0 : 1);
    LED8 = (led_index == 0 ? 0 : 1);
}
//右转
void turn_right() {
    LED1 = (led_index == 0 ? 0 : 1);
    LED2 = (led_index == 1 ? 0 : 1);
    LED3 = (led_index == 2 ? 0 : 1);
    LED4 = (led_index == 3 ? 0 : 1);
    LED5 = (led_index == 4 ? 0 : 1);
    LED6 = (led_index == 5 ? 0 : 1);
    LED7 = (led_index == 6 ? 0 : 1);
    LED8 = (led_index == 7 ? 0 : 1);
}

//接收数据
void receive() {
	u8 i;
    //接收数据
    delay_ms(1);
    if(COM1.RX_TimeOut > 0) {
        //超时计数
        if(--COM1.RX_TimeOut == 0) {
            if(COM1.RX_Cnt > 0) {
                // 这里处理收到的数据，做具体的逻辑，可以调用自己的on_uart1_recv
                for(i=0; i<COM1.RX_Cnt; i++)	{
                    // RX1_Buffer[i]存的是接收的每个字节，写出用 TX1_write2buff
                    current_direction = RX1_Buffer[i];
					if(current_direction == 's') printf("关闭转向灯\n");
					else if(current_direction == 'l') printf("左转\n");
					else if(current_direction == 'r') printf("右转\n");
                }
            }
            COM1.RX_Cnt = 0;
        }
    }
}