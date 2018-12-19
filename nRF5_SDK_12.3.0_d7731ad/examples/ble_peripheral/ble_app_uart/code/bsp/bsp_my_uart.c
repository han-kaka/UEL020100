#include "app_util_platform.h"
#include "nrf_drv_common.h"
#include "app_timer.h"
#include "nrf_drv_gpiote.h"

//�û�ͷ�ļ�
#include "bsp_my_uart.h"


//#define MS_1_SINGLE_INTERVAL            APP_TIMER_TICKS(1, APP_RTC_TIMER_PRESCALER)     /**< 1ms time interval (ticks). */
//#define UART_BUF_MAX_LEN  255

//APP_TIMER_DEF(m_ms_1_single_timer_id);                    /**< 1 ms single shot timer. */

Receive_Data_Type                          Receive_Data_Struct={0};

volatile bool uarton_status;
Uart_Mevent_Type Uart_Mevent_Struct;
//uint8_t volatile clk_gprs_sendx100ms,send_at_times;
////���ջ���
//char uart_rx_buf[UART_BUF_MAX_LEN];

volatile uint8_t   uart_rx_push_ptr;
volatile uint8_t   uart_rx_pop_ptr;
volatile uint8_t   uart_rx_number;



//typedef enum
//{
//IPD_I=0,
//IPD_P=1,
//IPD_D=2,
//IPD_L= 3,	
//}IPD_STEP_t;
//IPD_STEP_t IPD_STEP;
//#define IPD_BUF_MAXLEN 100
//uint16_t volatile IPD_lenth;
//uint8_t volatile ipd_push_ptr,IPD_endPtr,IPD_searchPtr;
//char ipd_buffer[IPD_BUF_MAXLEN];//IPD���ݻ���






//=====��ʼ�����ջ���========================
void init_uart_buf(void)
{ 	
		uart_rx_push_ptr = 0;
		uart_rx_pop_ptr  = 0;
		uart_rx_number   = 0;	
}

//=====�����ж�==================================
 void UART0_IRQHandler(void)
{    
		if(nrf_uart_int_enable_check(NRF_UART0, NRF_UART_INT_MASK_ERROR) &&
       nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_ERROR))
    {
				//�˴�����Ƿ��д�����жϼ���,�����¼���־        
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_ERROR);
        nrf_uart_int_disable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY | NRF_UART_INT_MASK_ERROR);
        if (uarton_status == false)
        {
            nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPRX);
        }
				Uart_Mevent_Struct.b.periph_errorst = 1;//Ӳ������������UART
    }
    else if(nrf_uart_int_enable_check(NRF_UART0, NRF_UART_INT_MASK_RXDRDY) &&
            nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_RXDRDY))
    {
				//��������
				nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);//���¼���־

				NB_ATCmdCB.Busy = 1;
			
				NB_ATCmdCB.RxBuf[NB_ATCmdCB.ucRxCnt++] = nrf_uart_rxd_get(NRF_UART0);
			
     #if SEGGER_RTT_DEBUG_UART_DATA
          SEGGER_RTT_printf(0, "%c", NB_ATCmdCB.RxBuf[NB_ATCmdCB.ucRxCnt-1]);
     #endif
			
				if(NB_ATCmdCB.ucRxCnt >= NB_RX_BUFF_LEN)
				{
						 NB_ATCmdCB.ucRxCnt = 0;
				}
				NB_ATCmdCB.ucByteTimeOut = 20;
					
				//------------����RX�����ֹͣ��������--------------------------
				if(uarton_status == false)
				{
						nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPRX);
				}   
    }
  
    if (nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_RXTO))
    {
				//�����������
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXTO);
        if(uarton_status)
        {
						//������RX������
            nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTRX);
        }	    
    }
}

//void ms_1_single_timeout_handler(void * p_context)
//{
//		Receive_Data_Struct.rec_data_len = Receive_Data_Struct.len_count;
//		uart_rx_complete_callback(g_Rx_Buffer,Receive_Data_Struct.rec_data_len);
//		Receive_Data_Struct.len_count = 0;
//}

