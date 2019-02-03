#ifndef __APP_COMM_NB86_H
#define __APP_COMM_NB86_H

#include "global.h"


#define BUFFSIZE                        256

#define TRAVERSE_AND_ADDEVT 1
#define TRAVERSE            0   

#define AUTH                1
#define NOT_AUTH            0
/* Bit defined */
#define BIT0			(0x01)
#define BIT1			(0x02)
#define BIT2			(0x04)
#define BIT3			(0x08)
#define BIT4			(0x10)
#define BIT5			(0x20)
#define BIT6			(0x40)
#define BIT7			(0x80)
///*************֡ͷ/֡β*************/
//#define FRAME_HEAD       0x7f
//#define FRAME_END        0xf7

//#define TRANSFER_FLAG       0x1b
//#define TRANSFER_CHAR1      0x1a
//#define TRANSFER_CHAR1_FLAG 0x01
//#define TRANSFER_CHAR2      0x1b
//#define TRANSFER_CHAR2_FLAG 0x02

///***********************************************************************/
///*********************�������·�������************************/
///*�������7bit ������    0:�����->�ն�,1:�ն�->�����*/

//#define WRITE_CTROL_CODE              0x20 
//#define READ_CTROL_CODE               0x21
//#define CONTROL_CTROL_CODE            0x22
//#define UPLOAD_CTROL_CODE             0x23    

////#define RETURN_WRITE_CTROL_CODE       0xa0   
////#define RETURN_READ_CTROL_CODE        0xa1
////#define RETURN_CONTROL_CTROL_CODE     0xa2
////#define RETURN_UPLOAD_CTROL_CODE      0xa3

///*****************************************************************/

/************************�������Ϣ**********************************/
//��ʼ������Ϣ
#define INIT_ID                         0x00

//��־����Ϣ
#define LOG_ID                          0x01
//��¼�������Ϣ
//#define AUTHENTICA_ID                   0xb010
//#define AUTH_CTROL_CODE                 0xa3
//#define AUTH_LEN                        0x09

////״̬��Ϣ�ϴ��������Ϣ
//#define STA_MESS_ID                     0xb041
//#define STA_MESS_CTROL_CODE             0xa3
//#define STA_MESS_LEN                    0x0D

////�����������Ϣ
//#define HEARTBEAT_ID                    0xb040
//#define HEARTBEAT_CTROL_CODE            0xa3
//#define HEARTBEAT_LEN                   0x03


////�������÷��ذ�
//#define SET_PARMTE_ID                   0xb020
//#define SET_PARMTE_CTROL_CODE           0xa0
//#define SET_PARMTE_LEN                  0x03

////��ѯ������
//#define QUERY_PARMTE_ID                 0xb021
//#define QUERY_PARMTE_CTROL_CODE         0xa1
//#define QUERY_PARMTE_LEN                0x1d

////����Ϣ��
//#define READ_MESS_ID                    0xb022
//#define READ_MESS_CTROL_CODE            0xa1
//#define READ_MESS_LEN                   0x10

////���ư�����
//#define CONTROL_ID                      0xb027
//#define CTROL_CODE                      0xa2
//#define CTROL_LEN                       0x03

////д��ַ���ذ�
//#define WRITE_ADDR_ID                   0xb024
//#define WRITE_ADDR_CTROL_CODE           0xa0
//#define WRITE_ADDR_LEN                  0x03

////����ַ���ذ�
//#define READ_ADDR_ID                    0xb025
//#define READ_ADDR_CTROL_CODE            0xa1
//#define READ_ADDR_LEN                   0x07

////��״̬��Ϣ��
//#define READ_STA_MESS_ID                0xb026
//#define READ_STA_MESS_CTROL_CODE        0xa1
//#define READ_STA_MESS_LEN               0x0D

////ϵͳ��������
//#define SYS_PARMTE_SET_ID               0xb028
//#define SYS_PARMTE_SET_CTROL_CODE       0xa0
//#define SYS_PARMTE_SET_LEN              0x03

/*****************************************************************/


/***********�������������������*************************/
#define SUB_TASK_PROCCTR_PER          (10U) /*10ms*/


/*******Ĭ�������������*********/	 
#define NET_PROTOCOL_DEFAULT        NET_PROTOCOL_TCP
#define SERVER_APN_DEFAULT          "ctnb"
#define SERVER_IP_DEFAULT           "180.101.147.115"
#define SERVER_PORT_DEFAULT         (9292U)
#define REPORT_PERIOD_DEFAULT       (30U)


//#define SERVER_PORT_LEN      (6)

