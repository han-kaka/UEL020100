#include "task_measure.h"

uint8_t Measure_Task(uint8_t prio)
{
//		SEGGER_RTT_printf(0, "Measure_Task\r\n");
	
    uint8_t m_SYS_SubTask_prio = 0;
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
						case MEASURE_VOLTAGE:
						{
						#if SEGGER_RTT_DEBUG_ADC
								SEGGER_RTT_printf(0, "MEASURE_VOLTAGE\r\n");
						#endif
								measure_voltage();
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
