#include "app_common.h"
#include <stdlib.h>

Sys_Flag_Type                           Sys_Flag_Struct = {.reserved = 0,}; 

																													 

//ÖÃÆğÈÎÎñ
void set_task(uint8_t main_task,uint8_t sub_task)
{
		uint16_t mm,ss;
		mm = 1<<main_task;
		ss = 1<<sub_task;
		g_Maintask |= mm;
		ga_Subtask[main_task] |= ss;
}

void Clr_Task(uint8_t main_task,uint8_t sub_task)
{
		uint16_t ss;
		if(sub_task == TASK_ALL)
		{
				ga_Subtask[main_task] = 0;
		}
		else
		{        
				ss = 1<<sub_task;
				ga_Subtask[main_task] &= ~ss;
		}
}

uint8_t clear_task(uint8_t prio, uint8_t m_SYS_SubTask_prio)
{
		ga_Subtask[prio] &=~ (1<<m_SYS_SubTask_prio);
		if(ga_Subtask[prio] == 0)
		{
				g_Maintask &=~(1<<prio);
				return true;
		}
		return false; 
}

//u8 Sum_Check(u8 *data,u8 length)
//{
//  u8 temp = 0;
//  for(u8 i= 0; i < length-1; ++i)
//  {
//    temp = temp + *(data + i);
//  }
//  
//  if( *(data+length-1) == temp )
//  {
//    return TRUE;
//  }
//  else
//  {
//    return FALSE;
//  }
//}

//u8 Sum(u8 *data,u8 length)
//{
//  u8 result=0;
//  for(u8 i= 0; i < length; ++i)
//  {
//    result = result + *(data + i);
//  }
//  return result;
//}
//  

//u8 BCD_2_Data8(u8 data)
//{
//  u8 result;
//  u8 high,low;

//  high = data>>4;
//  low = (data&0x0f);
//  result = high*10 + low;
//  return result;
//}


//u8 Data8_2_BCD(u8 data)
//{

//  u8 return_data = 0;
//  while(data >= 100)                                //?????99 ?????
//  {
//      data = data - 100;
//  }
//  while(data >= 10)
//  {
//      data = data - 10;
//      return_data = return_data + 16;
//  }  
//  return_data = return_data + data;

//  return return_data;
//}


//u16 Data16_2_BCD(u16 data)
//{

//  u16 return_data = 0;
//  while(data >= 10000)                                //?????99 ?????
//  {
//      data = data - 10000;
//  }
//  while(data >= 1000)
//  {
//      data = data - 1000;
//      return_data = return_data + 16*16*16;
//  }  
//  while(data >= 100)
//  {
//      data = data - 100;
//      return_data = return_data + 16*16;
//  }  
//  while(data >= 10)
//  {
//      data = data - 10;
//      return_data = return_data + 16;
//  }  
//  return_data = return_data + data;

//  return return_data;
//}

bool  Check_Strstr(char *data1, char *data2, uint16_t len)
{
    uint16_t i=0;
    char *p;
    p=data2;
	
    do
    {
        if(*data1==*p)
        { 
            p++;
            if(*p=='\0')
            {
                return true;
            }
        }
        else
        { 
            p=data2;
        }
		
        data1++;
        if(*data1=='\0')
        {
            return false;
        }
        i++;
    }while(i<len);
    
    return false;
}

//????0
uint8_t CntLeadZeros(uint8_t ucData)
{
		uint8_t ucIndex;
		ucIndex = 0;
		
		while(ucIndex < 8)
		{
				if(ucData & (0x01 << ucIndex))
				{
						return ucIndex;
				}
				ucIndex++;
		}
		return (uint8_t)0xFF;
}

//u8 Get_Symbol(u8 num_symbol,u8 *buf, char symbol)
//{
//	 u8 i=0;
//	 u8 j=0;
//	 u8 len=0;
//	 char str = symbol;
//	 u8 location=0;
//	 u8 *p_buf=buf;
//	 len=strlen((char *)p_buf);
//	 for(i=0;i<num_symbol;i++)
//	 {
//		 
//		  while(p_buf[j]!=str)
//			{
//				 j++;
//				 if(j>=len)
//				 {
//					  return FALSE;
//				 }
//			}
//			j+=1;
//			location=j;
//	 }
//	 return location;
//}

uint8_t Get_Symbol(uint8_t num_symbol, uint8_t *buf, char symbol)
{
	 uint8_t i=0;
	 uint8_t j=0;
	 uint8_t len=0;
	 char str = symbol;
	 uint8_t location=0;
	 uint8_t *p_buf=buf;
	 len=strlen((char *)p_buf);
	 for(i=0;i<num_symbol;i++)
	 {
		  while(p_buf[j]!=str)
			{
				 j++;
				 if(j>=len)
				 {
					  return false;
				 }
			}
			j+=1;
			location=j;
	 }
	 return location;
}

