#include "app_comm_NB86.h"
#include "nrf_drv_gpiote.h"

static uint16_t NB_msCount = 0;

uint8_t                                 g_Rx_Buffer[BUFFSIZE] = {0};
uint8_t                                 g_Tx_Buffer[BUFFSIZE] = {0};

NB_Handler        g_stNB_Handler;
PACKET_HEAD_Type  PacketHead;

typedef void(*NB_State_Fun)(uint8_t *RxBuf); /* ����״̬������*/
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


/******************GPRSָ�**********************************/
const NB_Cmd_Data_Type AT_CmdTbl[] = 
{
    /*------cmdstr-----------------------------------revtimeout(s)--cmdtruebackstr----------retry_times */
    /************************* GPRS  network ***************************/
      {"AT+CFUN=0\r\n",                                  3,       "OK",                     3},
      {"AT+CFUN?\r\n",                                   3,       "CFUN:0",                 3},
			{"AT+NCDP=180.101.147.115,5683\r\n",               3,       "OK",                     3},
			{"AT+NRB\r\n",                                     3,       "REBOOTING",              3},
			
			
      {"AT+CFUN=0\r\n",                                  3,       "OK",                     3},
      {"AT+CFUN?\r\n",                                   3,       "CFUN:0",                 3},
			{"AT+CGSN=1\r\n",                                  3,       "CGSN",                   3},
			{"AT+NCSEARFCN\r\n",                               3,       "OK",                     3},
			{"AT+NCONFIG=CELL_RESELECTION,TRUE\r\n",           3,       "OK",                     3},
			{"AT+CMEE=1\r\n",                                  3,       "OK",                     3},
			{"AT+CFUN=1\r\n",                                  3,       "OK",                     3},
			{"AT+CFUN?\r\n",                                   3,       "CFUN:1",                 3},
			{"AT+NCCID\r\n",                                   3,       "NCCID",                  3},
			{"AT+CGDCONT=1,\"IP\",\"ctnet\"\r\n",              3,       "OK",                     3},
			 
			{"AT+CGATT=1\r\n",                                 3,       "OK",                     3},
			{"AT+CGATT?\r\n",                                  30,      "CGATT:1",               15},	
      {"AT+CFUN=0\r\n",                                  3,       "OK",                     3},
      {"AT+CFUN?\r\n",                                   3,       "CFUN:0",                 3},
			{"AT+NCONFIG=CELL_RESELECTION,FALSE\r\n",          3,       "OK",                     3},
			{"AT+CPSMS=1,,,01000011,01000011\r\n",             3,       "OK",                     3},
			{"AT+CEDRXS=0,5,0101\r\n",                         3,       "OK",                     3},
			{"AT+NPSMR=1\r\n",                                 3,       "OK",                     3},
			{"AT+CFUN=1\r\n",                                  3,       "OK",                     3},
			{"AT+CFUN?\r\n",                                   3,       "CFUN:1",                 3},
			{"AT+CGATT=1\r\n",                                 3,       "OK",                     3},
			{"AT+CGATT?\r\n",                                  30,      "CGATT:1",               15},	
			{"AT+CGPADDR\r\n",                                 30,      "OK",                    15},	
      {"AT+CSQ\r\n",   			                             3,	      "+CSQ",         	        3},
			
			
      {"AT+NMGS=37,00\r\n",   			                     3,	      "OK",         	          3},
      {"AT+NQMGS\r\n",   			                           3,	      "OK",         	          3},
			
			{"AT+CSQ\r\n",   			                             3,	      "+CSQ",         	        3},
      {"AT+NMGS=29,01\r\n",   			                     3,	      "OK",         	          3},
      {"AT+NQMGS\r\n",   			                           3,	      "OK",         	          3},
			
      {"AT+NMGR\r\n",   			                           3,	      "",           	          3},
      {"AT+NQMGR\r\n",   			                           3,	      "OK",         	          3},
			{"AT+CGATT?\r\n",                                  30,      "CGATT:1",               15},
			{"AT+NPSMR?\r\n",                                  30,      "NPSMR:1,1",             15},


//      {"AT+CPIN?\r\n",                                   3,       "READY",                  3},
      {"AT+CGREG?\r\n",   			                     		 3,	      "+CGREG: 0,1",         	 15},

//      {"AT+MIPKEEPCONF=1,240,50,4\r\n",   			         3,	      "OK",         	          3},  /*ȥ��TCP�ײ���������*/
/*������Ϊ0ʱ��ԭʼ�����ʽ(1�ֽ�ת2�ֽ�)*/
/*������Ϊ1ʱ����ת��ֱ��������Ҳ�����ʶͷ*/
/*������Ϊ2ʱ����ת��ֱ�����������ʶͷ*/
      {"AT+GTSET=\"IPRFMT\",2\r\n",   			             3,	      "OK",         	          3},   //��ѡ��

/*������������*/
      {"AT+MIPCALL=1,\"CMNET\"\r\n",                     30,      "+MIPCALL:",              3},
      {"AT+MIPCALL?\r\n",                                3,       "+MIPCALL: 1",            3},
      {"AT+MIPOPEN=1,,\"120.77.174.116\",9292,0\r\n",    20,      "+MIPOPEN: 1,1",          3},
      {"AT+MIPOPEN?\r\n",                                3,       "+MIPOPEN: 1",            2},
/* ���ݷ���*/
      {"AT+MIPSEND=1\r\n",                               10,      ">",                      2},
      {"DATA",                                           6,       "OK",                     1},  //��Ҫ���͵����ݣ����Ż�
      
      
      {"AT+CSQ\r\n",   			                             3,	      "+CSQ",         	        3},  //���Ż�
      {"AT+CCID\r\n",                                    4,       "898",                    2},
      {"AT+CGSN\r\n",                                    4,       "OK",                     2},

      {NULL,                                          NULL,       "+MIPRTCP",            NULL},
      
};

/*************************��ʼ������***************************/
static const uint8_t s_ATCmdStep_Init[] = 
{
    NULL,
    NB_AT_CFUN_0_A,      
		NB_AT_CFUN1_A,      
		NB_AT_NCDP,                    
};

/************************* TCP/IP������·��������***************************/
static const uint8_t s_ATCmdStep_Connnect[] = 
{
    NB_AT_MIPCALL_1, NB_AT_MIPCALL1, NB_AT_MIPOPEN_1, //NB_AT_MIPOPEN1,                               
};

