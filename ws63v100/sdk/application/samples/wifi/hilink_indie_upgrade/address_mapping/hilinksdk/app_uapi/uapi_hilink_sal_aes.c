/**
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Implementation of the AES Encryption and Decryption Adaptation Layer Interfaces. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_aes.h"

int HILINK_SAL_AesGcmEncrypt(const HiLinkAesGcmParam *param, unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    return app_call4(APP_CALL_HILINK_SAL_AES_GCM_ENCRYPT, int, const HiLinkAesGcmParam *, param, unsigned char *, tag,
        unsigned int, tagLen, unsigned char *, buf);
}

int HILINK_SAL_AesGcmDecrypt(const HiLinkAesGcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    return app_call4(APP_CALL_HILINK_SAL_AES_GCM_DECRYPT, int,
        const HiLinkAesGcmParam *, param, const unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
}

int HILINK_SAL_AddPadding(HiLinkPaddingMode mode, unsigned char *out, unsigned int outLen, unsigned int dataLen)
{
    return app_call4(APP_CALL_HILINK_SAL_ADD_PADDING, int, HiLinkPaddingMode, mode, unsigned char *, out,
        unsigned int, outLen, unsigned int, dataLen);
}

int HILINK_SAL_GetPadding(HiLinkPaddingMode mode, const unsigned char *input,
    unsigned int inputLen, unsigned int *dataLen)
{
    return app_call4(APP_CALL_HILINK_SAL_GET_PADDING, int, HiLinkPaddingMode, mode, const unsigned char *, input,
        unsigned int, inputLen, unsigned int *, dataLen);
}

int HILINK_SAL_AesCbcEncrypt(const HiLinkAesCbcParam *param, unsigned char *buf)
{
    return app_call2(APP_CALL_HILINK_SAL_AES_CBC_ENCRYPT, int, const HiLinkAesCbcParam *, param, unsigned char *, buf);
}

int HILINK_SAL_AesCbcDecrypt(const HiLinkAesCbcParam *param, unsigned char *buf)
{
    return app_call2(APP_CALL_HILINK_SAL_AES_CBC_DECRYPT, int, const HiLinkAesCbcParam *, param, unsigned char *, buf);
}

int HILINK_SAL_AesCcmDecrypt(const HiLinkAesCcmParam *param, const unsigned char *tag,
    unsigned int tagLen, unsigned char *buf)
{
    return app_call4(APP_CALL_HILINK_SAL_AES_CCM_DECRYPT, int,
        const HiLinkAesCcmParam *, param, const unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
}
int HILINK_SAL_AesCcmEncrypt(const HiLinkAesCcmParam *param, unsigned char *tag, unsigned int tagLen,
    unsigned char *buf)
{
    return app_call4(APP_CALL_HILINK_SAL_AES_CCM_ENCRYPT, int, const HiLinkAesCcmParam *, param,
        unsigned char *, tag, unsigned int, tagLen, unsigned char *, buf);
}