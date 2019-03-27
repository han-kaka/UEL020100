#include "nrf_drv_gpiote.h"
#include "bsp_system.h"


//��ʼ��IO
void init_board(void)
{ 
    //��ʼ��IO
    nrf_drv_gpiote_init();
	
		//LED
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_pin_set(LED1);
	
		//adc�ɼ�
		nrf_gpio_cfg_input(V_BAT_AD, NRF_GPIO_PIN_NOPULL);
		nrf_gpio_cfg_output(V_BAT_CTRL);
		nrf_gpio_pin_clear(V_BAT_CTRL);                            //V_BAT
	
		nrf_gpio_cfg_input(FB_5V, NRF_GPIO_PIN_NOPULL);            //FB_5V       
	
    //�������
    nrf_gpio_cfg_output(INA);                                  //INA
    nrf_gpio_pin_clear(INA);	
    nrf_gpio_cfg_output(INB);                                  //INB
    nrf_gpio_pin_clear(INB);	
	
		//uart
    nrf_gpio_cfg_input(MCU_UART3_RX,NRF_GPIO_PIN_NOPULL);      //uart_rx	
    nrf_gpio_cfg_output(MCU_UART3_TX);                         //uart_tx
    nrf_gpio_pin_clear(MCU_UART3_TX);	
	
    //NBģ��
    nrf_gpio_cfg_output(NB_POWER_ON);		
    nrf_gpio_pin_clear(NB_POWER_ON);	                         //NB��Դ�ر�
    
    nrf_gpio_cfg_output(NB_RESET);	
    nrf_gpio_pin_set(NB_RESET);	                             //��λNBģ��
    
//    nrf_gpio_cfg_output(NB_PWRONKEY);	
//    nrf_gpio_pin_set(NB_PWRONKEY);	                          //����NB����PWRKEY	
}

//��ʼ��оƬ����
void init_peripheral(void)
{  
    //------��������ϵͳ��ʱ-------------------	
    init_system_time();
    start_system_time();
	
    //------��ʼ�������ܿ�---------------------	
    //------��ʼ��uartģ��---------------------
#if SEGGER_RTT_DEBUG_UART
    SEGGER_RTT_printf(0, "UART Start!\r\n");
#endif
    myuart_start();
    //init_dsr_detection();
    //setWDI();	
//	
//		drive_open();
//		saadc_init();
}
