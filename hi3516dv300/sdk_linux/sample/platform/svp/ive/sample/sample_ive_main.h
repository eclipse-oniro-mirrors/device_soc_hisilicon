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
#ifndef SAMPLE_IVE_MAIN_H
#define SAMPLE_IVE_MAIN_H
#include "hi_type.h"

/* function : show Canny sample */
HI_VOID SAMPLE_IVE_Canny(HI_CHAR chComplete);

/* function : show Gmm2 sample */
HI_VOID SAMPLE_IVE_Gmm2(HI_VOID);

/* function : show Occlusion detected sample */
HI_VOID SAMPLE_IVE_Od(HI_VOID);

/* function : show Md sample */
HI_VOID SAMPLE_IVE_Md(HI_VOID);

/* function : show Test Memory sample */
HI_VOID SAMPLE_IVE_TestMemory(HI_VOID);

/* function : show Sobel sample */
HI_VOID SAMPLE_IVE_Sobel(HI_VOID);

/* function : show St Lk sample */
HI_VOID SAMPLE_IVE_St_Lk(HI_VOID);

/* function : show Kcf sample */
HI_VOID SAMPLE_IVE_Kcf(HI_VOID);

/* function : show PerspTrans sample */
HI_VOID SAMPLE_IVE_PerspTrans(HI_VOID);

/* function :Canny sample signal handle */
HI_VOID SAMPLE_IVE_Canny_HandleSig(HI_VOID);

/* function : Gmm2 sample signal handle */
HI_VOID SAMPLE_IVE_Gmm2_HandleSig(HI_VOID);

/* function : Od sample signal handle */
HI_VOID SAMPLE_IVE_Od_HandleSig(HI_VOID);

/* function : Md sample signal handle */
HI_VOID SAMPLE_IVE_Md_HandleSig(HI_VOID);

/* function : TestMemory sample signal handle */
HI_VOID SAMPLE_IVE_TestMemory_HandleSig(HI_VOID);

/* function : Sobel sample signal handle */
HI_VOID SAMPLE_IVE_Sobel_HandleSig(HI_VOID);

/* function : St_Lk sample signal handle */
HI_VOID SAMPLE_IVE_St_Lk_HandleSig(HI_VOID);

/* function : kcf sample signal handle */
HI_VOID SAMPLE_IVE_Kcf_HandleSig(HI_VOID);

/* function : PerspTrans sample signal handle */
HI_VOID SAMPLE_IVE_PerspTrans_HandleSig(HI_VOID);

#endif
