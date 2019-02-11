#include "bsp_timer.h"
#include "app_timer.h"


#define MS_10_REPEAT_INTERVAL             APP_TIMER_TICKS(10, APP_RTC_TIMER_PRESCALER)   /**< 1s time interval (ticks). */
#define MS_100_REPEAT_INTERVAL             APP_TIMER_TICKS(100, APP_RTC_TIMER_PRESCALER)   /**< 1s time interval (ticks). */
//#define MS_50_REPEAT_INTERVAL              APP_TIMER_TICKS(50, APP_RTC_TIMER_PRESCALER)    /**< 1s time interval (ticks). */

APP_TIMER_DEF(m_system_timer_id);                  /**申请系统定时器 */
APP_TIMER_DEF(m_motor_timer_id);                   /**申请系统定时器 */

uint8_t timestamp[8] = {0};

volatile Tim_1s_Type Tim_1s_Struct;
volatile Tim_Ms_Type Tim_Ms_Struct;

static uint8_t monthLength( uint8_t lpyr, uint8_t mon );

void m_system_timeout_handler (void *p_context)
{ 
    Tim_Ms_Struct.sys_ms_count++;
		Tim_Ms_Struct.m_dly_ms++;
	
    if(Task_Flag_Struct.closeStaProc == DISABLE)
    {
//        APP_NB_TimeProc(10);
    }
		
		Bsp_Led_Tim(10);
		Bsp_Motor_Tim(10);
//    APP_PC_UART_TimeProc(10);
//    APP_Charge_TimeProc(10);
    if(Tim_Ms_Struct.sys_ms_count >= MS_TO_S)     // 1S时间到
    {		
				Tim_Ms_Struct.sys_ms_count -= MS_TO_S;
				if(Task_Flag_Struct.motordelay == ENABLE)
				{
						Tim_1s_Struct.motor_delay_count++;
						if(Tim_1s_Struct.motor_delay_count >= 2)
						{
								Tim_1s_Struct.motor_delay_count = 0;
								Task_Flag_Struct.motordelay = DISABLE;
								if(FOREWARD == Motor_OldStatus)
								{
										Motor_State_Struct = INVERSION;
								}
								else if(INVERSION == Motor_OldStatus)
								{
										Motor_State_Struct = FOREWARD;
								}
						}
				}
			
				if(Task_Flag_Struct.writeRom_F != DISABLE || Task_Flag_Struct.writeDynamic_F != DISABLE)
				{
						Tim_1s_Struct.rewriteCount++;
						if(Tim_1s_Struct.rewriteCount >= 180)   //如写未成功，那么间隔3MIN，重写
						{
								 Tim_1s_Struct.rewriteCount = 0; 
								 if(Task_Flag_Struct.writeRom_F != DISABLE)
								 {
											set_task(MEM_WRITE,MEM_STORE_SOLID_ROMDATA);
								 }
								 if(Task_Flag_Struct.writeDynamic_F != DISABLE)
								 {
											set_task(MEM_WRITE,MEM_STORE_DYNAMIC_ROMDATA);
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

void m_motor_timeout_handler(void *p_context)
{
//    setWDI();//在系统慢速定时器内清零	
			switch (use_channel_enum)
			{  
					case LOCKMOTOR_PWM_CHANNEL:
					{
							if(++Motor_Struct.motor_times > LOCK_PWM_DRIVE_TIME)
							{
//									stop_lockmotor_pwm();	
							}	
					}                    
							break;
					
					case UNLOCKMOTOR_FULL_CHANNEL:
					{
							//超时
							if(++Motor_Struct.motor_times > UNLOCK_FULL_DRIVE_TIME)
							{
									motor_state_struct.stop_motor_st = 1; 
							}
					}
							break;	
					
					case LOCKMOTOR_FULL_CHANNEL:
					{
							//超时
							if(++Motor_Struct.motor_times > LOCK_FULL_DRIVE_TIME)
							{
									motor_state_struct.stop_motor_st = 1; 
							}
					}                    
							break; 	
					
					case UNLOCKMOTOR_PWM_CHANNEL:
					{
							//超时处理
							if(++Motor_Struct.motor_times > UNLOCK_PWM_DRIVE_TIME)
							{
//									stop_unlockmotor_pwm();		//2.5s	
							}											
					}                    
							break;
					
					default:
							break;									 
			} 
}

//初始化马达的定时器
void init_motor_timer(void)
{
    uint32_t err_code;	
	
    err_code = app_timer_create(&m_motor_timer_id, APP_TIMER_MODE_REPEATED, m_motor_timeout_handler); //定时器号，定时器配置内定16MHZ(已处理预分频）16位宽,低优先权	
    APP_ERROR_CHECK(err_code); 
}

//开启马达定时器
void start_motor_time(void)
{ 
		uint32_t err_code; 
	  err_code = app_timer_stop(m_motor_timer_id);
	  APP_ERROR_CHECK(err_code);	
	
		err_code = app_timer_start(m_motor_timer_id, MS_100_REPEAT_INTERVAL, NULL);
		APP_ERROR_CHECK(err_code); 
}

//关闭马达定时器
void stop_motor_time(void)
{ 
		uint32_t err_code; 
	  err_code = app_timer_stop(m_motor_timer_id);
	  APP_ERROR_CHECK(err_code);	
}

//初始化系统定时器
void init_system_time(void)
{   
		uint32_t err_code;
	
		err_code = app_timer_create(&m_system_timer_id, APP_TIMER_MODE_REPEATED, m_system_timeout_handler);
		APP_ERROR_CHECK(err_code);
}

//开启系统定时器
void start_system_time(void)
{ 
		uint32_t err_code; 
	
	  err_code = app_timer_stop(m_system_timer_id);
	  APP_ERROR_CHECK(err_code);	
	
		err_code = app_timer_start(m_system_timer_id, MS_10_REPEAT_INTERVAL, NULL);
		APP_ERROR_CHECK(err_code); 
}

/*********************************************************************
 * @fn      monthLength
 *
 * @param   lpyr - 1 for leap year, 0 if not
 *
 * @param   mon - 0 - 11 (jan - dec)
 *
 * @return  number of days in specified month
 */
static uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
  uint8_t days = 31;

  if ( mon == 1 ) // feb
  {
    days = ( 28 + lpyr );
  }
  else
  {
    if ( mon > 6 ) // aug-dec
    {
      mon--;
    }

    if ( mon & 1 )
    {
      days = 30;
    }
  }

  return ( days );
}

/*********************************************************************
 * @fn      osal_ConvertUTCTime
 *
 * @brief   Converts UTCTime to UTCTimeStruct
 *
 * @param   tm - pointer to breakdown struct
 *
 * @param   secTime - number of seconds since 0 hrs, 0 minutes,
 *          0 seconds, on the 1st of January 2000 UTC
 *
 * @return  none
 */
void osal_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
  // calculate the time less than a day - hours, minutes, seconds
  {
    uint32_t day = secTime % DAY; 
    tm->seconds = day % 60UL;
    tm->minutes = (day % 3600UL) / 60UL;
    tm->hour = day / 3600UL;
  }

  // Fill in the calendar - day, month, year
  {
    uint16_t numDays = secTime / DAY;
    tm->year = BEGYEAR;
    while ( numDays >= YearLength( tm->year ) )
    {
      numDays -= YearLength( tm->year );
      tm->year++;
    }

    tm->month = 0;
    while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ) )
    {
      numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
      tm->month++;
    }

    tm->day = numDays;
  }
}

//延迟函数
void sys_ndelay(uint8_t x_ms)	
{
		//不要超过看门狗时间
		uint8_t temp;

		Tim_Ms_Struct.m_dly_ms = 0;
		while(true)
		{ 
				if(Tim_Ms_Struct.m_dly_ms >= x_ms)
				{
						break;	
				}	
				sd_app_evt_wait();	
//				setWDI();//在系统慢速定时器内清零	
				if(temp != Tim_Ms_Struct.m_dly_ms)
				{
						temp = Tim_Ms_Struct.m_dly_ms;
				}
		}
}







