//系统或包头文件
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_error.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_ppi.h"
#include "app_util_platform.h"
#include "ble_advertising.h"
//用户头文件
#include "bsp_lock_drive.h"

Motor_State_Type motor_state_struct;
Use_Channel_Type use_channel_enum;//这是共用一个通道切换
uint8_t open_lock_dir;


////**********************设置接收震动处理*****驱动马达，震动和报警三个共用两个GPIOTE通道和定时器2*****************	
//void release_drive_channel(void)
//{ 
//    nrf_ppi_channel_disable(DRV_PPI_CHANNEL2);//必须要关了此通道，否则仍保留影响到
//    switch (use_channel_by)
//    {
//        case LOCKMOTOR_PWM_CHANNEL:
//            nrf_drv_gpiote_out_uninit(MOTOR_POS_PIN);//采用刹车型PWM方式
//            nrf_gpio_cfg_output(MOTOR_POS_PIN);
//            nrf_gpio_pin_clear(MOTOR_POS_PIN);  
//            nrf_gpio_pin_clear(MOTOR_NEG_PIN); 
//            break;		 
//        case UNLOCKMOTOR_PWM_CHANNEL:
//            nrf_drv_gpiote_out_uninit(MOTOR_NEG_PIN);//采用刹车型PWM方式
//            nrf_gpio_cfg_output(MOTOR_NEG_PIN);
//            nrf_gpio_pin_clear(MOTOR_NEG_PIN);  
//            nrf_gpio_pin_clear(MOTOR_POS_PIN);
//            break;	
//        case NOTHING_USE_CHANNEL:
//             break;
//        default:
//             break;
//    }
//}

void setup_drive_channel(void)
{ 
    ret_code_t err_code;    
    nrf_drv_gpiote_out_config_t gpiote_config_out = GPIOTE_CONFIG_OUT_TASK_TOGGLE(false);//初始输出低	
    switch (use_channel_enum)
    {	 
        case UNLOCKMOTOR_FULL_CHANNEL:	
            foreward_motor();                    
            break;	
				
        case UNLOCKMOTOR_PWM_CHANNEL:  
            nrf_gpio_pin_clear(INA);
            err_code = nrf_drv_gpiote_out_init(INA, &gpiote_config_out);
            APP_ERROR_CHECK(err_code);	
				    nrf_gpio_pin_clear(INB);	
            break;		
				
        case LOCKMOTOR_FULL_CHANNEL:
            inversion_motor();
            break;	

        case LOCKMOTOR_PWM_CHANNEL:
            nrf_gpio_pin_clear(INB);
            err_code = nrf_drv_gpiote_out_init(INB, &gpiote_config_out);
            APP_ERROR_CHECK(err_code);	
				    nrf_gpio_pin_clear(INA);	
            break;	
				
        case NOTHING_USE_CHANNEL:
            break;
        default:
            break;
    }	
 }


static void turnToFreeChannel(void)
{
//    //释放通道
//    release_drive_channel();	
//    //配置通道
//    use_channel_by = NOTHING_USE_CHANNEL;
//    setup_drive_channel();	   
//    setup_drv_timer(800000);
}


////锁完成后或上电时检测锁动作态
//void manual_testworkst(void)
//{ 
//    uint16_t num;
//    Tim_100ms_Struct.m_lockkey100ms   = 0;
//    Tim_100ms_Struct.m_unlockkey100ms = 0;	

//    for(num=0; num<500; num++)
//    { 
//        if(app_mevent.b.work_in_lockst)
//        {	
//            if(nrf_gpio_pin_read(LOCKKEY_PIN))    //解锁态
//            {               
//                num = 0;
//                app_mevent.b.work_in_lockst = 0;
//            } 
//        }
//        else
//        {
//            if(!nrf_gpio_pin_read(LOCKKEY_PIN))   //锁态 
//            {
//                num = 0;
//                app_mevent.b.work_in_lockst = 1;					
//                m_clkx100ms = 0;
//            }
//        }
//    }
//    if(app_mevent.b.work_in_lockst)
//    {
//    //        transientDetectionSet();
//        NRF_LOG_DIRECT("lock close!\r\n");
//    }
//    else
//    {
//    //        transientDetectionSet_unlock();
//        NRF_LOG_DIRECT("lock open!\r\n");
//    }
//}

