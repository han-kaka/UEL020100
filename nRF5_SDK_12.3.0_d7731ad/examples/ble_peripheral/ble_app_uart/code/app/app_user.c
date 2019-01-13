#include "app_user.h"

uint16_t gFlagAdjustTime = 0;			/* 校时计数 */

SystemRunParam_t gSystemRunParam;

static uint8_t sLoginFlag = 0, sCurrentUserUID[8];

/**************************************************************************************************
 * @brief       ??????
 * @param       src-??
 * @return      1-(src1>src2); 0-(src1==src2); -1-(src1<src2);
 **************************************************************************************************
 */
int8_t UserMemCmp( const void *src1, const void *src2, unsigned int len )
{
		const uint8_t *pSrc1;
		const uint8_t *pSrc2;

		pSrc1 = src1;
		pSrc2 = src2;

		while (len--)
		{
				if (*pSrc1 > *pSrc2)
						return 1;
				else if (*pSrc1 < *pSrc2)
						return -1;
				pSrc1++;
				pSrc2++;
		}
		return 0;
}

void UserSetCurrentUser(uint8_t *uid)
{
	sLoginFlag = 1;
	memcpy(sCurrentUserUID, uid, 8);
}

uint8_t* UserGetCurrentUser(void)
{
	if (sLoginFlag)
		return sCurrentUserUID;
	else
		return 0;
}

void UserClrCurrentUser(void)
{
	sLoginFlag = 0;
	memset(sCurrentUserUID, 0, 8);
}

/**************************************************************************************************
 * @brief       读取用户信息编号
 * @param       id：用户UID
 * @return      用户信息编号
 **************************************************************************************************
 */
uint8_t UserSearchUserInfoNumber(uint8_t *id)
{
	uint8_t  i;
	UserInfo_t tempUserInfo;
	
	for (i = 0; i < MAX_USER_NUM; i ++)
	{
		while (UserGetUserInfo(i, &tempUserInfo) == 0xFF);
		if (((tempUserInfo.flag & 0x83) == 0x82) && (UserMemCmp(tempUserInfo.userId, id, 8) == 0))
		{
			break;
		}
	}
	return i;
}

/**************************************************************************************************
 * @brief       读取用户配置信息
 * @param       id：用户UID
 * @param       dat：用户配置信息
 * @return      0-成功；1-失败；
 **************************************************************************************************
 */
uint8_t UserReadUserInfoConfig(uint8_t *uid, uint8_t *dat)
{
	uint8_t  tmp, ret = 1;
	UserInfo_t tempUserInfo;
	
	tmp = UserSearchUserInfoNumber(uid);
	if (tmp < MAX_USER_NUM)
	{
		while (UserGetUserInfo(tmp, &tempUserInfo) == 0xFF);
		if (dat !=0 ) *dat = tempUserInfo.flag;
		ret = 0;
	}
	
	return ret;
}

////==============================================================================
////                           * 锁的设置参数 *
////==============================================================================
//uint8 SaveSetup(void)															// 保存设置到 EEPROM
//{
//	uint8  tmp;
//	
//	tmp = UserSaveAppData(P_EE_ADDR_INIT, &gSystemRunParam.flagInit);			// 配置改变时需要与 EEPROM 同步
//	if (gSystemRunParam.flagInit != 0xA5)
//	{
//		osal_memcpy(useraeskeybuf, defualtaes128key, 16);
//	}
//	if (gSystemRunParam.flagInit == 0xA5)
//	{
//		Save_SysRunState(2);
//	}
//	ChangeAdvData(2, gSystemRunParam.flagInit);									// 更新广播数据
//	
//	return tmp;
//}

//uint8 GetSetup(void)															// 从 EEPROM 获取配置
//{
//    uint8 tmp;
//	
//	tmp	= UserGetAppData(P_EE_ADDR_INIT, &gSystemRunParam.flagInit);
//    if ((gSystemRunParam.flagInit != 0xA5) && (gSystemRunParam.flagInit != 0x55))
//	{ gSystemRunParam.flagInit = 0; SaveSetup(); }								// 判断设置信息无效时，然后将其清零重新同步到 EEPROM 
//    else 
//	{ ChangeAdvData(2, gSystemRunParam.flagInit); }								// 否则改变广播状态
//	
//	return tmp;
//}

////==============================================================================
////                           * 锁的设置参数 *
////==============================================================================
//uint8 UserEepromInit(void)
//{
////	LogInfoIndex_t tmpLogInfoIndex;
////	
////	tmpLogInfoIndex.flag = 0xA0;
////	tmpLogInfoIndex.head = 0;
////	tmpLogInfoIndex.tail = 0;
////	return UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
//	return 1;
//}