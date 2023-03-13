#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"



void my_wdg_init()
{
	// 解除写保护
	wdt_register_write_enable(TRUE);
	
	// 设置分频以及重装载值
	wdt_divider_set(WDT_CLK_DIV_64);
	/* set reload value

   timeout = reload_value * (divider / lick_freq )    (s)

   lick_freq    = 40000 Hz
   divider      = 64
   reload_value = 2500

   timeout = 2500 * (64 / 40000 ) = 4s
	 value = timeout/64 * 40000
  */
  wdt_reload_value_set(2500 - 1);
  wdt_counter_reload();
}


flag_status btn_pressed(void){
	// 未按下
		if(gpio_input_data_read(GPIOA) & 0x01){
			delay_ms(10);
			return RESET;
		}
		// 按下
		else{
			// 防抖
			delay_ms(200);
			if(gpio_input_data_read(GPIOA) & 0x01){
				return SET;
			}
		}
}




int main(void)
{
	system_clock_config();	// 时钟初始化
	at32_board_init();			// 延时，LED，按钮初始化
	my_wdg_init();
	
	// 若不按按钮喂狗，灯会一灭一亮
	at32_led_off(LED2);
	delay_sec(1);
	at32_led_on(LED2);
	delay_sec(1);
	
	// 启动看门狗
	wdt_enable();
	
	
	while(1)
	{
		if (btn_pressed()) wdt_counter_reload();	//喂狗，保持灯亮
	}
}