//全速开锁
void bsp_open_motor_full_setup(void)
{    
		
    //配置通道
    use_channel_enum = LOCKMOTOR_FULL_CHANNEL;	
    setup_drive_channel();	
    //状态初始化	
    task_flag_struct.stop_motor_st = 0;
		start_motor_time();
}

//全速关锁
void bsp_close_motor_full_setup(void)
{   
    //配置通道
    use_channel_enum = UNLOCKMOTOR_FULL_CHANNEL;	
    setup_drive_channel();	
    //状态初始化	
    task_flag_struct.stop_motor_st = 0;
		start_motor_time();
	
//    release_drive_channel();	
//    //设置定时
//    nrf_drv_timer_pause(&TIMER_DRV);
//    nrf_drv_timer_clear(&TIMER_DRV); 
//    setup_drv_timer(64);
//    testKeyx64us    = 0;
//    driveMotorx64us = 0; 	

//    //配置通道	
//    use_channel_by = UNLOCKMOTOR_FULL_CHANNEL;	
//    setup_drive_channel();
////    nrf_gpio_pin_set(MOTOR_POS_PIN);
////    nrf_gpio_pin_clear(MOTOR_NEG_PIN);	
//    //状态初始化
//    app_mevent.b.stop_motor_st = 0;	
//    nrf_drv_timer_resume(&TIMER_DRV);
}

////解锁PWM设置
//void bsp_unlockmotor_pwm_setup(void)
//{    
//    //释放通道
//    release_drive_channel();	
//    //设周期	   
//    nrf_drv_timer_pause(&TIMER_DRV);
//    nrf_drv_timer_clear(&TIMER_DRV);
//    setup_drv_timer(64);
//    testKeyx64us    = 0;
//    driveMotorx64us = 0;
//    //配置通道
//    use_channel_by=UNLOCKMOTOR_PWM_CHANNEL;
//    setup_drive_channel();

//    //设脉宽
//    uint32_t time_ticks;
//    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_DRV,UNLOCK_MOTOR_PWM_WITDTH_LOW);//54);    //计算定时器的比较值
//    nrf_drv_timer_compare(&TIMER_DRV, NRF_TIMER_CC_CHANNEL1, time_ticks, false);//设置比较器    

//    //配置PPI	
//    uint32_t compare_evt_addr; //EEP
//    uint32_t gpiote_task_addr; //TEP  	
//    //PPI1
//    compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_DRV, NRF_TIMER_EVENT_COMPARE0);
//    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(MOTOR_NEG_PIN);	 
//    nrf_ppi_channel_endpoint_setup(DRV_PPI_CHANNEL1, compare_evt_addr,gpiote_task_addr);//capture_task_addr); 
//    //PPI2
//    compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_DRV, NRF_TIMER_EVENT_COMPARE1);
//    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(MOTOR_NEG_PIN);	
//    nrf_ppi_channel_endpoint_setup(DRV_PPI_CHANNEL2, compare_evt_addr,gpiote_task_addr);//capture_task_addr); 		
//    //开启通道  
//    nrf_ppi_channel_enable(DRV_PPI_CHANNEL1);
//    nrf_ppi_channel_enable(DRV_PPI_CHANNEL2); 	
//    nrf_gpio_pin_set(MOTOR_POS_PIN);//	
//    //nrf_gpio_pin_set(MOTOR_NEG_PIN);//仍保留前一工作状态
//    nrf_drv_timer_clear(&TIMER_DRV);
//    nrf_drv_gpiote_out_task_enable(MOTOR_NEG_PIN);//GPIO交给GPIOTE控制   
//    //设状态
//    app_mevent.b.stop_motor_st = 0;	
//    nrf_drv_timer_resume(&TIMER_DRV);	
//}


////上锁PWM设置
//void bsp_lockmotor_pwm_setup(void)
//{    
//    //释放通道
//    release_drive_channel();	
//    //设周期	
//    nrf_drv_timer_pause(&TIMER_DRV);
//    nrf_drv_timer_clear(&TIMER_DRV);
//    setup_drv_timer(64);	
//    testKeyx64us    = 0;
//    driveMotorx64us = 0;
//    //配置通道
//    use_channel_by=LOCKMOTOR_PWM_CHANNEL;
//    setup_drive_channel();	 
//    //设脉宽
//    uint32_t time_ticks;
//    time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_DRV, LOCK_MOTOR_PWM_WITDTH_LOW);//54);    //计算定时器的比较值
//    nrf_drv_timer_compare(&TIMER_DRV, NRF_TIMER_CC_CHANNEL1, time_ticks, false);//设置比较器    	  

