#include "at32f413_board.h"
#include "at32f413_clock.h"

// 周期tick数设置
const uint16_t TMR5_pr = 10000;		// 因为分频值为(system_core_clock / 10000)-1，所以周期 =1s
uint32_t capture1, capture2, capture_round = 0;		// 捕获初值，终值，周期数
flag_status capture_flag = SET;

// main函数局部变量
tmr_input_config_type tmr_ic_init_structure;
uint16_t div_value = 0;

// GPIO使能
void gpio_configuration(void)
{
  gpio_init_type gpio_init_struct;

  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
}

// 时钟使能
void crm_configuration(void)
{
  /* TMR5 clock enable */
  crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);

  /* gpioa gpiob clock enable */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
}



// 中断服务
void TMR5_GLOBAL_IRQHandler(void)
{
	double temp;
	if(tmr_flag_get(TMR5, TMR_OVF_FLAG) == SET)
	{
        // 小灯闪烁提示计数器满
		at32_led_toggle(LED3);
		capture_round += 1;
		tmr_flag_clear(TMR5,TMR_OVF_FLAG);
	}
	else if(tmr_flag_get(TMR5,TMR_C1_FLAG) == SET){
		if(capture_flag){
			capture1 = tmr_channel_value_get(TMR5, TMR_SELECT_CHANNEL_1);
			at32_led_on(LED2);
			capture_flag = RESET;
			//printf("caputre1:%d\r\n", capture1);
		}else {
			capture2 = tmr_channel_value_get(TMR5, TMR_SELECT_CHANNEL_1);
			//printf("caputre2:%d\r\n", capture2);
			//printf("round:%d\r\n", capture_round);
			at32_led_off(LED2);
			capture_flag = SET;
			// 发送数据
			temp = (capture2-capture1 + 10000) / 10000.0 - 1;	// 先+10000 防止无符号整数向下溢出
			
			printf("btn_gap:%.2fs\r\n", temp+capture_round);
			capture_round = 0;
		}
		tmr_flag_clear(TMR5,TMR_C1_FLAG);
	}
}

int main(void)
{
	system_clock_config();
	at32_board_init();
	uart_print_init(115200);
	
	crm_configuration();
	gpio_configuration();

	// 计算分频值
	div_value = (uint16_t)(system_core_clock / 10000) - 1;		// =7

	// TMR5周期与计数设置
	tmr_base_init(TMR5, TMR5_pr-1, div_value);
	tmr_cnt_dir_set(TMR5, TMR_COUNT_UP);

	// TMR5捕获设置
	tmr_input_default_para_init(&tmr_ic_init_structure);
	tmr_ic_init_structure.input_channel_select = TMR_SELECT_CHANNEL_1;		// 选择通道1
	tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;		// 不映射
	tmr_ic_init_structure.input_polarity_select = TMR_INPUT_RISING_EDGE;			// 上升沿捕获
	tmr_input_channel_init(TMR5, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);	// 不分频
	tmr_input_channel_filter_set(TMR5, TMR_SELECT_CHANNEL_1, 0x0f);
	
	// TMR5中断设置
	tmr_interrupt_enable(TMR5, TMR_OVF_INT|TMR_C1_INT, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR5_GLOBAL_IRQn, 0, 0);

	// tmr_period_buffer_enable(TMR5, TRUE);

	// 启动TMR5计时器
	tmr_counter_enable(TMR5, TRUE);

	while(1)
	{
		;
	}
}
