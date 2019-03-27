#include "app_comm_NB86.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

static uint16_t NB_msCount = 0;

uint8_t                                 g_Rx_Buffer[BUFFSIZE] = {0};
uint8_t                                 g_Tx_Buffer[BUFFSIZE] = {0};

NB_Handler        g_stNB_Handler;
PACKET_HEAD_Type  PacketHead;

typedef void(*NB_State_Fun)(uint8_t *RxBuf); /* 定义状态处理函数*/
//typedef void(*GPRS_State_Fun)(void);
//static void APP_NB_RefreshRetryMess(void);
static uint8_t APP_NB_EvtTraverse(uint8_t mode);


static void APP_NB_State_Init_Proc(uint8_t *RxBuf);
static void APP_NB_State_Connect_Proc(uint8_t *RxBuf);
static void APP_NB_State_Mess_Proc(uint8_t *RxBuf);
static void APP_NB_State_Comm_Proc(uint8_t *RxBuf);

static const NB_State_Fun APP_NB_StateProcTbl[] = 
{
    NULL,     //POWER_ON
    NULL,     //RESET
    APP_NB_State_Init_Proc,
		APP_NB_State_Connect_Proc,
		APP_NB_State_Mess_Proc,
		APP_NB_State_Comm_Proc,
		NULL,
    NULL,
    NULL,
    NULL,
};


/******************GPRS指令集**********************************/
const NB_Cmd_Data_Type AT_CmdTbl[] = 
{
    /*------cmdstr-----------------------------------revtimeout(s)--cmdtruebackstr----------retry_times */
    /************************* GPRS  network ***************************/
      {"AT+CFUN=0\r\n",                                  3,       "OK",                     3},
			{"AT+CFUN=1\r\n",                                  3,       "OK",                     3},
      {"AT+CFUN?\r\n",                                   3,       "CFUN:0",                 3},
			{"AT+CFUN?\r\n",                                   3,       "CFUN:1",                 3},
			{"AT+NCDP=180.101.147.115,5683\r\n",               3,       "OK",                     3},
			{"AT+NRB\r\n",                                     3,       "REBOOTING",              3},
			
			{"AT+CGSN=1\r\n",                                  3,       "CGSN",                   3},
			{"AT+NCSEARFCN\r\n",                               3,       "OK",                     3},
			{"AT+NCONFIG=CELL_RESELECTION,TRUE\r\n",           3,       "OK",                     3},
			{"AT+NCONFIG=CELL_RESELECTION,FALSE\r\n",          3,       "OK",                     3},
			{"AT+CMEE=1\r\n",                                  3,       "OK",                     3},
			{"AT+NCCID\r\n",                                   3,       "CCID:",                  3},
			
			{"AT+CGDCONT=1,\"IP\",\"ctnet\"\r\n",              3,       "OK",                     3},
			{"AT+CGATT=1\r\n",                                 3,       "OK",                     3},
			{"AT+CGATT=0\r\n",                                 3,       "OK",                     3},
			{"AT+CGATT?\r\n",                                  5,       "CGATT:1",               20},	
			{"AT+CGATT?\r\n",                                  5,       "CGATT:0",               20},	
			{"AT+CPSMS=1,,,01000011,01000011\r\n",             3,       "OK",                     3},
			{"AT+CEDRXS=0,5,0101\r\n",                         3,       "OK",                     3},
			{"AT+NPSMR=1\r\n",                                 3,       "OK",                     3},
			{"AT+CGPADDR\r\n",                                 5,       "OK",                    20},	
      
			{"AT+CSQ\r\n",   			                             3,	      "+CSQ",         	        3},
      {"AT+NMGS=37,00\r\n",   			                     3,	      "OK",         	          3},
      {"AT+NQMGS\r\n",   			                           3,	      "OK",         	          3},
			
      {"AT+NMGR\r\n",   			                           3,	      "OK",         	          3},			
			{"AT+NQMGR\r\n",   			                           3,	      "OK",         	          3},	

			{"AT+NPSMR?\r\n",   			                         3,	      "NPSMR:1,1",         	    3},				
			
};

/*************************初始化流程***************************/
static const uint8_t s_ATCmdStep_Init[] = 
{
    NULL,
    NB_AT_CFUN_0,      
		NB_AT_CFUN0,      
		NB_AT_NCDP, 
		NB_AT_NRB,	
};

/************************* TCP/IP数据链路建立连接***************************/
static const uint8_t s_ATCmdStep_Connnect[] = 
{
		NB_AT_CGDCONT,
		NB_AT_CSQ,
		NB_AT_CGATT_1,
		NB_AT_CGATT1,	
	  NB_AT_CFUN_0, 
		NB_AT_CFUN0,  
		NB_AT_CPSMS_1,
		NB_AT_CEDRXS_0,
		NB_AT_NPSMR_1,
	  NB_AT_CFUN_1, 
		NB_AT_CFUN1,  
		NB_AT_CGATT_1,
		NB_AT_CGATT1,
		NB_AT_CGPADDR,
};

/*************************数据发送流程***************************/
static const uint8_t s_ATCmdStep_Comm[] = 
{
		NB_AT_CSQ,
    NB_AT_NMGS,
		NB_AT_NQMGS,   	
};

static const uint8_t s_ATCmdStep_Mess[] = 
{
    NULL,
	  NB_AT_CFUN_0, 
		NB_AT_CFUN0,  
		NB_AT_CGSN_1,
		NB_AT_NCSEARFCN,
		NB_AT_NCONFIG_TRUE,
		NB_AT_CMEE_1,	
		NB_AT_CFUN_1,
		NB_AT_CFUN1,
		NB_AT_NCCID,
};

static const uint8_t s_ATCmdStep_Inquire[] = 
{
		NB_AT_NPSMR1,
};

