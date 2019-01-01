#ifndef __APP_COMM_BLE_H
#define __APP_COMM_BLE_H

#include "global.h"

///*******BLE模块控制块**********/
//typedef struct
//{
//		uint8_t State;                       /*模块当前状态*/
////		U8 StateOld;                    /*模块上一个状态*/
//		uint8_t StepPt;                      /*step ponit*/
////		U16 ulDelayCnt;                 /*延时计数*/
////		U16 ulProcCtrTaskPer;           /*状态函数处理计数*/
////		U16 ucCommEvent;                /*标记需要发送的数据类型*/
////		U8 ucStateEvent;                /*当前的事件状态*/
////		U8 ucDataID;                    /*标记正在发送的消息ID*/
////		U8 ucCommBusy;                  /*通信忙碌标记*/
////		U8 RxBuf[GPRS_RX_BUFF_LEN];     /*应用层接收到的数据*/
////		U8 TxBuf[GPRS_RX_BUFF_LEN];     /*应用层需要发送的数据*/
////		U8 ucTxLen;                     /*应用层发送数据长度*/              
////		Net_Parameters NetPar;          /*模块网络连接信息*/
////    COMM_Packet_Type CommPacket;    /*通讯数据包*/
////    Retry_Ctr_Type RetryCtrol;      /*重发控制*/
////    AT_CMD_CB ATCmdCB;              /*******AT指令通讯控制块**********/
//}BLE_Handler_t;

///*********BLE模块的工作状态****************/
//typedef enum 
//{
//		BLE_STATE_CRC,
//		BLE_STATE_DECODE,
//		BLE_STATE_ACTION,
//		BLE_STATE_ENCRYPT,
//		BLE_STATE_RETURN,
//		BLE_STATE_IDLE,
////    GPRS_STATE_POWER_ON = 0,
////    GPRS_STATE_RESET,
////    GPRS_STATE_INIT,      /*进入AT初始化流程*/
////		GPRS_STATE_CONNECT_NET,/*TCP/IP数据链路建立流程*/
////		GPRS_STATE_READ_MESS,
////		GPRS_STATE_COMM,
////		GPRS_STATE_IDLE,
////		GPRS_STATE_WAIT_ACK,
////		GPRS_STATE_MAX,
//   
//}BLE_STATE;

//extern BLE_Handler_t        g_stBLE_Handler;
//extern uint8_t              Char2_Once_Receive_Len;//用于数据接收和发送
//extern uint8_t              char2_all_receive_len;//用于数据接收和发送
//extern uint8_t              char2_all_receive[];//用于数据接收和发送
//extern uint8_t              char4_all_send[];//用于数据接收和发送

//void APP_BLETask_StateProc(void);
uint8_t ProcessCommand(uint8_t *pData, uint8_t command, uint16_t dataLen);
extern uint8_t              *pRecvBuffer;
extern uint8_t              stateProcessCommand;
extern uint8_t              char4_all_send[256];//用于数据发送

uint8_t CRC_8(uint8_t *pdata, uint16_t size);

uint8_t UserParseAppProtocolFormat(uint8_t *data_encrypt, uint16_t length);
uint8_t UserCheckAppProtocolFormat(uint8_t *pBuf, uint16_t inputLen);
void Put_Return(uint8_t command, uint8_t length);
#endif


