#include "bsp_timer.h"
#include "app_timer.h"


#define MS_10_REPEAT_INTERVAL             APP_TIMER_TICKS(10, APP_RTC_TIMER_PRESCALER)   /**< 1s time interval (ticks). */
#define MS_100_REPEAT_INTERVAL             APP_TIMER_TICKS(100, APP_RTC_TIMER_PRESCALER)   /**< 1s time interval (ticks). */
//#define MS_50_REPEAT_INTERVAL              APP_TIMER_TICKS(50, APP_RTC_TIMER_PRESCALER)    /**< 1s time interval (ticks). */

APP_TIMER_DEF(m_system_timer_id);                  /**申请系统定时器 */
APP_TIMER_DEF(m_motor_timer_id);                   /**申请系统定时器 */

static uint16_t s_MsCount = 0;

uint8_t timestamp[8] = {0};

volatile Tim_1s_Type Tim_1s_Struct;
volatile Tim_100ms_Type Tim_100ms_Struct;

static uint8_t monthLength( uint8_t lpyr, uint8_t mon );

void m_system_timeout_handler (void *p_context)
{ 
    s_MsCount++;
    if(Task_Flag_Struct.closeStaProc == DISABLE)
    {
//        APP_NB_TimeProc(10);
    }
		
		Bsp_Led_Tim(10);
		Bsp_Motor_Tim(10);
//    APP_PC_UART_TimeProc(10);
//    APP_Charge_TimeProc(10);
    if(s_MsCount >= MS_TO_S)     // 1S时间到
    {		
				s_MsCount =0;
				if(Task_Flag_Struct.motordelay == ENABLE)
				{
						Tim_1s_Struct.motor_delay_count++;
						if(Tim_1s_Struct.motor_delay_count >= 2)
						{
								Tim_1s_Struct.motor_delay_count = 0;
								Task_Flag_Struct.motordelay = DISABLE;
								if(FOREWARD == Motor_OldStatus)
								{
										Motor_Status = INVERSION;
								}
								else if(INVERSION == Motor_OldStatus)
								{
										Motor_Status = FOREWARD;
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
//    switch(event_type)
//    {
//        case NRF_TIMER_EVENT_COMPARE0:
//        {
//            switch (use_channel_by)
//            {  
//                case LOCKMOTOR_PWM_CHANNEL:
//                {
//                    if(++driveMotorx64us>LOCK_PWM_DRIVE_TIME)
//                    {
//                            stop_lockmotor_pwm();	
//                    }	
//                }                    
//                    break;
//                
//                case UNLOCKMOTOR_FULL_CHANNEL:
//                {
//                    //超时
//                    if(++driveMotorx64us>UNLOCK_FULL_DRIVE_TIME)
//                    {
//                            app_mevent.b.stop_motor_st = 1;
//                    }
//                    //检测中间和开锁位置
//                    if((!nrf_gpio_pin_read(AtMiddle_PIN))||(!nrf_gpio_pin_read(OPENKEY_PIN)))
//                    {
//                            if(++testKeyx64us>TESTMIDDLEKEY_TIME)
//                            {
//                                    app_mevent.b.stop_motor_st = 1;									 
//                            }	
//                    }
//                }
//                    break;	
//                
//                case LOCKMOTOR_FULL_CHANNEL:
//                {
//                    if(++driveMotorx64us>LOCK_FULL_DRIVE_TIME)
//                    {
//                            app_mevent.b.stop_motor_st = 1;      //stop_lockmotor_pwm();		stop_lockmotor_pwm();
//                    }			
//                    if((!nrf_gpio_pin_read(AtMiddle_PIN))||(!nrf_gpio_pin_read(LOCKKEY_PIN)))	                         
//                    {
//                            if(++testKeyx64us>TESTMIDDLEKEY_TIME)
//                            {
//                                    app_mevent.b.stop_motor_st = 1;											 
//                            }	
//                    } 
//                }                    
//                    break; 	
//                
//                case UNLOCKMOTOR_PWM_CHANNEL:
//                {
//                    //超时处理
//                    if(++driveMotorx64us>UNLOCK_PWM_DRIVE_TIME)
//                    {
//                            stop_unlockmotor_pwm();		//2.5s	
//                    }											
//                    if(!nrf_gpio_pin_read(OPENKEY_PIN))
//                    {													
//                            if(++testKeyx64us>TESTUNLOCKKEY_TIME)
//                            {
//                                    stop_unlockmotor_pwm();													 
//                            }	
//                    }	
//                }                    
//                    break;
//                
//                default:
//                    break;									 
//            } 
//        }
//            break;
//        default:
//            //Do nothing.
//            __NOP();
//            break;
//    }
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









