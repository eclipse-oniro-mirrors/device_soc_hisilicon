/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
 */

#ifndef __AACENC_H__
#define __AACENC_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/********************************Macro Definition********************************/
/** \addtogroup      AACENC */
/** @{ */ /** <!-- [AACENC] */

#ifdef MONO_ONLY
#define MAX_CHANNELS 1 /* aacenc encoder channels */
#else
#define MAX_CHANNELS 2
#endif

#define AACENC_BLOCKSIZE 1024 /* aacenc blocksize */

#define AACENC_VERSION_MAX_BYTE 64 /* version max byte size */

/** @} */                     /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      AACENC */
/** @{ */ /** <!-- [AACENC] */

typedef enum {
    HI_AACENC_OK                     = 0x0000,  /* No error happened. All fine. */

    HI_AACENC_INVALID_HANDLE         = 0x0020,  /* Handle passed to function call was invalid. */
    HI_AACENC_MEMORY_ERROR           = 0x0021,  /* Memory allocation failed. */
    HI_AACENC_UNSUPPORTED_PARAMETER  = 0x0022,  /* Parameter not available. */
    HI_AACENC_INVALID_CONFIG         = 0x0023,  /* Configuration not provided. */

    HI_AACENC_INIT_ERROR             = 0x0040,  /* General initialization error. */
    HI_AACENC_INIT_AAC_ERROR         = 0x0041,  /* AAC library initialization error. */
    HI_AACENC_INIT_SBR_ERROR         = 0x0042,  /* SBR library initialization error. */
    HI_AACENC_INIT_TP_ERROR          = 0x0043,  /* Transport library initialization error. */
    HI_AACENC_INIT_META_ERROR        = 0x0044,  /* Meta data library initialization error. */

    HI_AACENC_ENCODE_ERROR           = 0x0060,  /* The encoding process was interrupted by an unexpected error. */

    HI_AACENC_ENCODE_EOF             = 0x0080   /* End of file reached. */
} HI_AACENC_ERROR_E;

/** Defines AACENC quality */
typedef enum {
    AU_QualityExcellent = 0,
    AU_QualityHigh = 1,
    AU_QualityMedium = 2,
    AU_QualityLow = 3,
} AuQuality;

/** Defines AACENC format */
typedef enum {
    AACLC = 0,    /* AAC-LC format */
    EAAC = 1,     /* HEAAC or AAC+  or aacPlusV1 */
    EAACPLUS = 2, /* AAC++ or aacPlusV2 */
    AACLD = 3,    /* AAC LD(Low Delay) */
    AACELD = 4,   /* AAC ELD(Low Delay) */
} AuEncoderFormat;

/** Defines AACENC container */
typedef enum {
    AACENC_ADTS = 0,
    AACENC_LOAS = 1,
    AACENC_LATM_MCP1 = 2,
} AACENCTransportType;

/** Defines AACENC configuration */
typedef struct {
    AuQuality quality;
    AuEncoderFormat coderFormat;
    HI_S16 bitsPerSample;
    HI_S32 sampleRate;   /* audio file sample rate */
    HI_S32 bitRate;      /* encoder bit rate in bits/sec */
    HI_S16 nChannelsIn;  /* number of channels on input (1,2) */
    HI_S16 nChannelsOut; /* number of channels on output (1,2) */
    HI_S16 bandWidth;    /* targeted audio bandwidth in Hz */
    AACENCTransportType transtype;
} AACENC_CONFIG;

/* Defines AACENC version */
typedef struct hiAACENC_VERSION_S {
    HI_U8 aVersion[AACENC_VERSION_MAX_BYTE];
} AACENC_VERSION_S;

typedef HI_U32 AAC_ENCODER_S;

/** @} */ /** <!-- ==== Structure Definition End ==== */

/******************************* API declaration *****************************/
/** \addtogroup      AACENC */
/** @{ */ /** <!--  [AACENC] */

/**
\brief Get version information.
\attention \n
N/A
\param[in] pVersion       version describe struct
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE          : FAILURE
\see \n
N/A
*/
HI_S32 HI_AACENC_GetVersion(AACENC_VERSION_S *pVersion);

/**
\brief get reasonable default configuration.
\attention \n
N/A
\param[in] pstConfig    pointer to an configuration information structure
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE          : FAILURE
\see \n
N/A
*/
HI_S32 AACInitDefaultConfig(AACENC_CONFIG *pstConfig);

/**
\brief allocate and initialize a new encoder instance.
\attention \n
N/A
\param[in] phAacPlusEnc    pointer to an configuration information structure
\param[in] pstConfig    pointer to an configuration information structure
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE   : FAILURE
\see \n
N/A
*/
HI_S32 AACEncoderOpen(AAC_ENCODER_S **phAacPlusEnc, const AACENC_CONFIG *pstConfig);

/**
\brief allocate and initialize a new encoder instance
\attention \n
N/A
\param[in] hAacPlusEnc    pointer to an configuration information structure
\param[in] ps16PcmBuf    BLOCKSIZE*nChannels audio samples,interleaved
\param[in] pu8Outbuf    pointer to output buffer,(must be 6144/8*MAX_CHANNELS bytes large)
\param[in] ps32NumOutBytes    number of bytes in output buffer after processing
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE   : FAILURE
\see \n
N/A
*/
HI_S32 AACEncoderFrame(AAC_ENCODER_S *hAacPlusEnc, HI_S16 *ps16PcmBuf,
                       HI_U8 *pu8Outbuf, HI_S32 *ps32NumOutBytes);

/**
\brief close encoder device.
\attention \n
N/A
\param[in] hAacPlusEnc    pointer to an configuration information structure
\retval N/A
\see \n
N/A
*/
HI_VOID AACEncoderClose(AAC_ENCODER_S *hAacPlusEnc);

/**
\brief register sbrenc module.
\attention \n
N/A
\param[in] pModuleHandle  pointer to WorkHandle of sbrenc module
\retval ::HI_SUCCESS   : Success
\retval ::HI_FAILURE   : FAILURE
\see \n
N/A
*/
HI_S32 AACEncoderRegisterModule(HI_VOID *pModuleHandle);

/**
\brief Get WorkHandle of sbrenc module.
\attention \n
N/A
\retval ::HI_VOID *    : pointer to WorkHandle of sbrenc module
\see \n
N/A
*/
HI_VOID *HI_AAC_SBRENC_GetHandle(HI_VOID);

/** @} */ /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* __AACENC_H__ */