///*********************BUFF LEN***********************/
//#define GPRS_RTY_MESS_LEN         (40)
#define NB_RX_BUFF_LEN          (100)


#define NB_SendATCmdStr(pTxBuff, usLen)	        myuart_send(pTxBuff, usLen)


/*switch state*/
#define NB_IsSwitch_State()                       !MY_READ_REG(g_stNB_Handler.ucStateEvent)
#define NB_State_Event_Get(Event)                 MY_READ_BIT(g_stNB_Handler.ucStateEvent, Event)
#define NB_State_Event_Set(Event)                 MY_SET_BIT(g_stNB_Handler.ucStateEvent, Event)
#define NB_State_Event_Clr(Event)                 MY_CLEAR_BIT(g_stNB_Handler.ucStateEvent, Event)
//#if 0
///*Comm Event*/
//#define GPRS_Comm_Event_IsEmpty()                  !MY_READ_REG(g_stGPRS_Handler.ucCommEvent)
//#define GPRS_Comm_Event_Get(Event)                 MY_READ_BIT(g_stGPRS_Handler.ucCommEvent, Event)
//#define GPRS_Comm_Event_Set(Event)                 MY_SET_BIT(g_stGPRS_Handler.ucCommEvent, Event)
//#define GPRS_Comm_Event_Clr(Event)                 MY_CLEAR_BIT(g_stGPRS_Handler.ucCommEvent, Event)

///*Comm Return Event*/
//#define GPRS_Comm_Return_Event_IsEmpty()                  !MY_READ_REG(g_stGPRS_Handler.ucReturnEvent)
//#define GPRS_Comm_Return_Event_Get(Event)                 MY_READ_BIT(g_stGPRS_Handler.ucReturnEvent, Event)
//#define GPRS_Comm_Return_Event_Set(Event)                 MY_SET_BIT(g_stGPRS_Handler.ucReturnEvent, Event)
//#define GPRS_Comm_Return_Event_Clr(Event)                 MY_CLEAR_BIT(g_stGPRS_Handler.ucReturnEvent, Event)


///*Comm Retry  Event*/
//#define GPRS_Retry_Event_IsEmpty()                  !MY_READ_REG(g_stGPRS_Handler.RetryCtrol.rtyEvent)
//#define GPRS_Retry_Event_Get(Event)                 MY_READ_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
//#define GPRS_Retry_Event_Set(Event)                 MY_SET_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
//#define GPRS_Retry_Event_Clr(Event)                 MY_CLEAR_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
//#endif

#define NB_EVENT_IS_EMPTY(Event)                  !MY_READ_REG(Event)
#define NB_EVENT_GET(Event,Bit)                   MY_READ_BIT(Event, Bit)
#define NB_EVENT_SET(Event,Bit)                   MY_SET_BIT(Event, Bit)
#define NB_EVENT_CLR(Event,Bit)                   MY_CLEAR_BIT(Event, Bit)

#define Get_CommEvent(Event)                        CntLeadZeros(Event)
#define Get_NextState(Event)                        CntLeadZeros(Event)


//---------GPRSģ��Ĺ���״̬-----------------
typedef enum 
{
    NB_STATE_POWER_ON = 0,
    NB_STATE_RESET,
    NB_STATE_INIT,     //����AT��ʼ������
		NB_STATE_CONNECT_NET,//*TCP/IP������·��������
		NB_STATE_READ_MESS,
		NB_STATE_COMM,
		NB_STATE_IDLE,
		NB_STATE_WAIT_ACK,
		NB_STATE_MAX,
   
}NB_STATE;

//---------GPRSģ��ͨѶ������ID-----------
typedef enum 
{
//		UPLOAD_ID_AUTH = 0,          //���͵�¼��
//		UPLOAD_ID_STA_MESS,          //����״̬��Ϣ��
//		UPLOAD_ID_HEARTBEAT,         //����������
	
		UPLOAD_ID_INIT = 0,          //���͵�¼��
		UPLOAD_ID_LOG,               //����״̬��Ϣ��
		UPLOAD_ID_MAX,
		UPLOAD_ID_INVALID = 0xFF,

}GPRS_COMM_ID;

typedef enum 
{
//		RETURN_ID_SET_PARMTE=0,
//		RETURN_ID_QUERY_PARMTE,
//		RETURN_ID_READ_MESS,
//		RETURN_ID_CTROL,
//		RETURN_ID_WRITE_ADDR,
//		RETURN_ID_READ_ADDR,
//		RETURN_ID_READ_STA_MESS,
//		RETURN_ID_SYS_PARMTE_SET,
		RETURN_ID_MAX,
		RETURN_ID_INVALID = 0xFF,

}NB_COMM_RETURN_ID;


