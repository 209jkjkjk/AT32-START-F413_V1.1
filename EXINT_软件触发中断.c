#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"



void my_exint_init()
{
	exint_init_type exint_init_struct;
	
	// 启用时钟
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	// IO与中断绑定
  gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE0);
	// 设置中断模式
  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);
	// 设置中断优先级
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(EXINT0_IRQn, 1, 0);
}



void EXINT0_IRQHandler(void)
{
  at32_led_toggle(LED3);
	delay_ms(100);	// 防抖
	exint_flag_clear(EXINT_LINE_0);
}


flag_status btn_pressed(){
		if(gpio_input_data_read(GPIOA) & 0x01){
			delay_ms(10);
		}
		// 按下
		else{
			// 防抖
			delay_ms(10);
			if(gpio_input_data_read(GPIOA) & 0x01){
				return SET;
			}
			return RESET;
		}
		return RESET;
}



int main(void)
{
	system_clock_config();	// 时钟初始化
	at32_board_init();			// 延时，LED，按钮初始化
	my_exint_init();
	
	// 默认关灯
	at32_led_off(LED3);
	
	
	
	while(1)
	{
		// 每一秒自动触发中断反转
		delay_sec(1);
		exint_software_interrupt_event_generate(EXINT_LINE_0);
	}
}

