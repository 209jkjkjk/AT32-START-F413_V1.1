#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"

void TMR1_OVF_TMR10_IRQHandler(void)
{
  if(tmr_flag_get(TMR1, TMR_OVF_FLAG) == SET)
  {
    at32_led_toggle(LED3);
    tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
}


int main(void)
{
	// 准备获取系统各总线时钟频率
	crm_clocks_freq_type crm_clocks_freq_struct = {0};
	
	
	system_clock_config();	// 时钟初始化 19200 0000
	at32_board_init();			// 延时，LED，按钮初始化
	crm_clocks_freq_get(&crm_clocks_freq_struct);		// 获取系统各总线时间
	
	// 使能定时器时钟，并且初始化定时器
	crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
	// 16bit 最大值65535，另外因为apb1div=2，所以定时器时钟 = apb1clk*2 = 19200 0000 = ahb总线时钟
	tmr_base_init(TMR1, 65535, (crm_clocks_freq_struct.ahb_freq / 65536) - 1);
    tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
	
	// 使能定时器中断
    tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);

    //  启动定时器（默认使用内置时钟）
    tmr_counter_enable(TMR1, TRUE);
	
	
	while(1)
	{
		;
	}
}

