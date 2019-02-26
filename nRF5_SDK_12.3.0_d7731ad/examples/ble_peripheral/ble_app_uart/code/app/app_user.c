#include "app_user.h"

uint16_t gFlagAdjustTime = 0;			/* 校时计数 */

SystemRunParam_t gSystemRunParam; // 系统运行参数
UserInfo_t  gUserInfo; // 用户信息
uint16_t gCurrentLocalUser = 0xFFFF; // 当前操作本地用户的序号
uint8_t  gFlagHardRunning = 0; /* 硬件在运行中；bit0-蜂鸣器； */

static bool   sFlagPwrmgrHold = false;
static uint8_t  sLoginFlag = 0, sCurrentUserUID[8];


///**
//* @function SC_GenStrToInt
//* @brief    普通字符串转换为32位
//* @param    pdat 普通字符串
//* @param    None
//* @retval   32位数据
//*/
//uint32 SC_GenStrToInt(uint8 *pdat)
//{
//	uint8  i;
//	uint32 temp32 = 0;
//	
//	for (i = 0; i < 10; i ++)
//	{
//		if ((pdat[i] >= '0') && (pdat[i] <= '9'))
//		{
//			temp32 = (temp32 * 10) + (pdat[i] - '0');
//		}
//		else 
//		{
//			break;
//		}
//	}
//	return temp32;
//}

///**
//* @function SC_Strlen
//* @brief    返回ch字符串的长度
//* @param    ch 字符串
//* @retval   返回ch字符串的长度
//*/
//uint16 SC_Strlen(uint8 *ch)
//{
//	uint16  i = 0;
//	
//	while (*ch++)
//	{
//		if (++i >= 0xFFFF) break;
//	}
//	
//	return i;
//}

/**
* @function SC_StrPrintlen
* @brief    返回ch字符串的长度
* @param    ch 字符串
* @retval   返回ch字符串的长度
*/
uint16_t SC_StrPrintlen(uint8_t *ch)
{
	uint16_t  i = 0;
	
	while ((*ch >= 0x20) && (*ch <= 0x7F))
	{
		ch ++;
		if (++i >= 0xFFFF) break;
	}
	
	return i;
}

///**
//* @function SC_Strstr
//* @brief    在数组ch1中查找数组ch2
//* @param    ch1 数组1
//* @param    ch2 数组2
//* @param    len1 数组1长度
//* @param    len2 数组2长度
//* @retval   返回查找的地址;0-无效;
//*/
//uint8* SC_Strstr(uint8 *ch1, uint8 *ch2, uint16 len1, uint16 len2)
//{
//	uint16 i, j = 0;
//	uint8  *p = 0;
//	
//	if (len2 > len1)
//	{
//		return 0;
//	}
//	if (len2 == 0)
//	{
//		return ch1;
//	}
//	
//	for (i = 0; i < len1; i ++)
//	{
//		if (ch1[i] == ch2[j])
//		{
//			if (++j >= len2)
//			{
//				p = ch1 + i + 1 - len2;
//				break;
//			}
//		}
//		else 
//		{
//			j = 0;
//			if (ch1[i] == ch2[j])
//			{
//				j++;
//			}
//		}
//	}
//	
//	return p;
//}


/**************************************************************************************************
 * @brief       数组比较函数
 * @param       src-数组
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

///**************************************************************************************************
// * @brief       通过UID读取用户信息
// * @param       id：用户UID
// * @return      0-失败；1-成功；
// **************************************************************************************************
// */
//uint8 UserSearchUserInfoUID(uint16 num, uint8 *id)
//{
//	UserInfo_t tempUserInfo;
//	
//	while (UserGetUserInfo(num, &tempUserInfo) == 0xFF);
//	if ((tempUserInfo.flag & 0x83) == 0x82)
//	{
//		if (id != 0) osal_memcpy(id, tempUserInfo.userId, sizeof(tempUserInfo.userId));
//		return 1;
//	}
//	return 0;
//}
			
/**************************************************************************************************
 * @brief       读取用户UID
 * @param       num：用户内存编号
 * @param       id：用户UID
 * @return      0-成功；1-失败；
 **************************************************************************************************
 */
