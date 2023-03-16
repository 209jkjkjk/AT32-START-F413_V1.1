#include "at32f413_board.h"
#include "at32f413_clock.h"

__IO uint16_t adc1_ordinary_valuetab[3] = {0,1,2};

static void gpio_config(void);
static void dma_config(void);
static void adc_config(void);

static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5 | GPIO_PINS_6;
  gpio_init(GPIOA, &gpio_initstructure);
}

// 将ADC得到的数据不停写入dc1_ordinary_valuetab中
static void dma_config(void)
{
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  nvic_irq_enable(DMA1_Channel1_IRQn, 0, 0);
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 3;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc1_ordinary_valuetab;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
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
  adc_base_struct.repeat_mode = TRUE;					// 重复模式
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;		// 右对齐
  adc_base_struct.ordinary_channel_length = 3;			// 序列长度
  adc_base_config(ADC1, &adc_base_struct);
  // 设置序列
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_4, 1, ADC_SAMPLETIME_239_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_239_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 3, ADC_SAMPLETIME_239_5);
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  // DMA方式处理
  adc_dma_mode_enable(ADC1, TRUE);

	
  adc_enable(ADC1, TRUE);
	
  // 校准
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}


void DMA1_Channel1_IRQHandler(void){
	int i;
	if (dma_flag_get(DMA1_FDT1_FLAG) == SET){
		for (i = 0; i < 3; ++i){
			printf("pa%d: %.2fV ", i+4, (double)adc1_ordinary_valuetab[i] * 3.3 / 4095);
		}
		printf("\r\n");
		dma_flag_clear(DMA1_FDT1_FLAG);
	}
}


int main(void)
{
  system_clock_config();
  at32_board_init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_2);
  uart_print_init(115200);
  gpio_config();
  dma_config();
  adc_config();

  while(1)
  {
	  adc_ordinary_software_trigger_enable(ADC1, TRUE);
	  delay_ms(100);
	  at32_led_toggle(LED2);
  }
}
