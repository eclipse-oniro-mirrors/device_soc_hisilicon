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
#ifndef SAMPLE_IVE_QUEUE_H
#define SAMPLE_IVE_QUEUE_H

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct hiSAMPLE_IVE_NODE_S {
    VIDEO_FRAME_INFO_S stFrameInfo;
    struct hiSAMPLE_IVE_NODE_S *next;
} SAMPLE_IVE_NODE_S;

typedef struct hiSAMPLE_IVE_QUEUE_S {
    SAMPLE_IVE_NODE_S *front, *rear;
} SAMPLE_IVE_QUEUE_S;


#define QUEUE_CORE_ERROR_BASE     (1)
#define QUEUE_CORE_FRAMEWORK_ERROR_BASE (QUEUE_CORE_ERROR_BASE + 10000)

#define QUEUE_NULL_POINTER        (QUEUE_CORE_FRAMEWORK_ERROR_BASE + 1)
#define QUEUE_ILLEGAL_STATE       (QUEUE_CORE_FRAMEWORK_ERROR_BASE + 2)
#define QUEUE_OUT_OF_MEMORY       (QUEUE_CORE_FRAMEWORK_ERROR_BASE + 3)

SAMPLE_IVE_QUEUE_S* SAMPLE_IVE_QueueCreate(HI_S32 s32Len);
HI_VOID SAMPLE_IVE_QueueDestory(SAMPLE_IVE_QUEUE_S* pstQueueHead);
HI_VOID SAMPLE_IVE_QueueClear(SAMPLE_IVE_QUEUE_S* pstQueueHead);
HI_BOOL SAMPLE_IVE_QueueIsEmpty(SAMPLE_IVE_QUEUE_S* pstQueueHead);
HI_S32 SAMPLE_IVE_QueueSize(SAMPLE_IVE_QUEUE_S* pstQueueHead);
HI_S32 SAMPLE_IVE_QueueAddNode(SAMPLE_IVE_QUEUE_S* pstQueueHead, VIDEO_FRAME_INFO_S *pstFrameInfo);
SAMPLE_IVE_NODE_S* SAMPLE_IVE_QueueGetHeadNode(SAMPLE_IVE_QUEUE_S* pstQueueHead);
SAMPLE_IVE_NODE_S* SAMPLE_IVE_QueueGetNode(SAMPLE_IVE_QUEUE_S* pstQueueHead);
HI_VOID SAMPLE_IVE_QueueFreeNode(SAMPLE_IVE_NODE_S *pstNode);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SAMPLE_IVE_QUEUE_H */
