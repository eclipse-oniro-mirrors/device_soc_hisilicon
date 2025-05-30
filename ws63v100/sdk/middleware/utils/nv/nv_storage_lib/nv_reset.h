/*
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
 * Description: NV Storage Library Restore factory settings interface
 */

#ifndef NV_RESET_H_
#define NV_RESET_H_

#include "nv.h"
#include "nv_key.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define RESET_MODE_A        1
#define RESET_MODE_B        2

typedef struct {
    uint8_t mode;
    bool region_flag[KEY_ID_REGION_MAX_NUM];
} nv_reset_mode_t;

errcode_t kv_backup_keys(const nv_backup_mode_t *backup_flag);
errcode_t kv_restore_all_keys(void);
errcode_t kv_enable_restore_flag(const nv_reset_mode_t *nv_reset_mode);
errcode_t kv_backup_copy_unused_page_to_dragpage(uint32_t dragpage_location, uint32_t unused_page_location);

errcode_t kv_restore_set_region_flag(const bool *flag);
errcode_t kv_backup_delete_repeat_key(void);
errcode_t kv_backup_write_key(void);
errcode_t kv_backup_set_invalid_key(const kv_key_handle_t *key);
errcode_t kv_backup_find_write_position(uint16_t required_space, uint32_t *write_position);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* NV_RESET_H_ */