uint8_t UserReadUserInfoUID(uint8_t num, uint8_t *uid)
{
		uint8_t  ret = 1;
		UserInfo_t tempUserInfo;
		
		while (UserGetUserInfo(num, &tempUserInfo) == 0xFF);
		if ((tempUserInfo.flag & 0x83) == 0x82)
		{
				if (uid != 0) UserMemCmp(uid, tempUserInfo.userId, 8);
				ret = 0;
		}
		
		return ret;
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

///**************************************************************************************************
// * @brief       添加用户信息
// * @param       type：1-添加用户；2-添加密码；3-添加指纹；4-添加 RFID ；5-蓝牙钥匙；6-指静脉；11-添加用户带有效时间；
// * @return      0-成功；0xFF-未完成；other-失败；
// **************************************************************************************************
// */
//uint8 UserAddUserInfoToSystem(uint8 type, uint8 *id, uint8 *pData)
//{
//	uint8  i, tmp, ret = 1;
//	static uint8  sFlagState = 0;
//	static uint8  sUserId = 0;
//	static UserInfo_t tempUserInfo;
//	
//	if (type == 0)
//	{
//		for (i = 0; i < MAX_USER_NUM; i ++)
//		{
//			while (UserGetUserInfo(i, &tempUserInfo) == 0xFF);
//			if (((tempUserInfo.flag & 0x83) == 0x82) && (UserMemCmp(tempUserInfo.userId, id, 8) == 0))
//			{
//				ret = 0;
//			}
//		}
//	}
//	else if ((type == 1) || (type == 11)) /* 添加用户 */
//	{
//		if (sFlagState == 0)
//		{
//			sFlagState = 1;
//			sUserId = UserSearchUserInfoNumber(pData);
//			ret = 0xFF;
//		}
//		else if (sFlagState == 1)
//		{
//			sFlagState = 2;
//			ret = 0xFF;
//			if (sUserId < MAX_USER_NUM) // 已有此用户
//			{
//				while (UserGetUserInfo(sUserId, &tempUserInfo) == 0xFF);
//				if (type == 11) /* 添加用户有效时间 */
//				{
//					tempUserInfo.startTime = pData[8];
//					tempUserInfo.startTime = (tempUserInfo.startTime << 8) + pData[9];
//					tempUserInfo.startTime = (tempUserInfo.startTime << 8) + pData[10];
//					tempUserInfo.startTime = (tempUserInfo.startTime << 8) + pData[11];
//					tempUserInfo.endTime = pData[12];
//					tempUserInfo.endTime = (tempUserInfo.endTime << 8) + pData[13];
//					tempUserInfo.endTime = (tempUserInfo.endTime << 8) + pData[14];
//					tempUserInfo.endTime = (tempUserInfo.endTime << 8) + pData[15];
//					if (tempUserInfo.startTime > SECONDS2000YEAR) tempUserInfo.startTime -= SECONDS2000YEAR;
//					else tempUserInfo.startTime = 0;
//					if (tempUserInfo.endTime > SECONDS2000YEAR) tempUserInfo.endTime -= SECONDS2000YEAR;
//					else tempUserInfo.endTime = 0;
//				}
//				else
//				{
//					tempUserInfo.startTime = 0;
//					tempUserInfo.endTime = 0xFFFFFFFF;
//				}
//			}
//			else // 新用户
//			{
//				tmp = 1;
//				for (i = 0; i < MAX_USER_NUM; i ++)
//				{
//					while (UserGetUserInfo(i, &tempUserInfo) == 0xFF);
//					if (((tempUserInfo.flag & 0x03) != 0x02) || ((tempUserInfo.flag & 0x80) == 0))
//					{
//						tmp = 0;
//					}
//					else if (tempUserInfo.endTime < osal_getClock())
//					{
//						tmp = 0;
//					}
//					if (tmp == 0)
//					{
//						tempUserInfo.flag = 0x82;
//						osal_memcpy(tempUserInfo.userId, pData, 8);
//						if (type == 11) /* 添加用户有效时间 */
//						{
//							tempUserInfo.startTime = pData[8];
//							tempUserInfo.startTime = (tempUserInfo.startTime << 8) + pData[9];
//							tempUserInfo.startTime = (tempUserInfo.startTime << 8) + pData[10];
//							tempUserInfo.startTime = (tempUserInfo.startTime << 8) + pData[11];
//							tempUserInfo.endTime = pData[12];
//							tempUserInfo.endTime = (tempUserInfo.endTime << 8) + pData[13];
//							tempUserInfo.endTime = (tempUserInfo.endTime << 8) + pData[14];
//							tempUserInfo.endTime = (tempUserInfo.endTime << 8) + pData[15];
//							if (tempUserInfo.startTime > SECONDS2000YEAR) tempUserInfo.startTime -= SECONDS2000YEAR;
//							else tempUserInfo.startTime = 0;
//							if (tempUserInfo.endTime > SECONDS2000YEAR) tempUserInfo.endTime -= SECONDS2000YEAR;
//							else tempUserInfo.endTime = 0;
//						}
//						else
//						{
//							tempUserInfo.startTime = 0;
//							tempUserInfo.endTime = 0xFFFFFFFF;
//						}
//						sUserId = i;
//						break;
//					}
//				}
//			}
//		}
//		else if (sFlagState == 2)
//		{
//			if (sUserId < MAX_USER_NUM)
//			{
//				ret = UserSaveUserInfo(sUserId, &tempUserInfo);
//			}
//		}
//	}
//	else if (type == 2) /* 添加键盘密码 */
//	{
//		if (sFlagState == 0)
//		{
//			sFlagState = 1;
//			sUserId = UserSearchUserInfoNumber(id);
//			if (sUserId < MAX_USER_NUM)
//			{
//				while (UserGetUserInfo(sUserId, &tempUserInfo) == 0xFF);
//				if (((tempUserInfo.flag & 0x83) == 0x82) && (UserMemCmp(tempUserInfo.userId, id, 8) == 0))
//				{
//					tempUserInfo.flag |= 0x40;
//					osal_memcpy(tempUserInfo.password, pData, 8);
//					ret = 0xFF;
//				}
//			}
//		}
//		else if (sFlagState == 1)
//		{
//			if (sUserId < MAX_USER_NUM)
//			{
//				ret = UserSaveUserInfo(sUserId, &tempUserInfo);
//			}
//		}
//	}
//	else if (type == 3) /* 添加指纹 */
//	{
//		if (sFlagState == 0)
//		{
//			sFlagState = 1;
//			sUserId = UserSearchUserInfoNumber(id);
//			if (sUserId < MAX_USER_NUM)
//			{
//				while (UserGetUserInfo(sUserId, &tempUserInfo) == 0xFF);
//				if (((tempUserInfo.flag & 0x83) == 0x82) && (UserMemCmp(tempUserInfo.userId, id, 8) == 0))
//				{
//					tempUserInfo.flag |= 0x20;
//					tempUserInfo.fingerId = *pData;
//					tempUserInfo.fingerId <<= 8;
//					tempUserInfo.fingerId |= *(pData + 1);
//					ret = 0xFF;
//				}
//			}
//		}
//		else if (sFlagState == 1)
//		{
//			if (sUserId < MAX_USER_NUM)
//			{
//				ret = UserSaveUserInfo(sUserId, &tempUserInfo);
//			}
//		}
//	}
//	else if (type == 4) /* 添加 RFID */
//	{
//		if (sFlagState == 0)
//		{
//			sFlagState = 1;
//			sUserId = UserSearchUserInfoNumber(id);
//			if (sUserId < MAX_USER_NUM)
//			{
//				while (UserGetUserInfo(sUserId, &tempUserInfo) == 0xFF);
//				if (((tempUserInfo.flag & 0x83) == 0x82) && (UserMemCmp(tempUserInfo.userId, id, 8) == 0))
//				{
//					tempUserInfo.flag |= 0x10;
//					osal_memcpy(tempUserInfo.cardId, pData, 8);
//					ret = 0xFF;
//				}
//			}
//		}
//		else if (sFlagState == 1)
//		{
//			if (sUserId < MAX_USER_NUM)
//			{
//				ret = UserSaveUserInfo(sUserId, &tempUserInfo);
//			}
//		}
//	}
//	else if (type == 6) /* 添加指静脉 */
//	{
//		if (sFlagState == 0)
//		{
//			sFlagState = 1;
//			sUserId = UserSearchUserInfoNumber(id);
//			if (sUserId < MAX_USER_NUM)
//			{
//				while (UserGetUserInfo(sUserId, &tempUserInfo) == 0xFF);
//				if (((tempUserInfo.flag & 0x83) == 0x82) && (UserMemCmp(tempUserInfo.userId, id, 8) == 0))
//				{
//					tempUserInfo.flag |= 0x04;
//					tempUserInfo.fingerId = *pData;
//					tempUserInfo.fingerId <<= 8;
//					tempUserInfo.fingerId |= *(pData + 1);
//					ret = 0xFF;
//				}
//			}
//		}
//		else if (sFlagState == 1)
//		{
//			if (sUserId < MAX_USER_NUM)
//			{
//				ret = UserSaveUserInfo(sUserId, &tempUserInfo);
//			}
//		}
//	}
//	else
//	{
//		
//	}
//	
//	if (ret == 0xFF)
//	{
//		
//	}
//	else if (ret == 0)
//	{
//		sFlagState = 0;
//	}
//	else
//	{
//		sFlagState = 0;
//	}
//	
//	return ret;
//}

///**************************************************************************************************
// * @brief       删除用户信息
// * @param       id：用户UID
// * @return      删除的位置编号
// **************************************************************************************************
// */
//uint8 UserDelUserInfoFromSystem(uint8 *uid)
//{
//	uint8  tmp;
//	UserInfo_t tempUserInfo;
//	
//	tmp = UserSearchUserInfoNumber(uid);
//	if (tmp < MAX_USER_NUM)
//	{
//		while (UserGetUserInfo(tmp, &tempUserInfo) == 0xFF);
//		osal_memset(&tempUserInfo, 0, sizeof(UserInfo_t));
//		#ifdef FLASHUSERINFO
//		UserDelUserInfo(tmp);
//		#else
//		UserSaveUserInfo(tmp, &tempUserInfo);
//		#endif
//	}
//	
//	return tmp;
//}

///**************************************************************************************************
// * @brief       清空用户信息
// * @param       none
// * @return      none
// **************************************************************************************************
// */
//void UserClearUserInfoFromSystem(void)
//{
//	uint8  i;
//	UserInfo_t tempUserInfo;
//	
//	for (i = 0; i < MAX_USER_NUM; i ++)
//	{
//		while (UserGetUserInfo(i, &tempUserInfo) == 0xFF);
//		osal_memset(&tempUserInfo, 0, sizeof(UserInfo_t));
//		#ifdef FLASHUSERINFO
//		UserDelUserInfo(i);
//		#else
//		UserSaveUserInfo(i, &tempUserInfo);
//		#endif
//	}
//}

////==============================================================================
////                         * 系统的当前执行状态 *
////==============================================================================
////======LockState======
////0:初始化
////1:设置模式(添加管理员等)
////2:可以自动开门
////3:密码输入状态
//void Save_SysRunState(uint8 sta)                                                //保存系统工作状态
//{
//	gSystemRunParam.nowrunstate=sta;
//}
//uint8 Get_SysRunState(void)
//{
//	return gSystemRunParam.nowrunstate;
//}
////==============================================================================
////                         * 开始进入低功耗计时 *
////ty:
////      1:
////==============================================================================
//void WaitSystemExit(uint8 type, uint32 ms)
//{
//	if ( ms > osal_get_timeoutEx( simpleBLEPeripheral_User_TaskID, USER_WAITSYSEXIT_EVT ) )
//	{
//		osal_start_timerEx( simpleBLEPeripheral_User_TaskID, USER_WAITSYSEXIT_EVT, ms ); // 延时等待再次进入低功耗模式
//	}
//	if ( type > 0 )
//	{
//		if (!sFlagPwrmgrHold)
//		{
//			sFlagPwrmgrHold = TRUE;
//			osal_pwrmgr_task_state( simpleBLEPeripheral_User_TaskID, PWRMGR_HOLD );	// 不进入低功耗模式
//		}
//	}
//	osal_stop_timerEx( simpleBLEPeripheral_User_TaskID, USER_ALLADVINFOUPDATA_EVT ); // 停止广播更新事件
//}
////==============================================================================
////                         * 立即进入低功耗模式 *
////==============================================================================
//void NowSystemExit(void)
//{
//	if (gFlagHardRunning & 0x01)
//	{
//		osal_start_timerEx( simpleBLEPeripheral_User_TaskID, USER_WAITSYSEXIT_EVT, 100 );
//	}
//	else
//	{
//		if (gSystemRunParam.flagInit == 0xA5)
//		{
//			Save_SysRunState(2);													//系统进入可开锁低功耗模式
//		}
//		else
//		{
//			Save_SysRunState(1);													//系统进入可添加用户低功耗模式
//		}
//		UserStartAdv(0, 0);															//断开连接、停止广播;
//		{
//			sFlagPwrmgrHold = FALSE;
//			osal_pwrmgr_task_state( simpleBLEPeripheral_User_TaskID, PWRMGR_CONSERVE );	//进入低功耗模式
//		}
//		osal_start_timerEx( simpleBLEPeripheral_User_TaskID, USER_ALLADVINFOUPDATA_EVT, 30000 ); // 重置广播更新事件
//	}
//}

//void UserHardDelayNop(uint16 us)
//{
//	while (us --) asm("NOP");
//}

//void UserHardDelayMs(uint16 ms)
//{
//	uint16  i;
//	
//	while (ms --)
//		for (i = 0; i < 1600; i ++)
//			asm("NOP");
//}

//void UserHardWDGDelayMs(uint16 ms)
//{
//	uint16  i;
//	
//	while (ms --)
//		for (i = 0; i < 800; i ++)
//		{
//			WDCTL = 0xA0;
//			WDCTL = 0x50;
//		}
//}
////==============================================================================
////                            * 测试用程序 *
////==============================================================================
//void UserInitTest(void)
//{
//	uint8  tmp = 0, buf[16] = "6952697592";
//	
//	if (tmp != 0) tmp = 0;														// 无意义
//	UserHardWDGDelayMs(200);													// 初始上电延时

//	buf[0] = 0x5a;
//	tmp = UserSaveAppData(P_EE_ADDR_TSET, buf);									// 测试写入

//	buf[0] = buf[1] = 0x00;
//	HalI2CInit( 0xa2 >> 1, i2cClock_123KHZ );
//	HalI2C8BitAddrReadReg(2, &buf[9], 1);										// 读取时间秒
//	UserHardWDGDelayMs(1010);													// 延时
//	HalI2C8BitAddrReadReg(2, &buf[10], 1);										// 再次读取秒
//	while (buf[10] == buf[9])													// 判断时钟芯片是否正常运行
//	{
//		HalI2C8BitAddrReadReg(2, &buf[9], 1);									// 读取时间秒
//		UserHardWDGDelayMs(1010);													// 延时
//		HalI2C8BitAddrReadReg(2, &buf[10], 1);									// 再次读取秒
//		if (buf[10] != buf[9]) break;
//		BUZZER_ON; UserHardWDGDelayMs(100);
//		BUZZER_OFF; UserHardWDGDelayMs(100);
//		BUZZER_ON; UserHardWDGDelayMs(100);
//		BUZZER_OFF; UserHardWDGDelayMs(100);
//	}

//	buf[0] = 0;
//	tmp = UserGetAppData(P_EE_ADDR_TSET, buf);									// 获取测试的数据
//	while (buf[0] != 0x5a)														// 判断EEPROM是否正常工作
//	{
//		buf[0] = 0x5a;
//		tmp = UserSaveAppData(P_EE_ADDR_TSET, buf);								// 测试写入
//		BUZZER_ON; UserHardWDGDelayMs(100);
//		BUZZER_OFF; UserHardWDGDelayMs(100);
//		BUZZER_ON; UserHardWDGDelayMs(100);
//		BUZZER_OFF; UserHardWDGDelayMs(100);
//		buf[0] = 0;
//		tmp = UserGetAppData(P_EE_ADDR_TSET, buf);								// 获取测试的数据
//		BUZZER_ON; UserHardWDGDelayMs(100);
//		BUZZER_OFF; UserHardWDGDelayMs(1000);
//	}
//}
//void InitWatchDog(uint8 wtime)
//{
//	WDCTL = 0x00; // 打开IDLE
//	WDCTL |= wtime; 
//}
////////////////////////////////////////////////
//void FeetDog(void)
//{
//	WDCTL = 0xA0;
//	WDCTL = 0x50;
//}
////==============================================================================
////                           * 用户程序初始化 *
////==============================================================================
//void UserHalInit(void) // user init
//{
//	uint8  buf[6];
//	uint32 temp32;
//	UTCTimeStruct tempUTCTimeStruct;
//	
//	P2INP |= BV(6);
//#ifdef PCB_L1_2_3
//	P0DIR = 0xFF; P0 = 0x00;
//	P1DIR = 0xFF; P1 = 0x00;
//	P2DIR |= 0x01; P2_0 = 0;
//	
//	P0_2 = 1;
//	P0_3 = 1;
//	
//	P1INP |= BV(2);
//	P1INP |= BV(3);
//	
//	P1DIR &= ~BV(2);
//	P1DIR &= ~BV(3);
//	P1DIR &= ~BV(5);
//	P1DIR &= ~BV(6);
//	P1_7 = 1;
//	
//	P0DIR &= ~BV(0); P0INP |= 0x01; APCFG |= BV(0); P0SEL |= BV(0);
//	
//#elif PCB_L1_1_3
//	P0 = 0x00; P1 = 0x00;
//	P0DIR = 0xFF; P1DIR = 0xFF;
//	
//	P0_2 = 1;
//	P0_3 = 1;
//	
//	P1DIR &= ~BV(2);
//	P1_2 = 1;
//	P1DIR &= ~BV(3);
//	P1_3 = 1;
//	
//	P1_4 = 0;
//	
//	P1_7 = 1;
//	
//	P0DIR &= ~BV(0);
//	P0INP |= 0x01;
//	APCFG |= BV(0);
//	P0SEL |= BV(0);
//#else
//	P0 = 0x00; P1 = 0x00;
//	P0DIR = 0xFF; P1DIR = 0xFF;
//	
//	P0_2 = 1;
//	P0_3 = 1;
//	
//	P1DIR &= ~BV(2);
//	P1_2 = 1;
//	P1DIR &= ~BV(3);
//	P1_3 = 1;
//	
//	P1_4 = 0;
//	
//	P1_7 = 1;
//	
//	P0DIR &= ~BV(0);
//	P0INP |= 0x01;
//	APCFG |= BV(0);
//	P0SEL |= BV(0);
//	
//#endif
//	
////	HAL_ENABLE_INTERRUPTS();
////	UserHalUART1WriteISR("0123456789", 10);	
////	while (1);
//	
//	gSystemRunParam.flagAny = 0; // 初始化全局标志位
//	
//	// Hardware Initialize.
//	UserOutInit();
//	#ifdef HC595LED
//	UserLedInit();
//	#else
//	UserLedBackInit();// Back Light Led Initialize.
//	UserLedConnectInit();// 
//	#endif
//	UserEepromInit();
//	#ifdef USER_FINGERMODE
//	UserFingerModuleHardInit();
//	#elif defined USER_CARDSKY
//	UserCardSkyHardInit();
//	#endif
//	UserBeepHardInit();
//	UserMotoHardInit();// Motor Initialize.
//	UserInitTest();// Test Program Initialize.
//	#ifdef NBMODULE
//	UserNBModuleHardInit();
//	#endif
//	
//	UserGetPCF8563Hex(buf);
//	tempUTCTimeStruct.year = buf[0] + 2000;
//	tempUTCTimeStruct.month = buf[1] - 1;
//	tempUTCTimeStruct.day = buf[2] - 1;
//	tempUTCTimeStruct.hour = buf[3];
//	tempUTCTimeStruct.minutes = buf[4];
//	tempUTCTimeStruct.seconds = buf[5];
//	temp32 = osal_ConvertUTCSecs( &tempUTCTimeStruct );
//	osal_setClock(temp32);
//	
//	APCFG |= BV(0);
//	gSystemRunParam.batterPercent = 100;
//	ChangeAdvData(4, gSystemRunParam.batterPercent);
//	
//	UserGetLogInfo(0); // 读取内部日志数量且更新广播标志
//	
//	// Software Initialize.
//	GetSetup();// Get system param from EEPROM.
//	if (gSystemRunParam.flagInit == 0xA5)
//	{
//		Save_SysRunState(2); // 系统进入可开锁低功耗模式
//		UserGetAppData(P_EE_ADDR_AES128KEY, useraeskeybuf);
//	}
//	else
//	{
//		Save_SysRunState(1); // 系统进入可添加管理员低功耗模式
//	}
//}

//void UserSoftInit(void)
//{
////	InitWatchDog(0x08);
//	UserKeyBoardInit();
//	UserKeySetInit();
//	#ifdef HC595LED
//	UserLedSetReveresFlag(0x0000);
////	UserLedContorl(0x1FFF, 10, 10, 2);
//	UserLedModeSet(2);
//	#else
//	UserLedBackContorl(100, 100, 2);
//	UserLedConnectContorl(100, 100, 2);
//	#endif
//	UserBeepSoftInit();
//	UserMotoSoftInit();
//	#ifdef USER_FINGERMODE
//	UserFingerModuleSoftInit();
//	#elif defined USER_CARDSKY
//	UserCardSkySoftInit();
//	#endif
//	#ifdef NBMODULE
//	UserNBModuleSoftInit();
//	#endif
//}

////==============================================================================
////                         * 外部中断函数定义 *
////==============================================================================
//HAL_ISR_FUNCTION( halKeyOpenPort0Isr, P0INT_VECTOR )//interrupt:
//{
//	HAL_ENTER_ISR();
//#ifdef USER_ZSS
//	if ((P0IFG & BV(2)) && (P0IEN & BV(2)))
//	{
//		#ifdef USER_UART0
//		UserHalUART0ResumeISR();
//		#endif
//	}
//#endif
//	P0IFG = 0x00;
//	P0IF = 0x00;
//	IRCON &= ~BV(5);
//	HAL_EXIT_ISR();

//	return;
//}
//HAL_ISR_FUNCTION( halKey3Port1Isr, P1INT_VECTOR )
//{
//	HAL_ENTER_ISR();
//#ifdef USER_ZSS
//	if ((P1IFG & BV(2)) && (P1IEN & BV(2)))
//	{
//		P1IEN &= ~BV(2);
//		#ifdef PCB_L1_2_3
//		P1IEN &= ~BV(6);
//		#endif
//		osal_start_timerEx(Hal_TaskID, USE_RECHECK_KEYSET_EVENT, 2);
//	}
//	if ((P1IFG & BV(3)) && (P1IEN & BV(3)))
//	{
//		#ifdef PCB_L1_1_3
//		P1IEN &= ~BV(4);
//		P1IFG &= ~BV(4);
//		#endif
//		P1IEN &= ~BV(3);
//		osal_start_timerEx(Hal_TaskID, USE_RECHECK_KEYBOARD_EVENT, 2);
//	}
//	if ((P1IFG & BV(4)) && (P1IEN & BV(4)))
//	{
//		#ifdef PCB_L1_1_3
//		P1IEN &= ~BV(3);
//		P1IEN &= ~BV(4);
//		TriggerKeyBoard();
//		#endif
//	}
//	if (P1IFG & BV(5))
//	{
//		P1IEN &= ~BV(5);
//		#ifdef USER_FINGERMODE
//		UserFingerModuleSearch(UserFingerSearchCallback);
//		osal_start_timerEx(Hal_TaskID, USE_FINGER_PROCESS_EVENT, 2);
//		#elif defined USER_CARDSKY
//		osal_start_timerEx(Hal_TaskID, USE_CARDSKY_PROCESS_EVENT, 3);
//		#endif
//	}
//	if ((P1IFG & BV(6)) && (P1IEN & BV(6)))
//	{
//		#ifdef PCB_L1_2_3
//		P1IEN &= ~BV(6);
//		P1IEN &= ~BV(2);
//		osal_start_timerEx(Hal_TaskID, USE_RECHECK_KEYSET_EVENT, 2);
//		#endif
//	}
//	if ((P1IFG & BV(7)) && (P1IEN & BV(7)))
//	{
//		P1IEN &= ~BV(7);
//		#ifdef NBMODULE
//		UserNBModuleSetTodoFlag(0x02); /* 接收数据 */
//		#endif
//	}
//#endif
//	P1IFG &= 0x00;
//	P0IF = 0x00;
//	IRCON2 &= ~BV(3);
//	HAL_EXIT_ISR();
//	
//	return;
//}

