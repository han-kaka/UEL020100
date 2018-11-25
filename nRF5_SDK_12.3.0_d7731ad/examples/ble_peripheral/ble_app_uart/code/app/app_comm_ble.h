#ifndef __APP_COMM_BLE_H
#define __APP_COMM_BLE_H

#include "global.h"


#define PROTOCOL_APP_HEAD									0xF0A5		/* 协议头 */
#define PROTOCOL_APP_VERSION							0x0100		/* 协议版本 */
#define PROTOCOL_APP_SCENARIOS						0x01			/* 应用场景 */
#if defined ANGUMEN
	#define PROTOCOL_APP_VENDORCODE					0x08			/* 厂商代码 */
#elif defined NUNENG
	#define PROTOCOL_APP_VENDORCODE					0x09			/* 厂商代码 */
#elif defined SFG
	#define PROTOCOL_APP_VENDORCODE					0x0A			/* 厂商代码 */
#else
	#define PROTOCOL_APP_VENDORCODE					0x01			/* 厂商代码 */
#endif
#define PROTOCOL_APP_RESERVE							0x00			/* 保留 */
#define PROTOCOL_APP_SRC_MPHONE						0xAA			/* 数据来源手机 */
#define PROTOCOL_APP_SRC_GATEWAY					0x55			/* 数据来源网关 */
#define PROTOCOL_APP_TAIL									0x160D		/* 协议尾 */

#define PROTOCOL_APP_ERR_NONE							0x00			/* 协议成功 */
#define PROTOCOL_APP_ERR_GEN							0x01			/* 通用错误 */
#define PROTOCOL_APP_ERR_STORAGE					0x02			/* 存储错误 */
#define PROTOCOL_APP_ERR_MODE							0x03			/* 模式错误 */
#define PROTOCOL_APP_ERR_PARAM						0x04			/* 参数错误 */
#define PROTOCOL_APP_ERR_NOT_PERMIT				0x05			/* 操作不允许 */
#define PROTOCOL_APP_ERR_FINISHED					0x06			/* 操作完成 */
#define PROTOCOL_APP_ERR_VERSION					0x07			/* 版本不支持 */
#define PROTOCOL_APP_ERR_PASSWORD_SAME		0x08			/* 内有相同密码 */
#define PROTOCOL_APP_ERR_NOT_FOUND_NB			0x09			/* 未发现联网模块 */
#define PROTOCOL_APP_ERR_NOT_FOUND_SIM		0x0A			/* 未发现SIM卡 */

#define CMD_GET_HARD_INFO_VERSION					0x01			/* 查询硬件版本信息 */
#define CMD_GET_SOFT_INFO_VERSION					0x02			/* 查询软件版本信息 */
#define CMD_GET_AESKEY						    		0xA0			/* 初始化 */
#define CMD_ADD_ADMIN						      		0xA1			/* 添加管理员 */
#define CMD_ADD_USER											0xA2			/* 添加用户 */
#define CMD_READ_USER											0xA3			/* 读取用户 */
#define CMD_DEL_USER											0xA4			/* 删除用户 */
#define CMD_USER_LOGIN										0xA5			/* 用户登录 */
#define CMD_SET_KEYBOARD									0xA6			/* 设置键盘密码 */
#define CMD_ADD_PERIPHERAL								0xA7			/* 添加外设 */
#define CMD_SWITCH												0xA8			/* 开关指令 */
#define CMD_ADJUST_TIME										0xA9			/* 校时指令 */
#define CMD_READ_LOG											0xAA			/* 读取日志 */
#define CMD_READ_USER_CONFIG							0xAB			/* 读取用户信息 */
#define CMD_DEL_LOG												0xAC			/* 删除日志 */
#define CMD_FACTORYRESET									0xAD			/* 恢复出厂设置 */
#define CMD_TERFILEDATA										0xAE			/* 写设备文件数据 */
#define CMD_PARAM_CONFIG									0xAF			/* 参数配置指令 */
#define CMD_PARAM_READ										0xB0			/* 参数读取指令 */

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