//=========���ú�����UART=============================================
void myuart_start(void)
{	
//    uint32_t err_code;		

    if(uarton_status)
    {
        return;	
    }
    //��ʼ������		
    init_uart_buf();	

    //���ô���IO	
    //nrf_gpio_pin_set(TX_PIN_SELF);
    //nrf_gpio_cfg_output(TX_PIN_SELF);
    //nrf_gpio_cfg_input(RX_PIN_SELF, NRF_GPIO_PIN_NOPULL);

    nrf_uart_baudrate_set(NRF_UART0, NRF_UART_BAUDRATE_9600);//14400;//19200;//115200;////9600;//��һ��);
    nrf_uart_configure(NRF_UART0, NRF_UART_PARITY_EXCLUDED, NRF_UART_HWFC_DISABLED);//����ż���ر�������
    nrf_uart_txrx_pins_set(NRF_UART0, MCU_UART3_TX, MCU_UART3_RX);//TX RX����

    //�����жϿ��������ȼ�
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
    nrf_uart_int_disable(NRF_UART0, NRF_UART_INT_MASK_TXDRDY);	
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXTO);
    nrf_uart_int_enable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY | NRF_UART_INT_MASK_RXTO);   
    nrf_drv_common_irq_enable(UART0_IRQn, APP_IRQ_PRIORITY_LOW);

    //����UART����
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_ERROR);
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
    uarton_status = true; //״̬���� 
    //����UART0
    nrf_uart_enable(NRF_UART0);
    nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTRX); 

//    //����֡��ʱ��ʱ��
//    err_code = app_timer_create(&m_ms_1_single_timer_id,
//                                APP_TIMER_MODE_SINGLE_SHOT,
//                                ms_1_single_timeout_handler);		

//    APP_ERROR_CHECK(err_code);
}


//-----------�ر�UART0�ͻָ�IO-----------------------------------
void myuart_stop(void)
{
    //�ص�UART
    nrf_uart_disable(NRF_UART0);
    uarton_status = false;
    //IO�ָ�Ĭ��ֵ
    nrf_gpio_cfg_input(MCU_UART3_RX,NRF_GPIO_PIN_NOPULL);      //uart_rx	
    nrf_gpio_cfg_output(MCU_UART3_TX);                         //uart_tx
    nrf_gpio_pin_clear(MCU_UART3_TX);		
}

//-----------����һ�����ݣ���������ָ��ͳ���--------------------
void myuart_send(const uint8_t *p_data, uint8_t length)
{   
    uint8_t count;
    if(length && uarton_status)
    {
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
        nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTTX); //��������	
    
        for (count=0; count<length; count++)
        {
            if(uarton_status == false)
            {
                break;
            }
            nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);//�巢��׼��   
            nrf_uart_txd_set(NRF_UART0,p_data[count]);//����һ������
            //�ȷ��ͳɹ�
            while (!nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_TXDRDY))
            {
                if(uarton_status == false)
                {
                    break;
                }
            }
        }
        nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPTX); //ֹͣ���ͻ���	 
    }	
}	

////------------NB���Ѵ���--------------------------------------------
//static void DSR_Wake_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
//{ 
//    gsm_mevent.b.have_wakeup_uartst = true;
//    NRF_LOG_DIRECT("dsr wake handler!\r\n");
//    nrf_gpio_pin_clear(WAKEUP_DTR);	//����NBģ��
//    if(Sys_Lock_State_Struct.c1100_mode_flag == C1100_MODE_DRX_SLEEP)
//    {
//        Set_Task(COMM, COMM_READDATA);				
//    }	
//}

////--------------�ϵ��ʼ��--------------------------------------------
//void init_dsr_detection(void)
//{
//    //����ʱ��ʼ��GSM״̬ 
////		gsm_mevent.Whole=0;

//    //����NB���ѹ���	
//    nrf_drv_gpiote_in_config_t config = {.is_watcher = false,                     \
//                                         .hi_accuracy = false,                    \
//                                         .pull = NRF_GPIO_PIN_NOPULL,             \
//                                         .sense = NRF_GPIOTE_POLARITY_HITOLO,     \
//                                        };
//    //PULLDOWNΪ��ʡ��
//    nrf_drv_gpiote_in_init(WAKEUP_CHECK, &config, DSR_Wake_handler);//  
//    gsm_mevent.b.have_wakeup_uartst = false;				 
//    nrf_drv_gpiote_in_event_enable(WAKEUP_CHECK,true);
//}

