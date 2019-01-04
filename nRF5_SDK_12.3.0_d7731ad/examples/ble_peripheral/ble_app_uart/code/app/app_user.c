#include "app_user.h"

uint16_t gFlagAdjustTime = 0;			/* 校时计数 */

SystemRunParam_t gSystemRunParam;


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

