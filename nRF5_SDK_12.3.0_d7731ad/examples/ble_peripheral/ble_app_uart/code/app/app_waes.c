#include "app_waes.h"
#include "aes.h"

uint8_t AES_get_length(unsigned int length)
{
		return ((length>>4) + 1)<<4;
}

//////////////////////////////////////////////////////////////////////////
//	��������	AES_Decrypt
//	������		��������
//	���������	pCipherText -- ���ģ�������ܵ����ݣ��䳤��ΪnDataLen�ֽڡ�  
//				nDataLen	-- ���ݳ��ȣ����ֽ�Ϊ��λ������ΪAES_KEY_LENGTH/8����������
//        key			-- ��ʼ����Կ��
//				pIV			-- ��ʼ�����������ʹ��ECBģʽ������ΪNULL��
//	���������	pPlainText  -- ���ģ��������Ľ��ܺ�����ݣ�������pCipherText��ͬ��
//	����ֵ��	���ܺ���Ч���ݳ��ȡ�
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
				{// ���һ������
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
//	��������	AES_Encrypt_PKCS7
//	������		��������
//	���������	pPlainText	-- ���ģ�������ܵ����ݣ��䳤��ΪnDataLen�ֽڡ�
//				nDataLen	-- ���ݳ��ȣ����ֽ�Ϊ��λ
//        key			-- ��ʼ����Կ��
//				pIV			-- ��ʼ�����������ʹ��ECBģʽ������ΪNULL��
//	���������	pCipherText	-- ���ģ��������ļ��ܺ�����ݣ�������pPlainText��ͬ��
//	����ֵ��	�ޡ�
//////////////////////////////////////////////////////////////////////////
uint8_t AES_Encrypt_PKCS7(const uint8_t *pPlainText, uint8_t *pCipherText, uint8_t nDataLen, const uint8_t* key, const uint8_t *pIV)
{
		uint8_t i;
		//���ȵ���
		uint8_t length = nDataLen;
		nDataLen = ((nDataLen>>4) + 1)<<4;
		uint8_t p=(Nb<<2)-(length%(Nb<<2));
		if (pPlainText != pCipherText){memcpy(pCipherText,pPlainText,length);}
		//���
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
							

