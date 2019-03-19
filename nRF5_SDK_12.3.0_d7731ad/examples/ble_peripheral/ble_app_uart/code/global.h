#ifndef __GLOBAL_H
#define __GLOBAL_H

#pragma anon_unions

#include "main.h"

//#pragma anon_unions

//#define ESWAP(word) ((word)>>8 & 0xff | (word)<<8 & 0xff00)

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;											 				

#define Var_Fill(var,data)      for(uint16_t ii = 0; ii < sizeof(var); ++ii)\
                                *(((uint8_t*)(&var))+ii) = data              //�����var�����data

#define TmrSecondsToMilliseconds( n )   ( (uint32_t) ((n) * 1000) )

#define MY_SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define MY_CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define MY_READ_BIT(REG, BIT)    ((REG) & (BIT))

//#define MY_CLEAR_REG(REG)        ((REG) = (0x0))

//#define MY_WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define MY_READ_REG(REG)          ((REG))

#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))

#define NB_MS_TO_S                (1000UL)
#define HEARTBEAT_TIME              (2400UL)

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

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

#endif