/*校验AT指令应答结果是否正确*/
static uint8_t APP_GPRS_ATCmdAckCheck(uint8_t *RxBuf)
{   

//    //判断是否是服务器下行数据
//    if(Check_Strstr((char*)RxBuf, (char*)(char*)AT_CmdTbl[NB_AT_MIPRTCP].ExpResultStr, NB_ATCmdCB.ucRxLen) == true)
//    {
//        NB_ATCmdCB.RspType = RSP_TYPE_DATA;  //通信数据
//        return BACK_TRUE;   //接收到正确应答
//    }
    if(Check_Strstr((char*)RxBuf, "Lierda", NB_ATCmdCB.ucRxLen) == true) //GPRS模块准备就绪推送
    {
        task_flag_struct.atReday_F = ENABLE;
        NB_ATCmdCB.RspType = RSP_TYPE_CMD;
		#if SEGGER_RTT_DEBUG_CMDPROC
				SEGGER_RTT_printf(0, "gprs model ready\r\n");
		#endif
				return BACK_TRUE;
    }
		#if SEGGER_RTT_DEBUG_CMDPROC
//			SEGGER_RTT_printf(0, (char*)RxBuf);
//			SEGGER_RTT_printf(0, NB_ATCmdCB.ExpResultStr);
//			SEGGER_RTT_printf(0, "\r\n");
		#endif
//    if(NB_ATCmdCB.ATCmdPt != NB_AT_MIPOPEN_1)//socket连接时,也有可能回复MIPSTAT: 1,1
//    {
//        if(Check_Strstr((char*)RxBuf, "MIPSTAT: 1,1", NB_ATCmdCB.ucRxLen) == true)
//        {
//            NB_NetPar.NetConnSta = CONN_OFF;
//            g_stNB_Handler.State = NB_STATE_CONNECT_NET;    //重新连接socket
//						g_stNB_Handler.StepPt = 2;
//						g_stNB_Handler.AuthStatus = NOT_AUTH;
//						NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
//        }
//    }
//    if(NB_ATCmdCB.ATCmdPt != NB_AT_MIPCALL1) //由于IP获取错误，也会去查询有无IP
//    {
//        if(Check_Strstr((char*)RxBuf, "MIPCALL: 0", NB_ATCmdCB.ucRxLen) == true)
//        
//        { 
//            NB_NetPar.NetConnSta = CONN_OFF;
//						g_stNB_Handler.State = NB_STATE_CONNECT_NET;    //重新连接网络
//						g_stNB_Handler.StepPt = 0;
//						g_stNB_Handler.AuthStatus = NOT_AUTH;
//						NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
//        }
//    }
    
     //判断接收的数据是否含有期望应答
		if(Check_Strstr((char*)RxBuf, NB_ATCmdCB.ExpResultStr, NB_ATCmdCB.ucRxLen) == true)
		{
				if((NB_AT_CFUN0 == NB_ATCmdCB.ATCmdPt) ||
					 (NB_AT_CFUN1 == NB_ATCmdCB.ATCmdPt) ||
					 (NB_AT_CGATT1 == NB_ATCmdCB.ATCmdPt) ||
					 (NB_AT_CGATT0 == NB_ATCmdCB.ATCmdPt))
				{
						if(Check_Strstr((char*)RxBuf, "OK", 20) != true)
						{
								#if SEGGER_RTT_DEBUG_CFUN || SEGGER_RTT_DEBUG_CGATT
										SEGGER_RTT_printf(0, "at+cfun?/at+cgatt? error !\r\n");
										SEGGER_RTT_printf(0, (char*)RxBuf);
								#endif	
								return BACK_ERROR;  
						}
						#if SEGGER_RTT_DEBUG_CFUN || SEGGER_RTT_DEBUG_CGATT
								SEGGER_RTT_printf(0, "at+cfun?/at+cgatt? ok !\r\n");
						#endif
				}
				
				if((NB_AT_CGSN_1 == NB_ATCmdCB.ATCmdPt) ||
					 (NB_AT_NCCID == NB_ATCmdCB.ATCmdPt) ||
				   (NB_AT_CSQ == NB_ATCmdCB.ATCmdPt))
				{
						if(Check_Strstr((char*)RxBuf, "OK", 65) != true)
						{
								#if SEGGER_RTT_DEBUG_CGSN || SEGGER_RTT_DEBUG_NCCID
										SEGGER_RTT_printf(0, "at+cgsn=1/at+nccid error !\r\n");
										SEGGER_RTT_printf(0, (char*)RxBuf);
								#endif	
								return BACK_ERROR;  
						}
						#if SEGGER_RTT_DEBUG_CGSN || SEGGER_RTT_DEBUG_NCCID
								SEGGER_RTT_printf(0, "at+cgsn=1/at+nccid ok !\r\n");
						#endif
				}
				if(NB_AT_CGPADDR == NB_ATCmdCB.ATCmdPt)
				{
						if(Check_Strstr((char*)RxBuf, ".", 30) != true)
						{
								return BACK_ERROR;  
						}
				}
				
				NB_ATCmdCB.RspType = RSP_TYPE_CMD;//应答AT指令
				return BACK_TRUE;                 //接收到正确应答
		} 
    else
		{   
//				//由于获取网络侧地址和建立soctet连接都是分两条返回的，第一条都为"OK"
//				if((NB_ATCmdCB.ATCmdPt == NB_AT_MIPCALL_1) || NB_ATCmdCB.ATCmdPt == NB_AT_MIPOPEN_1)
//				{
//						if(Check_Strstr((char*)RxBuf, "OK", 4) == true)   //去掉这一句再试
//						{
//								return NO_BACK;   /*第一条"OK"返回正确，所以继续等待*/
//						}
//				}
			  //查询是否驻网成功，如不成功，那么等待3S，而不是返回错误，该条指令特殊
				if(NB_ATCmdCB.ATCmdPt == NB_AT_CGATT1) 
				{
						return NO_BACK;
				}
				return BACK_ERROR;   		
		}	
}

static uint8_t APP_NB_Transition(uint8_t *hex_data_buf, uint8_t len_hex, uint8_t *new_hex_data_buf, uint8_t *new_len_hex)
{
		
}

//static U8 APP_GPRS_Encrypt(u8 *hex_data_buf,u8 len_hex,u8 *new_hex_data_buf,u8 *new_len_hex)
//{
//  u8 len=0;
//  for(u8 i=0;i<len_hex;i++,len++)
//  {
//    switch(hex_data_buf[i])
//    {
//      case TRANSFER_CHAR1:
//              new_hex_data_buf[len]=TRANSFER_FLAG;
//              len++;
//              new_hex_data_buf[len]=TRANSFER_CHAR1_FLAG;
//              break;
//      case TRANSFER_CHAR2:
//              new_hex_data_buf[len]=TRANSFER_FLAG;
//              len++;
//              new_hex_data_buf[len]=TRANSFER_CHAR2_FLAG;
//              break;
//      default:
//              new_hex_data_buf[len]=hex_data_buf[i];
//              break;
//     }
//  }
//  *new_len_hex=len;
//  if(*new_len_hex<=GPRS_RX_BUFF_LEN){
//    return TRUE;
//  }
//  else{
//    return FALSE;
//  }
//}

//static void APP_GPRS_Data_Trope(u8 *hex_data_buf,u8 len_hex,u8 *new_hex_data_buf)
//{
//  u16 len_hex_cnt=0;
//  u16 len_temp=0;
//				
//  for(len_temp=0;len_hex_cnt<len_hex;len_hex_cnt++,len_temp++)      
//  {
//    if(hex_data_buf[len_hex_cnt]==TRANSFER_FLAG)
//    {
//      switch(hex_data_buf[len_hex_cnt+1])
//      {
//        case TRANSFER_CHAR1_FLAG:
//              new_hex_data_buf[len_temp]=TRANSFER_CHAR1;
//              len_hex_cnt++;
//              break;
//        case TRANSFER_CHAR2_FLAG:
//              new_hex_data_buf[len_temp]=TRANSFER_CHAR2;
//              len_hex_cnt++;
//              break;
//        default:
//        #if DEBUG_LOG
//              BSP_LOG("0x1b error\r\n");
//        #endif
//              break;
//      }
//    }
//    else
//    {
//       new_hex_data_buf[len_temp]=hex_data_buf[len_hex_cnt];
//    }
//  } 
//}
//static void APP_GPRS_GetSysStaMessPacket(U8 status,U8 *num,U8* Packet)
//{
//    U8 i=0;
//    U16 VoltValue=0;
//    i=*num;
//    if(status != CHARGE_DISABLE)
//    {
//           GPRS_SysStaMess.chargeTime = g_stCharge_Handler.ChargeTimCnt;    //充电时长
//           VoltValue = USB_Voltage_Value();
//           GPRS_SysStaMess.chargeVolt = Data16_2_BCD(VoltValue);
//           GPRS_SysStaMess.chargeElec = Data16_2_BCD(VoltValue*10);  
//    }
//    else
//    {
//           GPRS_SysStaMess.chargeTime = 0;
//           GPRS_SysStaMess.chargeVolt = 0;
//           GPRS_SysStaMess.chargeElec = 0;
//    }
//    GPRS_SysStaMess.refVolt   = Data16_2_BCD((u16)ADC_Vref_Calculate());
//    Packet[i++] = (GPRS_CommPacket.MessRead.DeviceState.state>>8) & 0xff;
//    Packet[i++] = (GPRS_CommPacket.MessRead.DeviceState.state) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.chargeTime>>8) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.chargeTime) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.chargeElec>>8) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.chargeElec) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.chargeVolt>>8) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.chargeVolt) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.refVolt>>8) & 0xff;
//    Packet[i++] = (GPRS_SysStaMess.refVolt) & 0xff;
//    *num = i;
//}

