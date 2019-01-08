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

