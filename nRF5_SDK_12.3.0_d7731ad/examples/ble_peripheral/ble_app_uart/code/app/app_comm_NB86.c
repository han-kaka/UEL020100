#include "app_comm_NB86.h"
#include "nrf_drv_gpiote.h"
//#include "nrf_delay.h"

static uint16_t NB_msCount = 0;

uint8_t                                 g_Rx_Buffer[BUFFSIZE] = {0};
uint8_t                                 g_Tx_Buffer[BUFFSIZE] = {0};

NB_Handler        g_stNB_Handler;
PACKET_HEAD_Type  PacketHead;

typedef void(*NB_State_Fun)(uint8_t *RxBuf); /* ¶¨Òå×´Ì¬´¦Àíº¯Êý*/
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


/******************GPRSÖ¸Áî¼¯**********************************/
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

/*************************³õÊ¼»¯Á÷³Ì***************************/
static const uint8_t s_ATCmdStep_Init[] = 
{
    NULL,
    NB_AT_CFUN_0,      
		NB_AT_CFUN0,      
		NB_AT_NCDP, 
		NB_AT_NRB,	
};

/************************* TCP/IPÊý¾ÝÁ´Â·½¨Á¢Á¬½Ó***************************/
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

/*************************Êý¾Ý·¢ËÍÁ÷³Ì***************************/
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

static const uint8_t s_ATCmdStep_Read[] = 
{
		NB_AT_NMGR,
		NB_AT_NQMGR,
};

static const uint8_t s_ATCmdStep_Inquire[] = 
{
		NB_AT_NPSMR1,
};