static uint16_t APP_NB_GetPacket(uint16_t messID, uint8_t* Packet)
{
    uint8_t i=0,j=0;
//    U8 check=0;
//    U8 ipLen=0;
//    U8 apnLen=0;
//    U8 fillZeroLen=0;
    uint8_t newLen = 0;
    uint8_t TxBuf[NB_RX_BUFF_LEN]={0};
		
    switch(messID)
    {
				case INIT_ID:
				{
						TxBuf[i++] = 0x00;
						TxBuf[i++] = 0x10;
						TxBuf[i++] = NB_CommPacket.Init_Data.CSQ;
						TxBuf[i++] = 0x00;
						TxBuf[i++] = 0x00;
						TxBuf[i++] = 0x00;
						TxBuf[i++] = 0x00;
						for(j=0; j<20; j++)
						{
								TxBuf[i++] = NB_CommPacket.Init_Data.iccid[j];
						}
						TxBuf[i++] = (uint8_t)(PROTOCOL_APP_VERSION >> 8);
						TxBuf[i++] = (uint8_t)(PROTOCOL_APP_VERSION);
						TxBuf[i++] = (uint8_t)(PROTOCOL_APP_SCENARIOS);
						TxBuf[i++] = (uint8_t)(PROTOCOL_APP_VENDORCODE);
						for (i = 0; i < 6; i ++)
						{
								TxBuf[i++] = NB_CommPacket.Init_Data.mac[j];
						}
				}
						break;
			
				case LOG_ID:
				{
//						Packet[i++] = 0x00;
//						Packet[i++] = 0x10;
//						Packet[i++] = NB_CommPacket.Init_Data.CSQ;
//						Packet[i++] = 0x00;
//						Packet[i++] = 0x00;
//						Packet[i++] = 0x00;
//						Packet[i++] = 0x00;
//						for(j=0; j<20; j++)
//						{
//								Packet[i++] = NB_CommPacket.Init_Data.iccid[j];
//						}
//						Packet[i++] = (uint8_t)(PROTOCOL_APP_VERSION >> 8);
//						Packet[i++] = (uint8_t)(PROTOCOL_APP_VERSION);
//						Packet[i++] = (uint8_t)(PROTOCOL_APP_SCENARIOS);
//						Packet[i++] = (uint8_t)(PROTOCOL_APP_VENDORCODE);
//						for (i = 0; i < 6; i ++)
//						{
//								Packet[i++] = NB_CommPacket.Init_Data.iccid[j];
//						}
				}
						break;
    }
    APP_NB_Transition(TxBuf, i, Packet, &newLen);
    return   (uint16_t)newLen;
}

static void APP_NB_RetryMessInit(void)
{
   NB_RetryCtrol.rtycnt = 0;
   NB_RetryCtrol.rtycntEn = DISABLE;
   
   NB_RetryCtrol.rtyTimCount =0;
   NB_RetryCtrol.rtyTime = RTY_TIME_INIT;
#if CONFIG_RETRY_COPY
   GPRS_RetryCtrol.rtyLen = 0;
   memset(GPRS_RetryCtrol.rtyBuff,0,GPRS_RTY_MESS_LEN);
#endif
}

//static void APP_GPRS_RefreshRetryMess(void)
//{
//   GPRS_RetryCtrol.rtycnt++;
//   GPRS_RetryCtrol.rtycntEn = ENABLE;
//   GPRS_RetryCtrol.rtyTimCount =0;
//   GPRS_RetryCtrol.rtyTime = RTY_TIME_INIT;   
//}

#if CONFIG_RETRY_COPY
static uint16_t APP_NB_WriteRetryIDPacket(uint8_t* Packet)
{
//重传时也重新组包，防止第一包数据出错，那么后续两包数据也会错误
//后续重传涉及到时间数据，那么需把数据保存，重新组包时再放入
//如重传时不重新组包，需把宏CONFIG_RETRY_COPY置位
    uint16_t usPacketLen=0;
#if SEGGER_RTT_DEBUG_NB86
		BSP_LOG("retry send mess\r\n");
#endif
    memcpy(Packet,GPRS_RetryCtrol.rtyBuff,GPRS_RetryCtrol.rtyLen);    //重传信息copy至发送缓存
    usPacketLen = GPRS_RetryCtrol.rtyLen;                             
    return usPacketLen;    
}

#endif

uint16_t APP_NB_WriteReturnIDPacket(uint8_t DataID, uint8_t* Packet)
{
    uint16_t usPacketLen=0;
	
		switch(DataID)
		{
//				case RETURN_ID_SET_PARMTE:
//				{
//					usPacketLen = APP_GPRS_GetPacket(SET_PARMTE_ID,SET_PARMTE_CTROL_CODE,SET_PARMTE_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send set parte mess\r\n");
//						#endif
//				}break;	
//				case RETURN_ID_QUERY_PARMTE:
//				{
//					usPacketLen = APP_GPRS_GetPacket(QUERY_PARMTE_ID,QUERY_PARMTE_CTROL_CODE,QUERY_PARMTE_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send query parte mess\r\n");
//						#endif
//				}break;
//				case RETURN_ID_READ_MESS:
//				{
//					usPacketLen = APP_GPRS_GetPacket(READ_MESS_ID,READ_MESS_CTROL_CODE,READ_MESS_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send read mess\r\n");
//						#endif
//				}break;
//				case RETURN_ID_CTROL:
//				{
//					usPacketLen = APP_GPRS_GetPacket(CONTROL_ID,CTROL_CODE,CTROL_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send ctrol mess\r\n");
//						#endif
//				}break;
//				case RETURN_ID_WRITE_ADDR:
//				{
//					usPacketLen = APP_GPRS_GetPacket(WRITE_ADDR_ID,WRITE_ADDR_CTROL_CODE,WRITE_ADDR_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send write addr mess\r\n");
//						#endif
//				}break;
//				case RETURN_ID_READ_ADDR:
//				{
//					usPacketLen = APP_GPRS_GetPacket(READ_ADDR_ID,READ_ADDR_CTROL_CODE,READ_ADDR_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send read addr mess\r\n");
//						#endif
//				}break;
//				case RETURN_ID_READ_STA_MESS:
//				{
//					usPacketLen = APP_GPRS_GetPacket(READ_STA_MESS_ID,READ_STA_MESS_CTROL_CODE,READ_STA_MESS_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send read status mess\r\n");
//						#endif
//				}break;
//				case RETURN_ID_SYS_PARMTE_SET:
//				{
//					usPacketLen = APP_GPRS_GetPacket(SYS_PARMTE_SET_ID,SYS_PARMTE_SET_CTROL_CODE,SYS_PARMTE_SET_LEN,Packet); 
//						#if SEGGER_RTT_DEBUG_NB86
//								BSP_LOG("send sys parmte set\r\n");
//						#endif
//				}break;
				default:
						break;
		}
		return usPacketLen;
}

