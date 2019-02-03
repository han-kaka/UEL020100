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
///*************帧头/帧尾*************/
//#define FRAME_HEAD       0x7f
//#define FRAME_END        0xf7

//#define TRANSFER_FLAG       0x1b
//#define TRANSFER_CHAR1      0x1a
//#define TRANSFER_CHAR1_FLAG 0x01
//#define TRANSFER_CHAR2      0x1b
//#define TRANSFER_CHAR2_FLAG 0x02

///***********************************************************************/
///*********************服务器下发控制码************************/
///*控制码第7bit 代表方向    0:服务端->终端,1:终端->服务端*/

//#define WRITE_CTROL_CODE              0x20 
//#define READ_CTROL_CODE               0x21
//#define CONTROL_CTROL_CODE            0x22
//#define UPLOAD_CTROL_CODE             0x23    

////#define RETURN_WRITE_CTROL_CODE       0xa0   
////#define RETURN_READ_CTROL_CODE        0xa1
////#define RETURN_CONTROL_CTROL_CODE     0xa2
////#define RETURN_UPLOAD_CTROL_CODE      0xa3

///*****************************************************************/

/************************包相关信息**********************************/
//初始化包信息
#define INIT_ID                         0x00

//日志包信息
#define LOG_ID                          0x01
//登录包相关信息
//#define AUTHENTICA_ID                   0xb010
//#define AUTH_CTROL_CODE                 0xa3
//#define AUTH_LEN                        0x09

////状态信息上传包相关信息
//#define STA_MESS_ID                     0xb041
//#define STA_MESS_CTROL_CODE             0xa3
//#define STA_MESS_LEN                    0x0D

////心跳包相关信息
//#define HEARTBEAT_ID                    0xb040
//#define HEARTBEAT_CTROL_CODE            0xa3
//#define HEARTBEAT_LEN                   0x03


////参数设置返回包
//#define SET_PARMTE_ID                   0xb020
//#define SET_PARMTE_CTROL_CODE           0xa0
//#define SET_PARMTE_LEN                  0x03

////查询参数包
//#define QUERY_PARMTE_ID                 0xb021
//#define QUERY_PARMTE_CTROL_CODE         0xa1
//#define QUERY_PARMTE_LEN                0x1d

////读信息包
//#define READ_MESS_ID                    0xb022
//#define READ_MESS_CTROL_CODE            0xa1
//#define READ_MESS_LEN                   0x10

////控制包返回
//#define CONTROL_ID                      0xb027
//#define CTROL_CODE                      0xa2
//#define CTROL_LEN                       0x03

////写地址返回包
//#define WRITE_ADDR_ID                   0xb024
//#define WRITE_ADDR_CTROL_CODE           0xa0
//#define WRITE_ADDR_LEN                  0x03

////读地址返回包
//#define READ_ADDR_ID                    0xb025
//#define READ_ADDR_CTROL_CODE            0xa1
//#define READ_ADDR_LEN                   0x07

////读状态信息包
//#define READ_STA_MESS_ID                0xb026
//#define READ_STA_MESS_CTROL_CODE        0xa1
//#define READ_STA_MESS_LEN               0x0D

////系统参数设置
//#define SYS_PARMTE_SET_ID               0xb028
//#define SYS_PARMTE_SET_CTROL_CODE       0xa0
//#define SYS_PARMTE_SET_LEN              0x03

/*****************************************************************/


/***********各个子任务的运行周期*************************/
#define SUB_TASK_PROCCTR_PER          (10U) /*10ms*/


/*******默认网络连接情况*********/	 
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


//---------GPRS模块的工作状态-----------------
typedef enum 
{
    NB_STATE_POWER_ON = 0,
    NB_STATE_RESET,
    NB_STATE_INIT,     //进入AT初始化流程
		NB_STATE_CONNECT_NET,//*TCP/IP数据链路建立流程
		NB_STATE_READ_MESS,
		NB_STATE_COMM,
		NB_STATE_IDLE,
		NB_STATE_WAIT_ACK,
		NB_STATE_MAX,
   
}NB_STATE;

