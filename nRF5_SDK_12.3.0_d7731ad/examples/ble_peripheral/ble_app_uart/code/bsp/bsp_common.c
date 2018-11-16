#include "bsp_common.h"

Task_Flag_Type Task_Flag_Struct;

static void Data_Recover(void)
{
    Solid_Romdata_Init();       
    Dynamic_Data_Init(); 
}

uint8_t Com_Bsp_Init(void)
{
//    Bsp_Peripheral_Init();      // peripheral init
    Bsp_Var_Init();             //ver init
    Data_Recover();             //data recover
    APP_COMM_Init();            //COMM state and event init
	
    return true;
}









