#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"



void my_wwdg_init()
{
	// 使能窗口看门狗时钟
	crm_periph_clock_enable(CRM_WWDT_PERIPH_CLOCK, TRUE);
	
	// 设置分频以及窗口下限
	wwdt_divider_set(WWDT_PCLK1_DIV_32768);
	wwdt_window_counter_set(0x6F);
	
	/* enable wwdt

   window_value:   (0x7F - 0x6F)
   timeout_value:  (0x7F - 0x40) + 1

   timeout = timeout_value * (divider / pclk1_freq )    (s)
   window  = window_value  * (divider / pclk1_freq )    (s)

   pclk1_freq   = 120 MHz
   divider      = 32768
   reload_value = 0x40 = 64
   window_value = 0x10 = 16

   timeout = 64 * (32768 / 96000000 ) = 0.0218s = 21.8ms
   window  = 16 * (32768 / 96000000 ) = 0.0055s = 5.5ms

  */
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
			return RESET;
		}
}




int main(void)
{
	system_clock_config();	// 时钟初始化
	at32_board_init();			// 延时，LED，按钮初始化
	my_wwdg_init();
	
	// 若发生重置，灯灭1秒后重新点亮
	at32_led_off(LED2);
	delay_sec(1);
	at32_led_on(LED2);
	
	// 启动窗口看门狗
	wwdt_enable(0x7F);
	
	
	while(1)
	{

    /* the reload time must within the window (8ms > 5.5ms) */
    delay_ms(8);

    /* update the wwdt window counter */
    wwdt_counter_set(0x7F);

    if(at32_button_press() == USER_BUTTON)
    {
      while(1);
    }
	}
}