//---------GPRS模块通讯的数据ID-----------
typedef enum 
{
//		UPLOAD_ID_AUTH = 0,          //发送登录包
//		UPLOAD_ID_STA_MESS,          //发送状态信息包
//		UPLOAD_ID_HEARTBEAT,         //发送心跳包
	
		UPLOAD_ID_INIT = 0,          //发送登录包
		UPLOAD_ID_LOG,               //发送状态信息包
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
//		COMM_Event_AUTH             = (0x01 << UPLOAD_ID_AUTH),                  //发送登录包
//		COMM_Event_STA_MESS         = (0x01 << UPLOAD_ID_STA_MESS),              //发送状态信息包
//		COMM_Event_HEARTBEAT        = (0x01 << UPLOAD_ID_HEARTBEAT),             //发送心跳包
	
		COMM_Event_INIT             = (0x01 << UPLOAD_ID_INIT),                  //发送登录包
		COMM_Event_LOG              = (0x01 << UPLOAD_ID_LOG),                   //发送心跳包
	
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


/*******GPRS应答类型*************/
typedef enum 
{
    RSP_TYPE_CMD = 0, /*应答AT指令*/
    RSP_TYPE_DATA,    /*通信数据*/
		RSP_TYPE_MAX,
}RSP_TYPE;


///*********GPRS模块操作选项***********/
//typedef enum
//{
//	GPRS_OPER_POWER_ON = 0,
//    GPRS_OPER_RESET,
//	GPRS_OPER_MAX,
//	
//}ENUM_GPRS_OPERATION;

/*********AT执行结果***********/
typedef enum 
{
    NO_BACK = 0,
    BACK_TIMEOUT,           //AT指令应答超时
    BACK_ERROR,             //AT指令应答错误
		BACK_PERTIMES_OUT,      //AT指令重发次数到了
		BACK_TRUE,              //AT指令应答内容正确
	
}AT_RESULT_TYPE;


/**********GPRS指令集*****************/
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



/*******网络类型*************/
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


/*******网络协议*************/
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
	const char* CmdStr;                /*指令符*/
	uint16_t usTimeOut;                /*接收超时*/
	const char* ExpResultStr;          /*期望结果*/
	uint8_t ucResendTimes;             /*重发次数*/
}NB_Cmd_Data_Type; 

/*网络参数*/
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
//    uint8_t productType;                                //产品类型
    uint8_t AckMeageFlowNum;              //应答消息流水号
    uint8_t TerminalMeageFlowNum;         //流水号 
    uint8_t addrRegion[ADDR_REGION_LEN];  //地址域
    uint8_t controlCode;                  //控制码
    uint8_t dataLen;                      //数据长度
    uint8_t *bodyData;                    //数据体
    
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
//    DEVICE_STATE_Type DeviceState;   //设备状态
//    uint8_t CSQ;                     //GPRS信号强度
//    uint8_t hardwareVerLen;          //硬件版本号长度
//    uint8_t hardwareVer[4];          //硬件版本号
//    uint8_t softwareVerLen;          //软件版本号长度
//    uint8_t softwareVer[4];          //软件版本号
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
    uint8_t CSQ;                     //NB信号强度
    uint8_t mac[MAC_LEN];                     //NB信号强度
}INIT_Type;

/*********通讯的数据包类型**************/
typedef struct
{
//		AUTH_DATA_Type      AuthData;
//		MESS_READ_Type      MessRead;
//		SYS_STA_MESS_Type   SysStaMess;
		
		INIT_Type          Init_Data;
	
}COMM_Packet_Type;

