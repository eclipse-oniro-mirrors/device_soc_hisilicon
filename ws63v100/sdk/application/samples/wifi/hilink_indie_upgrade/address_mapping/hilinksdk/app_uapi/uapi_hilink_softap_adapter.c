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
 * Description: SoftAP Adaptation Implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"

int HILINK_StartSoftAp(const char *ssid, unsigned int ssidLen)
{
    return app_call2(APP_CALL_HILINK_START_SOFT_AP, int, const char *, ssid, unsigned int, ssidLen);
}

int HILINK_StopSoftAp(void)
{
    return app_call0(APP_CALL_HILINK_STOP_SOFT_AP, int);
}