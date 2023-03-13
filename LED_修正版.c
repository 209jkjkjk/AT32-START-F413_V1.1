#include "at32f413_board.h"
#include "at32f413_clock.h"
#include "at32f413_crm.h"

#define pins GPIO_PINS_2|GPIO_PINS_3|GPIO_PINS_5


#define bitband_addr(addr, num) (0x42000000 + (addr - 0x40000000) * 32 + (num *4))
#define GPIOC_CRL GPIOC_BASE + 0x00
#define GPIOC_ODT GPIOC_BASE + 0x0C
#define GPIOA_CRL GPIOA_BASE + 0x00
#define GPIOA_IDT GPIOA_BASE + 0x08
#define CRM_PERIPH_APB2 CRM_BASE + 0x18
#define GPIOC_ODT2 bitband_addr(GPIOC_ODT, 2)
#define GPIOC_ODT3 bitband_addr(GPIOC_ODT, 3)
#define GPIOC_ODT5 bitband_addr(GPIOC_ODT, 5)
#define GPIOA_IDT0 bitband_addr(GPIOA_IDT, 0)
void led_init(void){
 
	// 启用时钟
	REG32(CRM_PERIPH_APB2) = 0x1 << 4;			// 第四位赋1，表示开启GPIOC时钟
	// 初始化GPIO
	// 设置篇GPIOC 2 3 5的输出模式
	REG32(GPIOC_CRL) &= (uint32_t)0xff0f00ff;	// 先清零
	REG32(GPIOC_CRL) |= (uint32_t)(0x2 << (2*4));			// 再赋值
	REG32(GPIOC_CRL) |= (uint32_t)(0x2 << (3*4));
	REG32(GPIOC_CRL) |= (uint32_t)(0x2 << (5*4));
	// 设置GPIOA 0的输入模式
	
	// 设置GPIOC 2 3 5的值，位带操作版本
	REG32(GPIOC_ODT2) = 1;
	REG32(GPIOC_ODT3) = 1;
	REG32(GPIOC_ODT5) = 1;
}

void led_activity(void){
	REG32(GPIOC_ODT2) = 0;
	delay_sec(1);
	REG32(GPIOC_ODT3) = 0;
	delay_sec(1);
	REG32(GPIOC_ODT5) = 0;
	delay_sec(1);
	REG32(GPIOC_ODT2) = 1;
	delay_sec(1);
	REG32(GPIOC_ODT3) = 1;
	delay_sec(1);
	REG32(GPIOC_ODT5) = 1;
	delay_sec(1);
}


int main(void)
{
	int i;
  //system_clock_config();
	
	led_init();
	
	delay_init();
	//REG32(GPIOC_ODT2) = 0;
	for(i = 0; i < 9; ++i){
		led_activity();
	}
	
	while(1)
	{
		gpio_bits_set(GPIOC,pins);
	}
	//at32_board_init();
}