#define RTY_TIME_INIT      (30UL)  /*30*1000 ms 即30s*/
#define RTY_TIMES_MAX      (3)
typedef struct
{
    uint8_t  rtycnt;                                         //重传计数
    uint8_t  rtycntEn;                                       //重传计数使能
#if CONFIG_RETRY_COPY
    uint8_t  rtyBuff[GPRS_RTY_MESS_LEN];                     //重传消息备份
    uint8_t  rtyLen;                                         //重传消息长度
#endif
    uint16_t rtyTimCount;                                    //重传定时器计数
    uint16_t rtyTime;                                        //重传时间
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

/*******AT指令通讯控制块**********/
typedef struct
{
    uint8_t SendATCmdEn;                 /*enable send ATCmd*/
		uint8_t ATCmdPt;                     /*当前AT命令索引*/
		uint8_t ucResendCnt;                 /*重发计数*/	
		uint8_t ucResendTimes;               /*重发次数*/
		uint32_t ulTimeOut;                  /*接收超时*/
		uint8_t ATCmdResult;                 /*当前AT指令通讯结果*/
		bool Busy;                           /*MCU与模块通讯忙碌标志*/
		uint8_t CmdStr[NB_RX_BUFF_LEN];      /*当前发送的指令符*/
		uint8_t ucCmdStrLen;                 /*当前指令长度*/
		char ExpResultStr[20];               /*当前发送的指令符的期望结果*/
		uint16_t ucByteTimeOut;              /*字节超时*/
		uint8_t RxBuf[NB_RX_BUFF_LEN];       /*接收数据缓存*/
		uint8_t ucRxCnt;                     /*接收数据计数*/
		uint8_t ucRxLen;                     /*接收数据长度*/
		uint8_t RxFrameOk;                   /*指示一帧接收完整*/
    uint8_t RspType;                     /*NB模块应答类型*/
}AT_CMD_CB; 

/*******GPRS模块控制块**********/
typedef struct
{
		uint8_t State;                       /*模块当前状态*/
		uint8_t StateOld;                    /*模块上一个状态*/
		uint8_t StepPt;                      /*step ponit*/
		uint16_t ulDelayCnt;                 /*延时计数*/
		uint16_t ulProcCtrTaskPer;           /*状态函数处理计数*/
		uint8_t ucCommEvent;                 /*标记需要发送的数据包*/
		uint8_t ucSendType;                  /*标记发送类型*/
		uint8_t ucStateEvent;                /*当前的事件状态*/
		uint8_t ucDataID;                    /*标记正在发送的消息ID*/
		uint8_t ucCommBusy;                  /*通信忙碌标记*/
		uint8_t RxBuf[NB_RX_BUFF_LEN];       /*应用层接收到的数据*/
		uint8_t TxBuf[NB_RX_BUFF_LEN];       /*应用层需要发送的数据*/
		uint8_t ucTxLen;                     /*应用层发送数据长度*/ 
		uint8_t AuthStatus;                  /*终端登录/鉴权状态*/
		NB_Tim_Type      TimProc;
		Net_Parameters   NetPar;             /*模块网络连接信息*/
    COMM_Packet_Type CommPacket;     		 /*通讯数据包*/
    Retry_Ctr_Type   RetryCtrol;    		 /*重发控制*/
    Event_Type       EvtProc;      			 /*事件处理*/
    AT_CMD_CB        ATCmdCB;      			 /*******AT指令通讯控制块**********/
	
}NB_Handler;

#define NB_NetPar                    (g_stNB_Handler.NetPar)
#define NB_RetryCtrol                (g_stNB_Handler.RetryCtrol)
#define NB_ATCmdCB                   (g_stNB_Handler.ATCmdCB)
#define NB_CommPacket                (g_stNB_Handler.CommPacket)
#define NB_EvtProc                   (g_stNB_Handler.EvtProc)
#define NB_SysStaMess                (g_stNB_Handler.CommPacket.SysStaMess)
#define NB_TimProc                   (g_stNB_Handler.TimProc)


/*************定义各个流程的step*******************/
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


