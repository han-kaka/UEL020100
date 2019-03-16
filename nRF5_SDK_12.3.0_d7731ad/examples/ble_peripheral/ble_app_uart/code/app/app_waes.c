#include "app_waes.h"
#include "aes.h"

uint8_t AES_get_length(unsigned int length)
{
		return ((length>>4) + 1)<<4;
}

//////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Decrypt
//	描述：		解密数据
//	输入参数：	pCipherText -- 密文，即需解密的数据，其长度为nDataLen字节。  
//				nDataLen	-- 数据长度，以字节为单位，必须为AES_KEY_LENGTH/8的整倍数。
//        key			-- 初始化秘钥。
//				pIV			-- 初始化向量，如果使用ECB模式，可设为NULL。
//	输出参数：	pPlainText  -- 明文，即由密文解密后的数据，可以与pCipherText相同。
//	返回值：	解密后有效数据长度。
//////////////////////////////////////////////////////////////////////////
uint8_t AES_Decrypt(uint8_t *pPlainText, const uint8_t *pCipherText, uint8_t nDataLen, const uint8_t* key, const uint8_t *pIV) 
{
		uint8_t i;
    uint8_t *pp=pPlainText;
		
		if (pPlainText != pCipherText)
		{
			memcpy(pPlainText, pCipherText, nDataLen);
		}
	
		pPlainText += nDataLen - 4*Nb;

		for(i=nDataLen/(4*Nb); i>0; i--, pPlainText-=4*Nb)
		{
				if (i == 1)
				{// 最后一块数据
						AES128_CBC_decrypt_buffer(pPlainText, pPlainText, 4*Nb, key, pIV);
				}
				else
				{
						AES128_CBC_decrypt_buffer(pPlainText, pPlainText, 4*Nb, key, pPlainText-4*Nb);
				}
		}
		i = *(pp + nDataLen - 1);
		if((i < nDataLen) && (i > 0))
		{
				i = nDataLen - i;
		}
		else
		{
				i = 0;
		}
		
		return i;
}


//////////////////////////////////////////////////////////////////////////
//	函数名：	AES_Encrypt_PKCS7
//	描述：		加密数据
//	输入参数：	pPlainText	-- 明文，即需加密的数据，其长度为nDataLen字节。
//				nDataLen	-- 数据长度，以字节为单位
//        key			-- 初始化秘钥。
//				pIV			-- 初始化向量，如果使用ECB模式，可设为NULL。
//	输出参数：	pCipherText	-- 密文，即由明文加密后的数据，可以与pPlainText相同。
//	返回值：	无。
//////////////////////////////////////////////////////////////////////////
uint8_t AES_Encrypt_PKCS7(const uint8_t *pPlainText, uint8_t *pCipherText, uint8_t nDataLen, const uint8_t* key, const uint8_t *pIV)
{
		uint8_t i;
		//长度调整
		uint8_t length = nDataLen;
		nDataLen = ((nDataLen>>4) + 1)<<4;
		uint8_t p=(Nb<<2)-(length%(Nb<<2));
		if (pPlainText != pCipherText){memcpy(pCipherText,pPlainText,length);}
		//填充
		uint8_t temp[16];
		memset(temp,p,16);
		if(length<nDataLen)
		{
				memcpy(pCipherText + length, temp, nDataLen - length);
		}
		if(length == nDataLen)
		{
			memcpy(pCipherText + length,temp,16);
		}

		for (i = nDataLen/(4*Nb); i > 0 ; i--, pCipherText += 4*Nb)
		{
				AES128_CBC_encrypt_buffer(pCipherText, pCipherText, 4*Nb, key, pIV);
//				#if AES_MODE == AES_MODE_CBC
//					XorBytes(pCipherText, pIV, 4*Nb);
//				#endif
//				BlockEncrypt(pCipherText);
				pIV = pCipherText;
		}
		return(nDataLen);
}
							

