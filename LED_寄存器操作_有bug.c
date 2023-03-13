#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"

#define pins GPIO_PINS_2|GPIO_PINS_3|GPIO_PINS_5

void led_init(void)
{
	gpio_init_type gpio_init_struct;						//gpio 结构体
 
	/* enable the led clock */
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);  //开启gpioc的时钟
 
	/* set default parameter */
	gpio_default_para_init(&gpio_init_struct);				//设置gpio结构体默认值
 
	/* configure the led gpio */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; 	//较大电流推动/吸入能力
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;		 		//推挽或者开漏（输入模式时，这个参数无效）
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;		 					//输出模式
	gpio_init_struct.gpio_pins = pins;	//IO 管脚
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE; 							//无上下拉
	gpio_init(GPIOC, &gpio_init_struct);
	gpio_bits_reset(GPIOC,pins); 			
	
}

#define addr_t volatile uint32_t
void led_init_register_version(void){
	addr_t GPIOC_CRL = GPIOC_BASE + 0x00;		// GPIOC_CRL的地址
	addr_t GPIOC_ODT = GPIOC_BASE + 0x0C;		// ODT的地址
 
	// 启用时钟
	addr_t crm_periph_addr = PERIPH_BASE + 0x18;		// CRM寄存器地址
	REG32(crm_periph_addr) = 0x1U << 4;			// 第四位赋1，表示GPIOC
 
	// 初始化GPIO
	// 设置篇GPIOC 2 3 5的输出模式
	REG32(GPIOC_CRL) &= 0xfff0f00f;	// 先清零
	REG32(GPIOC_CRL) |= 0x2 << 4;			// 再赋值
	REG32(GPIOC_CRL) |= 0x2 << (2*4);
	REG32(GPIOC_CRL) |= 0x2 << (4*4);
	// 设置GPIOC 2 3 5的值
	//REG32(GPIOC_ODT) = 0xffffffd2;
	REG32(GPIOC_ODT) &= ~(1<<2);
	REG32(GPIOC_ODT) |= 1<<3;
	REG32(GPIOC_ODT) &= ~(1<<5);
	//REG32(GPIOC_ODT) |= 1<<2;
	//REG32(GPIOC_ODT) &= ~(0U<<4);
	//gpio_bits_reset(GPIOC,pins); 
}

int main(void)
{
	int i;
	addr_t GPIOC_CRL = GPIOC_BASE + 0x00;		// GPIOC_CRL的地址
	addr_t GPIOC_ODT = GPIOC_BASE + 0x0C;		// ODT的地址
  system_clock_config();


  at32_board_init();
	
	//led_init();
	led_init_register_version();
	delay_init();
	
	
		for(i = 0; i < 22; ++i){
				REG32(GPIOC_ODT) &= ~(1<<2);
				delay_sec(1);
				REG32(GPIOC_ODT) &= ~(1<<3);
				delay_sec(1);
				REG32(GPIOC_ODT) &= ~(1<<5);
				delay_sec(1);
				REG32(GPIOC_ODT) |= 1<<2;
				delay_sec(1);
				REG32(GPIOC_ODT) |= 1<<3;
				delay_sec(1);
				REG32(GPIOC_ODT) |= 1<<5;
				delay_sec(1);
		}
	
		while(1)
		{
				//gpio_bits_set(GPIOC,pins);
			;
		}
}

