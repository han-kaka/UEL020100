#ifndef __BSP_BLE_TRANS_H
#define __BSP_BLE_TRANS_H

#include "ble_nus.h"

#include "global.h"



typedef struct
{
	uint16_t startCode;
	uint16_t version;
	uint8_t  appScenarios;
	uint8_t  vendorCode;
	uint8_t  res[3];
	uint8_t  command;
	uint8_t  dataSources;
	uint8_t  dataLen;
} ProtocolAppHeadFormat_t;

typedef struct
{
	uint8_t  crc8;
	uint16_t stopCode;
} ProtocolAppTailFormat_t;

typedef struct
{
	ProtocolAppHeadFormat_t headData;
	uint8_t  *pData;
	ProtocolAppTailFormat_t tailData;
} ProtocolAppFormat_t;


extern ProtocolAppFormat_t  sProtocolAppFormat;
extern uint8_t              tempkey[16];
extern uint8_t              data_encrypt[256];
extern uint8_t              *pRecvBuffer;

void bsp_ble_command_rx(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length);

//void bsp_ble_command_explain(uint8_t * p_data, uint16_t length);

uint8_t UserParseAppProtocolFormat(uint8_t *data_encrypt, uint16_t length);
uint8_t UserCheckAppProtocolFormat(uint8_t *pBuf, uint16_t inputLen);

uint8_t CRC_8(uint8_t *pdata, uint16_t size);

//uint32_t bsp_ble_value_tx(uint8_t * p_data, uint16_t length);
//void ble_process_task(void);

#endif


