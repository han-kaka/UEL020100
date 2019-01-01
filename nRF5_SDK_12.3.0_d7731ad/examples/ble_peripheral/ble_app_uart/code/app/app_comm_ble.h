#ifndef __APP_COMM_BLE_H
#define __APP_COMM_BLE_H

#include "global.h"

///*******BLEģ����ƿ�**********/
//typedef struct
//{
//		uint8_t State;                       /*ģ�鵱ǰ״̬*/
////		U8 StateOld;                    /*ģ����һ��״̬*/
//		uint8_t StepPt;                      /*step ponit*/
////		U16 ulDelayCnt;                 /*��ʱ����*/
////		U16 ulProcCtrTaskPer;           /*״̬�����������*/
////		U16 ucCommEvent;                /*�����Ҫ���͵���������*/
////		U8 ucStateEvent;                /*��ǰ���¼�״̬*/
////		U8 ucDataID;                    /*������ڷ��͵���ϢID*/
////		U8 ucCommBusy;                  /*ͨ��æµ���*/
////		U8 RxBuf[GPRS_RX_BUFF_LEN];     /*Ӧ�ò���յ�������*/
////		U8 TxBuf[GPRS_RX_BUFF_LEN];     /*Ӧ�ò���Ҫ���͵�����*/
////		U8 ucTxLen;                     /*Ӧ�ò㷢�����ݳ���*/              
////		Net_Parameters NetPar;          /*ģ������������Ϣ*/
////    COMM_Packet_Type CommPacket;    /*ͨѶ���ݰ�*/
////    Retry_Ctr_Type RetryCtrol;      /*�ط�����*/
////    AT_CMD_CB ATCmdCB;              /*******ATָ��ͨѶ���ƿ�**********/
//}BLE_Handler_t;

///*********BLEģ��Ĺ���״̬****************/
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
////    GPRS_STATE_INIT,      /*����AT��ʼ������*/
////		GPRS_STATE_CONNECT_NET,/*TCP/IP������·��������*/
////		GPRS_STATE_READ_MESS,
////		GPRS_STATE_COMM,
////		GPRS_STATE_IDLE,
////		GPRS_STATE_WAIT_ACK,
////		GPRS_STATE_MAX,
//   
//}BLE_STATE;

//extern BLE_Handler_t        g_stBLE_Handler;
//extern uint8_t              Char2_Once_Receive_Len;//�������ݽ��պͷ���
//extern uint8_t              char2_all_receive_len;//�������ݽ��պͷ���
//extern uint8_t              char2_all_receive[];//�������ݽ��պͷ���
//extern uint8_t              char4_all_send[];//�������ݽ��պͷ���

//void APP_BLETask_StateProc(void);
uint8_t ProcessCommand(uint8_t *pData, uint8_t command, uint16_t dataLen);
extern uint8_t              *pRecvBuffer;
extern uint8_t              stateProcessCommand;
extern uint8_t              char4_all_send[256];//�������ݷ���

uint8_t CRC_8(uint8_t *pdata, uint16_t size);

uint8_t UserParseAppProtocolFormat(uint8_t *data_encrypt, uint16_t length);
uint8_t UserCheckAppProtocolFormat(uint8_t *pBuf, uint16_t inputLen);
void Put_Return(uint8_t command, uint8_t length);
#endif


