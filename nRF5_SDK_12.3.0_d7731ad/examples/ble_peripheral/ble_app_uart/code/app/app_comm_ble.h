#ifndef __APP_COMM_BLE_H
#define __APP_COMM_BLE_H

#include "global.h"


#define PROTOCOL_APP_HEAD									0xF0A5		/* Э��ͷ */
#define PROTOCOL_APP_VERSION							0x0100		/* Э��汾 */
#define PROTOCOL_APP_SCENARIOS						0x01			/* Ӧ�ó��� */
#if defined ANGUMEN
	#define PROTOCOL_APP_VENDORCODE					0x08			/* ���̴��� */
#elif defined NUNENG
	#define PROTOCOL_APP_VENDORCODE					0x09			/* ���̴��� */
#elif defined SFG
	#define PROTOCOL_APP_VENDORCODE					0x0A			/* ���̴��� */
#else
	#define PROTOCOL_APP_VENDORCODE					0x01			/* ���̴��� */
#endif
#define PROTOCOL_APP_RESERVE							0x00			/* ���� */
#define PROTOCOL_APP_SRC_MPHONE						0xAA			/* ������Դ�ֻ� */
#define PROTOCOL_APP_SRC_GATEWAY					0x55			/* ������Դ���� */
#define PROTOCOL_APP_TAIL									0x160D		/* Э��β */

#define PROTOCOL_APP_ERR_NONE							0x00			/* Э��ɹ� */
#define PROTOCOL_APP_ERR_GEN							0x01			/* ͨ�ô��� */
#define PROTOCOL_APP_ERR_STORAGE					0x02			/* �洢���� */
#define PROTOCOL_APP_ERR_MODE							0x03			/* ģʽ���� */
#define PROTOCOL_APP_ERR_PARAM						0x04			/* �������� */
#define PROTOCOL_APP_ERR_NOT_PERMIT				0x05			/* ���������� */
#define PROTOCOL_APP_ERR_FINISHED					0x06			/* ������� */
#define PROTOCOL_APP_ERR_VERSION					0x07			/* �汾��֧�� */
#define PROTOCOL_APP_ERR_PASSWORD_SAME		0x08			/* ������ͬ���� */
#define PROTOCOL_APP_ERR_NOT_FOUND_NB			0x09			/* δ��������ģ�� */
#define PROTOCOL_APP_ERR_NOT_FOUND_SIM		0x0A			/* δ����SIM�� */

#define CMD_GET_HARD_INFO_VERSION					0x01			/* ��ѯӲ���汾��Ϣ */
#define CMD_GET_SOFT_INFO_VERSION					0x02			/* ��ѯ����汾��Ϣ */
#define CMD_GET_AESKEY						    		0xA0			/* ��ʼ�� */
#define CMD_ADD_ADMIN						      		0xA1			/* ��ӹ���Ա */
#define CMD_ADD_USER											0xA2			/* ����û� */
#define CMD_READ_USER											0xA3			/* ��ȡ�û� */
#define CMD_DEL_USER											0xA4			/* ɾ���û� */
#define CMD_USER_LOGIN										0xA5			/* �û���¼ */
#define CMD_SET_KEYBOARD									0xA6			/* ���ü������� */
#define CMD_ADD_PERIPHERAL								0xA7			/* ������� */
#define CMD_SWITCH												0xA8			/* ����ָ�� */
#define CMD_ADJUST_TIME										0xA9			/* Уʱָ�� */
#define CMD_READ_LOG											0xAA			/* ��ȡ��־ */
#define CMD_READ_USER_CONFIG							0xAB			/* ��ȡ�û���Ϣ */
#define CMD_DEL_LOG												0xAC			/* ɾ����־ */
#define CMD_FACTORYRESET									0xAD			/* �ָ��������� */
#define CMD_TERFILEDATA										0xAE			/* д�豸�ļ����� */
#define CMD_PARAM_CONFIG									0xAF			/* ��������ָ�� */
#define CMD_PARAM_READ										0xB0			/* ������ȡָ�� */

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