////--------------�ر�DSR���жϹ���---------------------------------------
//void stop_dsr_detection(void)
//{
//    nrf_drv_gpiote_in_uninit(WAKEUP_CHECK);
//    gsm_mevent.b.have_wakeup_uartst = false;
//    nrf_drv_gpiote_in_event_disable(WAKEUP_CHECK);
//}

////------------���� NB--------
//void  openGprs(void)
//{	
//    nrf_gpio_pin_set(WAKEUP_DTR);	
//    nrf_gpio_pin_set(NB_PWRONKEY);
//    nrf_gpio_pin_set(NB_RESET);	 
//    nrf_gpio_pin_set(NB_PWRON);	//NB��Դ����
//    sys_ndelay(3);
//    nrf_gpio_pin_clear(NB_PWRONKEY);	//����PWRONKEY

//    //��������
//    myuart_start();
//    //��ʼ�����һЩ��־	
//    gsm_mevent.Whole=0;
//    send_at_times=0;
//    clk_gprs_sendx100ms=0;
//    gsm_mevent.b.gprs_pwronst=1;
//}
////----------------�ر�NB----------------------
//void stopGprs(void)
//{
//	  //------��Ҫ�Ĺػ�����-----------------	
//		//myuart_send_char("AT+QICLOSE\r\n");
//		//sys_ndelay(3);
//		//AT+QIDEACT
//		nrf_gpio_pin_clear(NB_PWRON);	
//		nrf_gpio_pin_clear(NB_PWRONKEY);	
//		nrf_gpio_pin_clear(NB_RESET);	 //
//	nrf_gpio_pin_clear(WAKEUP_DTR);	//
//		
//		//�رմ���	
//		myuart_stop();
//		gsm_mevent.Whole=0;	
//		//gsm_mevent.b.gprs_pwronst=0;
//}

//void start_to_reconnect(void)
//{
//	//��������
//}
//void start_to_resetgprs(void)
//{	 //��������	
//}	
////=============����һ���ַ���  \0����== �255�ֽ�======================
//void myuart_send_char(const char *pstr)
//{ 
//		uint8_t len,i;	
//		len = 0;
//		for(i=0; i<255; i++)
//		{
//				if(pstr[i] == '\0')
//				{
//					break;	
//				}
//				len++;     		
//		}	 
//		myuart_send((const uint8_t *)pstr, len);
//}

////-------�Ƚ��ַ���------------------------------------------------
//bool cmp_std_string(char  * p_data,const char * s_data )
//{  
//		uint8_t i;
//		for( i= 0; i <UART_BUF_MAX_LEN; i++)
//		{
//				if((s_data[i]=='\0')||(s_data[i]=='\n'))
//				{
//						return (true);
//				}
//				if((char)p_data[i]!=s_data[i])
//				{
//						break;
//				}
//		}
//		return(false);
//}


//bool search_head_byte(const char * s_data,uint8_t *start_ptr )
//{	   
//		while(1)
//		{
//				if(*start_ptr == uart_rx_push_ptr)
//				{
//						return(false);//���
//				}
//				if((char)uart_rx_buf[*start_ptr]==*s_data)
//				{				 
//						break;
//				}
//				if(++(*start_ptr)>=UART_BUF_MAX_LEN)
//				{
//						*start_ptr=0;
//				}
//				if((char)uart_rx_buf[*start_ptr]=='\0'||((char)uart_rx_buf[*start_ptr]=='\n'))
//				{
//						return(false);//�ѵ�����Ҳû��
//				}
//		}
//		return (true);		
//}	