/*根据不同的数据ID,进行组帧，封装协议层的帧内容*/
static uint16_t APP_NB_WriteDataIDPacket(uint8_t DataID, uint8_t* Packet)
{
    uint16_t usPacketLen = 0;
		switch(DataID)
		{   
				case UPLOAD_ID_INIT:
				{
						usPacketLen = APP_NB_GetPacket(INIT_ID, Packet);
//				#if CONFIG_RETRY_COPY            
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //消息长度备份
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //消息备份   
//				#endif
				#if SEGGER_RTT_DEBUG_MESS
						SEGGER_RTT_printf(0, "init mess upload\r\n");
				#endif
				}
						break;
		
				case UPLOAD_ID_LOG:
				{
//						usPacketLen = APP_NB_GetPacket(AUTHENTICA_ID, AUTH_CTROL_CODE, AUTH_LEN, Packet);
//				#if CONFIG_RETRY_COPY            
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //消息长度备份
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //消息备份   
//				#endif
				#if SEGGER_RTT_DEBUG_MESS
						SEGGER_RTT_printf(0, "log mess upload\r\n");
				#endif
				}
						break;
				
//				case UPLOAD_ID_AUTH:
//				{
//						usPacketLen = APP_GPRS_GetPacket(AUTHENTICA_ID,AUTH_CTROL_CODE,AUTH_LEN,Packet);
//				#if CONFIG_RETRY_COPY            
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //消息长度备份
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //消息备份   
//				#endif
//				#if DEBUG_LOG
//						BSP_LOG("auth mess upload\r\n");
//				#endif
//				}
//						break;
//				
//				case UPLOAD_ID_STA_MESS:
//				{
//						usPacketLen = APP_GPRS_GetPacket(STA_MESS_ID,STA_MESS_CTROL_CODE,STA_MESS_LEN,Packet);
//				#if CONFIG_RETRY_COPY
//						GPRS_RetryCtrol.rtyLen = usPacketLen; 
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);
//				#endif
//				#if DEBUG_LOG
//						BSP_LOG("status mess upload\r\n");
//				#endif
//				}
//						break;	
//			
//				case UPLOAD_ID_HEARTBEAT:
//				{
//						usPacketLen = APP_GPRS_GetPacket(HEARTBEAT_ID,HEARTBEAT_CTROL_CODE,HEARTBEAT_LEN,Packet);
//				#if CONFIG_RETRY_COPY            
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //消息长度备份
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //消息备份   
//				#endif
//				#if DEBUG_LOG
//						BSP_LOG("send heartbeat mess\r\n");
//				#endif
//				}
//						break; 

				default:
						break;
		}
		return usPacketLen;
}

//static void APP_GPRS_SetIP(U8 *pFrameDes)
//{
//    U8 i=0;
//    U8 ipLen=0;
//    U8 portLen=0;
//    U8 data[6]={0};
//    U8 *ip = GPRS_NetPar.ServerIp;
//    U8 *port=NULL;
//    ipLen = strlen((char*)ip);
//    port = DecimalToASCII(GPRS_NetPar.ServerPort,data); //端口号转换支持1--65535
//    if(port != NULL)
//    {
//        portLen=strlen((char*)port);
//        pFrameDes[0] = '"';
//        for(i=0;i<ipLen;i++)
//        {
//           pFrameDes[i+1]=ip[i];
//        }
//        pFrameDes[ipLen+1] = '"';
//        pFrameDes[ipLen+2] = ',';
//        for(i=0;i<portLen;i++)
//        {
//           pFrameDes[ipLen+3+i]=port[i];
//        }
//        pFrameDes[ipLen+portLen+3]=',';
//        pFrameDes[ipLen+portLen+4]='0';
//        pFrameDes[ipLen+portLen+5]='\r';
//        pFrameDes[ipLen+portLen+6]='\n';
//        pFrameDes[ipLen+portLen+7]='\0';
//        return ;
//    }
//    //端口号转换出错，打印信息
//    
//}

static uint8_t APP_NB_WriteATPacket(uint8_t ATCmdIndex, uint8_t *pFrameDes)
{  
    uint8_t ucSendLen;
    uint8_t dataNum;
    char *endStr="\x1A\r\n";
		switch(ATCmdIndex)
		{				
//				case NB_AT_NCDP:
//				{
//						
//				}
//						break;
			
//				case NB_AT_CGDCONT:
//				{
//						
//				}
//						break;
			
				default:
				{
						strcpy((char*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
						ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);//默认的指令符长度
					
				}
						break;
		}
		return ucSendLen;    
}


static void APP_NB_SendATCmd(AT_CMD_CB* pNBATCmdCB)
{
		uint8_t ucResendCnt;
		uint8_t ATCmdIndex;
		AT_CMD_CB *pAtCmd = pNBATCmdCB;
		ATCmdIndex = pAtCmd->ATCmdPt;
		if(1 == pAtCmd->Busy)
		{
			return ;//当前忙碌状态
		}
		//根据不同的AT应答结果,设置重发次数
		if((BACK_TRUE == pAtCmd->ATCmdResult) || (BACK_PERTIMES_OUT == pAtCmd->ATCmdResult))
		{
				ucResendCnt = 0;//重发清零
		}
		else
		{
				ucResendCnt = NB_ATCmdCB.ucResendCnt;//备份重发次数
		}

		memset((uint8_t*)pAtCmd, 0, sizeof(NB_ATCmdCB));
		
		pAtCmd->Busy         = 1;          //设置忙绿状态
		pAtCmd->ATCmdPt      = ATCmdIndex; //更新AT索引号
		pAtCmd->ATCmdResult  = NO_BACK;
		pAtCmd->ucResendCnt  = ucResendCnt;  
		pAtCmd->ucResendTimes = AT_CmdTbl[ATCmdIndex].ucResendTimes;
		pAtCmd->ulTimeOut    = TmrSecondsToMilliseconds(AT_CmdTbl[ATCmdIndex].usTimeOut);//刷新超时时间

		////// 填充指令内容长度 //////
		pAtCmd->ucCmdStrLen = APP_NB_WriteATPacket(ATCmdIndex, pAtCmd->CmdStr);
		//填充指令符期望结果	
		strcpy(pAtCmd->ExpResultStr, AT_CmdTbl[ATCmdIndex].ExpResultStr);
		//发送AT指令
		NB_SendATCmdStr(pAtCmd->CmdStr, pAtCmd->ucCmdStrLen);
		//发送次数+1
		pAtCmd->ucResendCnt++;
}

static void APP_NB_State_Init_Proc(uint8_t *RxBuf)
{
		uint8_t ATCmdIndex;
		if(g_stNB_Handler.StepPt >= INIT_STEP_NUM)
		{
				return;	
		}	 
		ATCmdIndex = s_ATCmdStep_Init[g_stNB_Handler.StepPt];
		//////////////////////// AT指令应答正确 ///////////////////////////	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//AT指令应答内容正确
		{
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= INIT_STEP_NUM)
				{
						////// GPRS模块状态切换 ///////////////
						g_stNB_Handler.State = NB_STATE_READ_MESS;//切换到下一个流程
						g_stNB_Handler.StepPt = 0;     //clr step
						NB_NetPar.NetType = NET_NO_NET;//无联网状态状态
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_INIT;//继续当前流程		
				}	
		}
		//////////////////////// AT 指令达到重发次数 //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)//AT指令重发次数到了
		{
				switch(ATCmdIndex)
				{
						default:
						{
								#if SEGGER_RTT_DEBUG_INIT
										SEGGER_RTT_printf(0, "Init Reset\r\n");
								#endif
								g_stNB_Handler.State = NB_STATE_RESET;       //进行复位
								g_stNB_Handler.StepPt = 0;//clr step
								NB_NetPar.NetType = NET_NO_NET;
						}
							break;	
				}
		}
		//////////////////////// AT指令应答错误或超时 /////////////////////////	
		else
		{
				//没有到AT指令重发次数，所以再次发送AT指令
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//直接再次发送AT指令，然后等待应答
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stNB_Handler.StepPt];
				NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
		}	
}

