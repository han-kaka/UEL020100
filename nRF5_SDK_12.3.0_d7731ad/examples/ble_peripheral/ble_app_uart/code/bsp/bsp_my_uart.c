#include "app_util_platform.h"
#include "nrf_drv_common.h"
#include "app_timer.h"
#include "nrf_drv_gpiote.h"

//用户头文件
#include "bsp_my_uart.h"


//#define MS_1_SINGLE_INTERVAL            APP_TIMER_TICKS(1, APP_RTC_TIMER_PRESCALER)     /**< 1ms time interval (ticks). */
//#define UART_BUF_MAX_LEN  255

//APP_TIMER_DEF(m_ms_1_single_timer_id);                    /**< 1 ms single shot timer. */

Receive_Data_Type                          Receive_Data_Struct={0};

volatile bool uarton_status;
Uart_Mevent_Type Uart_Mevent_Struct;
//uint8_t volatile clk_gprs_sendx100ms,send_at_times;
////接收缓存
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
//char ipd_buffer[IPD_BUF_MAXLEN];//IPD数据缓存






//=====初始化接收缓存========================
void init_uart_buf(void)
{ 	
		uart_rx_push_ptr = 0;
		uart_rx_pop_ptr  = 0;
		uart_rx_number   = 0;	
}

//=====串口中断==================================
 void UART0_IRQHandler(void)
{    
		if(nrf_uart_int_enable_check(NRF_UART0, NRF_UART_INT_MASK_ERROR) &&
       nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_ERROR))
    {
				//此处检测是否有错误的中断激活,错误事件标志        
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_ERROR);
        nrf_uart_int_disable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY | NRF_UART_INT_MASK_ERROR);
        if (uarton_status == false)
        {
            nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPRX);
        }
				Uart_Mevent_Struct.b.periph_errorst = 1;//硬件错误需重启UART
    }
    else if(nrf_uart_int_enable_check(NRF_UART0, NRF_UART_INT_MASK_RXDRDY) &&
            nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_RXDRDY))
    {
				//接收数据
				nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);//清事件标志

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
					
				//------------若非RX激活，则停止接收任务--------------------------
				if(uarton_status == false)
				{
						nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPRX);
				}   
    }
  
    if (nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_RXTO))
    {
				//接收溢出处理
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXTO);
        if(uarton_status)
        {
						//若开启RX则重启
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

//=========配置和启动UART=============================================
void myuart_start(void)
{	
//    uint32_t err_code;		

    if(uarton_status)
    {
        return;	
    }
    //初始化缓存		
    init_uart_buf();	

    //配置串口IO	
    //nrf_gpio_pin_set(TX_PIN_SELF);
    //nrf_gpio_cfg_output(TX_PIN_SELF);
    //nrf_gpio_cfg_input(RX_PIN_SELF, NRF_GPIO_PIN_NOPULL);

    nrf_uart_baudrate_set(NRF_UART0, NRF_UART_BAUDRATE_9600);//14400;//19200;//115200;////9600;//慢一点);
    nrf_uart_configure(NRF_UART0, NRF_UART_PARITY_EXCLUDED, NRF_UART_HWFC_DISABLED);//无奇偶，关闭流控制
    nrf_uart_txrx_pins_set(NRF_UART0, MCU_UART3_TX, MCU_UART3_RX);//TX RX配置

    //设置中断开启和优先级
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
    nrf_uart_int_disable(NRF_UART0, NRF_UART_INT_MASK_TXDRDY);	
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXTO);
    nrf_uart_int_enable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY | NRF_UART_INT_MASK_RXTO);   
    nrf_drv_common_irq_enable(UART0_IRQn, APP_IRQ_PRIORITY_LOW);

    //开启UART接收
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_ERROR);
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
    uarton_status = true; //状态变真 
    //开启UART0
    nrf_uart_enable(NRF_UART0);
    nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTRX); 

//    //创建帧超时定时器
//    err_code = app_timer_create(&m_ms_1_single_timer_id,
//                                APP_TIMER_MODE_SINGLE_SHOT,
//                                ms_1_single_timeout_handler);		

//    APP_ERROR_CHECK(err_code);
}