/*************************���ݷ�������***************************/
static const uint8_t s_ATCmdStep_Comm[] = 
{
    NB_AT_MIPSEND_1, NB_AT_DATA,                             
};

static const uint8_t s_ATCmdStep_Mess[] = 
{
	  NB_AT_CFUN_0_B, 
		NB_AT_CFUN1_B,  
		NB_AT_CGSN_1,
	                     
};


/*У��ATָ��Ӧ�����Ƿ���ȷ*/
static uint8_t APP_GPRS_ATCmdAckCheck(uint8_t *RxBuf)
{   
    //�ж��Ƿ��Ƿ�������������
    if(Check_Strstr((char*)RxBuf, (char*)(char*)AT_CmdTbl[NB_AT_MIPRTCP].ExpResultStr, NB_ATCmdCB.ucRxLen) == true)
    {
        NB_ATCmdCB.RspType = RSP_TYPE_DATA;  //ͨ������
        return BACK_TRUE;   //���յ���ȷӦ��
    }
    if(Check_Strstr((char*)RxBuf, "Lierda", NB_ATCmdCB.ucRxLen) == true) //GPRSģ��׼����������
    {
        Task_Flag_Struct.atReday_F = ENABLE;
        NB_ATCmdCB.RspType = RSP_TYPE_CMD;
	    return BACK_TRUE;
    }

    if(NB_ATCmdCB.ATCmdPt != NB_AT_MIPOPEN_1)//socket����ʱ,Ҳ�п��ܻظ�MIPSTAT: 1,1
    {
        if(Check_Strstr((char*)RxBuf, "MIPSTAT: 1,1", NB_ATCmdCB.ucRxLen) == true)
        {
            NB_NetPar.NetConnSta = CONN_OFF;
            g_stNB_Handler.State = NB_STATE_CONNECT_NET;    //��������socket
						g_stNB_Handler.StepPt = 2;
						g_stNB_Handler.AuthStatus = NOT_AUTH;
						NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
        }
    }
    if(NB_ATCmdCB.ATCmdPt != NB_AT_MIPCALL1) //����IP��ȡ����Ҳ��ȥ��ѯ����IP
    {
        if(Check_Strstr((char*)RxBuf, "MIPCALL: 0", NB_ATCmdCB.ucRxLen) == true)
        
        { 
            NB_NetPar.NetConnSta = CONN_OFF;
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;    //������������
						g_stNB_Handler.StepPt = 0;
						g_stNB_Handler.AuthStatus = NOT_AUTH;
						NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
        }
    }
    
     //�жϽ��յ������Ƿ�������Ӧ��
		if(Check_Strstr((char*)RxBuf, NB_ATCmdCB.ExpResultStr, NB_ATCmdCB.ucRxLen) == true)
		{
				if(NB_ATCmdCB.ATCmdPt == NB_AT_CFUN1_A)
				{
						if(Check_Strstr((char*)RxBuf, "OK", 20) != true)   //ȥ����һ������
						{
								#if SEGGER_RTT_DEBUG_NB86_CFUN
										SEGGER_RTT_printf(0, "at+cfun_A? error !\r\n");
										SEGGER_RTT_printf(0, (char*)RxBuf);
								#endif	
								return BACK_ERROR;   //��һ��"OK"������ȷ�����Լ����ȴ�
						}
						#if SEGGER_RTT_DEBUG_NB86_CFUN
								SEGGER_RTT_printf(0, "at+cfun_A? ok !\r\n");
						#endif
				}
				if(NB_ATCmdCB.ATCmdPt == NB_AT_CFUN1_B)
				{
						if(Check_Strstr((char*)RxBuf, "OK", 20) != true)   //ȥ����һ������
						{
								#if SEGGER_RTT_DEBUG_NB86_CFUN
										SEGGER_RTT_printf(0, "at+cfun_B? error !\r\n");
										SEGGER_RTT_printf(0, (char*)RxBuf);
								#endif	
								return BACK_ERROR;   //��һ��"OK"������ȷ�����Լ����ȴ�
						}
						#if SEGGER_RTT_DEBUG_NB86_CFUN
								SEGGER_RTT_printf(0, "at+cfun_B? ok !\r\n");
						#endif
				}
				NB_ATCmdCB.RspType = RSP_TYPE_CMD;//Ӧ��ATָ��
				return BACK_TRUE;                 //���յ���ȷӦ��
		} 
    else
		{   
				//���ڻ�ȡ������ַ�ͽ���soctet���Ӷ��Ƿ��������صģ���һ����Ϊ"OK"
				if((NB_ATCmdCB.ATCmdPt == NB_AT_MIPCALL_1) || NB_ATCmdCB.ATCmdPt == NB_AT_MIPOPEN_1)
				{
						if(Check_Strstr((char*)RxBuf, "OK", 4) == true)   //ȥ����һ������
						{
								return NO_BACK;   /*��һ��"OK"������ȷ�����Լ����ȴ�*/
						}
				}
			  //��ѯ�Ƿ�פ���ɹ����粻�ɹ�����ô�ȴ�3S�������Ƿ��ش��󣬸���ָ������
				if(NB_ATCmdCB.ATCmdPt == NB_AT_CGREG1) 
				{
						return NO_BACK;
				}
			return BACK_ERROR;   		
		}	
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
//           GPRS_SysStaMess.chargeTime = g_stCharge_Handler.ChargeTimCnt;    //���ʱ��
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
//static U16 APP_GPRS_GetPacket(u16 messID,U8 ctrolcode,U8 datalen,U8* Packet)
//{
//    U8 i=0,j=0;
//    U8 check=0;
//    U8 ipLen=0;
//    U8 apnLen=0;
//    U8 fillZeroLen=0;
//    U8 newLen=0;
//    U8 TxBuf[GPRS_RX_BUFF_LEN]={0};
//#if DEBUG_RTY_FUN
//    static U8 rtyTestCnt=0;
//#endif
//   
//    TxBuf[i++] = FRAME_HEAD;
//    TxBuf[i++] = 0x02;    //��Ʒ����
//    for(j=0;j<ADDR_REGION_LEN;j++)
//    {
//       TxBuf[i++] = PacketHead.addrRegion[j]&0xff;
//    }
//    TxBuf[i++] = ctrolcode&0xff;
//    TxBuf[i++] = datalen&0xff;
//    TxBuf[i++] = (messID>>8)&0xff;
//    TxBuf[i++] = messID&0xff;
//    switch(messID)
//    {
//       case AUTHENTICA_ID:
//       {
//           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
//           for(j=0;j<6;j++)
//           {
//               TxBuf[i++] = GPRS_CommPacket.AuthData.iccid[j+4];
//           }
//           
//       }
//       break;
//       
//       case STA_MESS_ID:
//       {
//           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
//           GPRS_CommPacket.MessRead.DeviceState.staOfCharge = Charge_Flag.ChargeStatus;
//           GPRS_CommPacket.MessRead.DeviceState.PhoneOnLineSta = Charge_Flag.PhoneOnLineStatus;
//           APP_GPRS_GetSysStaMessPacket(GPRS_CommPacket.MessRead.DeviceState.staOfCharge,&i,TxBuf);
//       }
//       
//       break;
//       case HEARTBEAT_ID:
//       {
//           TxBuf[i++] = PacketHead.TerminalMeageFlowNum;
//           //����������Ϊ��
//       }
//       
//       break;

//       case SET_PARMTE_ID:
//       {
//           TxBuf[i++] = PacketHead.AckMeageFlowNum;
//           //������������Ϊ��
//       }
//       break;
//       case QUERY_PARMTE_ID:
//       {
//            TxBuf[i++] = PacketHead.AckMeageFlowNum;
//            ipLen  = strlen((char *)GPRS_NetPar.ServerIp);
//            apnLen = strlen((char *)GPRS_NetPar.ServerApn);
//            for(j=0;j<ipLen;j++)
//            {
//                TxBuf[i++] = (GPRS_NetPar.ServerIp[j]) & 0xff;
//            }
//            fillZeroLen = SERVER_IP_LEN-ipLen;
//            for(j=0;j<fillZeroLen;j++)        //Ϊ������SERVER_IP_LEN(17)���ȵ��ֽ�
//            {
//                TxBuf[i++] = 0;
//            }  
//            
//            TxBuf[i++] = (GPRS_NetPar.ServerPort>>8) & 0xff;
//            TxBuf[i++] = (GPRS_NetPar.ServerPort) & 0xff;
//            for(j=0;j<apnLen;j++)
//            {
//                TxBuf[i++] = GPRS_NetPar.ServerApn[j] & 0xff;
//            }
//            fillZeroLen = SERVER_APN_LEN - apnLen;
//            for(j=0;j<fillZeroLen;j++)
//            {
//                TxBuf[i++] = 0;
//            } 
//       }
//       break;
//       case READ_MESS_ID:
//       {
//            GPRS_CommPacket.MessRead.DeviceState.staOfCharge = Charge_Flag.ChargeStatus;
//            GPRS_CommPacket.MessRead.DeviceState.PhoneOnLineSta = Charge_Flag.PhoneOnLineStatus;
//            TxBuf[i++] = PacketHead.AckMeageFlowNum;
//            TxBuf[i++] = (GPRS_CommPacket.MessRead.DeviceState.state>>8) & 0xff;
//            TxBuf[i++] = (GPRS_CommPacket.MessRead.DeviceState.state) & 0xff;
//            TxBuf[i++] = (GPRS_CommPacket.MessRead.CSQ) & 0xff;
//            TxBuf[i++] = (GPRS_CommPacket.MessRead.hardwareVerLen) & 0xff;
//            for(j=0;j<GPRS_CommPacket.MessRead.hardwareVerLen;j++)
//            {
//                TxBuf[i++] = (GPRS_CommPacket.MessRead.hardwareVer[j]) & 0xff;
//            }
//            TxBuf[i++] = (GPRS_CommPacket.MessRead.softwareVerLen) & 0xff;
//            for(j=0;j<GPRS_CommPacket.MessRead.softwareVerLen;j++)
//            {
//                TxBuf[i++] = (GPRS_CommPacket.MessRead.softwareVer[j]) & 0xff;
//            }
//       }
//       break;
//       case CONTROL_ID:
//       {
//           TxBuf[i++] = PacketHead.AckMeageFlowNum;
//            //������������Ϊ��
//       }
//       break;
//       case WRITE_ADDR_ID:
//       {
//          TxBuf[i++] = PacketHead.AckMeageFlowNum;
//           //������������Ϊ��
//       }
//       break;
//       case READ_ADDR_ID:
//       {
//          TxBuf[i++] = PacketHead.AckMeageFlowNum;
//          for(j=0;j<4;j++)
//          {
//               TxBuf[i++] = (PacketHead.addrRegion[j]) & 0xff;
//          }
//       }
//       break;
//       case READ_STA_MESS_ID:
//       {
//          TxBuf[i++] = PacketHead.AckMeageFlowNum;
//          GPRS_CommPacket.MessRead.DeviceState.staOfCharge = Charge_Flag.ChargeStatus;
//          GPRS_CommPacket.MessRead.DeviceState.PhoneOnLineSta = Charge_Flag.PhoneOnLineStatus;
//          APP_GPRS_GetSysStaMessPacket(GPRS_CommPacket.MessRead.DeviceState.staOfCharge,&i,TxBuf);
//       }
//       break;
//       case SYS_PARMTE_SET_ID:
//       {
//          TxBuf[i++] = PacketHead.AckMeageFlowNum;
//           //������������Ϊ��
//       }
//       break;
//    }
//    check = Get_Xor(TxBuf,i); //����У���
//    TxBuf[i++] = check&0xff;
//#if DEBUG_RTY_FUN
//    rtyTestCnt++;
//    if(rtyTestCnt <=2)
//    {
//        for(j=0;j<3;j++)
//        {
//            TxBuf[i++]=j;
//        }
//    } 
//    if((rtyTestCnt <=8) && (rtyTestCnt >=4))
//    {
//         for(j=0;j<3;j++)
//         {
//              TxBuf[i++]=j;
//         }
//    }
//#endif     
//    TxBuf[i++] = FRAME_END;
//    APP_GPRS_Encrypt(TxBuf,i,Packet,&newLen);
//    return   (U16)newLen;
//    
//}

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
//�ش�ʱҲ�����������ֹ��һ�����ݳ�����ô������������Ҳ�����
//�����ش��漰��ʱ�����ݣ���ô������ݱ��棬�������ʱ�ٷ���
//���ش�ʱ�������������Ѻ�CONFIG_RETRY_COPY��λ
    uint16_t usPacketLen=0;
#if SEGGER_RTT_DEBUG_NB86
		BSP_LOG("retry send mess\r\n");
#endif
    memcpy(Packet,GPRS_RetryCtrol.rtyBuff,GPRS_RetryCtrol.rtyLen);    //�ش���Ϣcopy�����ͻ���
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

/*���ݲ�ͬ������ID,������֡����װЭ����֡����*/
static uint16_t APP_NB_WriteDataIDPacket(uint8_t DataID, uint8_t* Packet)
{
    uint16_t usPacketLen = 0;
		switch(DataID)
		{    
//				case UPLOAD_ID_AUTH:
//				{
//						usPacketLen = APP_GPRS_GetPacket(AUTHENTICA_ID,AUTH_CTROL_CODE,AUTH_LEN,Packet);
//				#if CONFIG_RETRY_COPY            
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //��Ϣ���ȱ���
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //��Ϣ����   
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
//						GPRS_RetryCtrol.rtyLen = usPacketLen;                   //��Ϣ���ȱ���
//						memcpy(GPRS_RetryCtrol.rtyBuff,Packet,usPacketLen);     //��Ϣ����   
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
//    port = DecimalToASCII(GPRS_NetPar.ServerPort,data); //�˿ں�ת��֧��1--65535
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
//    //�˿ں�ת��������ӡ��Ϣ
//    
//}

static uint8_t APP_NB_WriteATPacket(uint8_t ATCmdIndex, uint8_t *pFrameDes)
{  
    uint8_t ucSendLen;
    uint8_t dataNum;
    char *endStr="\x1A\r\n";
		switch(ATCmdIndex)
		{
//				case NB_AT_MIPOPEN_1:/*����socket����*/
//				{
//						strcpy((char*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
//						dataNum = Get_Comma(2, (uint8_t*)pFrameDes);
//						APP_GPRS_SetIP(&pFrameDes[dataNum]);			    
//						ucSendLen = strlen((char*)pFrameDes);
//					
//				}break;
//				
//				case NB_AT_DATA:/*�����������*/
//				{
//						memcpy(pFrameDes, g_stNB_Handler.TxBuf, g_stNB_Handler.ucTxLen);/*copy Ӧ�ò�����*/
//						ucSendLen = strlen(endStr);
//						memcpy(pFrameDes+g_stNB_Handler.ucTxLen,endStr,ucSendLen);        /*copy ��β��*/
//						ucSendLen += g_stNB_Handler.ucTxLen;
//					
//				}break;
				
				default:
				{
						strcpy((char*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
						ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);//Ĭ�ϵ�ָ�������
					
				}break;
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
			return ;//��ǰæµ״̬
		}
		//���ݲ�ͬ��ATӦ����,�����ط�����
		if((BACK_TRUE == pAtCmd->ATCmdResult) || (BACK_PERTIMES_OUT == pAtCmd->ATCmdResult))
		{
			ucResendCnt = 0;//�ط�����
		}
		else
		{
				ucResendCnt = NB_ATCmdCB.ucResendCnt;//�����ط�����
		}

		memset((uint8_t*)pAtCmd, 0, sizeof(NB_ATCmdCB));
		
		pAtCmd->Busy         = 1;          //����æ��״̬
		pAtCmd->ATCmdPt      = ATCmdIndex; //����AT������
		pAtCmd->ATCmdResult  = NO_BACK;
		pAtCmd->ucResendCnt  = ucResendCnt;  
		pAtCmd->ucResendTimes = AT_CmdTbl[ATCmdIndex].ucResendTimes;
		pAtCmd->ulTimeOut    = TmrSecondsToMilliseconds(AT_CmdTbl[ATCmdIndex].usTimeOut);//ˢ�³�ʱʱ��

		////// ���ָ�����ݳ��� //////
		pAtCmd->ucCmdStrLen = APP_NB_WriteATPacket(ATCmdIndex, pAtCmd->CmdStr);
		//���ָ����������	
		strcpy(pAtCmd->ExpResultStr, AT_CmdTbl[ATCmdIndex].ExpResultStr);
		//����ATָ��
		NB_SendATCmdStr(pAtCmd->CmdStr, pAtCmd->ucCmdStrLen);
		//���ʹ���+1
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
		//////////////////////// ATָ��Ӧ����ȷ ///////////////////////////	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//ATָ��Ӧ��������ȷ
		{
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= INIT_STEP_NUM)
				{
						////// GPRSģ��״̬�л� ///////////////
						g_stNB_Handler.State = NB_STATE_READ_MESS;//�л�����һ������
						g_stNB_Handler.StepPt = 0;     //clr step
						NB_NetPar.NetType = NET_NB;//����״̬״̬
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_INIT;//������ǰ����		
				}	
		}
		//////////////////////// AT ָ��ﵽ�ط����� //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)//ATָ���ط���������
		{
				switch(ATCmdIndex)
				{
						default:
						{
								#if SEGGER_RTT_DEBUG_NB86
										SEGGER_RTT_printf(0, "Init Reset\r\n");
								#endif
								g_stNB_Handler.State = NB_STATE_RESET;       //���и�λ
								g_stNB_Handler.StepPt = 0;//clr step
								NB_NetPar.NetType = NET_NO_NET;
						}
							break;	
				}
		}
		//////////////////////// ATָ��Ӧ������ʱ /////////////////////////	
		else
		{
				//û�е�ATָ���ط������������ٴη���ATָ��
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��
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
		//////////////////////// ATָ��Ӧ����ȷ ///////////////////////////	
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//ATָ��Ӧ��������ȷ
		{	           
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= CONNECT_STEP_NUM)
				{
						////// GPRSģ��״̬�л� ///////////////
						g_stNB_Handler.State = NB_STATE_READ_MESS;//�л�����һ������
						g_stNB_Handler.StepPt = 0;//clr step
						NB_NetPar.NetConnSta = CONN_ON;
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;//������ǰ����
				}			
		}
		//////////////////////// AT ָ��ﵽ�ط����� //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)
		{
				switch(ATCmdIndex)
				{
						case NB_AT_MIPCALL_1:   //ֱ�ӿ�ʼ��һ��ָ��
						{
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.State = NB_STATE_CONNECT_NET; //������ǰ����
								
						}
								break;
						
						default:
						{
								#if DEBUG_LOG
										BSP_LOG("Connect Reset\r\n");
								#endif
								g_stNB_Handler.State = NB_STATE_RESET;       //���и�λ
								g_stNB_Handler.StepPt = 0;//clr step
								NB_NetPar.NetType = NET_NO_NET;
						}
								break;	
				}	
		}
		//////////////////////// ATָ��Ӧ������ʱ /////////////////////////	
		else
		{
				//û�е�ATָ���ط������������ٴη���ATָ��
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��
				NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd	
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stNB_Handler.StepPt];
		}	
}

static void APP_NB_State_Mess_Proc(uint8_t *RxBuf)
{
		uint8_t ATCmdIndex;
		uint8_t asi_len=0;
		char *str = "898";
		char *p   = NULL;
		if(g_stNB_Handler.StepPt >= MESS_STEP_NUM)
		{
				return; 
		}   
		ATCmdIndex = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
		//////////////////////// ATָ��Ӧ����ȷ ///////////////////////////	 
		if(BACK_TRUE == NB_ATCmdCB.ATCmdResult)//ATָ��Ӧ��������ȷ
		{
				switch(ATCmdIndex)
				{
						case NB_AT_CGSN_1:
						{
								//save cgsn
								NB_CommPacket.MessRead.CSQ = Get_Uint_Data((char*)&RxBuf[Get_Comma(1, RxBuf)] - 3);

						}
								break;	
						
//						case NB_AT_CCID:
//						{
//								//��ȡICCID
//								p=strstr((char*)RxBuf,str);
//								asi_len = strlen(p);
//								if(asi_len > 20) asi_len = 20;    
//								Ascii_To_Hex(p, NB_CommPacket.AuthData.iccid, (uint16_t)asi_len); 
//						
//						}
								break;	
				
						default:
								break;
				}
				//next Step
				g_stNB_Handler.StepPt++;
				if(g_stNB_Handler.StepPt >= MESS_STEP_NUM)
				{
						////// GPRSģ��״̬�л� ///////////////
						g_stNB_Handler.State = NB_STATE_CONNECT_NET;//�л�����һ������
						g_stNB_Handler.StepPt = 0;     //clr step
				}
				else
				{
						g_stNB_Handler.State = NB_STATE_READ_MESS;//������ǰ����
				}   
		}
		//////////////////////// AT ָ��ﵽ�ط����� //////////////////////////	
		else if(BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult)//ATָ���ط���������
		{
				#if SEGGER_RTT_DEBUG_NB86
						SEGGER_RTT_printf(0, "Mess Reset\r\n");
				#endif
				g_stNB_Handler.State = NB_STATE_RESET;  //���и�λ
				g_stNB_Handler.StepPt = 0;//clr step
				NB_NetPar.NetType = NET_NO_NET;
		}
		//////////////////////// ATָ��Ӧ������ʱ /////////////////////////
		else
		{
				//û�е�ATָ���ط������������ٴη���ATָ��
				g_stNB_Handler.State = NB_STATE_WAIT_ACK;//ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��
				NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
				NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
		}
}

static void APP_NB_State_Comm_Proc(uint8_t *RxBuf)
{
//    U8 ATCmdIndex;  
//    static U8 errCnt=0;
//	if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
//	{
//		return;	
//	} 
//	ATCmdIndex = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
///************************ATָ��Ӧ����ȷ��**************************/	
//	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*ATָ��Ӧ��������ȷ*/
//	{
//		switch(ATCmdIndex)
//		{
//			case GPRS_AT_DATA:
//            {
//				
//				if(RESPOND_TYPE == g_stGPRS_Handler.ucSendType)
//				{
//				    GPRS_Event_Clr(GPRS_EvtProc.ucRespondEvt,(0x01 << g_stGPRS_Handler.ucDataID));
//				}
//				else if(RTY_TYPE == g_stGPRS_Handler.ucSendType)
//				{
//				    GPRS_Event_Clr(GPRS_EvtProc.ucRetryEvt,(0x01 << g_stGPRS_Handler.ucDataID));
//		            APP_GPRS_RefreshRetryMess();
//				}
//				else if(UPLOAD_TYPE == g_stGPRS_Handler.ucSendType)
//				{
//				    /*����ͨѶ���,�����Ӧ���¼�*/
//				    GPRS_Event_Clr(GPRS_EvtProc.ucUploadEvt,(0x01 << g_stGPRS_Handler.ucDataID));
//				    //�������������¼�������ȵ�Ӧ����ܷ�����һ�������ϴ���Ϣ            
//				    GPRS_Event_Set(GPRS_EvtProc.ucDelayEvt,(0x01 << g_stGPRS_Handler.ucDataID));     
//                    APP_GPRS_RefreshRetryMess();
//				}
//				GPRS_Event_Clr(g_stGPRS_Handler.ucCommEvent,(0x01 << g_stGPRS_Handler.ucDataID));
//		        errCnt=0;           /*ATָ��ɹ�������������*/
//		        GPRS_TimProc.heartbeatCount = 0;      /*������������*/
//			}break;	
//			
//			default:
//				break;	
//		}
//		/*next Step*/
//		g_stGPRS_Handler.StepPt++;
//		if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
//		{
//			/******GPRSģ��״̬�л�***************/
//			/*���ݱ�ǩ���ж��Ƿ�������������Ҫ���ͣ���������ٴν���comm��û�����˳�ͨѶ״̬*/
//			if(APP_GPRS_EvtTraverse(TRAVERSE) == FALSE)
//			{
//                g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*�л�����һ������*/
//				g_stGPRS_Handler.ucCommBusy= 0;/*Comm Complete*/
//			}
//			else
//			{
//				g_stGPRS_Handler.State = GPRS_STATE_COMM;/*������ǰ����*/		
//			}
//			
//			g_stGPRS_Handler.StepPt = 0;/**clr step*/
//		}
//		else
//		{
//		    g_stGPRS_Handler.State = GPRS_STATE_COMM;/*������ǰ����*/		
//		}		
//	}
///************************AT ָ��ﵽ�ط�����**************************/	
//	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
//	{
//	    if(GPRS_AT_DATA == ATCmdIndex)
//	    {
//	        g_stGPRS_Handler.State = GPRS_STATE_COMM;
//	        errCnt++;
//	    }
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
//		g_stGPRS_Handler.StepPt = 0;/**clr step*/
//		g_stGPRS_Handler.ucCommBusy= 0;	

//	}
///************************ATָ��Ӧ������ʱ*************************/	
//	else
//	{
//		/*û�е�ATָ���ط������������ٴη���ATָ��*/
//		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*ֱ���ٴη���ATָ�Ȼ��ȴ�Ӧ��*/
//        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
//		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
//	} 
}
/*����������·����ݲ��ӱ�ʶͷ���˺����ɷ���ATָ�������*/

//U8 APP_GPRS_RxDataCheck(U8 *rxBuff)   
//{
//    U8 i=0;
//    U8 rxLen=rxBuff[10]+13;
//    if(rxBuff[0] != FRAME_HEAD)       return FALSE;  //֡ͷУ��
//    if(rxBuff[1] != 0x02)       return FALSE;        //��Ʒ����У��
//    for(i=0;i<4;i++)
//    {
//        if(rxBuff[i+2] != PacketHead.addrRegion[i])  //��ַ��У��
//            return FALSE;
//    }
//    if(rxBuff[rxLen-1] != FRAME_END)   return FALSE;
//    if(FALSE == Xor_Check(rxBuff,rxLen))             //��У��
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
//         case WRITE_CTROL_CODE:             /*�������·�д���ݰ�*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             switch(command)
//             {
//                 case SET_PARMTE_ID:       //���ò���
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
//                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//����洢����
//                 }
//                 break;
//                 case WRITE_ADDR_ID:      //д��ַ
//                 {
//                      for(i=0;i<4;i++)
//                      {
//                            PacketHead.addrRegion[i] = p_buf[i];
//                      }
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_WRITE_ADDR);
//                      Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);//����洢����
//                 }
//                 break;
//                 case SYS_PARMTE_SET_ID:  //ϵͳ��������
//                 {
//                      Charge_ThresValue.PulloutTime     = (U16)((p_buf[0]<<8)+p_buf[1]);
//                      Charge_ThresValue.DeviationValue  = (U16)((p_buf[2]<<8)+p_buf[3]);
//                      GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_SYS_PARMTE_SET);
//                      Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_DATA);//����洢����
//                 }
//                 break;
//                 default:
//                    break;
//             }
//         }
//         break;
//         case READ_CTROL_CODE:              /*�������·������ݰ�*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             switch(command)
//             {
//                 case QUERY_PARMTE_ID:      //��ѯ��������
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_QUERY_PARMTE);
//                 }
//                 break;
//                 case READ_MESS_ID:         //����Ϣ
//                 {
//                       if(g_stGPRS_Handler.ucCommBusy == 0)
//                       {
//                            //�����ٶ�һ���ź�ǿ��
//                            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
//		                    g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //���¶�ȡ��Ϣ
//		                    g_stGPRS_Handler.StepPt = 0;
//		               }
//		               GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_MESS);
//		               
//                 }
//                 break;
//                 case READ_ADDR_ID:         //����ַ
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_ADDR);
//                 }
//                 break;
//                 case READ_STA_MESS_ID:         //����ַ
//                 {
//                       GPRS_Event_Set(GPRS_EvtProc.ucRespondEvt,COMM_Event_READ_STA_MESS);
//                 }
//                 break;
//                 default:
//                    break;
//             }
//         }
//         break;
//         case CONTROL_CTROL_CODE:           /*�������·����ư�*/
//         {
//             PacketHead.AckMeageFlowNum = flowNum;
//             if(command == CONTROL_ID)      //���ư�
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
//         case UPLOAD_CTROL_CODE:            /*���������������ϴ���*/
//         {
//             PacketHead.TerminalMeageFlowNum++;
//             switch(command)
//             {
//                 case AUTHENTICA_ID:       //��¼������
//                 {
//                    //��¼�ɹ�
//                    APP_GPRS_RetryMessInit();
//                    g_stGPRS_Handler.AuthStatus = AUTH;
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_AUTH); 
//                 }
//                 break;
//                 case STA_MESS_ID:         //״̬��Ϣ������
//                 {
//                    APP_GPRS_RetryMessInit();
//                    GPRS_Event_Clr(GPRS_EvtProc.ucDelayEvt,COMM_Event_STA_MESS);
//                 }
//                 break;
//                 case HEARTBEAT_ID:       //����������
//                 {
//                    //���������ͳɹ�
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
    if(NB_Event_IsEmpty(surEvt)== false)
    {
          ucID = Get_CommEvent(surEvt);
          NB_Event_Set(*objEvt,(1<<ucID));
    }
}