static void APP_NB_State_Connect_Proc(uint8_t *RxBuf)
{
    uint8_t ATCmdIndex;
	
		if(g_stNB_Handler.StepPt >= CONNECT_STEP_NUM)
		{
				return;	
		}
	
		ATCmdIndex = s_ATCmdStep_Connnect[g_stNB_Handler.StepPt];
		//////////////////////// AT指令应答正确 ///////////////////////////	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//AT指令应答内容正确
		{	           
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= CONNECT_STEP_NUM)
				{
						////// NB模块状态切换 ///////////////
						g_stNB_Handler.State = NB_STATE_IDLE;//切换到下一个流程
						g_stNB_Handler.StepPt = 0;//clr step
						NB_NetPar.NetConnSta = CONN_ON;
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;//继续当前流程
				}
		}
		//////////////////////// AT 指令达到重发次数 //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)
		{
				switch(ATCmdIndex)
				{
//						case NB_AT_MIPCALL_1:   //直接开始下一条指令
//						{
//								g_stNB_Handler.StepPt++;
//								g_stNB_Handler.State = NB_STATE_CONNECT_NET; //继续当前流程
//						}
//								break;
						
						default:
						{
								#if SEGGER_RTT_DEBUG_CONN
										SEGGER_RTT_printf(0, "Connect Reset\r\n");
								#endif
								g_stNB_Handler.State = NB_STATE_RESET;       //进行复位
								g_stNB_Handler.StepPt = 0;//clr step
								NB_NetPar.NetType = NET_NO_NET;
						}
								break;	
				}	
		}
		//////////////////////// AT指令应答错误或超时 /////////////////////////	
		else
		{
				//没有到AT指令重发次数，所以再次发送AT指令
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//直接再次发送AT指令，然后等待应答
				NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd	
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stNB_Handler.StepPt];
		}	
}

static void APP_NB_State_Mess_Proc(uint8_t *RxBuf)
{
		uint8_t i;
		uint8_t ATCmdIndex;
		char *pTemp   = NULL;
		if(g_stNB_Handler.StepPt >= MESS_STEP_NUM)
		{
				return; 
		}   
		ATCmdIndex = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
		//////////////////////// AT指令应答正确 ///////////////////////////	 
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//AT指令应答内容正确
		{
				switch(ATCmdIndex)
				{
						case NB_AT_CGSN_1:
						{
								//save cgsn
								pTemp = strstr((char*)RxBuf, "CGSN:");
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "cmd cgsn\r\n");
								#endif
								if(pTemp != NULL)
								{
										for (i = 0; i < 16; i ++)
										{
												if (((pTemp[5 + i] < '0') || (pTemp[5 + i] > '9')) && 
														((pTemp[5 + i] < 'a') || (pTemp[5 + i] > 'z')) && 
														((pTemp[5 + i] < 'A') || (pTemp[5 + i] > 'Z')))
												{
														break;
												}
										}
										if (i < 16) pTemp[5 + i] = 0x00;
										memcpy(NB_CommPacket.Init_Data.imei, pTemp+5, i);
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "len = %u, imei:%s. \r\n", i, NB_CommPacket.Init_Data.imei);
								#endif
										set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								}
						}
								break;	
						
						case NB_AT_NCCID:
						{
								//save nccid
								pTemp = strstr((char*)RxBuf, "NCCID:");
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "cmd nccid\r\n");
								#endif
								if(pTemp != NULL)
								{
										for (i = 0; i < 32; i ++)
										{
												if (((pTemp[6 + i] < '0') || (pTemp[6 + i] > '9')) && 
														((pTemp[6 + i] < 'a') || (pTemp[6 + i] > 'z')) && 
														((pTemp[6 + i] < 'A') || (pTemp[6 + i] > 'Z')))
												{
														break;
												}
										}
										if (i < 32) pTemp[6 + i] = 0x00;
										
										memcpy(NB_CommPacket.Init_Data.iccid, pTemp+6, i);
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "len = %u, nccid:%s. \r\n", i, NB_CommPacket.Init_Data.iccid);
								#endif
										set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								}
						}
								break;	
				
						default:
								break;
				}
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= MESS_STEP_NUM)
				{
						////// NB模块状态切换 ///////////////
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;//切换到下一个流程
						g_stNB_Handler.StepPt = 0;     //clr step
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_READ_MESS;//继续当前流程
				}   
		}
		//////////////////////// AT 指令达到重发次数 //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)//AT指令重发次数到了
		{
				#if SEGGER_RTT_DEBUG_MESS
						SEGGER_RTT_printf(0, "Mess Reset\r\n");
				#endif
				g_stNB_Handler.State = NB_STATE_RESET;  //进行复位
				g_stNB_Handler.StepPt = 0;//clr step
				NB_NetPar.NetType = NET_NO_NET;
		}
		//////////////////////// AT指令应答错误或超时 /////////////////////////
		else
		{
				//没有到AT指令重发次数，所以再次发送AT指令
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//直接再次发送AT指令，然后等待应答
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
				NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
		}
}

static void APP_NB_State_Comm_Proc(uint8_t *RxBuf)
{
		uint8_t i;
    uint8_t ATCmdIndex;  
    static uint8_t errCnt=0;
		char *p   = NULL;
		if(g_stNB_Handler.StepPt >= COMM_STEP_NUM)
		{
			return;	
		} 
		ATCmdIndex = s_ATCmdStep_Comm[g_stNB_Handler.StepPt];
		/************************AT指令应答正确·**************************/	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
		{
				switch(ATCmdIndex)
				{
						case NB_AT_CSQ:
						{
								//save csq
								p = strstr((char*)RxBuf, "+CSQ:");
								#if SEGGER_RTT_DEBUG_COMM
										SEGGER_RTT_printf(0, "cmd csq\r\n");
								#endif
								if(p != NULL)
								{
										NB_CommPacket.Init_Data.CSQ = 0;
										for (i = 0; i < 32; i ++)
										{
												if ((p[5 + i] < '0') || (p[5 + i] > '9'))
												{
														break;
												}
										}
										NB_CommPacket.Init_Data.CSQ = (NB_CommPacket.Init_Data.CSQ * 10) + p[5 + i] - '0';
										
								#if SEGGER_RTT_DEBUG_COMM
										SEGGER_RTT_printf(0, "csq = %u.\r\n", NB_CommPacket.Init_Data.CSQ);
								#endif
								}
						}
								break;
					
						case NB_AT_NQMGS:
						{								
								if(UPLOAD_TYPE == g_stNB_Handler.ucSendType)
								{
										/*本次通讯完成,清除对应的事件*/
										NB_EVENT_CLR(NB_EvtProc.ucUploadEvt,(0x01 << g_stNB_Handler.ucDataID));
								}
							
								NB_EVENT_CLR(g_stNB_Handler.ucCommEvent,(0x01 << g_stNB_Handler.ucDataID));
								errCnt=0;           /*AT指令成功，清除错误计数*/
								NB_TimProc.heartbeatCount = 0;      /*心跳计数清零*/
						}
								break;	
						
						default:
								break;	
				}
			//next Step
			g_stNB_Handler.StepPt++;
			if(g_stNB_Handler.StepPt >= COMM_STEP_NUM)
			{
					/******GPRS模块状态切换***************/
					/*数据标签来判断是否还有其它数据需要发送，如果有则再次进入comm，没有则退出通讯状态*/
					if(APP_NB_EvtTraverse(TRAVERSE) == false)
					{
							g_stNB_Handler.State = NB_STATE_IDLE;/*切换到下一个流程*/
							g_stNB_Handler.ucCommBusy= 0;/*Comm Complete*/
					}
					else
					{
							g_stNB_Handler.State = NB_STATE_COMM;/*继续当前流程*/		
					}
					
					g_stNB_Handler.StepPt = 0;/**clr step*/
			}
			else
			{
					g_stNB_Handler.State = NB_STATE_COMM;/*继续当前流程*/		
			}		
		}
/************************AT 指令达到重发次数**************************/	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)
		{
				if(NB_AT_NQMGS == ATCmdIndex)
				{
						g_stNB_Handler.State = NB_STATE_COMM;
						errCnt++;
				}
				
				else if((NB_AT_NMGS == ATCmdIndex) || (errCnt > 3))
				{
						#if DEBUG_LOG
									BSP_LOG("Comm Reset\r\n");
						#endif
						errCnt=0;
						g_stNB_Handler.State = NB_STATE_RESET;
						NB_NetPar.NetType = NET_NO_NET;
				}
				
				g_stNB_Handler.StepPt = 1;//clr step
				g_stNB_Handler.ucCommBusy= 0;	

		}