typedef enum 
{
//		COMM_Event_AUTH             = (0x01 << UPLOAD_ID_AUTH),                  //���͵�¼��
//		COMM_Event_STA_MESS         = (0x01 << UPLOAD_ID_STA_MESS),              //����״̬��Ϣ��
//		COMM_Event_HEARTBEAT        = (0x01 << UPLOAD_ID_HEARTBEAT),             //����������
	
		COMM_Event_INIT             = (0x01 << UPLOAD_ID_INIT),                  //���͵�¼��
		COMM_Event_LOG              = (0x01 << UPLOAD_ID_LOG),                   //����������
	
//    COMM_Event_SET_PARMTE       = (0x01 << RETURN_ID_SET_PARMTE),            
//    COMM_Event_QUERY_PARMTE     = (0x01 << RETURN_ID_QUERY_PARMTE),
//    COMM_Event_READ_MESS        = (0x01 << RETURN_ID_READ_MESS),
//    COMM_Event_CTROL            = (0x01 << RETURN_ID_CTROL),
//    COMM_Event_WRITE_ADDR       = (0x01 << RETURN_ID_WRITE_ADDR),
//    COMM_Event_READ_ADDR        = (0x01 << RETURN_ID_READ_ADDR),
//    COMM_Event_READ_STA_MESS    = (0x01 << RETURN_ID_READ_STA_MESS),
//    COMM_Event_SYS_PARMTE_SET   = (0x01 << RETURN_ID_SYS_PARMTE_SET),
	
}NB_COMM_Event;

/*State switch Event*/
typedef enum 
{
		STATE_Event_COMM   = BIT0,     /*go to Comm*/
    STATE_Event_MAX,
	
}GPRS_STATE_Event;


/*******GPRSӦ������*************/
typedef enum 
{
    RSP_TYPE_CMD = 0, /*Ӧ��ATָ��*/
    RSP_TYPE_DATA,    /*ͨ������*/
		RSP_TYPE_MAX,
}RSP_TYPE;


///*********GPRSģ�����ѡ��***********/
//typedef enum
//{
//	GPRS_OPER_POWER_ON = 0,
//    GPRS_OPER_RESET,
//	GPRS_OPER_MAX,
//	
//}ENUM_GPRS_OPERATION;

/*********ATִ�н��***********/
typedef enum 
{
    NO_BACK = 0,
    BACK_TIMEOUT,           //ATָ��Ӧ��ʱ
    BACK_ERROR,             //ATָ��Ӧ�����
		BACK_PERTIMES_OUT,      //ATָ���ط���������
		BACK_TRUE,              //ATָ��Ӧ��������ȷ
	
}AT_RESULT_TYPE;


/**********GPRSָ�*****************/
typedef enum 
{
		NB_AT_CFUN_0=0,
		NB_AT_CFUN_1,
		NB_AT_CFUN0,
		NB_AT_CFUN1,
		NB_AT_NCDP,
	  NB_AT_NRB,
	
		NB_AT_CGSN_1,
		NB_AT_NCSEARFCN,
		NB_AT_NCONFIG_TRUE,
		NB_AT_NCONFIG_FALSE,
		NB_AT_CMEE_1,
		NB_AT_NCCID,
	
		NB_AT_CGDCONT,
		NB_AT_CGATT_1,
		NB_AT_CGATT_0,
		NB_AT_CGATT1,
		NB_AT_CGATT0,
		NB_AT_CPSMS_1,
		NB_AT_CEDRXS_0,
		NB_AT_NPSMR_1,
		NB_AT_CGPADDR,
		
		NB_AT_CSQ,
		NB_AT_NMGS,
		NB_AT_NQMGS,
			
		NB_AT_NMGR,
		NB_AT_NQMGR,

		NB_AT_NPSMR1,
}AT_CMD_Type;

typedef enum 
{
    RESPOND_TYPE=1,
		RTY_TYPE,
    UPLOAD_TYPE,    
		MAX_TYPE,
}SEND_TYPE;



/*******��������*************/
typedef enum 
{
    NET_NO_NET=0,
    NET_NB,
		NET_TYPE_MAX,
	
}NET_TYPE;

typedef enum
{
    CONN_OFF=0,
    CONN_REDAY,
    CONN_ON, 
    NET_STATUS_MAX,
}NET_STATUS;