static uint8_t APP_NB_EvtTraverse(uint8_t mode)
{    
    if(NB_Event_IsEmpty(NB_EvtProc.ucRespondEvt) == false)
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucRespondEvt,&g_stNB_Handler.ucCommEvent);
        return RESPOND_TYPE;
    }
    else if(NB_Event_IsEmpty(NB_EvtProc.ucDelayEvt)== false)
    {        
        if(NB_Event_IsEmpty(NB_EvtProc.ucRetryEvt)== false)
        {
            if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucRetryEvt,&g_stNB_Handler.ucCommEvent);
            return RTY_TYPE;
        }
        return 0;
    }
    else if(NB_Event_IsEmpty(NB_EvtProc.ucUploadEvt)== false)
    {
        if(mode == TRAVERSE_AND_ADDEVT)  APP_NB_EventProc(NB_EvtProc.ucUploadEvt,&g_stNB_Handler.ucCommEvent);
        return UPLOAD_TYPE;
    }
    else if(NB_Event_IsEmpty(g_stNB_Handler.ucCommEvent)== false)
    {
        //�����ϳ����޷����е�����
        g_stNB_Handler.ucCommEvent = 0;
        #if SEGGER_RTT_DEBUG_NB86
             SEGGER_RTT_printf(0, "Event is Error \r\n");
        #endif
        return 0;
    }
    else
    {
        g_stNB_Handler.State = NB_STATE_IDLE;  //�л���IDLE̬
        return 0;
    }
}