/************************AT指令应答错误或超时*************************/	
		else
		{
				//没有到AT指令重发次数，所以再次发送AT指令
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//直接再次发送AT指令，然后等待应答
        NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stNB_Handler.StepPt];
		} 
}

/*如果服务器下发数据不加标识头，此函数可放入AT指令解析中*/

//U8 APP_GPRS_RxDataCheck(U8 *rxBuff)   
//{
//    U8 i=0;
//    U8 rxLen=rxBuff[10]+13;
//    if(rxBuff[0] != FRAME_HEAD)       return FALSE;  //帧头校验
//    if(rxBuff[1] != 0x02)       return FALSE;        //产品类型校验
//    for(i=0;i<4;i++)
//    {
//        if(rxBuff[i+2] != PacketHead.addrRegion[i])  //地址域校验
//            return FALSE;
//    }
//    if(rxBuff[rxLen-1] != FRAME_END)   return FALSE;
//    if(FALSE == Xor_Check(rxBuff,rxLen))             //和校验
//    {
//        return FALSE;
//    }
//    #if DEBUG_LOG
//    BSP_LOG("DECODE TRUE\r\n");
//    #endif
//    return TRUE;
//}
//static void APP_GPRS_RxDataDecode(U8 *rxBuff)
//{
//    U8 i=0;
//    U8 *p_buf=rxBuff+14;
//    U8 ctrCode  = rxBuff[9];
//  //  U8 dataLen  = rxBuff[10];
//    U16 command = ((U16)(rxBuff[11])<<8) + rxBuff[12]; 
//    U8 flowNum= rxBuff[13];
//    switch(ctrCode)
//    {
//         case WRITE_CTROL_CODE:             /*服务器下发写数据包*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             switch(command)
//             {
//                 case SET_PARMTE_ID:       //设置参数
//                 {
//                      for(i=0;i<SERVER_IP_LEN;i++)
//                      {
//                           GPRS_NetPar.ServerIp[i] = p_buf[i];
//                      }
//                      GPRS_NetPar.ServerPort = ((U16)(p_buf[SERVER_IP_LEN]<<8)+p_buf[SERVER_IP_LEN+1]);
//                      for(i=0;i<SERVER_APN_LEN;i++)
//                      {
//                           GPRS_NetPar.ServerApn[i] = p_buf[SERVER_IP_LEN+2+i];
//                      }
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_SET_PARMTE);
//                      set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//置起存储任务
//                 }
//                 break;
//                 case WRITE_ADDR_ID:      //写地址
//                 {
//                      for(i=0;i<4;i++)
//                      {
//                            PacketHead.addrRegion[i] = p_buf[i];
//                      }
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_WRITE_ADDR);
//                      set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//置起存储任务
//                 }
//                 break;
//                 case SYS_PARMTE_SET_ID:  //系统参数设置
//                 {
//                      Charge_ThresValue.PulloutTime     = (U16)((p_buf[0]<<8)+p_buf[1]);
//                      Charge_ThresValue.DeviationValue  = (U16)((p_buf[2]<<8)+p_buf[3]);
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_SYS_PARMTE_SET);
//                      set_task(MEM_WRITE, MEM_STORE_DYNAMIC_DATA);//置起存储任务
//                 }
//                 break;
//                 default:
//                    break;
//             }
//         }
//         break;
//         case READ_CTROL_CODE:              /*服务器下发读数据包*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             switch(command)
//             {
//                 case QUERY_PARMTE_ID:      //查询参数设置
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_QUERY_PARMTE);
//                 }
//                 break;
//                 case READ_MESS_ID:         //读信息
//                 {
//                       if(g_stGPRS_Handler.ucCommBusy == 0)
//                       {
//                            //这里再读一次信号强度
//                            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
//		                    g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //重新读取信息
//		                    g_stGPRS_Handler.StepPt = 0;
//		               }
//		               GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_MESS);
//		               
//                 }
//                 break;
//                 case READ_ADDR_ID:         //读地址
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_ADDR);
//                 }
//                 break;
//                 case READ_STA_MESS_ID:         //读地址
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_STA_MESS);
//                 }
//                 break;
//                 default:
//                    break;
//             }
//         }
//         break;
//         case CONTROL_CTROL_CODE:           /*服务器下发控制包*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             if(command == CONTROL_ID)      //控制包
//             {
//                 Charge_ThresValue.ChargeTime =  (U16)((p_buf[0]<<8)+p_buf[1]);
//                 if(Charge_ThresValue.ChargeTime != 0) 
//                 {
//                    Charge_Flag.ChargeAction = ENABLE;
//                    Charge_Flag.ChargeStatus = CHARGE_ENABLE;
//                 }
//                 else
//                 {
//                    g_stCharge_Handler.State = STATE_CHARGE_OFF;
//                    Charge_Flag.ChargeStatus = CHARGE_DISABLE;
//                    g_stCharge_Handler.CloseType = CLOSE_CMD;
//                 }
//                 set_task(MEM_WRITE,MEM_STORE_DYNAMIC_DATA);
//                 GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_CTROL);
//             }
//         }
//         break;
//         case UPLOAD_CTROL_CODE:            /*服务器返回主动上传包*/
//         {
//             PacketHead.TerminalMeageFlowNum++;
//             switch(command)
//             {
//                 case AUTHENTICA_ID:       //登录包返回
//                 {
//                    //登录成功
//                    APP_GPRS_RetryMessInit();
//                    g_stGPRS_Handler.AuthStatus = AUTH;
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_AUTH); 
//                 }
//                 break;
//                 case STA_MESS_ID:         //状态信息包返回
//                 {
//                    APP_GPRS_RetryMessInit();
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_STA_MESS);
//                 }
//                 break;
//                 case HEARTBEAT_ID:       //心跳包返回
//                 {
//                    //心跳包发送成功
//                    APP_GPRS_RetryMessInit();
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_HEARTBEAT);
//                 }
//                 break;

//                 default:
//                    break;
//             }
//         }
//         break;
//         default:
//                 break;
//    }
//}

static void APP_NB_EventProc(uint8_t surEvt, uint8_t* objEvt)
{
    uint8_t ucID=0;
    if(false == NB_EVENT_IS_EMPTY(surEvt)) 
    {
				ucID = Get_CommEvent(surEvt);
				NB_EVENT_SET(*objEvt, (1<<ucID));
    }
}

static uint8_t APP_NB_EvtTraverse(uint8_t mode)
{    
    if(false == NB_EVENT_IS_EMPTY(NB_EvtProc.ucUploadEvt))
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucUploadEvt, &g_stNB_Handler.ucCommEvent);
        return UPLOAD_TYPE;
    }
    else if(false == NB_EVENT_IS_EMPTY(g_stNB_Handler.ucCommEvent))
    {
        //理论上程序无法运行到这里
        g_stNB_Handler.ucCommEvent = 0;
    #if SEGGER_RTT_DEBUG_NB86
        SEGGER_RTT_printf(0, "Event is Error \r\n");
    #endif
        return 0;
    }
    else
    {
        g_stNB_Handler.State = NB_STATE_IDLE;  //切换至IDLE态
        return 0;
    }
}