/*******����Э��*************/
typedef enum 
{
    NET_PROTOCOL_UDP=0,
    NET_PROTOCOL_TCP,
    NET_PROTOCOL_COAP,
    NET_PROTOCOL_MQTT,
		NET_PROTOCOL_MAX,
}NET_PROTOCOL;

typedef struct
{
	const char* CmdStr;                /*ָ���*/
	uint16_t usTimeOut;                /*���ճ�ʱ*/
	const char* ExpResultStr;          /*�������*/
	uint8_t ucResendTimes;             /*�ط�����*/
}NB_Cmd_Data_Type; 

/*�������*/
typedef struct
{
		NET_TYPE NetType;
		NET_PROTOCOL NetProtocol;
		uint8_t NetConnSta;
		uint8_t ServerApn[SERVER_APN_LEN];
		uint8_t ServerIp[SERVER_IP_LEN];
    uint16_t ServerPort;
    uint16_t ulReportPeriod; 
}Net_Parameters;

#define ADDR_REGION_LEN   7
typedef struct
{
//    uint8_t productType;                                //��Ʒ����
    uint8_t AckMeageFlowNum;              //Ӧ����Ϣ��ˮ��
    uint8_t TerminalMeageFlowNum;         //��ˮ�� 
    uint8_t addrRegion[ADDR_REGION_LEN];  //��ַ��
    uint8_t controlCode;                  //������
    uint8_t dataLen;                      //���ݳ���
    uint8_t *bodyData;                    //������
    
}PACKET_HEAD_Type;

//typedef struct
//{
//   uint8_t imei[IMEI_LEN];
//   uint8_t iccid[ICCID_LEN];
//    
//}AUTH_DATA_Type;


//typedef union
//{
//    uint16_t state;
//    struct
//    {
//        uint16_t staOfEle        :1;
//        uint16_t staOfCharge     :1;
//        uint16_t PhoneOnLineSta  :1;
//        uint16_t reserve         :13;
//    };
//    
//}DEVICE_STATE_Type;


//typedef struct
//{
//    DEVICE_STATE_Type DeviceState;   //�豸״̬
//    uint8_t CSQ;                     //GPRS�ź�ǿ��
//    uint8_t hardwareVerLen;          //Ӳ���汾�ų���
//    uint8_t hardwareVer[4];          //Ӳ���汾��
//    uint8_t softwareVerLen;          //����汾�ų���
//    uint8_t softwareVer[4];          //����汾��
//    
//}MESS_READ_Type;


//typedef struct
//{
//    uint16_t chargeTime;
//    uint16_t chargeElec;
//    uint16_t chargeVolt;
//    uint16_t refVolt;
//    
//}SYS_STA_MESS_Type;


typedef struct
{
		uint8_t imei[IMEI_LEN];
		uint8_t iccid[ICCID_LEN];
    uint8_t CSQ;                     //NB�ź�ǿ��
    uint8_t mac[MAC_LEN];                     //NB�ź�ǿ��
}INIT_Type;

/*********ͨѶ�����ݰ�����**************/
typedef struct
{
//		AUTH_DATA_Type      AuthData;
//		MESS_READ_Type      MessRead;
//		SYS_STA_MESS_Type   SysStaMess;
		
		INIT_Type          Init_Data;
	
}COMM_Packet_Type;

#define RTY_TIME_INIT      (30UL)  /*30*1000 ms ��30s*/
#define RTY_TIMES_MAX      (3)
typedef struct
{
    uint8_t  rtycnt;                                         //�ش�����
    uint8_t  rtycntEn;                                       //�ش�����ʹ��
#if CONFIG_RETRY_COPY
    uint8_t  rtyBuff[GPRS_RTY_MESS_LEN];                     //�ش���Ϣ����
    uint8_t  rtyLen;                                         //�ش���Ϣ����
#endif
    uint16_t rtyTimCount;                                    //�ش���ʱ������
    uint16_t rtyTime;                                        //�ش�ʱ��
}Retry_Ctr_Type;

typedef struct
{
    uint16_t heartbeatCount;
    
}NB_Tim_Type;


typedef struct
{
//    uint8_t ucRespondEvt;
//    uint8_t ucDelayEvt;
//    uint8_t ucRetryEvt;
    uint8_t ucUploadEvt;
}Event_Type;

