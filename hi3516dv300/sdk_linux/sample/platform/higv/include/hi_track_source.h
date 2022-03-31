/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HI_TRACK_SOURCE_H
#define _HI_TRACK_SOURCE_H

#include <stdint.h>
#include "hi_mw_type.h"
typedef enum hiTrack_SourceType_E {
    HI_TRACK_SOURCE_TYPE_PRIV = 0,
    HI_TRACK_SOURCE_TYPE_VIDEO,
    HI_TRACK_SOURCE_TYPE_AUDIO,
    HI_TRACK_SOURCE_TYPE_BUTT
} HI_Track_SourceType_E;

typedef enum hiTrack_VideoCodec_E {
    HI_TRACK_VIDEO_CODEC_H264 = 96,
    HI_TRACK_VIDEO_CODEC_H265 = 98,
    HI_TRACK_VIDEO_CODEC_MJPEG = 102,
    HI_TRACK_VIDEO_CODEC_BUTT
} HI_Track_VideoCodec_E;

typedef enum hiTrack_AudioCodec_E {
    HI_TRACK_AUDIO_CODEC_G711Mu = 0,  /* G.711 Mu */
    HI_TRACK_AUDIO_CODEC_G711A = 8,   /* G.711 A */
    HI_TRACK_AUDIO_CODEC_LPCM = 23,   /* LPCM see hi_common.h file from sdk */
    HI_TRACK_AUDIO_CODEC_G726 = 97,   /* G.726 */
    HI_TRACK_AUDIO_CODEC_AMR = 101,   /* AMR encoder format */
    HI_TRACK_AUDIO_CODEC_ADPCM = 104, /* ADPCM */
    HI_TRACK_AUDIO_CODEC_AAC = 105,
    HI_TRACK_AUDIO_CODEC_WAV = 108, /* WAV encoder */
    HI_TRACK_AUDIO_CODEC_MP3 = 109,
    HI_TRACK_AUDIO_CODEC_BUTT
} HI_Track_AudioCodec_E;

typedef struct hiTrack_VideoSourceInfo_S {
    HI_Track_VideoCodec_E enCodecType;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32BitRate;
    HI_FLOAT frameRate;
    HI_U32 u32Gop;
    HI_FLOAT fSpeed;
} HI_Track_VideoSourceInfo_S;

typedef struct hiTrack_AudioSourceInfo_S {
    HI_Track_AudioCodec_E enCodecType;
    HI_U32 u32ChnCnt;
    HI_U32 u32SampleRate;
    HI_U32 u32AvgBytesPerSec;
    HI_U32 u32SamplesPerFrame;
    HI_U16 u16SampleBitWidth;
} HI_Track_AudioSourceInfo_S;

typedef struct hiTrack_PrivateSourceInfo_S {
    HI_U32 u32PrivateData;
    HI_FLOAT frameRate;
    HI_U32 u32BytesPerSec;
    HI_BOOL bStrictSync;
} HI_Track_PrivateSourceInfo_S;

typedef struct hiTrack_Source_S HI_Track_Source_S;
typedef HI_Track_Source_S *HI_Track_Source_Handle;

typedef HI_S32 (*HI_Track_Source_Start_FN)(HI_Track_Source_Handle pTrackSource, HI_MW_PTR pCaller);
typedef HI_S32 (*HI_Track_Source_Stop_FN)(HI_Track_Source_Handle pTrackSource, HI_MW_PTR pCaller);
typedef HI_S32 (*HI_Track_Source_RequestKeyFrame_FN)(HI_Track_Source_Handle pTrackSource, HI_MW_PTR pCaller);

struct hiTrack_Source_S {
    intptr_t s32PrivateHandle;  // venc/aenc/ handle
    HI_Track_Source_Start_FN pfnSourceStart;
    HI_Track_Source_Stop_FN pfnSourceStop;
    HI_Track_Source_RequestKeyFrame_FN pfnRequestKeyFrame;

    HI_Track_SourceType_E enTrackType;
    union {
        HI_Track_VideoSourceInfo_S stVideoInfo;  /** video track info */
        HI_Track_AudioSourceInfo_S stAudioInfo;  /** audio track info */
        HI_Track_PrivateSourceInfo_S stPrivInfo; /** private track info */
    } unTrackSourceAttr;
};

#endif