//    //配置PPI	 
//    uint32_t compare_evt_addr; //EEP
//    uint32_t gpiote_task_addr; //TEP  	
//    //PPI1
//    compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_DRV, NRF_TIMER_EVENT_COMPARE0);
//    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(MOTOR_POS_PIN);	 
//    nrf_ppi_channel_endpoint_setup(DRV_PPI_CHANNEL1, compare_evt_addr,gpiote_task_addr);//capture_task_addr); 
//    //PPI2
//    compare_evt_addr = nrf_drv_timer_event_address_get(&TIMER_DRV, NRF_TIMER_EVENT_COMPARE1);
//    gpiote_task_addr = nrf_drv_gpiote_out_task_addr_get(MOTOR_POS_PIN);	
//    nrf_ppi_channel_endpoint_setup(DRV_PPI_CHANNEL2, compare_evt_addr,gpiote_task_addr);//capture_task_addr); 		
//    //开启通道
//    nrf_ppi_channel_enable(DRV_PPI_CHANNEL1);
//    nrf_ppi_channel_enable(DRV_PPI_CHANNEL2); 	
//    nrf_gpio_pin_set(MOTOR_NEG_PIN);//	
//    //nrf_gpio_pin_set(MOTOR_POS_PIN);//仍保留前一工作状态
//    nrf_drv_timer_clear(&TIMER_DRV);
//    nrf_drv_gpiote_out_task_enable(MOTOR_POS_PIN);//GPIO交给GPIOTE控制   
//    //设状态    
//    app_mevent.b.stop_motor_st = 0;	
//    nrf_drv_timer_resume(&TIMER_DRV);	
//}


///////////////////////////////////////////////////
////退出上锁时PWM驱动
// void stop_lockmotor_pwm(void)
//{
//    nrf_drv_gpiote_out_task_disable(MOTOR_POS_PIN);
//    nrf_gpio_pin_set(MOTOR_POS_PIN);//刹车型PWM,上锁要先清正极
//    nrf_gpio_pin_set(MOTOR_NEG_PIN);
//    app_mevent.b.stop_motor_st = 1;
//}	

///////////////////////////////////////////////////
////退出解锁时PWM驱动
//void stop_unlockmotor_pwm(void)
//{
//    nrf_drv_gpiote_out_task_disable(MOTOR_NEG_PIN);
//    nrf_gpio_pin_set(MOTOR_NEG_PIN);//刹车型PWM,解锁要先清负极
//    nrf_gpio_pin_set(MOTOR_POS_PIN);//
//    app_mevent.b.stop_motor_st = 1;
//}	

//bool testDriveLockKey(void)
//{
//    uint8_t i,j;
//    i=0;
//		j=0;
//	
//    while(1)
//    {
//        if(nrf_gpio_pin_read(LOCKKEY_PIN))
//        {
//            if(++i>30)
//                return true;
//        }
//        else
//        {
//            if(++j>60)
//                return false;
//        }
//    }
//}

//bool testDriveOpenKey(void)
//{
//		uint8_t i,j;
//		i = 0;
//		j = 0;
//		while(1)
//		{
//				if(nrf_gpio_pin_read(OPENKEY_PIN))
//				{
//						if(++i>30)
//						{
//							return true;
//						}
//				}
//				else
//				{
//						if(++j>60)
//						{
//							return false;
//						}
//				}
//		}
//}

//void drive_halfCloseLock(void)	
//{
//    uint8_t drtimes;
//    //开启定时器
//    app_mevent.b.exit_sleepst = true;
//    exit_sleep_proc();
//    //----------全速上锁-------------------------	
//    for(drtimes=0; drtimes<5; drtimes++)	
//    { 
//        if(!testDriveOpenKey()) 
//        {	
//            bsp_lockmotor_full_setup();  
//            while(true)
//            {
//                if(app_mevent.b.stop_motor_st)
//                {	  
//                    app_mevent.b.stop_motor_st=0;
//                    //turnToFreeChannel();	
//                    break;		 
//                }
//            }	 
//            turnToFreeChannel();	
//        }	 
//        //----------转PWM上锁-------------------------
//        bsp_lockmotor_pwm_setup();  
//        while(true)
//        {
//            if(app_mevent.b.stop_motor_st)
//            {	  
//                app_mevent.b.stop_motor_st=0;
//                //turnToFreeChannel();	
//                break;		 
//            }
//        }
//        turnToFreeChannel();
//				
//        nrf_gpio_cfg_output(MOTOR_POS_PIN); 
//        nrf_gpio_pin_set(MOTOR_POS_PIN); 
//        nrf_gpio_cfg_output(MOTOR_NEG_PIN);	
//        nrf_gpio_pin_set(MOTOR_NEG_PIN);	
//				
//        if(testDriveOpenKey())
//				{
//						break;
//				}
//        sys_ndelay(5);	 	
//    }