/*Ð£ÑéATÖ¸ÁîÓ¦´ð½á¹ûÊÇ·ñÕýÈ·*/
static uint8_t APP_GPRS_ATCmdAckCheck(uint8_t *RxBuf)
{   

//    //ÅÐ¶ÏÊÇ·ñÊÇ·þÎñÆ÷ÏÂÐÐÊý¾Ý
//    if(Check_Strstr((char*)RxBuf, (char*)(char*)AT_CmdTbl[NB_AT_MIPRTCP].ExpResultStr, NB_ATCmdCB.ucRxLen) == true)
//    {
//        NB_ATCmdCB.RspType = RSP_TYPE_DATA;  //Í¨ÐÅÊý¾Ý
//        return BACK_TRUE;   //½ÓÊÕµ½ÕýÈ·Ó¦´ð
//    }
    if(Check_Strstr((char*)RxBuf, "Lierda", NB_ATCmdCB.ucRxLen) == true) //GPRSÄ£¿é×¼±¸¾ÍÐ÷ÍÆËÍ
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
//    if(NB_ATCmdCB.ATCmdPt != NB_AT_MIPOPEN_1)//socketÁ¬½ÓÊ±,Ò²ÓÐ¿ÉÄÜ»Ø¸´MIPSTAT: 1,1
//    {
//        if(Check_Strstr((char*)RxBuf, "MIPSTAT: 1,1", NB_ATCmdCB.ucRxLen) == true)
//        {
//            NB_NetPar.NetConnSta = CONN_OFF;
//            g_stNB_Handler.State = NB_STATE_CONNECT_NET;    //ÖØÐÂÁ¬½Ósocket
//						g_stNB_Handler.StepPt = 2;
//						g_stNB_Handler.AuthStatus = NOT_AUTH;
//						NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
//        }
//    }
//    if(NB_ATCmdCB.ATCmdPt != NB_AT_MIPCALL1) //ÓÉÓÚIP»ñÈ¡´íÎó£¬Ò²»áÈ¥²éÑ¯ÓÐÎÞIP
//    {
//        if(Check_Strstr((char*)RxBuf, "MIPCALL: 0", NB_ATCmdCB.ucRxLen) == true)
//        
//        { 
//            NB_NetPar.NetConnSta = CONN_OFF;
//						g_stNB_Handler.State = NB_STATE_CONNECT_NET;    //ÖØÐÂÁ¬½ÓÍøÂç
//						g_stNB_Handler.StepPt = 0;
//						g_stNB_Handler.AuthStatus = NOT_AUTH;
//						NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
//        }
//    }
    
     //ÅÐ¶Ï½ÓÊÕµÄÊý¾ÝÊÇ·ñº¬ÓÐÆÚÍûÓ¦´ð
		if(Check_Strstr((char*)RxBuf, NB_ATCmdCB.ExpResultStr, NB_ATCmdCB.ucRxLen) == true)
		{
				if((NB_ATCmdCB.ATCmdPt == NB_AT_CFUN0) ||
					 (NB_ATCmdCB.ATCmdPt == NB_AT_CFUN1) ||
					 (NB_ATCmdCB.ATCmdPt == NB_AT_CGATT1) ||
					 (NB_ATCmdCB.ATCmdPt == NB_AT_CGATT0))
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
				
				if((NB_ATCmdCB.ATCmdPt == NB_AT_CGSN_1) ||
					 (NB_ATCmdCB.ATCmdPt == NB_AT_NCCID) ||
				   (NB_ATCmdCB.ATCmdPt == NB_AT_CSQ))
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
				NB_ATCmdCB.RspType = RSP_TYPE_CMD;//Ó¦´ðATÖ¸Áî
				return BACK_TRUE;                 //½ÓÊÕµ½ÕýÈ·Ó¦´ð
		} 
    else
		{   
//				//ÓÉÓÚ»ñÈ¡ÍøÂç²àµØÖ·ºÍ½¨Á¢soctetÁ¬½Ó¶¼ÊÇ·ÖÁ½Ìõ·µ»ØµÄ£¬µÚÒ»Ìõ¶¼Îª"OK"
//				if((NB_ATCmdCB.ATCmdPt == NB_AT_MIPCALL_1) || NB_ATCmdCB.ATCmdPt == NB_AT_MIPOPEN_1)
//				{
//						if(Check_Strstr((char*)RxBuf, "OK", 4) == true)   //È¥µôÕâÒ»¾äÔÙÊÔ
//						{
//								return NO_BACK;   /*µÚÒ»Ìõ"OK"·µ»ØÕýÈ·£¬ËùÒÔ¼ÌÐøµÈ´ý*/
//						}
//				}
			  //²éÑ¯ÊÇ·ñ×¤Íø³É¹¦£¬Èç²»³É¹¦£¬ÄÇÃ´µÈ´ý3S£¬¶ø²»ÊÇ·µ»Ø´íÎó£¬¸ÃÌõÖ¸ÁîÌØÊâ
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
//           GPRS_SysStaMess.chargeTime = g_stCharge_Handler.ChargeTimCnt;    //³äµçÊ±³¤
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
//ÖØ´«Ê±Ò²ÖØÐÂ×é°ü£¬·ÀÖ¹µÚÒ»°üÊý¾Ý³ö´í£¬ÄÇÃ´ºóÐøÁ½°üÊý¾ÝÒ²»á´íÎó
//ºóÐøÖØ´«Éæ¼°µ½Ê±¼äÊý¾Ý£¬ÄÇÃ´Ðè°ÑÊý¾Ý±£´æ£¬ÖØÐÂ×é°üÊ±ÔÙ·ÅÈë
//ÈçÖØ´«Ê±²»ÖØÐÂ×é°ü£¬Ðè°ÑºêCONFIG_RETRY_COPYÖÃÎ»
    uint16_t usPacketLen=0;
#if SEGGER_RTT_DEBUG_NB86
		BSP_LOG("retry send mess\r\n");
#endif
    memcpy(Packet,GPRS_RetryCtrol.rtyBuff,GPRS_RetryCtrol.rtyLen);    //ÖØ´«ÐÅÏ¢copyÖÁ·¢ËÍ»º´æ
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

/*¸ù¾Ý²»Í¬µÄÊý¾ÝID,½øÐÐ×éÖ¡£¬·â×°Ð­Òé²ãµÄÖ¡ÄÚÈÝ*/
static uint16_t APP_NB_WriteDataIDPacket(uint8_t DataID, uint8_t* Packet)
{
    uint16_t usPacketLen = 0;
		switch(DataID)
		{   
				case UPLOAD_ID_INIT:
				{
						usPacketLen = APP_NB_GetPacket(INIT_ID, Packet);
//				#if CONFIG_RETRY_COPY            
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //ÏûÏ¢³¤¶È±¸·Ý
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //ÏûÏ¢±¸·Ý   
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
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //ÏûÏ¢³¤¶È±¸·Ý
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //ÏûÏ¢±¸·Ý   
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
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //ÏûÏ¢³¤¶È±¸·Ý
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //ÏûÏ¢±¸·Ý   
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
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //ÏûÏ¢³¤¶È±¸·Ý
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //ÏûÏ¢±¸·Ý   
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
//    port = DecimalToASCII(GPRS_NetPar.ServerPort,data); //¶Ë¿ÚºÅ×ª»»Ö§³Ö1--65535
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
//    //¶Ë¿ÚºÅ×ª»»³ö´í£¬´òÓ¡ÐÅÏ¢
//    
//}

static uint8_t APP_NB_WriteATPacket(uint8_t ATCmdIndex, uint8_t *pFrameDes)
{  
    uint8_t ucSendLen;
    uint8_t dataNum;
    char *endStr="\x1A\r\n";
		switch(ATCmdIndex)
		{
//				case NB_AT_MIPOPEN_1:/*½¨Á¢socketÁ¬½Ó*/
//				{
//						strcpy((char*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
//						dataNum = Get_Comma(2, (uint8_t*)pFrameDes);
//						APP_GPRS_SetIP(&pFrameDes[dataNum]);			    
//						ucSendLen = strlen((char*)pFrameDes);
//					
//				}break;
//				
//				case NB_AT_DATA:/*Ìî³äÊý¾ÝÄÚÈÝ*/
//				{
//						memcpy(pFrameDes, g_stNB_Handler.TxBuf, g_stNB_Handler.ucTxLen);/*copy Ó¦ÓÃ²ãÊý¾Ý*/
//						ucSendLen = strlen(endStr);
//						memcpy(pFrameDes+g_stNB_Handler.ucTxLen,endStr,ucSendLen);        /*copy ½áÎ²·û*/
//						ucSendLen += g_stNB_Handler.ucTxLen;
//					
//				}break;
				
				default:
				{
						strcpy((char*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
						ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);//Ä¬ÈÏµÄÖ¸Áî·û³¤¶È
					
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
			return ;//µ±Ç°Ã¦Âµ×´Ì¬
		}
		//¸ù¾Ý²»Í¬µÄATÓ¦´ð½á¹û,ÉèÖÃÖØ·¢´ÎÊý
		if((BACK_TRUE == pAtCmd->ATCmdResult) || (BACK_PERTIMES_OUT == pAtCmd->ATCmdResult))
		{
			ucResendCnt = 0;//ÖØ·¢ÇåÁã
		}
		else
		{
				ucResendCnt = NB_ATCmdCB.ucResendCnt;//±¸·ÝÖØ·¢´ÎÊý
		}

		memset((uint8_t*)pAtCmd, 0, sizeof(NB_ATCmdCB));
		
		pAtCmd->Busy         = 1;          //ÉèÖÃÃ¦ÂÌ×´Ì¬
		pAtCmd->ATCmdPt      = ATCmdIndex; //¸üÐÂATË÷ÒýºÅ
		pAtCmd->ATCmdResult  = NO_BACK;
		pAtCmd->ucResendCnt  = ucResendCnt;  
		pAtCmd->ucResendTimes = AT_CmdTbl[ATCmdIndex].ucResendTimes;
		pAtCmd->ulTimeOut    = TmrSecondsToMilliseconds(AT_CmdTbl[ATCmdIndex].usTimeOut);//Ë¢ÐÂ³¬Ê±Ê±¼ä

		////// Ìî³äÖ¸ÁîÄÚÈÝ³¤¶È //////
		pAtCmd->ucCmdStrLen = APP_NB_WriteATPacket(ATCmdIndex, pAtCmd->CmdStr);
		//Ìî³äÖ¸Áî·ûÆÚÍû½á¹û	
		strcpy(pAtCmd->ExpResultStr, AT_CmdTbl[ATCmdIndex].ExpResultStr);
		//·¢ËÍATÖ¸Áî
		NB_SendATCmdStr(pAtCmd->CmdStr, pAtCmd->ucCmdStrLen);
		//·¢ËÍ´ÎÊý+1
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
		//////////////////////// ATÖ¸ÁîÓ¦´ðÕýÈ· ///////////////////////////	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//ATÖ¸ÁîÓ¦´ðÄÚÈÝÕýÈ·
		{
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= INIT_STEP_NUM)
				{
						////// GPRSÄ£¿é×´Ì¬ÇÐ»» ///////////////
						g_stNB_Handler.State = NB_STATE_READ_MESS;//ÇÐ»»µ½ÏÂÒ»¸öÁ÷³Ì
						g_stNB_Handler.StepPt = 0;     //clr step
						NB_NetPar.NetType = NET_NO_NET;//ÎÞÁªÍø×´Ì¬×´Ì¬
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_INIT;//¼ÌÐøµ±Ç°Á÷³Ì		
				}	
		}
		//////////////////////// AT Ö¸Áî´ïµ½ÖØ·¢´ÎÊý //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)//ATÖ¸ÁîÖØ·¢´ÎÊýµ½ÁË
		{
				switch(ATCmdIndex)
				{
						default:
						{
								#if SEGGER_RTT_DEBUG_INIT
										SEGGER_RTT_printf(0, "Init Reset\r\n");
								#endif
								g_stNB_Handler.State = NB_STATE_RESET;       //½øÐÐ¸´Î»
								g_stNB_Handler.StepPt = 0;//clr step
								NB_NetPar.NetType = NET_NO_NET;
						}
							break;	
				}
		}
		//////////////////////// ATÖ¸ÁîÓ¦´ð´íÎó»ò³¬Ê± /////////////////////////	
		else
		{
				//Ã»ÓÐµ½ATÖ¸ÁîÖØ·¢´ÎÊý£¬ËùÒÔÔÙ´Î·¢ËÍATÖ¸Áî
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//Ö±½ÓÔÙ´Î·¢ËÍATÖ¸Áî£¬È»ºóµÈ´ýÓ¦´ð
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
		//////////////////////// ATÖ¸ÁîÓ¦´ðÕýÈ· ///////////////////////////	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//ATÖ¸ÁîÓ¦´ðÄÚÈÝÕýÈ·
		{	           
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= CONNECT_STEP_NUM)
				{
						////// NBÄ£¿é×´Ì¬ÇÐ»» ///////////////
						g_stNB_Handler.State = NB_STATE_IDLE;//ÇÐ»»µ½ÏÂÒ»¸öÁ÷³Ì
						g_stNB_Handler.StepPt = 0;//clr step
						NB_NetPar.NetConnSta = CONN_ON;
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;//¼ÌÐøµ±Ç°Á÷³Ì
				}			
		}
		//////////////////////// AT Ö¸Áî´ïµ½ÖØ·¢´ÎÊý //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)
		{
				switch(ATCmdIndex)
				{
//						case NB_AT_MIPCALL_1:   //Ö±½Ó¿ªÊ¼ÏÂÒ»ÌõÖ¸Áî
//						{
//								g_stNB_Handler.StepPt++;
//								g_stNB_Handler.State = NB_STATE_CONNECT_NET; //¼ÌÐøµ±Ç°Á÷³Ì
//						}
//								break;
						
						default:
						{
								#if SEGGER_RTT_DEBUG_CONN
										SEGGER_RTT_printf(0, "Connect Reset\r\n");
								#endif
								g_stNB_Handler.State = NB_STATE_RESET;       //½øÐÐ¸´Î»
								g_stNB_Handler.StepPt = 0;//clr step
								NB_NetPar.NetType = NET_NO_NET;
						}
								break;	
				}	
		}
		//////////////////////// ATÖ¸ÁîÓ¦´ð´íÎó»ò³¬Ê± /////////////////////////	
		else
		{
				//Ã»ÓÐµ½ATÖ¸ÁîÖØ·¢´ÎÊý£¬ËùÒÔÔÙ´Î·¢ËÍATÖ¸Áî
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//Ö±½ÓÔÙ´Î·¢ËÍATÖ¸Áî£¬È»ºóµÈ´ýÓ¦´ð
				NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd	
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stNB_Handler.StepPt];
		}	
}

static void APP_NB_State_Mess_Proc(uint8_t *RxBuf)
{
		uint8_t i;
		uint8_t ATCmdIndex;
//		uint8_t asi_len=0;
//		char *str = "898";
		char *p   = NULL;
		if(g_stNB_Handler.StepPt >= MESS_STEP_NUM)
		{
				return; 
		}   
		ATCmdIndex = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
		//////////////////////// ATÖ¸ÁîÓ¦´ðÕýÈ· ///////////////////////////	 
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//ATÖ¸ÁîÓ¦´ðÄÚÈÝÕýÈ·
		{
				switch(ATCmdIndex)
				{
						case NB_AT_CGSN_1:
						{
								//save cgsn
								p = strstr((char*)RxBuf, "CGSN:");
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "cmd cgsn\r\n");
								#endif
								if(p != NULL)
								{
										for (i = 0; i < 16; i ++)
										{
												if (((p[5 + i] < '0') || (p[5 + i] > '9')) && 
														((p[5 + i] < 'a') || (p[5 + i] > 'z')) && 
														((p[5 + i] < 'A') || (p[5 + i] > 'Z')))
												{
														break;
												}
										}
										if (i < 16) p[5 + i] = 0x00;
										memcpy(NB_CommPacket.Init_Data.imei, p+5, i);
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "len = %u, imei:%s. \r\n", i, NB_CommPacket.Init_Data.imei);
								#endif
										//Ascii_To_Hex(p, NB_CommPacket.AuthData.imei, (uint16_t)asi_len); 
										set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								}
						}
								break;	
						
						case NB_AT_NCCID:
						{
								//save nccid
								p = strstr((char*)RxBuf, "NCCID:");
								#if SEGGER_RTT_DEBUG_MESS
										SEGGER_RTT_printf(0, "cmd nccid\r\n");
								#endif
								if(p != NULL)
								{
										for (i = 0; i < 32; i ++)
										{
												if (((p[6 + i] < '0') || (p[6 + i] > '9')) && 
														((p[6 + i] < 'a') || (p[6 + i] > 'z')) && 
														((p[6 + i] < 'A') || (p[6 + i] > 'Z')))
												{
														break;
												}
										}
										if (i < 32) p[6 + i] = 0x00;
										
										memcpy(NB_CommPacket.Init_Data.iccid, p+6, i);
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
						////// NBÄ£¿é×´Ì¬ÇÐ»» ///////////////
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;//ÇÐ»»µ½ÏÂÒ»¸öÁ÷³Ì
						g_stNB_Handler.StepPt = 0;     //clr step
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_READ_MESS;//¼ÌÐøµ±Ç°Á÷³Ì
				}   
		}
		//////////////////////// AT Ö¸Áî´ïµ½ÖØ·¢´ÎÊý //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)//ATÖ¸ÁîÖØ·¢´ÎÊýµ½ÁË
		{
				#if SEGGER_RTT_DEBUG_MESS
						SEGGER_RTT_printf(0, "Mess Reset\r\n");
				#endif
				g_stNB_Handler.State = NB_STATE_RESET;  //½øÐÐ¸´Î»
				g_stNB_Handler.StepPt = 0;//clr step
				NB_NetPar.NetType = NET_NO_NET;
		}
		//////////////////////// ATÖ¸ÁîÓ¦´ð´íÎó»ò³¬Ê± /////////////////////////
		else
		{
				//Ã»ÓÐµ½ATÖ¸ÁîÖØ·¢´ÎÊý£¬ËùÒÔÔÙ´Î·¢ËÍATÖ¸Áî
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//Ö±½ÓÔÙ´Î·¢ËÍATÖ¸Áî£¬È»ºóµÈ´ýÓ¦´ð
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
		/************************ATÖ¸ÁîÓ¦´ðÕýÈ·¡¤**************************/	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)/*ATÖ¸ÁîÓ¦´ðÄÚÈÝÕýÈ·*/
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
					
						case NB_AT_NMGS:
						{								
								if(UPLOAD_TYPE == g_stNB_Handler.ucSendType)
								{
										/*±¾´ÎÍ¨Ñ¶Íê³É,Çå³ý¶ÔÓ¦µÄÊÂ¼þ*/
										NB_EVENT_CLR(NB_EvtProc.ucUploadEvt,(0x01 << g_stNB_Handler.ucDataID));
//										//ÈçÔÚÕâÀïÖÃÆðÊÂ¼þ£¬ÔòÐèµÈµ½Ó¦´ð²ÅÄÜ·¢ËÍÏÂÒ»ÌõÖ÷¶¯ÉÏ´«ÏûÏ¢            
//										GPRS_Event_Set(GPRS_EvtProc.ucDelayEvt,(0x01 << g_stNB_Handler.ucDataID));     
//										APP_GPRS_RefreshRetryMess();
								}
							
								NB_EVENT_CLR(g_stNB_Handler.ucCommEvent,(0x01 << g_stNB_Handler.ucDataID));
								errCnt=0;           /*ATÖ¸Áî³É¹¦£¬Çå³ý´íÎó¼ÆÊý*/
								NB_TimProc.heartbeatCount = 0;      /*ÐÄÌø¼ÆÊýÇåÁã*/
						}
								break;	
						
						default:
								break;	
				}
			//next Step
			g_stNB_Handler.StepPt++;
			if(g_stNB_Handler.StepPt >= COMM_STEP_NUM)
			{
					/******GPRSÄ£¿é×´Ì¬ÇÐ»»***************/
					/*Êý¾Ý±êÇ©À´ÅÐ¶ÏÊÇ·ñ»¹ÓÐÆäËüÊý¾ÝÐèÒª·¢ËÍ£¬Èç¹ûÓÐÔòÔÙ´Î½øÈëcomm£¬Ã»ÓÐÔòÍË³öÍ¨Ñ¶×´Ì¬*/
					if(APP_NB_EvtTraverse(TRAVERSE) == false)
					{
							g_stNB_Handler.State = NB_STATE_IDLE;/*ÇÐ»»µ½ÏÂÒ»¸öÁ÷³Ì*/
							g_stNB_Handler.ucCommBusy= 0;/*Comm Complete*/
					}
					else
					{
							g_stNB_Handler.State = NB_STATE_COMM;/*¼ÌÐøµ±Ç°Á÷³Ì*/		
					}
					
					g_stNB_Handler.StepPt = 0;/**clr step*/
			}
			else
			{
					g_stNB_Handler.State = NB_STATE_COMM;/*¼ÌÐøµ±Ç°Á÷³Ì*/		
			}		
		}
/************************AT Ö¸Áî´ïµ½ÖØ·¢´ÎÊý**************************/	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)
		{
				if(NB_AT_NMGS == ATCmdIndex)
				{
						g_stNB_Handler.State = NB_STATE_COMM;
						errCnt++;
				}
//	    else if((GPRS_AT_MIPSEND_1 == ATCmdIndex) || (errCnt > 3))
//	    {
//            #if DEBUG_LOG
//                  BSP_LOG("Comm Reset\r\n");
//            #endif
//            errCnt=0;
//	        g_stGPRS_Handler.State = GPRS_STATE_RESET;
//	        GPRS_NetPar.NetType = NET_NO_NET;
//	    }
//	    
				g_stNB_Handler.StepPt = 0;//clr step
				g_stNB_Handler.ucCommBusy= 0;	

		}
/************************ATÖ¸ÁîÓ¦´ð´íÎó»ò³¬Ê±*************************/	
		else
		{
				//Ã»ÓÐµ½ATÖ¸ÁîÖØ·¢´ÎÊý£¬ËùÒÔÔÙ´Î·¢ËÍATÖ¸Áî
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//Ö±½ÓÔÙ´Î·¢ËÍATÖ¸Áî£¬È»ºóµÈ´ýÓ¦´ð
        NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stNB_Handler.StepPt];
		} 
}

/*Èç¹û·þÎñÆ÷ÏÂ·¢Êý¾Ý²»¼Ó±êÊ¶Í·£¬´Ëº¯Êý¿É·ÅÈëATÖ¸Áî½âÎöÖÐ*/

//U8 APP_GPRS_RxDataCheck(U8 *rxBuff)   
//{
//    U8 i=0;
//    U8 rxLen=rxBuff[10]+13;
//    if(rxBuff[0] != FRAME_HEAD)       return FALSE;  //Ö¡Í·Ð£Ñé
//    if(rxBuff[1] != 0x02)       return FALSE;        //²úÆ·ÀàÐÍÐ£Ñé
//    for(i=0;i<4;i++)
//    {
//        if(rxBuff[i+2] != PacketHead.addrRegion[i])  //µØÖ·ÓòÐ£Ñé
//            return FALSE;
//    }
//    if(rxBuff[rxLen-1] != FRAME_END)   return FALSE;
//    if(FALSE == Xor_Check(rxBuff,rxLen))             //ºÍÐ£Ñé
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
//         case WRITE_CTROL_CODE:             /*·þÎñÆ÷ÏÂ·¢Ð´Êý¾Ý°ü*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             switch(command)
//             {
//                 case SET_PARMTE_ID:       //ÉèÖÃ²ÎÊý
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
//                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//ÖÃÆð´æ´¢ÈÎÎñ
//                 }
//                 break;
//                 case WRITE_ADDR_ID:      //Ð´µØÖ·
//                 {
//                      for(i=0;i<4;i++)
//                      {
//                            PacketHead.addrRegion[i] = p_buf[i];
//                      }
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_WRITE_ADDR);
//                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//ÖÃÆð´æ´¢ÈÎÎñ
//                 }
//                 break;
//                 case SYS_PARMTE_SET_ID:  //ÏµÍ³²ÎÊýÉèÖÃ
//                 {
//                      Charge_ThresValue.PulloutTime     = (U16)((p_buf[0]<<8)+p_buf[1]);
//                      Charge_ThresValue.DeviationValue  = (U16)((p_buf[2]<<8)+p_buf[3]);
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_SYS_PARMTE_SET);
//                      Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_DATA);//ÖÃÆð´æ´¢ÈÎÎñ
//                 }
//                 break;
//                 default:
//                    break;
//             }
//         }
//         break;
//         case READ_CTROL_CODE:              /*·þÎñÆ÷ÏÂ·¢¶ÁÊý¾Ý°ü*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             switch(command)
//             {
//                 case QUERY_PARMTE_ID:      //²éÑ¯²ÎÊýÉèÖÃ
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_QUERY_PARMTE);
//                 }
//                 break;
//                 case READ_MESS_ID:         //¶ÁÐÅÏ¢
//                 {
//                       if(g_stGPRS_Handler.ucCommBusy == 0)
//                       {
//                            //ÕâÀïÔÙ¶ÁÒ»´ÎÐÅºÅÇ¿¶È
//                            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
//		                    g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //ÖØÐÂ¶ÁÈ¡ÐÅÏ¢
//		                    g_stGPRS_Handler.StepPt = 0;
//		               }
//		               GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_MESS);
//		               
//                 }
//                 break;
//                 case READ_ADDR_ID:         //¶ÁµØÖ·
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_ADDR);
//                 }
//                 break;
//                 case READ_STA_MESS_ID:         //¶ÁµØÖ·
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_STA_MESS);
//                 }
//                 break;
//                 default:
//                    break;
//             }
//         }
//         break;
//         case CONTROL_CTROL_CODE:           /*·þÎñÆ÷ÏÂ·¢¿ØÖÆ°ü*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             if(command == CONTROL_ID)      //¿ØÖÆ°ü
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
//                 Set_Task(MEM_WRITE,MEM_STORE_DYNAMIC_DATA);
//                 GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_CTROL);
//             }
//         }
//         break;
//         case UPLOAD_CTROL_CODE:            /*·þÎñÆ÷·µ»ØÖ÷¶¯ÉÏ´«°ü*/
//         {
//             PacketHead.TerminalMeageFlowNum++;
//             switch(command)
//             {
//                 case AUTHENTICA_ID:       //µÇÂ¼°ü·µ»Ø
//                 {
//                    //µÇÂ¼³É¹¦
//                    APP_GPRS_RetryMessInit();
//                    g_stGPRS_Handler.AuthStatus = AUTH;
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_AUTH); 
//                 }
//                 break;
//                 case STA_MESS_ID:         //×´Ì¬ÐÅÏ¢°ü·µ»Ø
//                 {
//                    APP_GPRS_RetryMessInit();
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_STA_MESS);
//                 }
//                 break;
//                 case HEARTBEAT_ID:       //ÐÄÌø°ü·µ»Ø
//                 {
//                    //ÐÄÌø°ü·¢ËÍ³É¹¦
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
//    if(false == NB_Event_IsEmpty(NB_EvtProc.ucRespondEvt))
//    {
//        if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucRespondEvt,&g_stNB_Handler.ucCommEvent);
//        return RESPOND_TYPE;
//    }
//    else if(false == NB_Event_IsEmpty(NB_EvtProc.ucDelayEvt))
//    {        
//        if(NB_Event_IsEmpty(NB_EvtProc.ucRetryEvt)== false)
//        {
//            if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucRetryEvt,&g_stNB_Handler.ucCommEvent);
//            return RTY_TYPE;
//        }
//        return 0;
//    }
//    else if(false == NB_Event_IsEmpty(NB_EvtProc.ucUploadEvt))
//    {
//        if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucUploadEvt,&g_stNB_Handler.ucCommEvent);
//        return UPLOAD_TYPE;
//    }
	
    if(false == NB_EVENT_IS_EMPTY(NB_EvtProc.ucUploadEvt))
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucUploadEvt, &g_stNB_Handler.ucCommEvent);
        return UPLOAD_TYPE;
    }
    else if(false == NB_EVENT_IS_EMPTY(g_stNB_Handler.ucCommEvent))
    {
        //ÀíÂÛÉÏ³ÌÐòÎÞ·¨ÔËÐÐµ½ÕâÀï
        g_stNB_Handler.ucCommEvent = 0;
    #if SEGGER_RTT_DEBUG_NB86
        SEGGER_RTT_printf(0, "Event is Error \r\n");
    #endif
        return 0;
    }
    else
    {
        g_stNB_Handler.State = NB_STATE_IDLE;  //ÇÐ»»ÖÁIDLEÌ¬
        return 0;
    }
}

