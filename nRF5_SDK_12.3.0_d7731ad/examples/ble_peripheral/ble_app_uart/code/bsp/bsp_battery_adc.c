#include "nrf.h"
//#include "app_util_platform.h"
#include "nrf_drv_adc.h"
//#include "nordic_common.h"
//#include "boards.h"
//#include "nrf_log.h"
//#include "app_error.h"
//#include "lpd_bsp_system.h"
//#include "lpd_bsp_timer.h"
//#include "string.h"
#include "bsp_battery_adc.h"
//#include "nrf_drv_wdt.h"
//#include "lpd_bsp_flash_rw.h"
//#include "lpd_bsp_temperature.h"


//һ�βɼ�����
#define ADC_BUFFER_SIZE      4                                /**< Size of buffer for ADC samples.  */

////���òɼ�ͨ��
////�����¸�ͨ�� 1.2V�ο���1/3����
//#define NRF_DRV_ADC_NTC_CHANNEL(analog_input)                        \
// {{{                                                                 \
//    .resolution = NRF_ADC_CONFIG_RES_10BIT,                          \
//    .input      = ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling,   \
//    .reference  = NRF_ADC_CONFIG_REF_VBG,                            \
//    .ain        = (analog_input)                                     \
// }}, NULL}


//volatile uint32_t m_outbattery_value;        //һ����̫����ֵ��
volatile uint32_t m_battery_value;           //һ���ǵ��ֵ,
//volatile uint32_t m_rt_value;                //һ���¶Ȳο�ֵ��
//volatile uint32_t m_ntc_value;               //һ���¶�ֵ��
volatile uint32_t test_adc_value;            //һ���Բɼ��ľ�ֵ

//static nrf_drv_adc_channel_t bat_outside_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_4); /**< Channel instance. Default configuration used. */
static nrf_drv_adc_channel_t bat_inside_channel_config  = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_0); /**< Channel instance. Default configuration used. */
//static nrf_drv_adc_channel_t rt_channel_config          = NRF_DRV_ADC_NTC_CHANNEL(NRF_ADC_CONFIG_INPUT_5);     /**< Channel instance. Default configuration used. */
//static nrf_drv_adc_channel_t ntc_channel_config         = NRF_DRV_ADC_NTC_CHANNEL(NRF_ADC_CONFIG_INPUT_6);     /**< Channel instance. Default configuration used. */
 
static nrf_adc_value_t       adc_buffer[ADC_BUFFER_SIZE]; /**< ADC buffer. */
//uint8_t nTimesTestBat;
Current_ADC_Enum current_adc = BATTERY_ADC;
ADC_Sample_Enum adc_sample = ADC_START;
bool need_adc_sample = false;
AD_Type AD_struct;
//voltage_adc_t voltage_adc;


static void voltage_process_task(void);

static void adc_init(void);
static void adc_config(void);
static void adc_sampling(void);

static void start_adc_detect(void);
static void stop_adc_detect(void);
static void adc_calculate(void);

/*
 * �ɼ��жϷ������
 */ 
static void adc_event_handler(nrf_drv_adc_evt_t const * p_event)
{    
		//����һ������·�����������µ�һ�βɼ�  nrf_drv_adc_sample();//�ֶ��л�����һ���Ĵ���SIZE,�ͻ����һ�βɼ�    
    if (p_event->type == NRF_DRV_ADC_EVT_DONE)
    {  
        uint32_t i;
			  uint32_t max_value;
				uint32_t min_value;
				uint32_t avr_value = 0;	
			
        if(p_event->data.done.size>2)
        {   
            max_value = p_event->data.done.p_buffer[0];
            min_value = p_event->data.done.p_buffer[0];
            for (i = 0; i < p_event->data.done.size; i++)
            { 
                if(min_value > p_event->data.done.p_buffer[i])
                {
                    min_value = p_event->data.done.p_buffer[i];
                }
                if(max_value < p_event->data.done.p_buffer[i])
                {
                    max_value = p_event->data.done.p_buffer[i];
                }
                avr_value += p_event->data.done.p_buffer[i];         					
            }
            //ȥ��һ�����ĺ���С������ 
            avr_value = (avr_value-max_value-min_value)/(p_event->data.done.size-2);
        }
        else 
				{
            avr_value = p_event->data.done.p_buffer[0];//��ֹ����̫С��
				}
        test_adc_value = avr_value;
		#if SEGGER_RTT_DEBUG_ADC	
				SEGGER_RTT_printf(0, "test_adc_value =  %d\r\n", test_adc_value);
		#endif  
        task_flag_struct.stop_adc_st = 1;//����һ��ֹͣ�¼�	
		#if SEGGER_RTT_DEBUG_ADC	
				SEGGER_RTT_printf(0, "app_mevent.b.stop_adc_st =  %d\r\n", task_flag_struct.stop_adc_st);
		#endif 
    }
}

