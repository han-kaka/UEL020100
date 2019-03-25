#include "bsp_common.h"

Task_Flag_Type task_flag_struct;

//初始化硬件外设
static void bsp_peri_init(void)
{
    init_board();
	
    init_peripheral();
	
//    Bsp_Rcc_Init();
//    Bsp_GPIO_Init();
//    Bsp_Tim2_Init();
//    Bsp_Tim3_Delay_Init();    

//    Bsp_UART1_Init(115200);
//    Bsp_UART2_Init(115200);
//    IWDG_Init();
}


static void data_recover(void)
{
    solid_data_init();  
//				gSystemRunParam.flagInit = 0;
//				memcpy(useraeskeybuf, tempkey, 16);
//				set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);	
    dynamic_data_init();
}

//应用层初始化
uint8_t com_bsp_init(void)
{
    bsp_peri_init();            // peripheral init
		
    bsp_var_init();             //ver init
    data_recover();             //data recover
	
		Tim_1s_Struct.sys_10s_count = 0;
		set_task(MEASURE, MEASURE_VOLTAGE);
    app_comm_init();            //COMM state and event init

    return true;
}









