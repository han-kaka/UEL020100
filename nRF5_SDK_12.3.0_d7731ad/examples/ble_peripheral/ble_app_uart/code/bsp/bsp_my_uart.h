#ifndef __LPD_BSP_MY_UART_H
#define __LPD_BSP_MY_UART_H

#include "nrf_drv_uart.h"
#include "global.h"

typedef union 
{ uint32_t Whole;
	struct
  {
  uint8_t gprs_pwronst           :1;
	uint8_t have_wakeup_uartst     :1;
	uint8_t gprs_atokst            :1;
	uint8_t periph_errorst         :1; //´®¿Ú´íÎó
	uint8_t receiverightst         :1; //ÊÕµ½">"
	uint8_t reversed5     :1;	
	uint8_t reversed6     :1;	
	uint8_t reversed7     :1;
		
	uint8_t reversed8     :1;
	uint8_t reversed9     :1;
	uint8_t reversed10    :1;
	uint8_t reversed11    :1;	
	uint8_t reversed12    :1; 
	uint8_t reversed13    :1;	
	uint8_t reversed14    :1;	
	uint8_t reversed15    :1;
		
	uint8_t reversed16    :1;
	uint8_t reversed17    :1;
	uint8_t reversed18    :1;
	uint8_t reversed19    :1;	
	uint8_t reversed20    :1; 
	uint8_t reversed21    :1;	
	uint8_t reversed22    :1;	
	uint8_t reversed23    :1;

  uint8_t reversed24    :1;
	uint8_t reversed25    :1;
	uint8_t reversed26    :1;
	uint8_t reversed27    :1;	
	uint8_t reversed28    :1; 
	uint8_t reversed29    :1;	
	uint8_t reversed30    :1;	
	uint8_t reversed31    :1;
	
  }b;
}	Uart_Mevent_Type;

////typedef enum {
////		DATA_REC_NO_FINISH				= 0,
////		DATA_REC_IS_ING						= 1,
////		DATA_REC_FINISH						= 2
////}rec_state_type;

typedef struct 
{		
//    rec_state_type   rec_data_state;
    uint16_t         rec_data_len;
    uint16_t         len_count;
}Receive_Data_Type;

extern Uart_Mevent_Type Uart_Mevent_Struct;
//extern volatile bool uarton_status;
//extern uint8_t volatile clk_gprs_sendx100ms;

extern Receive_Data_Type Receive_Data_Struct;


//uart
void myuart_start(void);
void myuart_stop(void);
void myuart_send(const uint8_t *p_data, uint8_t length);
////string
//bool cmp_uart_part_string(const char * s_data,uint8_t *start_ptr);
//bool search_uart_string(const char * s_data,uint8_t *start_ptr);
//bool search_head_byte(const char * s_data,uint8_t *start_ptr);
//bool cmp_std_string(char  * p_data,const char * s_data );

////NB
////void myuart_task(void);
//void init_dsr_detection(void);
//void stop_dsr_detection(void);
//void openGprs(void);
//void stopGprs(void);

//void myuart_send_char(const char *pstr);

void uart_rx_complete_callback(uint8_t *p_buf,uint16_t len);
////void uart_send_string(const uint8_t *p_data,uint16_t length);

#endif