static void APP_NB_Reset_Init(void)
{                 
    //��ʱ�¼�����������ϴ��¼���
    //�����ʱ�¼�ȫ������������ϴ��¼���
    APP_NB_EventProc(NB_EvtProc.ucDelayEvt, &NB_EvtProc.ucUploadEvt);  
    g_stNB_Handler.AuthStatus = NOT_AUTH;
    NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);
    NB_EvtProc.ucRespondEvt =0;
    NB_EvtProc.ucRetryEvt =0;
    NB_EvtProc.ucDelayEvt =0;
    g_stNB_Handler.ucCommEvent = 0;
    NB_NetPar.NetType = NET_NO_NET;           //������״̬����
    NB_NetPar.NetConnSta = CONN_OFF;
    APP_NB_RetryMessInit();                   //�ش���ر�����ʼ��
}

void APP_COMM_Init(void)
{    
#if DEBUG_TROPE_FUN
    PacketHead.TerminalMeageFlowNum = 0x19;
#endif
	g_stNB_Handler.State = NB_STATE_POWER_ON; 
	g_stNB_Handler.StateOld = NB_STATE_POWER_ON; 
	g_stNB_Handler.StepPt = 0;
	g_stNB_Handler.AuthStatus = NOT_AUTH;
	NB_Event_Set(NB_EvtProc.ucUploadEvt, COMM_Event_AUTH);//�ϵ��跢��һ����¼��
	Set_Task(COMM, COMM_STATE_PROC);     //����GPRS״̬��������
}