/*******ATָ��ͨѶ���ƿ�**********/
typedef struct
{
    uint8_t SendATCmdEn;                 /*enable send ATCmd*/
		uint8_t ATCmdPt;                     /*��ǰAT��������*/
		uint8_t ucResendCnt;                 /*�ط�����*/	
		uint8_t ucResendTimes;               /*�ط�����*/
		uint32_t ulTimeOut;                  /*���ճ�ʱ*/
		uint8_t ATCmdResult;                 /*��ǰATָ��ͨѶ���*/
		bool Busy;                           /*MCU��ģ��ͨѶæµ��־*/
		uint8_t CmdStr[NB_RX_BUFF_LEN];      /*��ǰ���͵�ָ���*/
		uint8_t ucCmdStrLen;                 /*��ǰָ���*/
		char ExpResultStr[20];               /*��ǰ���͵�ָ������������*/
		uint16_t ucByteTimeOut;              /*�ֽڳ�ʱ*/
		uint8_t RxBuf[NB_RX_BUFF_LEN];       /*�������ݻ���*/
		uint8_t ucRxCnt;                     /*�������ݼ���*/
		uint8_t ucRxLen;                     /*�������ݳ���*/
		uint8_t RxFrameOk;                   /*ָʾһ֡��������*/
    uint8_t RspType;                     /*NBģ��Ӧ������*/
}AT_CMD_CB; 

/*******GPRSģ����ƿ�**********/
typedef struct
{
		uint8_t State;                       /*ģ�鵱ǰ״̬*/
		uint8_t StateOld;                    /*ģ����һ��״̬*/
		uint8_t StepPt;                      /*step ponit*/
		uint16_t ulDelayCnt;                 /*��ʱ����*/
		uint16_t ulProcCtrTaskPer;           /*״̬�����������*/
		uint8_t ucCommEvent;                 /*�����Ҫ���͵����ݰ�*/
		uint8_t ucSendType;                  /*��Ƿ�������*/
		uint8_t ucStateEvent;                /*��ǰ���¼�״̬*/
		uint8_t ucDataID;                    /*������ڷ��͵���ϢID*/
		uint8_t ucCommBusy;                  /*ͨ��æµ���*/
		uint8_t RxBuf[NB_RX_BUFF_LEN];       /*Ӧ�ò���յ�������*/
		uint8_t TxBuf[NB_RX_BUFF_LEN];       /*Ӧ�ò���Ҫ���͵�����*/
		uint8_t ucTxLen;                     /*Ӧ�ò㷢�����ݳ���*/ 
		uint8_t AuthStatus;                  /*�ն˵�¼/��Ȩ״̬*/
		NB_Tim_Type      TimProc;
		Net_Parameters   NetPar;             /*ģ������������Ϣ*/
    COMM_Packet_Type CommPacket;     		 /*ͨѶ���ݰ�*/
    Retry_Ctr_Type   RetryCtrol;    		 /*�ط�����*/
    Event_Type       EvtProc;      			 /*�¼�����*/
    AT_CMD_CB        ATCmdCB;      			 /*******ATָ��ͨѶ���ƿ�**********/
	
}NB_Handler;

#define NB_NetPar                    (g_stNB_Handler.NetPar)
#define NB_RetryCtrol                (g_stNB_Handler.RetryCtrol)
#define NB_ATCmdCB                   (g_stNB_Handler.ATCmdCB)
#define NB_CommPacket                (g_stNB_Handler.CommPacket)
#define NB_EvtProc                   (g_stNB_Handler.EvtProc)
#define NB_SysStaMess                (g_stNB_Handler.CommPacket.SysStaMess)
#define NB_TimProc                   (g_stNB_Handler.TimProc)


/*************����������̵�step*******************/
#define INIT_STEP_NUM        NumberOfElements(s_ATCmdStep_Init)
#define CONNECT_STEP_NUM     NumberOfElements(s_ATCmdStep_Connnect)
#define MESS_STEP_NUM        NumberOfElements(s_ATCmdStep_Mess)
#define COMM_STEP_NUM        NumberOfElements(s_ATCmdStep_Comm)

extern PACKET_HEAD_Type    PacketHead;
extern NB_Handler        g_stNB_Handler;

extern uint8_t                          g_Rx_Buffer[BUFFSIZE];
extern uint8_t                          g_Tx_Buffer[BUFFSIZE];

void APP_NB_TimeProc(uint16_t usPeriod);
//void APP_GPRS_NetParConfig(void);
void app_comm_init(void);


void APP_SubTask_CmdProc(void);
void APP_SubTask_StateProc(void);
//void APP_SuTask_DecodeProc(void);


//U8 APP_GPRS_RxDataCheck(U8 *rxBuff);

uint16_t APP_GPRS_WriteReturnIDPacket(uint8_t DataID, uint8_t* Packet);

#endif