static void APP_NB_Reset_Init(void)
{                 
    //ÑÓÊ±ÊÂ¼þÌí¼ÓÖÁÖ÷¶¯ÉÏ´«ÊÂ¼þÖÐ
    //Ðè°ÑÑÓÊ±ÊÂ¼þÈ«²¿Ìí¼ÓÖÁÖ÷¶¯ÉÏ´«ÊÂ¼þÖÐ
//    APP_NB_EventProc(NB_EvtProc.ucDelayEvt, &NB_EvtProc.ucUploadEvt);  
    g_stNB_Handler.AuthStatus = NOT_AUTH;
    NB_EVENT_SET(NB_EvtProc.ucUploadEvt, COMM_EVENT_INIT);
//    NB_EvtProc.ucRespondEvt =0;
//    NB_EvtProc.ucRetryEvt =0;
//    NB_EvtProc.ucDelayEvt =0;
    g_stNB_Handler.ucCommEvent = 0;
    NB_NetPar.NetType = NET_NO_NET;           //ÎÞÍøÂç×´Ì¬Á¬½Ó
    NB_NetPar.NetConnSta = CONN_OFF;
    APP_NB_RetryMessInit();                   //ÖØ´«Ïà¹Ø±äÁ¿³õÊ¼»¯
}

//Ä£¿éÍ¨Ñ¶Ïà¹Ø±äÁ¿³õÊ¼»¯
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
//		BSP_NB_RESET_SET
//		while(1)
//		{}
		NB_EVENT_SET(NB_EvtProc.ucUploadEvt, COMM_EVENT_INIT);//ÉÏµçÐè·¢ËÍÒ»¸öµÇÂ¼°ü
		set_task(COMM, COMM_STATE_PROC);     //Æô¶¯GPRS×´Ì¬´¦ÀíÈÎÎñ
}