//-----------关闭UART0和恢复IO-----------------------------------
void myuart_stop(void)
{
    //关掉UART
    nrf_uart_disable(NRF_UART0);
    uarton_status = false;
    //IO恢复默认值
    nrf_gpio_cfg_input(MCU_UART3_RX,NRF_GPIO_PIN_NOPULL);      //uart_rx	
    nrf_gpio_cfg_output(MCU_UART3_TX);                         //uart_tx
    nrf_gpio_pin_clear(MCU_UART3_TX);		
}

//-----------发送一串数据，输入数组指针和长度--------------------
void myuart_send(const uint8_t *p_data, uint8_t length)
{   
    uint8_t count;
    if(length && uarton_status)
    {
        nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);
        nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTTX); //开启发送	
    
        for (count=0; count<length; count++)
        {
            if(uarton_status == false)
            {
                break;
            }
            nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_TXDRDY);//清发送准备   
            nrf_uart_txd_set(NRF_UART0,p_data[count]);//发送一个数据
            //等发送成功
            while (!nrf_uart_event_check(NRF_UART0, NRF_UART_EVENT_TXDRDY))
            {
                if(uarton_status == false)
                {
                    break;
                }
            }
        }
        nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STOPTX); //停止发送机制	 
    }	
}	

////------------NB唤醒处理--------------------------------------------
//static void DSR_Wake_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
//{ 
//    gsm_mevent.b.have_wakeup_uartst = true;
//    NRF_LOG_DIRECT("dsr wake handler!\r\n");
//    nrf_gpio_pin_clear(WAKEUP_DTR);	//唤醒NB模块
//    if(Sys_Lock_State_Struct.c1100_mode_flag == C1100_MODE_DRX_SLEEP)
//    {
//        Set_Task(COMM, COMM_READDATA);				
//    }	
//}

////--------------上电初始化--------------------------------------------
//void init_dsr_detection(void)
//{
//    //开机时初始化GSM状态 
////		gsm_mevent.Whole=0;

//    //开启NB唤醒功能	
//    nrf_drv_gpiote_in_config_t config = {.is_watcher = false,                     \
//                                         .hi_accuracy = false,                    \
//                                         .pull = NRF_GPIO_PIN_NOPULL,             \
//                                         .sense = NRF_GPIOTE_POLARITY_HITOLO,     \
//                                        };
//    //PULLDOWN为了省电
//    nrf_drv_gpiote_in_init(WAKEUP_CHECK, &config, DSR_Wake_handler);//  
//    gsm_mevent.b.have_wakeup_uartst = false;				 
//    nrf_drv_gpiote_in_event_enable(WAKEUP_CHECK,true);
//}

////--------------关闭DSR脚中断功能---------------------------------------
//void stop_dsr_detection(void)
//{
//    nrf_drv_gpiote_in_uninit(WAKEUP_CHECK);
//    gsm_mevent.b.have_wakeup_uartst = false;
//    nrf_drv_gpiote_in_event_disable(WAKEUP_CHECK);
//}

////------------启动 NB--------
//void  openGprs(void)
//{	
//    nrf_gpio_pin_set(WAKEUP_DTR);	
//    nrf_gpio_pin_set(NB_PWRONKEY);
//    nrf_gpio_pin_set(NB_RESET);	 
//    nrf_gpio_pin_set(NB_PWRON);	//NB电源开启
//    sys_ndelay(3);
//    nrf_gpio_pin_clear(NB_PWRONKEY);	//拉低PWRONKEY

//    //开启串口
//    myuart_start();
//    //初始缓存和一些标志	
//    gsm_mevent.Whole=0;
//    send_at_times=0;
//    clk_gprs_sendx100ms=0;
//    gsm_mevent.b.gprs_pwronst=1;
//}
////----------------关闭NB----------------------
//void stopGprs(void)
//{
//	  //------必要的关机流程-----------------	
//		//myuart_send_char("AT+QICLOSE\r\n");
//		//sys_ndelay(3);
//		//AT+QIDEACT
//		nrf_gpio_pin_clear(NB_PWRON);	
//		nrf_gpio_pin_clear(NB_PWRONKEY);	
//		nrf_gpio_pin_clear(NB_RESET);	 //
//	nrf_gpio_pin_clear(WAKEUP_DTR);	//
//		
//		//关闭串口	
//		myuart_stop();
//		gsm_mevent.Whole=0;	
//		//gsm_mevent.b.gprs_pwronst=0;
//}