static void APP_NB_Reset_Init(void)
{                 
    //延时事件添加至主动上传事件中
    //需把延时事件全部添加至主动上传事件中
    g_stNB_Handler.AuthStatus = NOT_AUTH;
    NB_EVENT_SET(NB_EvtProc.ucUploadEvt, COMM_EVENT_INIT);
    g_stNB_Handler.ucCommEvent = 0;
    NB_NetPar.NetType = NET_NO_NET;           //无网络状态连接
    NB_NetPar.NetConnSta = CONN_OFF;
    APP_NB_RetryMessInit();                   //重传相关变量初始化
}

//模块通讯相关变量初始化
void app_comm_init(void)
{
//#if DEBUG_TROPE_FUN
//    PacketHead.TerminalMeageFlowNum = 0x19;
//#endif
		g_stNB_Handler.State = NB_STATE_POWER_ON; 
		g_stNB_Handler.StateOld = NB_STATE_POWER_ON; 
		g_stNB_Handler.StepPt = 0;
		g_stNB_Handler.AuthStatus = NOT_AUTH;
//		BSP_NB_POWERON_SET;
//		nrf_delay_us(1000);
//		BSP_NB_RESET_CLEAR
//		while(1)
//		{}
		NB_EVENT_SET(NB_EvtProc.ucUploadEvt, COMM_EVENT_INIT);//上电需发送一个登录包
		set_task(COMM, COMM_STATE_PROC);     //启动GPRS状态处理任务
}

void APP_NB_TimeProc(uint16_t usPeriod)
{
    NB_msCount += usPeriod;
//    if(GPRS_ATCmdCB.Busy == 1)   //在数据接收时，AT指令也忙碌，这边可能会有冲突，把接收时，忙碌标志去除
//	{
		/*********AT指令应答超时**************/
		if(NB_ATCmdCB.ulTimeOut >= usPeriod)
		{
				NB_ATCmdCB.ulTimeOut -= usPeriod;
		}
		else
		{
				NB_ATCmdCB.ATCmdResult = BACK_TIMEOUT;/*超时*/ 
		}
//	}
		
		if(NB_ATCmdCB.ucByteTimeOut >= usPeriod)
		{
				NB_ATCmdCB.ucByteTimeOut -= usPeriod;
				if(NB_ATCmdCB.ucByteTimeOut < usPeriod)
				{
						NB_ATCmdCB.RxFrameOk = 1;
						set_task(COMM, COMM_CMD_PROC);
				}
		}
		
		/***************** NB 模块延时计 ************************/
		if(g_stNB_Handler.ulDelayCnt >= usPeriod)    
		{
				g_stNB_Handler.ulDelayCnt -= usPeriod;
		}
    else
    {
        g_stNB_Handler.ulDelayCnt = 0;
    }
		
		 /*****************NB 模块状态处理函数*************************/	
		g_stNB_Handler.ulProcCtrTaskPer += usPeriod;
		if(g_stNB_Handler.ulProcCtrTaskPer >= SUB_TASK_PROCCTR_PER)
		{
				g_stNB_Handler.ulProcCtrTaskPer -= SUB_TASK_PROCCTR_PER;
				set_task(COMM, COMM_STATE_PROC);
		}	
		if(NB_msCount >= NB_MS_TO_S)   //ms to S
		{
				NB_msCount =0;
			
				/***************** 心跳包处理 *************************/
				if((CONN_ON == NB_NetPar.NetConnSta) && (AUTH == g_stNB_Handler.AuthStatus))  /*心跳包*/
				{
						NB_TimProc.heartbeatCount++;
						if(NB_TimProc.heartbeatCount >= HEARTBEAT_TIME)
						{
								NB_TimProc.heartbeatCount = 0;
								NB_EVENT_SET(NB_EvtProc.ucUploadEvt, COMM_EVENT_HEARTBEAT);
						}
				}
		}
}


/*模块AT指令解析任务*/
void APP_SubTask_CmdProc(void)
{
		#if SEGGER_RTT_DEBUG_CMDPROC
//			SEGGER_RTT_printf(0, "APP SubTask CmdProc !\r\n");
		#endif
//    nrf_uart_disable(NRF_UART0);
    NB_ATCmdCB.Busy = 0;   
    NB_ATCmdCB.ucRxLen = NB_ATCmdCB.ucRxCnt;
    NB_ATCmdCB.ucRxCnt = 0;
    NB_ATCmdCB.RxFrameOk = 0;
    memcpy(g_stNB_Handler.RxBuf, NB_ATCmdCB.RxBuf, NB_ATCmdCB.ucRxLen);
//    nrf_uart_enable(NRF_UART0);

    NB_ATCmdCB.ATCmdResult = APP_GPRS_ATCmdAckCheck(g_stNB_Handler.RxBuf); 
    ////////// 应答错误直接返回 /////////////////////// 
    if(BACK_ERROR == NB_ATCmdCB.ATCmdResult) 
    {
        return;
    }
    ////////// 应答AT指令直接返回,通过APP_SubTask_StateProc 处理 ///
    if(RSP_TYPE_CMD == NB_ATCmdCB.RspType)
    {
        return;
    }
    NB_ATCmdCB.ATCmdResult = NO_BACK;
}