////=====����ʼλ�������Ƚϲ����ַ���
//bool cmp_uart_part_string(const char * s_data,uint8_t *start_ptr) //�ӿ�ʼָ�뿪ʼ�����ַ���
//{   
//		uint8_t ptr_tmp;
//		uint8_t i;
//	  
//	  ptr_tmp = *start_ptr;
//	  i       = 0;
//	
//    while(search_head_byte(s_data,start_ptr ))
//		{
//				//�ѵ�����ĸ��ͬ
//				i=0;
//				ptr_tmp = *start_ptr;
//				while(1)
//				{	
//						if((s_data[i]=='\0')||(s_data[i]=='\n'))
//						{
//								*start_ptr = ptr_tmp;
//								return (true);//�ѵ���ͬ����					
//						}
//						if(ptr_tmp == uart_rx_push_ptr)
//						{				  //���
//								return(false);
//						}	
//						if((uart_rx_buf[ptr_tmp]=='\0')||(uart_rx_buf[ptr_tmp]=='\n'))
//						{
//								return false;
//						}//�ѽ���
//						if((s_data[i]==(char)uart_rx_buf[ptr_tmp])||s_data[i]=='*')
//						{
//								//�ȶ���ͬ������һ��ĸ
//								if(++ptr_tmp >= UART_BUF_MAX_LEN)
//								{
//										ptr_tmp=0;//����ָ��
//								}
//								i++;				
//						}
//						else
//						{
//								//����ͬ
//								if(++(*start_ptr) >= UART_BUF_MAX_LEN)
//								{
//										*start_ptr=0;//ָ��ָ����һ					 
//								}
//								break;//�˳�		����һϵ����ͬ����ĸʱstart_ptrҲ�ڲ��ϵ���ǰ��		
//						}			
//				}
//		}
//		return(false);//�����Ҳû��
//}	

////==========��һ�п�ʼ��������,��ָ�������β����ָ�����START_PTR===============
//bool search_uart_string(const char * s_data,uint8_t *start_ptr) 
//{
//		*start_ptr = uart_rx_pop_ptr;
//		return(cmp_uart_part_string(s_data,start_ptr));
//}


////===========Ϊ�˼���һ�����ݣ�OK==========================
//void update_uart_rx_pop_ptr(void)
//{ 
//		uint8_t pop_tmp;
//		pop_tmp = uart_rx_pop_ptr;
//		if(uart_rx_number)
//		{
//				while(1)
//				{ 
//						if(pop_tmp == uart_rx_push_ptr)
//						{
//								//���
//								uart_rx_number = 0;
//								break;
//						}				
//						if((uart_rx_buf[pop_tmp]=='\n')||(uart_rx_buf[pop_tmp]=='\0'))
//						{ 
//								//һ�����ݽ�β
//								if(++pop_tmp >= UART_BUF_MAX_LEN)
//								{
//										pop_tmp = 0;
//								}
//								uart_rx_number--;
//								break;
//						}			
//						if(++pop_tmp >= UART_BUF_MAX_LEN)
//						{
//								pop_tmp = 0;
//						}
//				}
//				uart_rx_pop_ptr = pop_tmp;	//���µ���ָ��
//		}
//}


////void myuart_task(void)
////{ uint8_t start_ptr;
////	//-------------����AT����-------------------------
////  if(!gsm_mevent.b.gprs_atokst)
////	{
////		if(uart_rx_number)
////		{//�н��գ����Ƿ���OK
////			start_ptr=uart_rx_pop_ptr;
////		 if(search_uart_string("OK",&start_ptr))
////		  {//�յ�OK
////			 send_at_times=0;	
////		   gsm_mevent.b.gprs_atokst=1;	
////			 nrf_gpio_pin_clear (NB_PWRONKEY);//�˳�����ģʽ
////			 sys_ndelay(2);      			
////		  }		
////      update_uart_rx_pop_ptr();//����ʱҪ���µ���ָ�������			
////		}			
////	 else if(clk_gprs_sendx100ms>12)
////	 {clk_gprs_sendx100ms=0;
////		send_at_times++;
////	  myuart_send_char("AT\r\nAT\r\nAT\r\nAT\r\n");
////	 }
////	 if(send_at_times>20)//��������5�� 20
////	 {//����	
////    start_to_resetgprs();			
////	 }   	 
////	}
////	if(gsm_mevent.b.periph_errorst)
////	{ //���ڴ����� 
////	  myuart_stop();
////		sys_ndelay(3);
////		myuart_start();	
////		gsm_mevent.b.periph_errorst=0;
////	}
////}