////��ȡ̫���ܵ�ص�ѹ 000-999(0.00-9.99V)
//static uint16_t getIntergeActualSunChargeVol(void)
//{
//    uint32_t vol;//��ȡ����ֵ 
//	
//    if(m_outbattery_value > 0x0ffff)
//    {
//        vol = (0x0ffff)*420;
//    } 
//    else
//    {
//        vol = m_outbattery_value*420;
//    }
//    vol = vol/MAX_BATTERY_VALUE;	
//    if(vol>999)
//		{
//        vol = 999;
//		}
//    return (uint16_t)vol;
//}

/*
 * ��ȡ��ص�ѹ 000-500 (0.00-5.00V)
 */ 
static uint16_t get_battery_vol(void)
{
    uint32_t vol;//��ȡ����ֵ 	
  
    if(m_battery_value > MAX_BATTERY_VALUE)
    {
        vol = MAX_BATTERY_VALUE*420;
    } 
    else
    {
        vol = m_battery_value*420;
    }
		vol = vol/MAX_BATTERY_VALUE;	
		if(vol>500)
		{
				vol = 500;
		}
		return (uint16_t)vol;  //MAX_BATTERY_VALUE
}

/*
 * adcͨ������
 */ 
void adc_config(void)
{
    ret_code_t ret_code;
	
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;//���õ����ȼ�
    config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;//����Ϊ������ȼ�
	
    ret_code = nrf_drv_adc_init(&config, adc_event_handler);
    APP_ERROR_CHECK(ret_code);
	
    switch(current_adc)
    {
//        //ѡȡͨ��
//        case RT_REFER_ADC:
//            nrf_gpio_pin_clear(NTC_PWRON);
//            nrf_drv_adc_channel_enable(&rt_channel_config);
//            break;
//				
//        case NTC_ADC: 
//            nrf_gpio_pin_clear(NTC_PWRON);
//            nrf_drv_adc_channel_enable(&ntc_channel_config);
//            break;
//				
//        case SOLAR_ADC:
//            nrf_gpio_pin_set(NTC_PWRON);
//            nrf_drv_adc_channel_enable(&bat_outside_channel_config);
//            break;
//				
        case BATTERY_ADC:
            nrf_gpio_pin_set(V_BAT_CTRL);
//            current_adc = BATTERY_ADC;
            nrf_drv_adc_channel_enable(&bat_inside_channel_config);
            break;
				
        default:	
            break;  
    }
}

/*
 * ����adcͨ���Ͳ���
 */ 
static void start_adc_detect(void)//ÿ��������Ҫ�����»���
{   
    ret_code_t ret_code;	
	
		//����־λ�л�ͨ��
    adc_config();
    //���û����MCB  
    ret_code = nrf_drv_adc_buffer_convert(adc_buffer, ADC_BUFFER_SIZE);
    APP_ERROR_CHECK(ret_code);
//    m_battery100ms=0;	 
    //�����ɼ�
    nrf_drv_adc_sample();
}

/*
 * �ͷŲɼ�ͨ����ʡ��
 */
static void stop_adc_detect(void)
{   
    nrf_gpio_pin_clear(V_BAT_CTRL);
    switch(current_adc)
    {
//        case RT_REFER_ADC: 				   
//            nrf_drv_adc_channel_disable(&rt_channel_config);
//            break;				
//        case NTC_ADC:				    
//            nrf_drv_adc_channel_disable(&ntc_channel_config);
//            break;
//        case SOLAR_ADC:				     
//            nrf_drv_adc_channel_disable(&bat_outside_channel_config);
//            break;
        case BATTERY_ADC:	
            nrf_drv_adc_channel_disable(&bat_inside_channel_config);	
            break;
        default:	
            break;  
    }	
//    nrf_gpio_cfg_input(OUTSIDE_PWR_PIN ,NRF_GPIO_PIN_NOPULL);	
    nrf_gpio_cfg_input(V_BAT_AD,NRF_GPIO_PIN_NOPULL);
//    nrf_gpio_cfg_input(RT_REFER ,NRF_GPIO_PIN_NOPULL);	
//    nrf_gpio_cfg_input(NT_SENSOR,NRF_GPIO_PIN_NOPULL);		
    nrf_drv_adc_uninit();
}

/*
 * �����ѹֵ
 */