void APP_NB_TimeProc(uint16_t usPeriod)
{
    NB_msCount += usPeriod;
//    if(GPRS_ATCmdCB.Busy == 1)   //ÔÚÊý¾Ý½ÓÊÕÊ±£¬ATÖ¸ÁîÒ²Ã¦Âµ£¬Õâ±ß¿ÉÄÜ»áÓÐ³åÍ»£¬°Ñ½ÓÊÕÊ±£¬Ã¦Âµ±êÖ¾È¥³ý
//	{
		/*********ATÖ¸ÁîÓ¦´ð³¬Ê±**************/
		if(NB_ATCmdCB.ulTimeOut >= usPeriod)
		{
				NB_ATCmdCB.ulTimeOut -= usPeriod;
				if(NB_ATCmdCB.ulTimeOut < usPeriod)
				{
						NB_ATCmdCB.ATCmdResult = BACK_TIMEOUT;/*³¬Ê±*/ 
				}
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
		/***************** NB Ä£¿éÑÓÊ±¼ÆÊ ý*************************/
		if(g_stNB_Handler.ulDelayCnt >= usPeriod)    
		{
				g_stNB_Handler.ulDelayCnt -= usPeriod;
		}
    else
    {
        g_stNB_Handler.ulDelayCnt = 0;
    }
		 /*****************NB Ä£¿é×´Ì¬´¦Àíº¯Êý*************************/	
		g_stNB_Handler.ulProcCtrTaskPer += usPeriod;
		if(g_stNB_Handler.ulProcCtrTaskPer >= SUB_TASK_PROCCTR_PER)
		{
				g_stNB_Handler.ulProcCtrTaskPer -= SUB_TASK_PROCCTR_PER;
				set_task(COMM, COMM_STATE_PROC);
		}	
		if(NB_msCount >= NB_MS_TO_S)   //ms to S
		{
				NB_msCount =0;
//				/**************** NB Ä£¿éÖØ´«´¦Àí*************************/
//				if(NB_RetryCtrol.rtycntEn != DISABLE)
//				{
//						NB_RetryCtrol.rtyTimCount++;
//						if(NB_RetryCtrol.rtyTimCount >= NB_RetryCtrol.rtyTime)
//						{
//								NB_RetryCtrol.rtyTimCount=0;
//								NB_RetryCtrol.rtycntEn = DISABLE;
//								APP_NB_EventProc(NB_EvtProc.ucDelayEvt,&NB_EvtProc.ucRetryEvt);
//						}
//				}
				/***************** ÐÄÌø°ü´¦Àí *************************/
				if((NB_NetPar.NetConnSta   == CONN_ON) && (AUTH == g_stNB_Handler.AuthStatus))  /*ÐÄÌø°ü*/
				{
						NB_TimProc.heartbeatCount++;
						if(NB_TimProc.heartbeatCount >= HEARTBEAT_TIME)
						{
								NB_TimProc.heartbeatCount = 0;
//								NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_HEARTBEAT);
						}
				}
		}
}


/*Ä£¿éATÖ¸Áî½âÎöÈÎÎñ*/
void APP_SubTask_CmdProc(void)
{
		#if SEGGER_RTT_DEBUG_CMDPROC
//			SEGGER_RTT_printf(0, "APP SubTask CmdProc !\r\n");
		#endif
    nrf_uart_disable(NRF_UART0);
//		disableInterrupts();
    NB_ATCmdCB.Busy = 0;   
    NB_ATCmdCB.ucRxLen = NB_ATCmdCB.ucRxCnt;
    NB_ATCmdCB.ucRxCnt = 0;
    NB_ATCmdCB.RxFrameOk = 0;
    memcpy(g_stNB_Handler.RxBuf,NB_ATCmdCB.RxBuf,NB_ATCmdCB.ucRxLen);
    nrf_uart_enable(NRF_UART0);
//		enableInterrupts();

    NB_ATCmdCB.ATCmdResult = APP_GPRS_ATCmdAckCheck(g_stNB_Handler.RxBuf); 
    ////////// Ó¦´ð´íÎóÖ±½Ó·µ»Ø /////////////////////// 
    if(BACK_ERROR == NB_ATCmdCB.ATCmdResult) 
    {
        return;
    }
    ////////// Ó¦´ðATÖ¸ÁîÖ±½Ó·µ»Ø,Í¨¹ýAPP_SubTask_StateProc ´¦Àí ///
    if(RSP_TYPE_CMD == NB_ATCmdCB.RspType)
    {
        return;
    }
    /////////// ¶Ô·þÎñÆ÷µÄÏÂÐÐÊý¾Ý½øÐÐÐ­Òé½âÎöºÍÏàÓ¦´¦Àí ////////////////
    if(RSP_TYPE_DATA == NB_ATCmdCB.RspType)
    {
        set_task(COMM,COMM_DECODE);
        #if SEGGER_RTT_DEBUG_NB86
						SEGGER_RTT_printf(0, "START DECODE\r\n");
        #endif
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
						//Èç¹ûÃ»ÓÐ½á¹û¾ÍÖ±½Ó·µ»Ø
						if(NO_BACK == NB_ATCmdCB.ATCmdResult)
						{
								return;	
						}  
						//ATÓ¦´ð³¬Ê±£¬»òÕßATÖ¸ÁîÓ¦´ð½âÎö´íÎó£¬ÔòÅÐ¶ÏÊÇ·ñÐèÒªÖØ·¢
						if((NB_ATCmdCB.ATCmdResult == BACK_ERROR) || (NB_ATCmdCB.ATCmdResult == BACK_TIMEOUT))  
						{
								if(NB_ATCmdCB.ucResendCnt >= NB_ATCmdCB.ucResendTimes)
								{	
										NB_ATCmdCB.ATCmdResult = BACK_PERTIMES_OUT;//ÖØ·¢´ÎÊýµ½ÁË
								}			
						}
						//¸ù¾Ý²»Í¬µÄ½á¹û£¬×öÏàÓ¦´¦Àí
						if(APP_NB_StateProcTbl[g_stNB_Handler.StateOld] != NULL)
						{
								APP_NB_StateProcTbl[g_stNB_Handler.StateOld](g_stNB_Handler.RxBuf);		
						}
						if((BACK_TRUE == NB_ATCmdCB.ATCmdResult) || (BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult))/*ATÖ¸ÁîÓ¦´ðÄÚÈÝÕýÈ·*/
						{
								//¸üÐÂStateOld
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
								BSP_NB_RESET_CLEAR
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "clear nb poweron and reset !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 1200;      //À­µÍ1.2s
						}
            else if(1 == g_stNB_Handler.StepPt)
						{
								BSP_NB_POWERON_SET;
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "set nb poweron !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 100;  //À­¸ß10ms
								LED_Status = CONN_INDI;            //ÕýÔÚÁªÍøLEDÖ¸Ê¾
						}
						else if(2 == g_stNB_Handler.StepPt)
						{
								BSP_NB_RESET_SET;
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "set nb reset !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 7000; //µÈ´ý50msºóÔÙ²Ù×÷NBÄ£¿é
						}
						else//next state
						{
								//Ò»Ð©±äÁ¿Çå0
						#if SEGGER_RTT_DEBUG_POWER_ON
								SEGGER_RTT_printf(0, "next state init !\r\n");
						#endif
                NB_NetPar.NetType = NET_NO_NET;//ÎÞÍøÂç×´Ì¬Á¬½Ó
								g_stNB_Handler.StateOld = NB_STATE_POWER_ON;	
								g_stNB_Handler.State = NB_STATE_INIT;//¿ªÊ¼ÏÂÒ»¸öÁ÷³Ì
								g_stNB_Handler.StepPt = 0;	    
						}
				}
						break;
         ///////////// ¸Ã×´Ì¬ÎªGPRSÄ£¿éPOWER_OFF£¬ËùÒÔÖ´ÐÐÍê¸ÃÁ÷³ÌºóÐèPOWER_ON /////////////
         // Èç¸ÃÁ÷³ÌÖ±½ÓÍê³ÉÄ£¿éµÄ¸´Î»£¬ÄÇÃ´Ö´ÐÐÍê¸ÃÁ÷³Ìºó,¿É¿ªÊ¼ATÖ¸ÁîÁ÷³Ì //
				case NB_STATE_RESET:  
        {
						if(g_stNB_Handler.ulDelayCnt > 0)
            {
								return;
            }
            if(0 == g_stNB_Handler.StepPt)
						{
								//Òý½Å²Ù×÷
								BSP_NB_RESET_CLEAR;
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 200; //À­µÍ200ms
								LED_Status = SYS_INDI;     //ÏµÍ³LEDÖ¸Ê¾
						}
						else if(1 == g_stNB_Handler.StepPt)
						{
								//Òý½Å²Ù×÷
								BSP_NB_RESET_SET;
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 50;
						}
						else//next state
						{
								//Ò»Ð©±äÁ¿Çå0
                NB_NetPar.NetType = NET_NO_NET;               //ÎÞÍøÂç×´Ì¬Á¬½Ó
								g_stNB_Handler.StateOld = NB_STATE_RESET;	
								g_stNB_Handler.State = NB_STATE_POWER_ON;   //¿ªÊ¼ÏÂÒ»¸öÁ÷³Ì
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
								// Õâ±ßÌí¼Ó¸´Î»´ÎÊý¼ÆÊý£¬Èç³¬¹ýÉè¶¨¸´Î»´ÎÊý£¬ÄÇÃ´ÔÝÊ±²»½øÐÐ¶ÔGPRSµÄ²Ù×÷ //
								//// ¾ßÌåÊµÏÖ¿É¹Ø±Õ¶¨Ê±Æ÷ÀïÃæµÄÖÜÆÚ×´Ì¬´¦Àíº¯Êý /////
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
								g_stNB_Handler.ulDelayCnt = 20000; //20S NB AT×¼±¸¾ÍÐ÷³¬Ê±Ê±¼ä
						}
						else
						{
						#if SEGGER_RTT_DEBUG_INIT
								SEGGER_RTT_printf(0, "init nb has start !\r\n");
						#endif
								g_stNB_Handler.ulDelayCnt = 0;  //¼ÆÊýÇå0
								task_flag_struct.atReday_F = DISABLE; //AT¾ÍÐ÷Ê§ÄÜ
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
								g_stNB_Handler.ulDelayCnt = 20000; //20S GPRS AT×¼±¸¾ÍÐ÷³¬Ê±Ê±¼ä
						}
						else
						{
						#if SEGGER_RTT_DEBUG_MESS
								SEGGER_RTT_printf(0, "mess nb has start !\r\n");
						#endif
								g_stNB_Handler.ulDelayCnt = 0;  //¼ÆÊýÇå0
								task_flag_struct.atReday_F = DISABLE; //AT¾ÍÐ÷Ê§ÄÜ
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
								g_stNB_Handler.ucCommBusy = 1;    //±ê¼ÇÕýÔÚºÍ·þÎñÆ÷Í¨Ñ¶
//								LED_Status = DATA_SEND_INDI;    //ÕýÔÚÍ¨Ñ¶LEDÖ¸Ê¾
						}
            
						if(NB_AT_NMGS == s_ATCmdStep_Comm[g_stNB_Handler.StepPt]) 
						{   
								//APP_NB_WriteReturnIDPacketÓëAPP_NB_WriteDataIDPacketº¯Êý¿ÉÒÔºÏ¶þÎªÒ»
								//µ«¿¼ÂÇµ½ºóÐøÏîÄ¿Ð­Òé¿ÉÄÜÔö¶à£¬×îÖÕ»¹ÊÇµÃ°ÑÖ÷¶¯ÉÏ´«ºÍÓ¦´ðÏûÏ¢·Ö¿ª
								switch(g_stNB_Handler.ucSendType)
								{
//										case RESPOND_TYPE:
//										{
//												g_stNB_Handler.ucDataID = Get_CommEvent(NB_EvtProc.ucRespondEvt);
//												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteReturnIDPacket(g_stNB_Handler.ucDataID, g_stNB_Handler.TxBuf);
//										}
//												break;
//										
//										case RTY_TYPE:
//										{
//												g_stNB_Handler.ucDataID = Get_CommEvent(NB_EvtProc.ucRetryEvt);
//										#if CONFIG_RETRY_COPY
//												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteRetryIDPacket(g_stNB_Handler.TxBuf);
//										#else
//												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteDataIDPacket(g_stNB_Handler.ucDataID, g_stNB_Handler.TxBuf);
//										#endif
//										}
//												break;
//										
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
            //´ËÊ±ÖØ·¢¼ÆÊýÎª3£¬²¢ÇÒÎªµÚÒ»ÌõÖ¸Áî¡£ÒòÎªµÚ¶þÌõÖ¸Áî»á°ÑÖØ´«¼ÆÊý¼Ó1
						if((NB_RetryCtrol.rtycnt >= RTY_TIMES_MAX) && (g_stNB_Handler.StepPt ==0))   
						{
								g_stNB_Handler.StateOld = NB_STATE_RESET;   //¸´Î»GPRSÄ£¿é
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
						//ÅÐ¶ÏÊÇ·ñÓÐÊý¾ÝÐèÒªÍ¨Ñ¶
						if(APP_NB_EvtTraverse(TRAVERSE) != false)
						{
								//ÓÐÊý¾ÝÒªÍ¨Ñ¶ÔòÉèÖÃÇÐ»»µ½COM×´Ì¬µÄÊÂ¼þ
								NB_State_Event_Set(STATE_Event_COMM);
						}
									
						if(NB_IsSwitch_State())  //ÏÖ²»×ö´¦Àí£¬ºóÐø¿ÉÓÃ×öµÍ¹¦ºÄ
						{
								break;
						}
						
						//»ñÈ¡ÏÂÒ»¸öÓÅÏÈ¼¶¸ßµÄ×´Ì¬
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
//    if(APP_GPRS_RxDataCheck(TropeBuf) == FALSE)  return;   //±ê¼Ç´íÎóÂë£¬´ýÌí¼Ó
//    APP_GPRS_RxDataDecode(TropeBuf);
//}


