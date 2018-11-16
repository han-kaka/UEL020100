#include "app_comm_ble.h"
#include "nrf_drv_rng.h"

BLE_Handler_t        g_stBLE_Handler;

uint8_t              Char2_Once_Receive_Len = 0;//�������ݽ���
uint8_t              char2_all_receive_len = 0;//�������ݽ���
uint8_t              char2_all_receive[256];//�������ݽ���
uint8_t              char4_all_send_flag = 0;//�������ݷ���
uint8_t              char4_all_send_len = 0;//�������ݷ���
uint8_t              char4_all_send[256];//�������ݷ���

static uint8_t f_tempkey = 0; /* ������Կ��־λ */

//void APP_BLETask_StateProc(void)
//{
//    switch(g_stBLE_Handler.State)
//    {
//				case BLE_STATE_CRC:
//        {
//						
//				}
//						break;
//				
//				case BLE_STATE_DECODE:
//        {
//				}
//						break;
//				
//				case BLE_STATE_ACTION:
//        {
//				}
//						break;
//				
//				case BLE_STATE_ENCRYPT:
//        {
//				}
//						break;
//				
//				case BLE_STATE_RETURN:
//        {
//				}
//						break;
//				
//        default:
//				{
//						g_stBLE_Handler.State = BLE_STATE_IDLE;
//				}
//						break;
//		}			
//	
//}

/**************************************************************************************************
 * @brief       ���ش������
 * @param       command������
 * @param       errCode���������
 * @return      None
 **************************************************************************************************
 */
static void UserReturnErrCode(uint8_t command, uint8_t errCode)
{
	uint8_t  postion = 0;
	uint8_t  *pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
	
	pSendbuf[postion++] = errCode;
//	Put_Return(command, postion);
}


/**************************************************************************************************
 * @brief       ���ش�����������
 * @param       command������
 * @param       errCode���������
 * @param       pData������
 * @param       len�����ݳ���
 * @return      0-�ɹ���other-ʧ�ܣ�
 **************************************************************************************************
 */
static void UserReturnErrCodeAndData(uint8_t command, uint8_t errCode, char *pData, uint8_t len)
{
	uint8_t  postion = 0;
	uint8_t  *pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
	
	pSendbuf[postion++] = errCode;
	memcpy(pSendbuf + postion, pData, len);
	postion += len;
//	Put_Return(command, postion++);
}

uint8_t ProcessCommand(uint8_t *pData, uint8_t command, uint16_t dataLen)
{
		uint8_t  i, tmp;
		uint8_t  ret = 0;
		uint8_t  *pSendbuf;
		uint16_t sendLength = 0;
//	#if (defined USER_FINGERMODE) && (defined USER_FINGERMODEVENA)
//	uint16 temp16;
//	#endif
//	uint32 tmp32;
//	static uint8  sTemp;
//	static uint8  sReadUserCnt = 0;
//	static uint8  sFlagAddUser = 0;
		uint8_t  buf[16];
//	static UserInfo_t tempUserInfo;
//	UTCTimeStruct *pTempTimeStruct = (UTCTimeStruct *)(buf + 6);
//	LogInfo_t *pTmpLogInfo = (LogInfo_t *)buf;
//	
		if (sProtocolAppFormat.headData.version != PROTOCOL_APP_VERSION)			// �ж�Э��ͷ��ȷ��
		{UserReturnErrCode(command, PROTOCOL_APP_ERR_VERSION);return ret;}
		if (dataLen == 0)
		{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);return ret;}
		if ((gSystemRunParam.flagInit != 0xA5) && (command != CMD_GET_AESKEY))
		{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);return ret;}
		switch (command)
		{
				case CMD_GET_HARD_INFO_VERSION:											/* ��ȡӲ���汾��Ϣ */
						UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, HARD_VERSION_INFO, sizeof(HARD_VERSION_INFO));
						break;
				
				case CMD_GET_SOFT_INFO_VERSION:											/* ��ѯ����汾��Ϣ */
						UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, SOFT_VERSION_INFO, sizeof(SOFT_VERSION_INFO));
						break;
				
				case CMD_GET_AESKEY:													/* ��ʼ�� */
				{
						if (*pData != 0x01)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
						
						if(gSystemRunParam.flagInit == 0xA5) break;
						
						pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
						pSendbuf[sendLength++] = PROTOCOL_APP_ERR_NONE;
						nrf_drv_rng_rand(tempkey, 16); 
//						LL_PseudoRand( tempkey, 16 );
						memcpy(pSendbuf + sendLength, tempkey, 16);
						sendLength += 16;
						f_tempkey = 1;
						memset(buf, 0, 8);
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, buf);
//						Put_Return(command, sendLength);
						gSystemRunParam.flagInit = 0xA5;
