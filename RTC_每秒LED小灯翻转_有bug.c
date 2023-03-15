#include "at32f413_board.h"
#include "at32f413_clock.h"

void my_rtc_init(void){
	// 打开访问权限
	pwc_battery_powered_domain_access(TRUE);
	while(crm_flag_get(CRM_LEXT_STABLE_FLAG) == RESET);		// 等待时钟稳定
	// 配置分频使频率=1Hz
	rtc_wait_config_finish();
	rtc_divider_set(32767);
	rtc_wait_config_finish();
	
	// 中断使能
	rtc_interrupt_enable(RTC_TS_FLAG, TRUE);
}


void my_nvic_init(){
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_2);
	nvic_irq_enable(RTC_IRQn, 2, 0);
}


// 时钟使能
void crm_configuration(void){
	crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_BPR_PERIPH_CLOCK, TRUE);
	// 选择rtc时钟源
	crm_clock_source_enable(CRM_CLOCK_SOURCE_LEXT, TRUE);
    crm_rtc_clock_enable(TRUE);
}


// 中断服务
void RTC_IRQHandler(void)
{
	if(rtc_flag_get(RTC_TS_FLAG) == SET){
		// 每秒LED3反转
		at32_led_toggle(LED3);
		rtc_flag_clear(RTC_TS_FLAG);
	}
}

int main(void)
{
	system_clock_config();
	at32_board_init();
	my_nvic_init();
	crm_configuration();
	my_rtc_init();

	while(1)
	{
		// 每半秒LED2反转
		delay_ms(500);
		at32_led_toggle(LED2);
	}
}
