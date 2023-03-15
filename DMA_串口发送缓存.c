#include "at32f413_board.h"
#include "at32f413_clock.h"
 
 
void usart1_configuration(void)
{
	gpio_init_type gpio_init_struct;

	// 启用时钟
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);


	// 设置tx引脚参数
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pins = GPIO_PINS_9;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOA, &gpio_init_struct);

	// 设置rx引脚参数
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = GPIO_PINS_10;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(GPIOA, &gpio_init_struct);

	// 设置串口通信参数
	usart_init(USART1, 115200, USART_DATA_8BITS, USART_STOP_1_BIT);
	usart_transmitter_enable(USART1, TRUE);
	usart_receiver_enable(USART1, TRUE);
	
	// 启动DMA发送
	usart_dma_transmitter_enable(USART1, TRUE);
	// 打开中断
	//usart_interrupt_enable(USART1, USART_IDLE_INT, TRUE);
	//nvic_irq_enable(USART1_IRQn, 0, 0);
	
	// 串口使能
	usart_enable(USART1, TRUE);
}


// dma发送缓存区
static uint8_t dma_tx_buffer[] = "abaaba\r\n";
static uint32_t dma_tx_buffer_len = sizeof(dma_tx_buffer);

void dma_configuration(void)
{
	dma_init_type dma_init_struct;

	// 时钟使能
	crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);


	// 配置DMA1CHANNEL1
	// 配置传输方向
	dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
	// 设置内存缓存区
	dma_init_struct.buffer_size = dma_tx_buffer_len;
	dma_init_struct.memory_base_addr = (uint32_t)dma_tx_buffer;
	dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
	dma_init_struct.memory_inc_enable = TRUE;
	// 设置外设输入
	dma_init_struct.peripheral_base_addr = (uint32_t)&USART1->dt;
	dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
	dma_init_struct.peripheral_inc_enable = FALSE;
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
	dma_init_struct.loop_mode_enable = FALSE;	// 禁用循环模式
	dma_init(DMA1_CHANNEL1, &dma_init_struct);

	// 配置弹性映射
	dma_flexible_config(DMA1, FLEX_CHANNEL1, DMA_FLEXIBLE_UART1_TX);

}


/*void USART1_IRQHandler(void)
{	
	// IDLEF中断空闲说明传输结束
	if(usart_flag_get(USART1, USART_IDLEF_FLAG) != RESET)               // USART1总线空闲
	{
		
		
		USART1->sts;                      // USART1清除空闲中断标志位
		USART1->dt;	
	}
		
}*/


int main(void)
{
  system_clock_config();
  at32_board_init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_2);
  usart1_configuration();
  dma_configuration();

	
	while(1){
		// 关闭通道 - step3
		dma_channel_enable(DMA1_CHANNEL1, FALSE);
		delay_sec(1);   // 慢慢发
        // 重新启动通道 - step1
		dma_data_number_set(DMA1_CHANNEL1, dma_tx_buffer_len);
		dma_channel_enable(DMA1_CHANNEL1, TRUE);
        // 等待发送完毕 - step2
		while(dma_flag_get(DMA1_FDT1_FLAG) == RESET);
		dma_flag_clear(DMA1_FDT1_FLAG);
	}
}