void APP_NB_TimeProc(uint16_t usPeriod)
{
    NB_msCount += usPeriod;
//    if(GPRS_ATCmdCB.Busy == 1)   //�����ݽ���ʱ��ATָ��Ҳæµ����߿��ܻ��г�ͻ���ѽ���ʱ��æµ��־ȥ��
//	{
		/*********ATָ��Ӧ��ʱ**************/
		if(NB_ATCmdCB.ulTimeOut >= usPeriod)
		{
				NB_ATCmdCB.ulTimeOut -= usPeriod;
				if(NB_ATCmdCB.ulTimeOut < usPeriod)
				{
						NB_ATCmdCB.ATCmdResult = BACK_TIMEOUT;/*��ʱ*/ 
				}
		}
//	}
		if(NB_ATCmdCB.ucByteTimeOut >= usPeriod)
		{
				NB_ATCmdCB.ucByteTimeOut -= usPeriod;
				if(NB_ATCmdCB.ucByteTimeOut < usPeriod)
				{
						NB_ATCmdCB.RxFrameOk = 1;
						Set_Task(COMM, COMM_CMD_PROC);
				}
		}
		/*****************GPRSģ����ʱ����*************************/
		if(g_stNB_Handler.ulDelayCnt >= usPeriod)    
		{
				g_stNB_Handler.ulDelayCnt -= usPeriod;
		}
    else
    {
        g_stNB_Handler.ulDelayCnt = 0;
    }
		 /*****************CPRS ģ��״̬������*************************/	
		g_stNB_Handler.ulProcCtrTaskPer += usPeriod;
		if(g_stNB_Handler.ulProcCtrTaskPer >= SUB_TASK_PROCCTR_PER)
		{
				g_stNB_Handler.ulProcCtrTaskPer -= SUB_TASK_PROCCTR_PER;
				Set_Task(COMM, COMM_STATE_PROC);
		}	
		if(NB_msCount >= NB_MS_TO_S)   //ms to S
		{
				NB_msCount =0;
				/*****************CPRS ģ���ش�����*************************/
				if(NB_RetryCtrol.rtycntEn != DISABLE)
				{
						NB_RetryCtrol.rtyTimCount++;
						if(NB_RetryCtrol.rtyTimCount >= NB_RetryCtrol.rtyTime)
						{
								NB_RetryCtrol.rtyTimCount=0;
								NB_RetryCtrol.rtycntEn = DISABLE;
								APP_NB_EventProc(NB_EvtProc.ucDelayEvt,&NB_EvtProc.ucRetryEvt);
						}
				}
				/*****************����������*************************/
				if((NB_NetPar.NetConnSta   == CONN_ON) && (AUTH == g_stNB_Handler.AuthStatus))  /*������*/
				{
						NB_TimProc.heartbeatCount++;
						if(NB_TimProc.heartbeatCount >= HEARTBEAT_TIME)
						{
								NB_TimProc.heartbeatCount = 0;
								NB_Event_Set(NB_EvtProc.ucUploadEvt,COMM_Event_HEARTBEAT);
						}
				}
		}
}