void APP_SubTask_StateProc(void)
{
		static uint8_t ResetTimesCount = 0;
	
    switch(g_stNB_Handler.State)
    {
				case NB_STATE_WAIT_ACK:
        {
						//如果没有结果就直接返回
						if(NO_BACK == NB_ATCmdCB.ATCmdResult)
						{
								return;	
						}  
						//AT应答超时，或者AT指令应答解析错误，则判断是否需要重发
						if((NB_ATCmdCB.ATCmdResult == BACK_ERROR) || (NB_ATCmdCB.ATCmdResult == BACK_TIMEOUT))  
						{
								if(NB_ATCmdCB.ucResendCnt >= NB_ATCmdCB.ucResendTimes)
								{	
										NB_ATCmdCB.ATCmdResult = BACK_PERTIMES_OUT;//重发次数到了
								}			
						}
						//根据不同的结果，做相应处理
						if(APP_NB_StateProcTbl[g_stNB_Handler.StateOld] != NULL)
						{
								APP_NB_StateProcTbl[g_stNB_Handler.StateOld](g_stNB_Handler.RxBuf);		
						}
						if((BACK_TRUE == NB_ATCmdCB.ATCmdResult) || (BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult))/*AT指令应答内容正确*/
						{
								//更新StateOld
								g_stNB_Handler.StateOld = NB_STATE_WAIT_ACK;
						}
						NB_ATCmdCB.ulTimeOut = 0;
						NB_ATCmdCB.Busy = 0;              
				}
						break;
				 
        case NB_STATE_POWER_ON:
        {
						if(g_stNB_Handler.ulDelayCnt > 0)
						{
								return;
						}
						if(0 == g_stNB_Handler.StepPt)
						{
								BSP_NB_POWERON_CLEAR;
								BSP_NB_RESET_SET;
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "clear nb poweron and reset !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 1200;      //拉低1.2s
						}
            else if(1 == g_stNB_Handler.StepPt)
						{
								BSP_NB_POWERON_SET;
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "set nb poweron !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 100;  //拉高100ms
								LED_Status = CONN_INDI;            //正在联网LED指示
						}
						else if(2 == g_stNB_Handler.StepPt)
						{
								BSP_NB_RESET_CLEAR;
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "set nb reset !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 7000; //等待7000ms后再操作NB模块
						}
						else//next state
						{
								//一些变量清0
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "next state init !\r\n");
						#endif
                NB_NetPar.NetType = NET_NO_NET;//无网络状态连接
								g_stNB_Handler.StateOld = NB_STATE_POWER_ON;	
								g_stNB_Handler.State = NB_STATE_INIT;//开始下一个流程
								g_stNB_Handler.StepPt = 0;	    
						}
				}
						break;
         ///////////// 该状态为GPRS模块POWER_OFF，所以执行完该流程后需POWER_ON /////////////
         // 如该流程直接完成模块的复位，那么执行完该流程后,可开始AT指令流程 //
				case NB_STATE_RESET:  
        {
						if(g_stNB_Handler.ulDelayCnt > 0)
            {
								return;
            }
            if(0 == g_stNB_Handler.StepPt)
						{
								//引脚操作
								BSP_NB_RESET_CLEAR;
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 200; //拉低200ms
								LED_Status = SYS_INDI;     //系统LED指示
						}
						else if(1 == g_stNB_Handler.StepPt)
						{
								//引脚操作
								BSP_NB_RESET_SET;
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 50;
						}
						else//next state
						{
								//一些变量清0
                NB_NetPar.NetType = NET_NO_NET;               //无网络状态连接
								g_stNB_Handler.StateOld = NB_STATE_RESET;	
								g_stNB_Handler.State = NB_STATE_POWER_ON;   //开始下一个流程
								g_stNB_Handler.StepPt = 0;	
								APP_NB_Reset_Init();
								ResetTimesCount++;
								if(ResetTimesCount >=3)
								{
										ResetTimesCount = 0;
										task_flag_struct.closeStaProc = ENABLE;
								#if SEGGER_RTT_DEBUG_RESET
										SEGGER_RTT_printf(0, "Close State Proc\r\n");
								#endif
								}
								// 这边添加复位次数计数，如超过设定复位次数，那么暂时不进行对GPRS的操作 //
								//// 具体实现可关闭定时器里面的周期状态处理函数 /////
						}
        }
						break;
				 
				case NB_STATE_INIT:
        {
						if((g_stNB_Handler.ulDelayCnt > 0) && (task_flag_struct.atReday_F == DISABLE))
            {
								return;
            }
            if(0 == g_stNB_Handler.StepPt)
						{
						#if SEGGER_RTT_DEBUG_INIT
								SEGGER_RTT_printf(0, "init wait nb start !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 10000; //10S NB AT准备就绪超时时间
						}
						else
						{
						#if SEGGER_RTT_DEBUG_INIT
								SEGGER_RTT_printf(0, "init nb has start !\r\n");
						#endif
								g_stNB_Handler.ulDelayCnt = 0;  //计数清0
								task_flag_struct.atReday_F = DISABLE; //AT就绪失能
								g_stNB_Handler.StateOld = g_stNB_Handler.State;
								g_stNB_Handler.State = NB_STATE_WAIT_ACK;
								NB_ATCmdCB.SendATCmdEn = 1; //enable send ATCmd
								NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stNB_Handler.StepPt];  
						}
				}
						break;
				
        case NB_STATE_CONNECT_NET:
				{				
						g_stNB_Handler.StateOld = g_stNB_Handler.State; 
						g_stNB_Handler.State = NB_STATE_WAIT_ACK;
						NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
						NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stNB_Handler.StepPt];
         }
						break;	
				 
        case NB_STATE_READ_MESS:
        {
						if((g_stNB_Handler.ulDelayCnt > 0) && (task_flag_struct.atReday_F == DISABLE))
            {
								return;
            }
            if(0 == g_stNB_Handler.StepPt)
						{
						#if SEGGER_RTT_DEBUG_MESS
								SEGGER_RTT_printf(0, "mess wait nb start !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 10000; //20S GPRS AT准备就绪超时时间
						}
						else
						{
						#if SEGGER_RTT_DEBUG_MESS
								SEGGER_RTT_printf(0, "mess nb has start !\r\n");
						#endif
								g_stNB_Handler.ulDelayCnt = 0;  //计数清0
								task_flag_struct.atReday_F = DISABLE; //AT就绪失能
								g_stNB_Handler.StateOld = g_stNB_Handler.State; 
								g_stNB_Handler.State = NB_STATE_WAIT_ACK;
								NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
								NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
						}
        }
						break;
				
        case NB_STATE_COMM:
        {
            g_stNB_Handler.ucSendType = APP_NB_EvtTraverse(TRAVERSE_AND_ADDEVT);
            if((false == g_stNB_Handler.ucSendType) || (NB_EVENT_IS_EMPTY(g_stNB_Handler.ucCommEvent)))
            {
                break;
            }

						if(0 == g_stNB_Handler.StepPt)
						{
								g_stNB_Handler.ucCommBusy = 1;    //标记正在和服务器通讯
//								LED_Status = DATA_SEND_INDI;    //正在通讯LED指示
						}
            
						if(NB_AT_NMGS == s_ATCmdStep_Comm[g_stNB_Handler.StepPt]) 
						{   
								//APP_NB_WriteReturnIDPacket与APP_NB_WriteDataIDPacket函数可以合二为一
								//但考虑到后续项目协议可能增多，最终还是得把主动上传和应答消息分开
								switch(g_stNB_Handler.ucSendType)
								{									
										case UPLOAD_TYPE:
										{
												g_stNB_Handler.ucDataID = Get_CommEvent(NB_EvtProc.ucUploadEvt);
												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteDataIDPacket(g_stNB_Handler.ucDataID, g_stNB_Handler.TxBuf);
										}
												break;
										
										default:
												break;
								}
						}
            //此时重发计数为3，并且为第一条指令。因为第二条指令会把重传计数加1
						if((NB_RetryCtrol.rtycnt >= RTY_TIMES_MAX) && (g_stNB_Handler.StepPt ==0))   
						{
								g_stNB_Handler.StateOld = NB_STATE_RESET;   //复位GPRS模块
								g_stNB_Handler.State = NB_STATE_RESET;
								g_stNB_Handler.StepPt = 0;			    
             #if DEBUG_LOG
                BSP_LOG("Rty Reset\r\n");
             #endif
								return;
						}
						g_stNB_Handler.StateOld = g_stNB_Handler.State; 
						g_stNB_Handler.State = NB_STATE_WAIT_ACK;
						NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd 
						NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stNB_Handler.StepPt];
        }
						break;
				
        case NB_STATE_IDLE:
				{
						uint8_t NextStateOffset;
						//判断是否有数据需要通讯
						if(APP_NB_EvtTraverse(TRAVERSE) != false)
						{
								//有数据要通讯则设置切换到COM状态的事件
								NB_State_Event_Set(STATE_Event_COMM); 
						}
									
						if(NB_IsSwitch_State())  //现不做处理，后续可用做低功耗
						{
								break;
						}
						
						//获取下一个优先级高的状态
						NextStateOffset = Get_NextState(g_stNB_Handler.ucStateEvent);
						g_stNB_Handler.State = NB_STATE_COMM + NextStateOffset;
						NB_State_Event_Clr(0x01 << NextStateOffset);
						g_stNB_Handler.StateOld = NB_STATE_IDLE; 
						g_stNB_Handler.StepPt = 0;		
				}
						break;	
				
				default:
				{
						g_stNB_Handler.State = NB_STATE_INIT;
						g_stNB_Handler.StepPt = 0;
				}
						break;
		}
    if(1 == NB_ATCmdCB.SendATCmdEn)
    {
				NB_ATCmdCB.SendATCmdEn = 0;
				APP_NB_SendATCmd(&NB_ATCmdCB);
    }
}


//void APP_SuTask_DecodeProc(void)
//{   
//    U8  TropeBuf[GPRS_RX_BUFF_LEN]={0};
//    U8  rxLen = GPRS_ATCmdCB.ucRxLen;
//    U8 *rxBuff = &g_stGPRS_Handler.RxBuf[Get_Comma(2,g_stGPRS_Handler.RxBuf)];
//    APP_GPRS_Data_Trope(rxBuff,rxLen,TropeBuf);
//    if(APP_GPRS_RxDataCheck(TropeBuf) == FALSE)  return;   //标记错误码，待添加
//    APP_GPRS_RxDataDecode(TropeBuf);
//}


