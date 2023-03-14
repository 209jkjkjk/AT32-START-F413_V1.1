/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f413_board.h"
#include "at32f413_clock.h"

// 周期设置
const uint16_t tmr3_pr = 6666;
// 占空比设置
const uint16_t dt_min = 22, dt_max = tmr3_pr/2;	// 在占空比为1/2时即可达到最大亮度，不用为0基本上就灭了
uint16_t ch1_dt = 22;
int8_t dt_dir = 1;		// 变化方向

// main函数局部变量
tmr_output_config_type tmr_oc_init_structure;
uint16_t div_value = 0;

// GPIO复用使能
void gpio_configuration(void)
{
  gpio_init_type gpio_init_struct;

  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);
}

// 时钟使能
void crm_configuration(void)
{
  /* tmr3 clock enable */
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  /* gpioa gpiob clock enable */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
}



// 中断服务
void TMR3_GLOBAL_IRQHandler(void)
{
	if(tmr_flag_get(TMR3, TMR_OVF_FLAG) == SET)
	{
		if((ch1_dt >= dt_max && dt_dir > 0) ||
			(ch1_dt <= dt_min && dt_dir < 0))
		{
				dt_dir = -dt_dir;
		}
		ch1_dt += dt_dir;
		tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, ch1_dt);
		tmr_flag_clear(TMR3, TMR_OVF_FLAG);
	}
}

int main(void)
{
	system_clock_config();
	at32_board_init();
	crm_configuration();
	gpio_configuration();

	// 计算分频值
	div_value = (uint16_t)(system_core_clock / 24000000) - 1;		// =7

	// tmr3周期与计数设置
	tmr_base_init(TMR3, tmr3_pr-1, div_value);
	tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
	// tmr3中断设置
	tmr_interrupt_enable(TMR3, TMR_OVF_INT, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR3_GLOBAL_IRQn, 0, 0);

	// tmr3输出设置
	tmr_output_default_para_init(&tmr_oc_init_structure);
	tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;		// DT大于VAL有效
	tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;			// 有效电平为高
	tmr_oc_init_structure.oc_output_state = TRUE;
	tmr_oc_init_structure.oc_idle_state = FALSE;
	
	tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
	tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, ch1_dt);				// 设置DT值
	tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_1, TRUE);		// 即影子寄存器模式，延迟更新
	// tmr_period_buffer_enable(TMR3, TRUE);

	// 启动tmr3计时器
	tmr_counter_enable(TMR3, TRUE);

	while(1)
	{
		// 软件实现呼吸灯
		if(gpio_input_data_read(GPIOA) & 0x40)at32_led_on(LED3);
		else at32_led_off(LED3);

	}
}