void uart_rx_complete_callback(uint8_t *p_buf,uint16_t len)
{
//    if(Check_Strstr((char *)p_buf,"ERROR",len)==true)
//    {
//        if(RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_COPS5)
//        {
//            Sys_Flag_Struct.CEREG_flag = ENABLE;
//        }
//    }
//    if(Check_Strstr((char *)p_buf,NB_Cmd_Data_Struct[RE_NB_Cmd_Struct.current_NB_cmd].cmdtruebackstr,len)==true)
//    {
//        if(RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_LSIPPUSH)
//        {
//            NRF_LOG_DIRECT("trs data succeed\r\n");
//        }
//            
//        RE_NB_Cmd_Struct.rev_result = BACK_TRUE;
//        Disable_Timer_NB_Ack_Timeout();
//    }
//    else
//    {
//        if(RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_LSIPREAD_1)
//        {
//            if(Check_Strstr((char *)p_buf,"+LSIPREAD=1",len) == true)
//            {
//                RE_NB_Cmd_Struct.rev_result=BACK_ERROR;
//                Disable_Timer_NB_Ack_Timeout();
//            }						
//        }
//        if((RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_LSIPCALL) || (RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_LSIPCALL1))
//        {
//            if(Check_Strstr((char *)p_buf,"+LSIPCALL:0",len) == true)        //����δ����
//            {
//                RE_NB_Cmd_Struct.rev_result=BACK_ERROR;
//                Disable_Timer_NB_Ack_Timeout();
//            }
//        }
//        if(RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_LSIPCLOSE1)
//        {
//            if(Check_Strstr((char *)p_buf,"+LSIPCLOSE",len)==true)
//            {
//                RE_NB_Cmd_Struct.rev_result=BACK_ERROR;
//                Disable_Timer_NB_Ack_Timeout();
//            }
//        }				
//        if(RE_NB_Cmd_Struct.current_NB_cmd == NB_AT_LSIPOPEN)
//        {
//            if(Check_Strstr((char *)p_buf,"ERROR",len)==true)
//            {
//                RE_NB_Cmd_Struct.rev_result = BACK_ERROR;
//                Disable_Timer_NB_Ack_Timeout();
//            }
//        }
//    }		
//    
//    if(Check_Strstr((char *)p_buf,"+LSIPRTCP",len) == true)
//    {
//        NRF_LOG_DIRECT("receive data succeed\r\n");
//        Lipaidang_Sever_Package_Struct.rev_data_num++;
////				Set_Task(COMM,COMM_DECODE);
//    }
//    if(DISABLE == Sys_Flag_Struct.check_net_flag)
//    {
//        if(Sys_Net_State_Struct.net_state == CONN_ON)
//        {
//            if(Check_Strstr((char *)p_buf,"+LSIPCALL",len)==true)             //????PSM??,??MCU??????
//            {
//                Sys_Flag_Struct.trs_auth_flag = DISABLE;
//                Sys_Net_State_Struct.net_state = CONN_OFF;
//                Set_Task(COMM, COMM_NETWORK_CONN);
//                NRF_LOG_DIRECT("connet off\r\n");
//            }
//            else if(Check_Strstr((char *)p_buf,"+LSIPCLOSE",len)==true)             
//            {
//                Sys_Flag_Struct.trs_auth_flag = DISABLE;
//                Sys_Net_State_Struct.socket_state = CONN_OFF;
//                Set_Task(COMM, COMM_SOCKET_CONN);
//                NRF_LOG_DIRECT("socket off\r\n");
//            } 
//        }
//    }
//    if(Check_Strstr((char *)p_buf,"+CEREG 0",len) == true)
//    {
//    }
//    if(Check_Strstr((char *)p_buf,"+CGREG 0",len) == true)
//    {	
//    }
}


////void uart_send_string(const uint8_t *p_data,uint16_t length)
////{
////    for (uint32_t i = 0; i < length; i++)
////    {
////       while (app_uart_put(p_data[i]) != NRF_SUCCESS);
////    }	
////}