/*ģ��ATָ���������*/
void APP_SubTask_CmdProc(void)
{
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
    ////////// Ӧ�����ֱ�ӷ��� /////////////////////// 
    if(BACK_ERROR == NB_ATCmdCB.ATCmdResult) 
    {
        return;
    }
    ////////// Ӧ��ATָ��ֱ�ӷ���,ͨ��APP_SubTask_StateProc ���� ///
    if(RSP_TYPE_CMD == NB_ATCmdCB.RspType)
    {
        return;
    }
    /////////// �Է��������������ݽ���Э���������Ӧ���� ////////////////
    if(RSP_TYPE_DATA == NB_ATCmdCB.RspType)
    {
        Set_Task(COMM,COMM_DECODE);
        #if DEBUG_LOG
					BSP_LOG("START DECODE\r\n");
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
						//���û�н����ֱ�ӷ���
						if(NO_BACK == NB_ATCmdCB.ATCmdResult)
						{
								return;	
						}  
						//ATӦ��ʱ������ATָ��Ӧ������������ж��Ƿ���Ҫ�ط�
						if((NB_ATCmdCB.ATCmdResult == BACK_ERROR) || (NB_ATCmdCB.ATCmdResult == BACK_TIMEOUT))  
						{
								if(NB_ATCmdCB.ucResendCnt >= NB_ATCmdCB.ucResendTimes)
								{	
										NB_ATCmdCB.ATCmdResult = BACK_PERTIMES_OUT;//�ط���������
								}			
						}
						//���ݲ�ͬ�Ľ��������Ӧ����
						if(APP_NB_StateProcTbl[g_stNB_Handler.StateOld] != NULL)
						{
								APP_NB_StateProcTbl[g_stNB_Handler.StateOld](g_stNB_Handler.RxBuf);		
						}
						if((BACK_TRUE == NB_ATCmdCB.ATCmdResult) || (BACK_PERTIMES_OUT == NB_ATCmdCB.ATCmdResult))/*ATָ��Ӧ��������ȷ*/
						{
								//����StateOld
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
                 // G510 Power on���̣�
                 // ___         ___
                 //    |_______|
                 //     800ms
                 //   ��·ת�� ��->�ͣ���->��
								BSP_NB_POWERON_SET;
						#if SEGGER_RTT_DEBUG_NB86
								SEGGER_RTT_printf(0, "set nb poweron !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 10;      //����10ms
								//LED_Status = CONN_INDI;            //��������LEDָʾ
						}
						else if(1 == g_stNB_Handler.StepPt)
						{
								BSP_NB_RESET_SET;
						#if SEGGER_RTT_DEBUG_NB86
								SEGGER_RTT_printf(0, "set nb reset !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 500; //�ȴ�1S���ٲ���GPRSģ��
						}
						else//next state
						{
								//һЩ������0
						#if SEGGER_RTT_DEBUG_NB86
								SEGGER_RTT_printf(0, "next state init !\r\n");
						#endif
                NB_NetPar.NetType = NET_NO_NET;//������״̬����
								g_stNB_Handler.StateOld = NB_STATE_POWER_ON;	
								g_stNB_Handler.State = NB_STATE_INIT;//��ʼ��һ������
								g_stNB_Handler.StepPt = 0;	    
						}
				}
						break;
         ///////////// ��״̬ΪGPRSģ��POWER_OFF������ִ��������̺���POWER_ON /////////////
         // �������ֱ�����ģ��ĸ�λ����ôִ��������̺�,�ɿ�ʼATָ������ //
				case NB_STATE_RESET:  
        {
						if(g_stNB_Handler.ulDelayCnt > 0)
            {
								return;
            }
            if(0 == g_stNB_Handler.StepPt)
						{
								//���Ų���
								BSP_NB_RESET_RESET;
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 200; //����3.5S
//								LED_Status = SYS_INDI;     //ϵͳLEDָʾ
						}
						else if(1 == g_stNB_Handler.StepPt)
						{
								//���Ų���
								BSP_NB_RESET_SET;
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 100;
						}
						else//next state
						{
								//һЩ������0
                NB_NetPar.NetType = NET_NO_NET;               //������״̬����
								g_stNB_Handler.StateOld = NB_STATE_RESET;	
								g_stNB_Handler.State = NB_STATE_POWER_ON;   //��ʼ��һ������
								g_stNB_Handler.StepPt = 0;	
								APP_NB_Reset_Init();
								ResetTimesCount++;
								if(ResetTimesCount >=3)
								{
										ResetTimesCount = 0;
										Task_Flag_Struct.closeStaProc = ENABLE;
								#if SEGGER_RTT_DEBUG_NB86
										SEGGER_RTT_printf(0, "Close State Proc\r\n");
								#endif
								}
              // �����Ӹ�λ�����������糬���趨��λ��������ô��ʱ�����ж�GPRS�Ĳ��� //
              //// ����ʵ�ֿɹرն�ʱ�����������״̬������ /////
						}
        }
						break;
				 
				case NB_STATE_INIT:
        {
						if((g_stNB_Handler.ulDelayCnt > 0) && (Task_Flag_Struct.atReday_F == DISABLE))
            {
								return;
            }
            if(0 == g_stNB_Handler.StepPt)
						{
						#if SEGGER_RTT_DEBUG_NB86
								SEGGER_RTT_printf(0, "wait nb start !\r\n");
						#endif
								g_stNB_Handler.StepPt++;
								g_stNB_Handler.ulDelayCnt = 20000; //20S GPRS AT׼��������ʱʱ��
						}
						else
						{
						#if SEGGER_RTT_DEBUG_NB86
								SEGGER_RTT_printf(0, "nb has start !\r\n");
						#endif
								g_stNB_Handler.ulDelayCnt = 0;  //������0
								Task_Flag_Struct.atReday_F = DISABLE; //AT����ʧ��
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
            g_stNB_Handler.StateOld = g_stNB_Handler.State; 
						g_stNB_Handler.State = NB_STATE_WAIT_ACK;
						NB_ATCmdCB.SendATCmdEn = 1;//enable send ATCmd
						NB_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stNB_Handler.StepPt];
        }
						break;
				
        case NB_STATE_COMM:
        {
            g_stNB_Handler.ucSendType = APP_NB_EvtTraverse(TRAVERSE_AND_ADDEVT);
            if((false == g_stNB_Handler.ucSendType) || (NB_Event_IsEmpty(g_stNB_Handler.ucCommEvent)))
            {
                break;
            }

						if(0 == g_stNB_Handler.StepPt)
						{
								g_stNB_Handler.ucCommBusy = 1;    //������ںͷ�����ͨѶ
//								LED_Status = DATA_SEND_INDI;    //����ͨѶLEDָʾ
						}
            
						if(NB_AT_DATA == s_ATCmdStep_Comm[g_stNB_Handler.StepPt]) 
						{   
								//APP_NB_WriteReturnIDPacket��APP_NB_WriteDataIDPacket�������Ժ϶�Ϊһ
								//�����ǵ�������ĿЭ��������࣬���ջ��ǵð������ϴ���Ӧ����Ϣ�ֿ�
								switch(g_stNB_Handler.ucSendType)
								{
										case RESPOND_TYPE:
										{
												g_stNB_Handler.ucDataID = Get_CommEvent(NB_EvtProc.ucRespondEvt);
												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteReturnIDPacket(g_stNB_Handler.ucDataID, g_stNB_Handler.TxBuf);
										}
												break;
										
										case RTY_TYPE:
										{
												g_stNB_Handler.ucDataID = Get_CommEvent(NB_EvtProc.ucRetryEvt);
										#if CONFIG_RETRY_COPY
												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteRetryIDPacket(g_stNB_Handler.TxBuf);
										#else
												g_stNB_Handler.ucTxLen = (uint8_t)APP_NB_WriteDataIDPacket(g_stNB_Handler.ucDataID, g_stNB_Handler.TxBuf);
										#endif
										}
												break;
										
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
            //��ʱ�ط�����Ϊ3������Ϊ��һ��ָ���Ϊ�ڶ���ָ�����ش�������1
						if((NB_RetryCtrol.rtycnt >= RTY_TIMES_MAX) && (g_stNB_Handler.StepPt ==0))   
						{
								g_stNB_Handler.StateOld = NB_STATE_RESET;   //��λGPRSģ��
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
						//�ж��Ƿ���������ҪͨѶ
						if(APP_NB_EvtTraverse(TRAVERSE) != false)
						{
								//������ҪͨѶ�������л���COM״̬���¼�
								NB_State_Event_Set(STATE_Event_COMM);
						}
									
						if(NB_IsSwitch_State())  //�ֲ������������������͹���
						{
								break;
						}
						
						//��ȡ��һ�����ȼ��ߵ�״̬
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
//    if(APP_GPRS_RxDataCheck(TropeBuf) == FALSE)  return;   //��Ǵ����룬�����
//    APP_GPRS_RxDataDecode(TropeBuf);
//}


