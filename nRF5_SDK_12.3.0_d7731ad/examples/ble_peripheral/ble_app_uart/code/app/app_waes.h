#ifndef __APP_WAES_H
#define __APP_WAES_H

#include "global.h"

#define Nb	4							// �ԡ��֡���4�ֽڣ�Ϊ��λ�ļӽ������ݿ��С���̶�Ϊ4

uint8_t AES_get_length(unsigned int length);

uint8_t AES_Decrypt(uint8_t *pPlainText, const uint8_t *pCipherText, uint8_t nDataLen, const uint8_t* key, const uint8_t *pIV);

uint8_t AES_Encrypt_PKCS7(const uint8_t *pPlainText, uint8_t *pCipherText, uint8_t nDataLen, const uint8_t* key, const uint8_t *pIV);
#endif