//						SaveSetup();
//						UserSoundSuccess();
				}
						break;
				
				case CMD_ADD_ADMIN:														/* ��ӹ���Ա */
				case CMD_ADD_USER:														/* ����û� */
				{
//						if (stateProcessCommand == 0)
//						{
//							if ((dataLen != 16) && (dataLen != 24))
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//							UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//							
//							if (dataLen == 16) 
//							{
//								sFlagAddUser = 1;
//								if (UserMemCmp(pData + 8, buf, 8) <= 0) break;
//								UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData + 8);
//							}
//							else 
//							{
//								sFlagAddUser = 11;
//								if (UserMemCmp(pData + 16, buf, 8) <= 0) break;
//								UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData + 16);
//							}
//							stateProcessCommand = 1;
//							ret = 0xFF;
//						}
//						else if (stateProcessCommand == 1)
//						{
//							ret = UserAddUserInfoToSystem(sFlagAddUser, 0, pData);
//							if (ret == 0)
//							{
//								sReadUserCnt = 0;
//								UserSetCurrentUser(pData);
//								UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, pData + 8, 16);
//							}
//							else if (ret == 0xFF)
//							{
//								
//							}
//							else
//							{
//								UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_GEN, pData + 8, 16);
//							}
//						}
//						else 
//						{
//							UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_GEN, pData + 8, 16);
//						}
				}
						break;
				
				case CMD_READ_USER:														/* ��ȡ�û� */
				{
//						if ((dataLen != 1) || (*pData != 0xA2))
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//						if (UserGetCurrentUser() == 0)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* δ��¼������Ȩ�޴��� */
//						while (1)
//						{
//							if (sReadUserCnt < MAX_USER_NUM)
//							{
//								if (UserReadUserInfoUID(sReadUserCnt, buf) == 0)
//								{
//									UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, buf, 8);
//									sReadUserCnt++;
//									break;
//								}
//								sReadUserCnt++;
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_FINISHED);
//								break;
//							}
//						}
				}
						break;
				
				case CMD_DEL_USER:														/* ɾ���û� */
				{
//						if (dataLen != 8)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//						if (UserGetCurrentUser() == 0)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* δ��¼������Ȩ�޴��� */
//						
//						UserDelUserInfoFromSystem(pData);
//						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
				}	
					break;
				
				case CMD_USER_LOGIN:													/* �û���¼ */
				{
//						if (dataLen != 16)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//						if (UserMemCmp(pData + 8, buf, 8) <= 0) break;
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData + 8);
//						tmp = UserSearchUserInfoNumber(pData);
//						if (tmp < MAX_USER_NUM)
//						{
//							sReadUserCnt = 0;
//							UserSetCurrentUser(pData);
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//						}
//						else
//						{
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//						}
				}
					break;
				
				case CMD_SET_KEYBOARD:													/* ���ü������� */
				{
//						if (stateProcessCommand == 0)
//						{
//							stateProcessCommand = 1;
//							ret = 0xFF;
//							
//							if (dataLen != 8)
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//							if (UserGetCurrentUser() == 0)
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* δ��¼������Ȩ�޴��� */
//						}
//						else if (stateProcessCommand == 1)
//						{
//							for (sTemp = 0; sTemp < MAX_USER_NUM; sTemp ++)
//							{
//								while (UserGetUserInfo(sTemp, &tempUserInfo) == 0xFF);
//								if ((tempUserInfo.flag & 0xC3) != 0xC2) continue;
//								if (UserMemCmp(tempUserInfo.userId, UserGetCurrentUser(), sizeof(tempUserInfo.userId)) == 0) continue;
//								if (UserMemCmp(tempUserInfo.password, pData, sizeof(tempUserInfo.password)) == 0)
//								{
//									break;
//								}
//							}
//							if (sTemp < MAX_USER_NUM)
//							{
//								stateProcessCommand = 3;
//							}
//							else
//							{
//								stateProcessCommand = 2;
//							}
//							ret = 0xFF;
//						}
//						else if (stateProcessCommand == 2)
//						{
//							ret = UserAddUserInfoToSystem(2, UserGetCurrentUser(), pData);
//							if (ret == 0)
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//							}
//							else if (ret == 0xFF)
//							{
//								
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//							}
//						}
//						else if (stateProcessCommand == 3)
//						{
//							osal_memset(buf, 0, 8);
//							ret = UserAddUserInfoToSystem(2, tempUserInfo.userId, buf);
//							if (ret == 0)
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_PASSWORD_SAME);
//							}
//							else if (ret == 0xFF)
//							{
//								
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//							}
//						}
				}
						break;
				
				case CMD_ADD_PERIPHERAL:												/* ������� */
				{
//						if (dataLen != 1)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//						if (UserGetCurrentUser() == 0)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* δ��¼������Ȩ�޴��� */
//						tmp = UserSearchUserInfoNumber(UserGetCurrentUser());
//						if (tmp >= MAX_USER_NUM)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}
//						if (*pData == 1)
//						{
//							#if (defined USER_FINGERMODE) && (!(defined USER_FINGERMODEVENA))
//							if (UserFingerModuleAddition(tmp, UserFingerAdditionCallback) == FALSE)
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//								break;
//							}
//							#else
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//							UserSoundFailuer();
//							break;
//							#endif
//						}
//						else if (*pData == 2)
//						{
//							#ifdef USER_ICCARD
//							if (UserICCardAddition(UserGetCurrentUser()) == FALSE)
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//								break;
//							}
//							#else
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//							UserSoundFailuer();
//							break;
//							#endif
//							
//						}
//						else if (*pData == 4)
//						{
//							#if (defined USER_FINGERMODE) && (defined USER_FINGERMODEVENA)
//							if (UserFingerModuleAddition(tmp, UserFingerAdditionCallback) == FALSE)
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//								break;
//							}
//							#else
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//							UserSoundFailuer();
//							break;
//							#endif
//						}
//						else
//						{
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//							UserSoundFailuer();
//							break;
//						}
//						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
				}
						break;
					
				case CMD_SWITCH:														/* ����ָ�� */
				{
//						if (dataLen != 9)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//						if (UserMemCmp(pData, buf, 8) <= 0) break;
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData);
//						pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
//						pSendbuf[sendLength ++] = PROTOCOL_APP_ERR_NONE;
//						pSendbuf[sendLength ++] = gSystemRunParam.batterPercent;
//						tmp = 0;
//						if (gFlagAdjustTime <= 1) {tmp |= BV(0);}
//						if (UserGetLogInfo(0) == 1) {tmp |= BV(1);}
//						pSendbuf[sendLength ++] = tmp;
//						Put_Return(command, sendLength);
//						if (pData[8] == 0xA1)
//							UserMotorON(motorOpenTyp_mobile);
//						else if (pData[8] == 0xA2)
//							UserMotorOFF();
				}
						break;
				
				case CMD_ADJUST_TIME:													/* Уʱָ�� */
				{
//						#if PROTOCOL_APP_VERSION == 0x0100
//			/****************************** V1.0 �汾����ʼ *****************************/
//						if (dataLen != 4)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//						//if (UserGetCurrentUser() == 0)
//						//{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* δ��¼������Ȩ�޴��� */
//						tmp32 = pData[0];
//						tmp32 = (tmp32 << 8) | pData[1];
//						tmp32 = (tmp32 << 8) | pData[2];
//						tmp32 = (tmp32 << 8) | pData[3];
//			/****************************** V1.0 �汾������� *****************************/
//						#else
//			/****************************** V1.1 �汾����ʼ *****************************/
//						if (dataLen != 12)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//						
//						UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//						if (UserMemCmp(pData, buf, 8) <= 0) break;
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData);
//						
//						tmp32 = pData[8];
//						tmp32 = (tmp32 << 8) | pData[9];
//						tmp32 = (tmp32 << 8) | pData[10];
//						tmp32 = (tmp32 << 8) | pData[11];
//			/****************************** V1.1 �汾������� *****************************/
//						#endif
//						
//						if (tmp32 < SECONDS2000YEAR)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						tmp32 -= SECONDS2000YEAR;
//						osal_setClock(tmp32);
//						osal_ConvertUTCTime( pTempTimeStruct, tmp32 );
//						buf[0] = pTempTimeStruct->year - 2000;
//						buf[1] = pTempTimeStruct->month + 1;
//						buf[2] = pTempTimeStruct->day + 1;
//						buf[3] = pTempTimeStruct->hour;
//						buf[4] = pTempTimeStruct->minutes;
//						buf[5] = pTempTimeStruct->seconds;
//						UserSetPCF8563Hex(buf);
//						gFlagAdjustTime = 60000; /* ��Լһ��ʱ�� */
//						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
				}
						break;
				
				case CMD_READ_LOG:														/* ��ȡ��־ */
				{
//						pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
//						if (UserGetLogInfo(pTmpLogInfo) == 1)
//						{
//							pSendbuf[sendLength ++] = PROTOCOL_APP_ERR_NONE;
//							osal_memcpy(pSendbuf + sendLength, pTmpLogInfo->userId, sizeof(pTmpLogInfo->userId));
//							sendLength += sizeof(pTmpLogInfo->userId);
//							pTmpLogInfo->time += SECONDS2000YEAR;
//							pSendbuf[sendLength ++] = pTmpLogInfo->time >> 24;
//							pSendbuf[sendLength ++] = pTmpLogInfo->time >> 16;
//							pSendbuf[sendLength ++] = pTmpLogInfo->time >> 8;
//							pSendbuf[sendLength ++] = pTmpLogInfo->time >> 0;
//							pSendbuf[sendLength ++] = pTmpLogInfo->action;
//						}
//						else 
//						{
//							pSendbuf[sendLength ++] = PROTOCOL_APP_ERR_FINISHED;
//							osal_memset(pSendbuf + sendLength, 0, 13);
//							sendLength += 13;
//						}
//						Put_Return(command, sendLength);
				}
						break;
			
				case CMD_READ_USER_CONFIG:												/* ��ȡ�û���Ϣ */
				{
//						if ((dataLen != 1) || (*pData != 0xAB))
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//						if (UserGetCurrentUser() == 0)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* δ��¼������Ȩ�޴��� */
//						
//						if (UserReadUserInfoConfig(UserGetCurrentUser(), &tmp) == 0)
//						{
//							buf[0] = 0; buf[1] = 0;
//							if (tmp & 0x40) buf[1] |= 0x01;
//							if (tmp & 0x20) buf[1] |= 0x02;
//							if (tmp & 0x10) buf[1] |= 0x04;
//							if (tmp & 0x08) buf[1] |= 0x08;
//							UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, buf, 2);
//						}
//						
//						while (1)
//						{
//							if (sReadUserCnt < MAX_USER_NUM)
//							{
//								if (UserReadUserInfoUID(sReadUserCnt, buf) == 0)
//								{
//									UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, buf, 8);
//									sReadUserCnt++;
//									break;
//								}
//								sReadUserCnt++;
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_FINISHED);
//								break;
//							}
//						}
				}
						break;
				
				case CMD_DEL_LOG:														/* ɾ����־ */
				{
//						//	if (dataLen != 16)
//						//	{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* ���ݳ��ȴ��󣬷��ز������� */
//							osal_memcpy(pTmpLogInfo->userId, pData, sizeof(pTmpLogInfo->userId));
//							tmp32 = pData[sizeof(pTmpLogInfo->userId) + 0];
//							tmp32 = (tmp32 << 8) | pData[sizeof(pTmpLogInfo->userId) + 1];
//							tmp32 = (tmp32 << 8) | pData[sizeof(pTmpLogInfo->userId) + 2];
//							tmp32 = (tmp32 << 8) | pData[sizeof(pTmpLogInfo->userId) + 3];
//							pTmpLogInfo->time = tmp32 - SECONDS2000YEAR;
//							if (UserDelLogInfo(pTmpLogInfo) == 1)
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//							}
//							else 
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//							}
					}
							break;
					
				case CMD_FACTORYRESET:													/* �ָ��������� */
				{
//						if (dataLen != 129)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						if (pData[8] != 0xAA)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//						if (UserMemCmp(pData, buf, 8) <= 0) break;
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData);
//						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//						UserClearUserInfoFromSystem();
//						gSystemRunParam.flagInit = 0x00;									// �������Ա������
//						SaveSetup();														// ��������
//						Save_SysRunState(1);												// ��ӹ���Աģʽ
//						WaitSystemExit(0, 20000);
//						ChangescanRspData(sizeof ( lockinitname ), (uint8*)lockinitname);	// ��ʼ���㲥����
//						UserSaveAppData(P_EE_ADDR_LOCKNAME, (uint8*)lockinitname);			// update to EEPROM
//						UserStartAdv(1, 0);													// ������ʼ�㲥
//						UserSoundSuccess();
				}
						break;
				
				case CMD_TERFILEDATA:													/* д�豸�ļ����� */
				{
//						#if (defined USER_FINGERMODE) && (defined USER_FINGERMODEVENA)
//						temp16 = pData[0];
//						temp16 = (temp16 << 8) | pData[1]; /* ��ȡ����� */
//						if (temp16 == 0) /* �ļ���Ϣ */
//						{
//							if (pData[2] != 1)  /* �ж�ָ������ */
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//							tmp32 = pData[3];
//							tmp32 = (tmp32 << 8) | pData[4];
//							tmp32 = (tmp32 << 8) | pData[5];
//							tmp32 = (tmp32 << 8) | pData[6]; /* ��ȡ�ļ����� */
//							sTransferFile.dataLength = tmp32;
//							tmp32 = pData[7];
//							tmp32 = (tmp32 << 8) | pData[8];
//							tmp32 = (tmp32 << 8) | pData[9];
//							tmp32 = (tmp32 << 8) | pData[10]; /* ��ȡ�ļ��ۼӺ� */
//							sTransferFile.dataSum = tmp32;
//							
//							tmp = UserSearchUserInfoNumber(pData + 11);
//							if (tmp >= MAX_USER_NUM)
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN); break;}
//							
//							UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//			//				if (UserMemCmp(pData + 19, buf, 8) <= 0) break;
//							UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData + 19);
//							
//							UserFingerModuleAddUserDataStart(tmp, sTransferFile.dataLength, UserTransferFileCallback);
//						}
//						else if (temp16 == 0xFFFF) /* ������� */
//						{
//							UserFingerModuleAddUserDataEnd(0xFFFF, sTransferFile.dataSum);
//						}
//						else /* �ļ����� */
//						{
//							if (sTransferFile.currentNum != temp16)
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//							UserFingerModuleAddUserDataIng(temp16, pData + 2);
//						}
//						#else
//						UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
//						#endif
				}
						break;
				
				case CMD_PARAM_CONFIG:													/* ��������ָ�� */
				{
//						if (dataLen < 9)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//						if (UserMemCmp(pData, buf, 8) <= 0) break;
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData);
//						if (pData[8] == 0x01)	// ���ÿ�������
//						{
//							if ((pData[9] == 0x01) || pData[9] == 0x02)
//							{
//								UserSaveAppData(P_EE_ADDR_OPENLOCKDIR, pData + 9);
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//								SC_ProtocolOpenLockDir(pData[9]);
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//							}
//						}
//						else if (pData[8] == 0x10)	// ����IP��ַ�Ͷ˿ں�
//						{
//							UserSaveAppData(P_EE_ADDR_NB_IP, pData + 9);
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//						}
//						else if (pData[8] == 0x11)	// ����APN
//						{
//							UserSaveAppData(P_EE_ADDR_NB_APN, pData + 9);
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//						}
//						else
//						{
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//						}
				}
						break;
					
				case CMD_PARAM_READ:													/* ������ȡָ�� */
				{
//						if (dataLen < 9)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//						UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//						if (UserMemCmp(pData, buf, 8) <= 0) break;
//						UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData);
//						if (pData[8] == 0x12)	// ����IMEI
//						{
//							if (gSystemRunParam.flagAny & 0x01)
//							{
//								i = 0;
//								pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
//								pSendbuf[i ++] = PROTOCOL_APP_ERR_NONE;
//								UserGetAppData(P_EE_ADDR_NB_IMEI, pSendbuf + i);
//								tmp = SC_StrPrintlen(pSendbuf + i) % 17;
//								pSendbuf[i + tmp] = 0; i ++;
//								i += tmp;
//								Put_Return(command, i);
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_FOUND_NB);
//							}
//						}
//						else if (pData[8] == 0x13)	// ICCID
//						{
//							if (gSystemRunParam.flagAny & 0x02)
//							{
//								i = 0;
//								pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
//								pSendbuf[i ++] = PROTOCOL_APP_ERR_NONE;
//								UserGetAppData(P_EE_ADDR_NB_ICCID, pSendbuf + i);
//								tmp = SC_StrPrintlen(pSendbuf + i) % 30;
//								pSendbuf[i + tmp] = 0; i ++;
//								i += tmp;
//								Put_Return(command, i);
//							}
//							else
//							{
//								UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_FOUND_SIM);
//							}
//						}
//						else
//						{
//							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//						}
				}
						break;
				default:
						UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
						break;
		}
		
		if (ret == 0xFF)
		{
			
		}
		else
		{
//				stateProcessCommand = 0;
		}
		
		return ret;
}


