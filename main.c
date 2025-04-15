//ת��� = UART + ��ˮ��
//����l	��ת
//����r	��ת
//����s	�ر�

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
    //��ʼ�� GPIO UART1 Timer0
    GPIO_config();
    UART_config();
    Timer_config();
    //����ȫ���ж�
    EA = 1;
    //��LED_SW
    LED_SW = 0;


    while (1) {
		receive();
    }
}


//��ʼ��GPIO
void GPIO_config(void) {
    //����LED��ʼ��Ϊ�������
    P4_MODE_OUT_PP(GPIO_Pin_5);
    P2_MODE_OUT_PP(GPIO_Pin_7);
    P2_MODE_OUT_PP(GPIO_Pin_6);
    P2_MODE_OUT_PP(GPIO_Pin_3);
    P2_MODE_OUT_PP(GPIO_Pin_2);
    P2_MODE_OUT_PP(GPIO_Pin_1);
    P2_MODE_OUT_PP(GPIO_Pin_0);
    P1_MODE_OUT_PP(GPIO_Pin_5);
    P1_MODE_OUT_PP(GPIO_Pin_4);
    //��ʼ������P30 P31Ϊ׼˫��
    P3_MODE_IO_PU(GPIO_Pin_0);
    P3_MODE_IO_PU(GPIO_Pin_1);
}

//��ʼ��UART1
void UART_config(void) {
    // >>> �ǵ���� NVIC.c, UART.c, UART_Isr.c <<<
    COMx_InitDefine		COMx_InitStructure;					//�ṹ����
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;			//������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;				//��������,   ENABLE��DISABLE
    COMx_InitStructure.BaudRateDouble = DISABLE;			//�����ʼӱ�, ENABLE��DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);		//��ʼ������1 UART1,UART2,UART3,UART4

    NVIC_UART1_Init(ENABLE,Priority_1);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    UART1_SW(UART1_SW_P30_P31);		// ����ѡ��, UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

//��ʼ�� Timer0
void Timer_config(void) {
    TIM_InitTypeDef		TIM_InitStructure;						//�ṹ����
    //��ʱ��0��16λ�Զ���װ, �ж�Ƶ��Ϊ1000HZ
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 1000UL);		//��ֵ,
    TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

//�ر�
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
//��ת
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
//��ת
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

//��������
void receive() {
	u8 i;
    //��������
    delay_ms(1);
    if(COM1.RX_TimeOut > 0) {
        //��ʱ����
        if(--COM1.RX_TimeOut == 0) {
            if(COM1.RX_Cnt > 0) {
                // ���ﴦ���յ������ݣ���������߼������Ե����Լ���on_uart1_recv
                for(i=0; i<COM1.RX_Cnt; i++)	{
                    // RX1_Buffer[i]����ǽ��յ�ÿ���ֽڣ�д���� TX1_write2buff
                    current_direction = RX1_Buffer[i];
					if(current_direction == 's') printf("�ر�ת���\n");
					else if(current_direction == 'l') printf("��ת\n");
					else if(current_direction == 'r') printf("��ת\n");
                }
            }
            COM1.RX_Cnt = 0;
        }
    }
}