uint8_t Get_Comma(uint8_t num_comma, uint8_t *buf)
{
	 uint8_t i=0;
	 uint8_t j=0;
	 uint8_t len=0;
	 char str=',';
	 uint8_t location=0;
	 uint8_t *p_buf=buf;
	 len=strlen((char *)p_buf);
	 for(i=0;i<num_comma;i++)
	 {
		 
		  while(p_buf[j]!=str)
			{
				 j++;
				 if(j>=len)
				 {
					  return false; 
				 }
			}
			j+=1;
			location=j;
	 }
	 return location;
}

//u8* DecimalToASCII(u32 port,u8*data)
//{
//    u8 num,i;
//    
//    num = port/1000000000+0x30;
//    data[0] = num;
//    num = port%1000000000/100000000+0x30;
//    data[1] = num;
//    num = port%100000000/10000000+0x30; 
//    data[2] = num;
//    num = port%10000000/10000000+0x30;
//    data[3] = num;
//    num = port%1000000/1000000+0x30;
//    data[4] = num;
//    num = port%100000/10000+0x30;
//    data[5] = num;
//    num = port%10000/1000+0x30;
//    data[6] = num;
//    num = port%1000/100+0x30;
//    data[7] = num;
//    num = port%100/10+0x30;
//    data[8] = num;
//    num = port%10+0x30;
//    data[9] = num;
//    data[10] = 0;
//    
//    for(i=0;i<10;i++)
//    {
//        if(data[i] != 0x30) return (&data[i]);
//        
//    }
//    return NULL;
//}

uint8_t Get_Uint_Data(const char *buf)
{
		uint8_t i=0;
		uint8_t len=0;
		uint8_t int_data=0;
		uint8_t byte=0;
		int8_t str=',';

		char data[20];
		len=strlen((char *)buf);
		while(buf[i]!=str)
		{
				data[i]=buf[i];
				i++;
				if(i>=len)
				{
						data[i]='\0';
						int_data=atoi(data); 
						byte=(uint8_t)int_data;
						return byte;
				}
		}
		data[i]='\0';
		int_data=atoi(data);
		byte=(uint8_t)int_data;

		return byte;
}

uint8_t get_xor(uint8_t *hex_data_buf, uint16_t len_hex)
{
    uint8_t result = hex_data_buf[0];
    for(uint8_t i=1; i<len_hex; i++)
    {
        result ^= hex_data_buf[i];
    }
    return result;
}

bool xor_check(uint8_t *hex_data_buf, uint16_t len_hex)
{
    uint8_t i;
    uint8_t temp = hex_data_buf[0];
    for(i=1; i<(len_hex-1); i++)
    {
        temp ^= hex_data_buf[i];
    }
    if(temp == hex_data_buf[i])
    {
//				#if SEGGER_RTT_DEBUG_FLASH
//					SEGGER_RTT_printf(0, "xor true!\r\n");
//				#endif
        return true;
    }
    else
    {
//				#if SEGGER_RTT_DEBUG_FLASH
//					SEGGER_RTT_printf(0, "xor false!\r\n");
//				#endif
        return false;
    }
}

//U8 Get_Uint_Data(const S8 *buf)
//{
//	U8 i=0;
//	U8 len=0;
//	U8 int_data=0;
//	U8 byte=0;
//	S8 str=',';

//	S8 data[20];
//	len=strlen((S8 *)buf);
//	while(buf[i]!=str)
//	{
//		data[i]=buf[i];
//		i++;
//		if(i>=len)
//		{
//			data[i]='\0';
//			int_data=atoi(data);
//			byte=(U8)int_data;
//			return byte;
//		}
//	}
//	data[i]='\0';
//	int_data=atoi(data);
//	byte=(U8)int_data;
//	return byte;
//}


static void asc_To_hex(char *ascs, uint8_t *hex)
{
		uint8_t h,l;
		h=l=0;
	
		if((ascs[0]>=0x30)&&(ascs[0]<=0x39)){
			h=ascs[0]-0x30;
		}
		else if((ascs[0]>=0x41)&&(ascs[0]<=0x46)){
			h=ascs[0]-0x37;//h=ascs_h+10-0x41;
		}
		else if((ascs[0]>=0x61)&&(ascs[0]<=0x66)){
			h=ascs[0]-0x57;//h=ascs_h+10-0x61;
		}

		if((ascs[1]>=0x30)&&(ascs[1]<=0x39)){
			l=ascs[1]-0x30;
		}
		else if((ascs[1]>=0x41)&&(ascs[1]<=0x46)){
			l=ascs[1]-0x37;//l=ascs_l+10-0x41;
		}
		else if((ascs[1]>=0x61)&&(ascs[1]<=0x66)){
			l=ascs[1]-0x57;//l=ascs_l+10-0x61;
		}
		
		*hex = (h<<4)|l;
		return;
} 

uint16_t Ascii_To_Hex(char *ascs, uint8_t *hexs, uint16_t length)
{
		uint16_t i;
		uint16_t j;
		
		for(i=0,j=0;i<length;i+=2)
		{
				asc_To_hex(ascs+i,hexs+j);
				j++;
		}
		
		return j;
}

