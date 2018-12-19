#include "bsp_timer.h"
#include "app_timer.h"


#define MS_10_REPEAT_INTERVAL             APP_TIMER_TICKS(10, APP_RTC_TIMER_PRESCALER)   /**< 1s time interval (ticks). */
#define MS_100_REPEAT_INTERVAL             APP_TIMER_TICKS(100, APP_RTC_TIMER_PRESCALER)   /**< 1s time interval (ticks). */
//#define MS_50_REPEAT_INTERVAL              APP_TIMER_TICKS(50, APP_RTC_TIMER_PRESCALER)    /**< 1s time interval (ticks). */

APP_TIMER_DEF(m_clock_timer_id);                  /**申请系统定时器 */
//APP_TIMER_DEF(m_qf_timer_id);                     /**申请系统定时器 */

static uint16_t s_MsCount=0;

volatile Tim_1s_Type Tim_1s_Struct;
volatile Tim_100ms_Type Tim_100ms_Struct;

void m_clock_timeout_handler (void * p_context)
{ 
    s_MsCount++;
    if(Task_Flag_Struct.closeStaProc == DISABLE)
    {
        APP_NB_TimeProc(10);
    }
		
		Bsp_Led_Tim(10);
		Bsp_Motor_Tim(10);
//    APP_PC_UART_TimeProc(10);
//    APP_Charge_TimeProc(10);
    if(s_MsCount >= MS_TO_S)     // 1S时间到
    {		
				s_MsCount =0;
				if(Task_Flag_Struct.writeRom_F != DISABLE || Task_Flag_Struct.writeDynamic_F != DISABLE)
				{
						Tim_1s_Struct.rewriteCount++;
						if(Tim_1s_Struct.rewriteCount >= 180)   //如写未成功，那么间隔3MIN，重写
						{
								 Tim_1s_Struct.rewriteCount = 0; 
								 if(Task_Flag_Struct.writeRom_F != DISABLE)
								 {
											Set_Task(MEM_WRITE,MEM_STORE_SOLID_ROMDATA);
								 }
								 if(Task_Flag_Struct.writeDynamic_F != DISABLE)
								 {
											Set_Task(MEM_WRITE,MEM_STORE_DYNAMIC_ROMDATA);
								 }
						}
				}		
				if(Task_Flag_Struct.closeStaProc != DISABLE) //重新打开NB状态处理计数
				{
						Tim_1s_Struct.reOpenStaProcCount++;
						if(Tim_1s_Struct.reOpenStaProcCount >= RE_OPEN_STA_PROC_TIME)
						{
								 Tim_1s_Struct.reOpenStaProcCount = 0;
								 Task_Flag_Struct.closeStaProc = DISABLE;
						}
				}
		}
		
		
		
		
		
//		if(Sys_Flag_Struct.air_pressure_flag == ENABLE)
//		{
//				//Set_Task(MEASURE, MEASURE_CMP);
//		}
//    if((++Tim_100ms_Struct.sys_timex100ms) > 9)
//    {
//				//1s timer
//				Tim_100ms_Struct.sys_timex100ms = 0;
//				Set_Task(TIMER, TIMER_1S);
////			
////				Set_Task(MEASURE, MEASURE_VOLTAGE);
////				motor_inflate_timer();
////				//Mpu6050_test_data();
////				hp303_id_cheak();
////				//ReadCalCoef();
////				//ReadHP303B();
//		}
//		Set_Task(TIMER, TIMER_100MS);
////		add_air_pressure_timer();
////		oled_display_timer();
////		//switch_botton_timer();
////		button_led();
////		if(oled_battery_level != 1)
////		{
////				button_delay_timer();
////				m_qf_delay_timer();
////				right_button_timer();
////				lift_button_timer();
////				cmp_button_timer();
////				m_home_button_timer();
////				version_button_timer();
////		}
}	

void init_systime(void)
{   
		uint32_t err_code;
	
		err_code = app_timer_create(&m_clock_timer_id, APP_TIMER_MODE_REPEATED, m_clock_timeout_handler);
		APP_ERROR_CHECK(err_code);
//		err_code = app_timer_create(&m_qf_timer_id, APP_TIMER_MODE_SINGLE_SHOT, m_qf_timeout_handler);
//		APP_ERROR_CHECK(err_code);
}

void start_systime(void)
{ 
		uint32_t err_code; 
	
	  err_code = app_timer_stop(m_clock_timer_id);
	  APP_ERROR_CHECK(err_code);
	
//		m_clkx100ms = 0;
//	
//		Tim_100ms_Struct.m_clkx100ms = 0;		
	
		err_code = app_timer_start(m_clock_timer_id, MS_10_REPEAT_INTERVAL, NULL);
		APP_ERROR_CHECK(err_code); 
}











