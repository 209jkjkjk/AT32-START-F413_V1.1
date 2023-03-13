#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"



void usart1_init(u32 bound)
{
	gpio_init_type gpio_init_struct;
	/*Enable the UART Clock*/
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);		//开启GPIOA的时钟
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);		//开启USART1的时钟
	   
    gpio_default_para_init(&gpio_init_struct);
	/* Configure the UART1 TX pin */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;	//较大电流推动/吸入能力
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;				//推挽输出
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;								//复用
    gpio_init_struct.gpio_pins = GPIO_PINS_9;								//PA9
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;							//无上下拉
    gpio_init(GPIOA, &gpio_init_struct);
 
	/* Configure the UART1 RX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;	//较大电流推动/吸入能力
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;				//推挽输出
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;							//输入模式
    gpio_init_struct.gpio_pins = GPIO_PINS_10;								//PA10
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;								//上拉
    gpio_init(GPIOA, &gpio_init_struct);
	
	nvic_irq_enable(USART1_IRQn, 0, 0);      						//使能串口1中断，优先级0，次优先级0
	
	/*Configure UART param*/
    usart_init(USART1, bound, USART_DATA_8BITS, USART_STOP_1_BIT);		//波特率，8数据位，1停止位
    usart_hardware_flow_control_set(USART1,USART_HARDWARE_FLOW_NONE);	//无硬件流操作
    usart_parity_selection_config(USART1,USART_PARITY_NONE);			//无校验
    usart_transmitter_enable(USART1, TRUE);								//使能发送
    usart_receiver_enable(USART1, TRUE);								//使能接收
	
	usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);				//使能串口接收中断
	usart_interrupt_enable(USART1, USART_IDLE_INT, TRUE);				//使能串口空闲中断
    usart_enable(USART1, TRUE);											//使能串口
	
}



// 接受缓存定义
#define USART_BUF_LEN 200
uint16_t rxbuf[USART_BUF_LEN];//, txbuf[USART_BUF_LEN];
uint8_t usartStatus = 0;							// 0或1
uint8_t rxlen=0;//, txlen=0;


void USART1_IRQHandler(void)
{	
	// RDBF中断说明数据来了，要接受
	if(usart_flag_get(USART1, USART_RDBF_FLAG) == SET)              // USART1接收中断响应
	{
		rxbuf[rxlen++] = usart_data_receive(USART1);			// USART1读取数据寄存器	
	}
	// IDLEF中断空闲说明传输结束
	if(usart_flag_get(USART1, USART_IDLEF_FLAG) != RESET)               // USART1总线空闲
	{
		USART1->sts;                      // USART1清除空闲中断标志位
		USART1->dt;												// USART1清除空闲中断标志位
		usartStatus = 1;										// USART1接收完成标志位
	}
		
}


void usart1_txdatas(uint16_t *SendData,uint16_t len)
{
	u16 i=0;
	for(i=0;i<len;i++)
	{
		while(usart_flag_get(USART1 , USART_TDBE_FLAG) != SET);	//发送寄存器空
		usart_data_transmit(USART1, SendData[i]);					//发生数据
		while(usart_flag_get(USART1, USART_TDC_FLAG) != SET);		//发生完成
	}
	
}



int main(void)
{
	system_clock_config();
	delay_init();
	usart1_init(115200);
	
	while(1)
	{
		if(usartStatus){
			printf("back:");
			usart1_txdatas(rxbuf, rxlen);
			rxlen = 0;
			usartStatus = 0;
		}
	}
}

