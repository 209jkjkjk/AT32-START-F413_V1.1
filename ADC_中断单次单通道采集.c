#include "at32f413_board.h"
#include "at32f413_clock.h"

__IO uint16_t adc1_ordinary_valuetab[3] = {0,1,2};
__IO uint16_t dma_trans_complete_flag = 0;

static void gpio_config(void);
static void adc_config(void);

static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;// | GPIO_PINS_5 | GPIO_PINS_6;
  gpio_init(GPIOA, &gpio_initstructure);
}


static void adc_config(void)
{
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_adc_clock_div_set(CRM_ADC_DIV_6);		// adc时钟分频

  /* select combine mode */
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);		// 独立模式
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;					// 序列模式
  //adc_base_struct.repeat_mode = TRUE;					// 重复模式
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;		// 右对齐
  adc_base_struct.ordinary_channel_length = 1;			// 序列长度
  adc_base_config(ADC1, &adc_base_struct);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_4, 1, ADC_SAMPLETIME_239_5);
  //adc_ordinary_channel_set(ADC1, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_239_5);
  //adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 3, ADC_SAMPLETIME_239_5);
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  //adc_dma_mode_enable(ADC1, TRUE);
	// 用中断的方式处理
	adc_interrupt_enable(ADC1, ADC_CCE_INT, TRUE);
	nvic_irq_enable(ADC1_2_IRQn,0,0);
	
  adc_enable(ADC1, TRUE);
	
  // 校准
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}


void ADC1_2_IRQHandler(void){
	if (adc_flag_get(ADC1, ADC_CCE_FLAG) == SET){
		adc1_ordinary_valuetab[0] = adc_ordinary_conversion_data_get(ADC1);		// 读数据时自动清零中断状态位
		printf("val: %.2fV\r\n", (double)adc1_ordinary_valuetab[0] * 3.3 / 4095);
	}
}


int main(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);
  gpio_config();
  adc_config();
  

  at32_led_on(LED2);
  while(1)
  {
	  adc_ordinary_software_trigger_enable(ADC1, TRUE);
	  delay_ms(200);
	  at32_led_toggle(LED2);
  }
}
