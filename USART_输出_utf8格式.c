#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"


void usart1_printf(void)
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
    gpio_init_struct.gpio_pins = GPIO_PINS_9;								//pa9
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;							//无上下拉
    gpio_init(GPIOA, &gpio_init_struct);
	
	usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);			//串口1,115200波特率，8数据位，1停止位
    usart_hardware_flow_control_set(USART1,USART_HARDWARE_FLOW_NONE);		//无硬件流操作
    usart_parity_selection_config(USART1,USART_PARITY_NONE);				//无校验
    usart_transmitter_enable(USART1, TRUE);									//使能发送
 
	usart_enable(USART1, TRUE);												//使能串口1
 
}


int main(void)
{
	int i = 0;
	
	delay_init();
	usart1_printf();
	
	while(1)
	{
		// 在bsp中已设置printf重定向
		printf("%d\n", i++);
		printf("纳尼\n");
		delay_sec(1);
	}
}