//void start_to_reconnect(void)
//{
//	//重连机制
//}
//void start_to_resetgprs(void)
//{	 //重启机制	
//}	
////=============发送一个字符串  \0结束== 最长255字节======================
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

////-------比较字符串------------------------------------------------
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
//						return(false);//溢出
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
//						return(false);//搜到结束也没有
//				}
//		}
//		return (true);		
//}	

////=====从起始位置搜索比较部分字符串
//bool cmp_uart_part_string(const char * s_data,uint8_t *start_ptr) //从开始指针开始部分字符串
//{   
//		uint8_t ptr_tmp;
//		uint8_t i;
//	  
//	  ptr_tmp = *start_ptr;
//	  i       = 0;
//	
//    while(search_head_byte(s_data,start_ptr ))
//		{
//				//搜到首字母相同
//				i=0;
//				ptr_tmp = *start_ptr;
//				while(1)
//				{	
//						if((s_data[i]=='\0')||(s_data[i]=='\n'))
//						{
//								*start_ptr = ptr_tmp;
//								return (true);//搜到相同内容					
//						}
//						if(ptr_tmp == uart_rx_push_ptr)
//						{				  //溢出
//								return(false);
//						}	
//						if((uart_rx_buf[ptr_tmp]=='\0')||(uart_rx_buf[ptr_tmp]=='\n'))
//						{
//								return false;
//						}//已结束
//						if((s_data[i]==(char)uart_rx_buf[ptr_tmp])||s_data[i]=='*')
//						{
//								//比对相同进入下一字母
//								if(++ptr_tmp >= UART_BUF_MAX_LEN)
//								{
//										ptr_tmp=0;//更新指针
//								}
//								i++;				
//						}
//						else
//						{
//								//不相同
//								if(++(*start_ptr) >= UART_BUF_MAX_LEN)
//								{
//										*start_ptr=0;//指针指向下一					 
//								}
//								break;//退出		若有一系列相同的字母时start_ptr也在不断的往前走		
//						}			
//				}
//		}
//		return(false);//到溢出也没有
//}	

////==========从一行开始搜索数据,并指向此数据尾部的指针存入START_PTR===============
//bool search_uart_string(const char * s_data,uint8_t *start_ptr) 
//{
//		*start_ptr = uart_rx_pop_ptr;
//		return(cmp_uart_part_string(s_data,start_ptr));
//}


////===========为了减掉一行数据，OK==========================
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
//								//溢出
//								uart_rx_number = 0;
//								break;
//						}				
//						if((uart_rx_buf[pop_tmp]=='\n')||(uart_rx_buf[pop_tmp]=='\0'))
//						{ 
//								//一行数据结尾
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
//				uart_rx_pop_ptr = pop_tmp;	//更新弹出指针
//		}
//}


////void myuart_task(void)
////{ uint8_t start_ptr;
////	//-------------开机AT测试-------------------------
////  if(!gsm_mevent.b.gprs_atokst)
////	{
////		if(uart_rx_number)
////		{//有接收，查是否有OK
////			start_ptr=uart_rx_pop_ptr;
////		 if(search_uart_string("OK",&start_ptr))
////		  {//收到OK
////			 send_at_times=0;	
////		   gsm_mevent.b.gprs_atokst=1;	
////			 nrf_gpio_pin_clear (NB_PWRONKEY);//退出开机模式
////			 sys_ndelay(2);      			
////		  }		
////      update_uart_rx_pop_ptr();//结束时要更新弹出指针和行数			
////		}			
////	 else if(clk_gprs_sendx100ms>12)
////	 {clk_gprs_sendx100ms=0;
////		send_at_times++;
////	  myuart_send_char("AT\r\nAT\r\nAT\r\nAT\r\n");
////	 }
////	 if(send_at_times>20)//开机键须5秒 20
////	 {//重启	
////    start_to_resetgprs();			
////	 }   	 
////	}
////	if(gsm_mevent.b.periph_errorst)
////	{ //串口错误处理 
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
//            if(Check_Strstr((char *)p_buf,"+LSIPCALL:0",len) == true)        //网络未连上
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



