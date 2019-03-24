#include "task_communicate.h"

uint8_t COMM_Task(uint8_t prio)
{
		SEGGER_RTT_printf(0, "COMM_Task\r\n");
	
    uint8_t m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {   
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        { 
						case COMM_CMD_PROC:            
            {
//                APP_SubTask_CmdProc();
            }
								break;
            
            case COMM_STATE_PROC:            
            {
//                APP_SubTask_StateProc();
            }
								break;

            case COMM_DECODE: 
            {
                //APP_SuTask_DecodeProc();
            }
								break;
						
            case COMM_PC_DECODE:
            {
                //APP_SuTask_PC_DecodeProc();
            }
								break;
						
            case COMM_PC_RETURN:
            {
                //APP_SuTask_PC_ReturnProc();
            }
								break;
						
            default:
								break;
         }
         if(clear_task(prio,m_SYS_SubTask_prio) == true)
        {
            return true;
        }
    }
    return false;
}