static void adc_calculate(void)
{
		switch(current_adc)
		{
//				case RT_REFER_ADC:
//						if(m_rt_value != 0)
//						{                                 
//								m_rt_value = (test_adc_value + m_rt_value) / 2;
//						}
//						else
//						{
//								m_rt_value = test_adc_value;
//						}
//						voltage_adc.voltage_ref = m_rt_value;
//						NRF_LOG_DIRECT("nRef_value = %d\r\n",voltage_adc.voltage_ref);
//						current_adc = NTC_ADC;
//						break;

//				case NTC_ADC:
//						if(m_ntc_value != 0)
//						{
//								m_ntc_value = (test_adc_value+m_ntc_value) / 2;
//						}
//						else
//						{
//								m_ntc_value = test_adc_value;
//						}
//						voltage_adc.voltage_ntc = m_ntc_value;
//						NRF_LOG_DIRECT("ntc_value = %d \r\n",voltage_adc.voltage_ntc);
//						AD_Struct.temp_value = Temp_Vol_Calculate(&voltage_adc);
//						if(AD_Struct.temp_value > ALARM_TEMPERATURE_VALUE)
//						{
//															//���±���
//						}
//						current_adc = SOLAR_ADC;                            
//						break;

//				case SOLAR_ADC:
//						if(m_outbattery_value != 0)
//						{
//								m_outbattery_value = (test_adc_value+m_outbattery_value) / 2;
//						}
//						else
//						{
//								m_outbattery_value = test_adc_value;
//						}
//						AD_Struct.voltage_solar = getIntergeActualSunChargeVol();

//						//  NRF_LOG_DIRECT("out_value = %d \r\n",voltage_adc.voltage_solar);    
//						current_adc = BATTERY_ADC;                            
//						break;

				case BATTERY_ADC:
				{
						if(m_battery_value != 0)
						{
								m_battery_value = (test_adc_value+m_battery_value)/2;
						}
						else
						{
								m_battery_value = test_adc_value;
						}
						AD_struct.voltage_battery = get_battery_vol();
						if(AD_struct.voltage_battery < ALARM_VOLTAGE_VALUE) //�͵�������,������ֵ����
						{

						}
						// NRF_LOG_DIRECT("battery_value = %d \r\n",voltage_adc.voltage_battery);
						current_adc = IDLE_ADC;
						need_adc_sample = false;
				}
						break;

				default:
						break;
		}
}

static void adc_sampling(void)
{
    switch(adc_sample)
    {
        case ADC_START:
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "ADC START\r\n");
				#endif
//            setWDI();
            start_adc_detect();
            adc_sample = ADC_SAMPLING;
            break;
				
        case ADC_SAMPLING:
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "ADC SAMPLING\r\n");
				#endif
//            setWDI();
            if(task_flag_struct.stop_adc_st != 1)
            {
                nrf_drv_adc_sample();
                sys_ndelay(10);
            }
            else
            {
                adc_sample = ADC_STOP;
            }
            break;
						
        case ADC_STOP:
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "ADC STOP\r\n");
				#endif
          //  NRF_LOG_DIRECT(" ADC_STOP\r\n"); 
//            setWDI();
            if(task_flag_struct.stop_adc_st == 1)
            { 		
                task_flag_struct.stop_adc_st = 0;
                stop_adc_detect();
                adc_sample = ADC_START;
                adc_calculate();
            }
            break;
						
        default:
            break;
    }
}

static void adc_init(void)
{
    switch(current_adc)
    {
        case RT_REFER_ADC: 	
            need_adc_sample = true;
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "RT REFER ADC\r\n");
				#endif				
            break;	
				
        case NTC_ADC:
						need_adc_sample = true;
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "NTC ADC\r\n");
				#endif                           
            break;
				
        case SOLAR_ADC:	
						need_adc_sample = true;	
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "SOLAR ADC\r\n");
				#endif				               
            break;
				
        case BATTERY_ADC:	  
						need_adc_sample = true;
				#if SEGGER_RTT_DEBUG_ADC	
						SEGGER_RTT_printf(0, "BATTERY ADC\r\n");
				#endif
            break;	
				
//        case IDLE_ADC:
//          //  NRF_LOG_DIRECT("IDLE_ADC \r\n"); 
//            if(m_battery100ms >ADC_SAMPLING_TIME_100MS)
//            {                 
//                current_adc=RT_REFER_ADC;                  
//            }
//            else
//            {
//                need_adc_sample = false;
//            }
//            break;
				
        default:
            current_adc = RT_REFER_ADC;
            break;  
    }
}

static void voltage_process_task(void)
{
    if(adc_sample == ADC_START)
    {
        adc_init();
    }        
    if(need_adc_sample)
    {
		#if SEGGER_RTT_DEBUG_ADC	
				SEGGER_RTT_printf(0, "adc sample\r\n");
		#endif
        adc_sampling();
    }
}

void measure_voltage(void)
{
		current_adc = RT_REFER_ADC;
		adc_sample = ADC_START;
		while(1)
		{
				voltage_process_task();
				if(false == need_adc_sample)
				{
						break;
				}
		}
}