//    sys_ndelay(3);	 	
//    nrf_gpio_pin_clear(MOTOR_POS_PIN);	
//    nrf_gpio_pin_clear(MOTOR_NEG_PIN);
//}

//开锁
void drive_open(void)
{	
    //---------开启定时器------------------------
		init_motor_timer();
	
    //-----------全速开锁------------------------
		bsp_open_motor_full_setup();
		while(true)
		{
				if(task_flag_struct.stop_motor_st)
				{
						task_flag_struct.stop_motor_st = 0;
						//turnToFreeChannel();	
						break;		 
				}
		}
		turnToFreeChannel();	   
		//----------开锁等待--------------------------	
		stop_motor();
		#if SEGGER_RTT_DEBUG_MOTOR	
				SEGGER_RTT_printf(0, "lock open!\r\n");
		#endif
    sys_ndelay(150);
    //----------全速上锁-------------------------	
		bsp_close_motor_full_setup();
		while(true)
		{
				if(task_flag_struct.stop_motor_st)
				{	  
						task_flag_struct.stop_motor_st = 0;
						//turnToFreeChannel();	
						break;		 
				}
		}
		turnToFreeChannel();	
		stop_motor_time();
    sys_ndelay(30);
}

//bool open_motor_task(void)
//{ 	
//    NRF_LOG_DIRECT(" lock open start \r\n");
//    drive_openlock();
//    NRF_LOG_DIRECT(" lock open end \r\n");
//    manual_testworkst();
//    if(app_mevent.b.work_in_lockst)		 
//    {   
//        sys_ndelay(8);
//        m_clkx100ms = 0;
//        return false;
//    }
//    else 
//    {  
//        sys_ndelay(2);
//        return true;		 
//    } 
//}	

void drive_half_open(void)	
{
    uint8_t drive_times;
    //---------开启定时器------------------------
		init_motor_timer();
	
    //-----------全速开锁------------------------
    for(drive_times=0; drive_times<5; drive_times++)
    { 
        bsp_open_motor_full_setup();
//        while(true)
//        {
//            if(app_mevent.b.stop_motor_st)
//            {	  
//                app_mevent.b.stop_motor_st=0;
//                //turnToFreeChannel();	
//                break;		 
//            }
//        }
//        turnToFreeChannel();	   
//        //------------转PWM开锁-----------------------
//        bsp_unlockmotor_pwm_setup(); 
//        while(true)
//        {
//            if(app_mevent.b.stop_motor_st)
//            {
//                //stop_motor_pwm();
//                app_mevent.b.stop_motor_st = 0;
//                //turnToFreeChannel();	
//                break;		 
//            }
//        }   
//        turnToFreeChannel();	
//        //----------开锁等待--------------------------	
//        nrf_gpio_pin_set(MOTOR_POS_PIN);//全速	刹车
//        nrf_gpio_pin_set(MOTOR_NEG_PIN);//极性
//        if(testDriveLockKey())
//				{
//            break;	
//				}
//        sys_ndelay(5);//
    }
//    NRF_LOG_DIRECT(" lock open mid \r\n");
//    sys_ndelay(6);//
//    nrf_gpio_pin_clear(MOTOR_POS_PIN);//全速	刹车
//    nrf_gpio_pin_clear(MOTOR_NEG_PIN);//极性		
//    sys_ndelay(3);//
}

//bool open_fencing_task(void)
//{ 	
//    NRF_LOG_DIRECT(" lock half open start\r\n");
//    drive_halfOpenLock();
//    NRF_LOG_DIRECT(" lock half open end\r\n");
//	
//    return (!testDriveOpenKey());
//}
