/*
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */

#ifndef VIDEO_BUF_BODY_INC
#define VIDEO_BUF_BODY_INC

#line 1 "video_buf.c"
/*
 * Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <https://www.gnu.org/licenses/>.
 */

#include "ot_osal.h"
#include "ot_common.h"
#include "securec.h"
#include "ot_debug.h"
#include "ot_common_vb.h"
#include "mkp_vb.h"
#include "proc_ext.h"
#include "dev_ext.h"
#include "vb_ext.h"
#include "mod_ext.h"
#include "mm_ext.h"
#include "vb_drv.h"
#include "vb_supplement.h"
#include "ot_inner_video.h"
#include "ot_inner_sys_ipc.h"

#define vb_mk_handle(pool_id, blk_id) ((((pool_id) & 0xffff) << 16) | ((blk_id) & 0xffff))
#define vb_hdle_to_pool_id(handle) ((handle) >> 16)
#define vb_hdle_to_blk_id(handle) ((handle) & 0xffff)

#define VB_MAX_BLK_CNT   10240
#define VB_MAX_POOL_SIZE 4294967295ULL /* 4GB */
#define CHN_INFO_NONE 9999

#define vb_check_initialization_status_return(ret)                    \
    do {                                                              \
        if (g_pools == TD_NULL) {                                 \
            ot_trace_vb(OT_DBG_ERR, "VB not initialized!\n"); \
            return ret;                                               \
        }                                                             \
    } while (0)

#define vb_check_pool_id_return(pool_id, ret)                       \
    do {                                                              \
        if ((pool_id) >= g_vb_conf.max_pool_cnt) {                      \
            ot_trace_vb(OT_DBG_ERR, "pool ID [%u] is great than max pool id [%u]!\n", \
                        pool_id, g_vb_conf.max_pool_cnt);             \
            return ret;                                               \
        }                                                             \
    } while (0)

#define vb_check_user_id_return(uid, ret)                           \
    do {                                                              \
        if ((uid) >= OT_VB_MAX_USER) {                                     \
            ot_trace_vb(OT_DBG_ERR, "uid should between [0, %d)! \n", OT_VB_MAX_USER); \
            return ret;                                               \
        }                                                             \
    } while (0)

#define vb_check_null_ptr_return(ptr) \
    do { \
        if (((td_void *)(td_uintptr_t)(ptr)) == TD_NULL) { \
            ot_trace_vb(OT_DBG_ERR, "illegal parameter is null!\n"); \
            return OT_ERR_VB_NULL_PTR; \
        } \
    } while (0)

typedef struct {
    td_u32 blk_cnt;
    td_u64 blk_size;
    td_s32 owner;
    ot_vb_uid uid;
    ot_vb_remap_mode vb_remap_mode;
    td_char buf_name[OT_MAX_MMZ_NAME_LEN];
} vb_pool_common_info;

typedef struct {
    td_u64 align_blk_size;
    td_u32 align_sup_cached_size;
    td_u32 align_sup_no_cache_size;
    td_u32 align_sup_cached_blk_size;
    td_u32 align_sup_no_cache_blk_size;
} vb_create_pool_size_set;

typedef struct {
    struct osal_list_head list;
    td_u32 blk_id;

    td_phys_addr_t phys_addr;
    td_void *vir_addr;

    td_s32 sum_cnt; /* sum of total user count */
    td_s32 user_cnt[OT_VB_MAX_USER];

    td_u64 blk_size;

    ot_video_supplement supplement;
} vb_blk_info;

typedef struct {
    td_u32 pool_id;
    vb_blk_info *blk;

    td_bool is_comm_pool;
    ot_vb_remap_mode vb_remap_mode;

    td_u32 blk_cnt;
    td_u64 blk_size;
    td_u64 pool_size;

    td_phys_addr_t pool_phy_addr;
    td_void *pool_vir_addr;

    struct osal_list_head free_list;
    struct osal_list_head busy;

    /* being used to link common pools */
    struct osal_list_head list;

    /* count of free_list block. */
    td_u32 free_blk_cnt;
    td_u32 min_free_blk_cnt;
    td_s32 user_cnt_sum[OT_VB_MAX_USER];

#define VB_POOL_IDLE     0x10 /* none operating this pool */
#define VB_POOL_DEAD     0x12 /* destroying */
#define VB_POOL_ZOMBIE   0x14 /* async destroy */
    td_u32 state;
    td_bool async_flag;
    td_bool user;

    td_char ac_pool_name[OT_MAX_MMZ_NAME_LEN];

    /* pool owner (who create the pool) */
    td_s32 pool_owner;
    ot_vb_uid uid;

    td_u32 supplement_cached_size;
    td_u32 supplement_no_cached_size;
    td_phys_addr_t supplement_cached_phy;
    td_phys_addr_t supplement_no_cache_phy;
    td_void *supplement_cached_vir;
    td_void *supplement_no_cache_vir;
} vb_pool_attr;

/* array of pointer to the pool */
static vb_pool_attr **g_pools = TD_NULL;

/* array of pointer to the first block in every pool */
static vb_blk_info **g_blks = TD_NULL;

/* video buffer configuration */
static ot_vb_cfg g_vb_conf;

static td_bool g_is_conf = TD_FALSE;

/* common pools list */
static struct osal_list_head g_comm_pools;

/* module video buffer configuration */
static ot_vb_cfg g_vb_mod_conf[OT_VB_MAX_USER];
static td_bool g_is_mod_conf[OT_VB_MAX_USER] = { [0 ...(OT_VB_MAX_USER - 1)] = TD_FALSE };
static td_bool g_is_mod_pool_init[OT_VB_MAX_USER] = { [0 ...(OT_VB_MAX_USER - 1)] = TD_FALSE };

/* vb supplement configuration */
static ot_vb_supplement_cfg g_supplement_conf = {0};

/* the semaphore is used to avoid race condition for allocation of pool ID and
 * list of common pools.
 */
static osal_semaphore_t g_sema;

/* vb init flag */
static int g_vb_init = TD_FALSE;

osal_spinlock_t g_vb_spin_lock;

td_u32 g_vb_force_exit = 0;

#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
/* vb_pool async */
static osal_wait_t g_vb_async_destroy_wait = {0};
static osal_task_t *g_vb_async_destroy_td = TD_NULL;
static struct osal_list_head g_zombie_state_pools;
#endif

#define vb_spin_lock(flags) osal_spin_lock_irqsave(&g_vb_spin_lock, &(flags))

#define vb_spin_unlock(flags) osal_spin_unlock_irqrestore(&g_vb_spin_lock, &(flags))

#define VB_ALIGN_LEN     256
#define vb_get_aligned_size(len) (VB_ALIGN_LEN * (((len) + VB_ALIGN_LEN - 1) / VB_ALIGN_LEN))
#define vb_get_default_aligned_size(len) (OT_DEFAULT_ALIGN * (((len) + OT_DEFAULT_ALIGN - 1) / OT_DEFAULT_ALIGN))

static td_s32 vb_check_block_id(td_u32 pool_id, td_u32 blk_id)
{
    if (blk_id > g_pools[pool_id]->blk_cnt) {
        ot_trace_vb(OT_DBG_ERR, "Pool [%u] invalid block id [0x%x]! \n", pool_id, blk_id);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

void vb_set_force_exit(td_u32 value)
{
    g_vb_force_exit = value;
}

static td_s32 vb_check_phy_addr(const vb_pool_attr *pool, td_phys_addr_t phys_addr)
{
    if ((phys_addr < pool->pool_phy_addr) ||
        (phys_addr >= pool->pool_phy_addr + pool->pool_size)) {
        ot_trace_vb(OT_DBG_ERR, "phy_addr = %lx error! pool[%d]: phy_addr = %lx, size = %llu\n",
                    (td_ulong)phys_addr, pool->pool_id, (td_ulong)pool->pool_phy_addr, pool->pool_size);
            return OT_ERR_VB_ILLEGAL_PARAM;
        }

    return TD_SUCCESS;
}

static td_s32 vb_check_create_pool_param(const vb_pool_common_info *info, const td_u32 *pool_id)
{
    if ((info->owner < OT_POOL_OWNER_USER) && (info->owner > OT_POOL_OWNER_COMMON)) {
        ot_trace_vb(OT_DBG_EMERG, "vb owner:%d is illegal!\n", info->owner);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (pool_id == TD_NULL) {
        ot_trace_vb(OT_DBG_EMERG, "poolid NULL!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (info->vb_remap_mode >= OT_VB_REMAP_MODE_BUTT) {
        ot_trace_vb(OT_DBG_EMERG, "vb_remap_mode:%d is illegal!\n", info->vb_remap_mode);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (osal_strnlen(info->buf_name, OT_MAX_MMZ_NAME_LEN) <= 0 ||
        osal_strnlen(info->buf_name, OT_MAX_MMZ_NAME_LEN) >= OT_MAX_MMZ_NAME_LEN) {
        ot_trace_vb(OT_DBG_EMERG, "buf_name ERR!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (info->uid >= OT_VB_UID_BUTT) {
        ot_trace_vb(OT_DBG_EMERG, "vb_uid:%d ERR!\n", info->uid);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (info->blk_cnt > VB_MAX_BLK_CNT) {
        ot_trace_vb(OT_DBG_EMERG, "illegal blk_cnt(%d), [1, %d]!\n", info->blk_cnt, VB_MAX_BLK_CNT);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    return TD_SUCCESS;
}

static td_s32 vb_creat_common_vb_pool(const vb_pool_common_info *info, vb_pool_attr *pool,
    const td_char *pc_mmz_name, vb_create_pool_size_set *pool_size)
{
    td_s32 ret;
    const td_u64 align_blk_size = vb_get_aligned_size(info->blk_size);
    /*
     * kernel_only标志，对于公共VB、模块VB和用户VB默认是FALSE；
     * 对于私有VB，VO的display buffer、解码的输出buffer和编码的参考帧buffer也支持用户接口获取，设置为false也没有影响
     */
    const td_bool kernel_only = TD_FALSE;
    cmpi_mmz_malloc_param malloc_param;

    pool_size->align_blk_size = align_blk_size;

    /* 此处增加VB池大小的判断限制在4G，MMZ限制1G，防止32位系统VB池大小处于[4G, 5G]之间出现越界反而申请成功的情况 */
    if ((align_blk_size > VB_MAX_POOL_SIZE) || (align_blk_size * info->blk_cnt > VB_MAX_POOL_SIZE)) {
        ot_trace_vb(OT_DBG_EMERG, "[size = %llu, cnt = %u]vb mmz alloc:%s total size is larger than 4GB!\n",
            align_blk_size, info->blk_cnt, pc_mmz_name);
        return OT_ERR_VB_NO_MEM;
    }

    malloc_param.mmz_name = pc_mmz_name;
    malloc_param.buf_name = info->buf_name;
    malloc_param.phys_addr = &pool->pool_phy_addr;
    malloc_param.virt_addr = (td_void **)&pool->pool_vir_addr;
    malloc_param.len = (td_ulong)(align_blk_size * info->blk_cnt);
    malloc_param.kernel_only = kernel_only;

    if (info->vb_remap_mode == OT_VB_REMAP_MODE_NONE) {
        pool->pool_phy_addr = cmpi_mmz_malloc(pc_mmz_name, info->buf_name,
            (td_ulong)(align_blk_size * info->blk_cnt), kernel_only);
        if (pool->pool_phy_addr == 0) {
            ret = OT_ERR_VB_NO_MEM;
        } else {
            ret = TD_SUCCESS;
        }
        pool->pool_vir_addr = TD_NULL;
    } else if (info->vb_remap_mode == OT_VB_REMAP_MODE_NOCACHE) {
        ret = cmpi_mmz_malloc_nocache(&malloc_param);
    } else {
        ret = cmpi_mmz_malloc_cached(&malloc_param);
    }

    if (ret != TD_SUCCESS) {
        ot_trace_vb(OT_DBG_EMERG, "[size = %llu, cnt = %u]vb mmz alloc:%s failed!\n",
            align_blk_size, info->blk_cnt, pc_mmz_name);
        return OT_ERR_VB_NO_MEM;
    }

    return TD_SUCCESS;
}

static td_void vb_pool_size_init(vb_create_pool_size_set *pool_size, const vb_pool_common_info *info)
{
    td_u32 cached_blk_size   = vb_get_supplement_size(&g_supplement_conf, TD_TRUE);
    td_u32 cached_size       = cached_blk_size * info->blk_cnt;
    td_u32 no_cache_blk_size = vb_get_default_aligned_size(vb_get_supplement_size(&g_supplement_conf, TD_FALSE));
    td_u32 no_cache_size     = vb_get_default_aligned_size(no_cache_blk_size * info->blk_cnt);

    pool_size->align_sup_cached_blk_size   = cached_blk_size;
    pool_size->align_sup_cached_size       = cached_size;
    pool_size->align_sup_no_cache_blk_size = no_cache_blk_size;
    pool_size->align_sup_no_cache_size     = no_cache_size;
}

static td_s32 vb_create_supplement_no_cache(const vb_pool_common_info *info, vb_pool_attr *pool,
    const td_char *mmz_name, const vb_create_pool_size_set *pool_size)
{
    td_s32 ret;
    td_char supp_name[OT_MAX_MMZ_NAME_LEN] = {0};
    cmpi_mmz_malloc_param malloc_param = {
        .mmz_name = mmz_name,
        .buf_name = supp_name,
        .phys_addr = &pool->supplement_no_cache_phy,
        .virt_addr = (td_void **)&pool->supplement_no_cache_vir,
        .len = pool_size->align_sup_no_cache_size,
        .kernel_only = TD_FALSE,
    };

    if (pool_size->align_sup_no_cache_size == 0) {
        pool->supplement_no_cache_phy = 0;
        pool->supplement_no_cache_vir = TD_NULL;
        return TD_SUCCESS;
    }

    if (snprintf_s(supp_name, OT_MAX_MMZ_NAME_LEN, OT_MAX_MMZ_NAME_LEN - 1, "%s_%s",
        info->buf_name, "sup_nc") < 0) {
        ot_trace_vb(OT_DBG_ERR, "the buf len of name(%s) too long ,copy failed!\n", info->buf_name);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    ret = cmpi_mmz_malloc_nocache(&malloc_param);
    if (ret != TD_SUCCESS) {
        ot_trace_vb(OT_DBG_ERR, "supplement nocache malloc(size:%u) from mmz:%s err!\n",
            pool_size->align_sup_no_cache_size, mmz_name);
        return OT_ERR_VB_NO_MEM;
    }
    return TD_SUCCESS;
}

static td_s32 vb_create_supplement_cached(const vb_pool_common_info *info, vb_pool_attr *pool,
    const td_char *mmz_name, const vb_create_pool_size_set *pool_size)
{
    td_s32 ret;
    td_char supp_name[OT_MAX_MMZ_NAME_LEN] = {0};
    cmpi_mmz_malloc_param malloc_param = {
        .mmz_name = mmz_name,
        .buf_name = supp_name,
        .phys_addr = &pool->supplement_cached_phy,
        .virt_addr = (td_void **)&pool->supplement_cached_vir,
        .len = pool_size->align_sup_cached_size,
        .kernel_only = TD_FALSE,
    };

    if (pool_size->align_sup_cached_size == 0) {
        pool->supplement_cached_phy = 0;
        pool->supplement_cached_vir = TD_NULL;
        return TD_SUCCESS;
    }

    if (snprintf_s(supp_name, OT_MAX_MMZ_NAME_LEN, OT_MAX_MMZ_NAME_LEN - 1, "%s_%s",
        info->buf_name, "sup_c") < 0) {
        ot_trace_vb(OT_DBG_ERR, "the buf len of name(%s) too long ,copy failed!\n", info->buf_name);
        cmpi_mmz_free(pool->supplement_no_cache_phy, pool->supplement_no_cache_vir);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    ret = cmpi_mmz_malloc_cached(&malloc_param);
    if (ret != TD_SUCCESS) {
        ot_trace_vb(OT_DBG_EMERG, "supplement cached malloc(size:%u) from mmz:%s err!\n",
            pool_size->align_sup_cached_size, mmz_name);
        cmpi_mmz_free(pool->supplement_no_cache_phy, pool->supplement_no_cache_vir);
        return OT_ERR_VB_NO_MEM;
    }
    return TD_SUCCESS;
}

static td_s32 vb_create_supplement_vb_pool(const vb_pool_common_info *info, vb_pool_attr *pool,
    const td_char *mmz_name, vb_create_pool_size_set *pool_size)
{
    td_s32 ret;

    if (info->owner == OT_POOL_OWNER_PRIVATE && info->uid != OT_VB_UID_VDEC) {
        return TD_SUCCESS;
    }
    vb_pool_size_init(pool_size, info);

    ret = vb_create_supplement_no_cache(info, pool, mmz_name, pool_size);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    return vb_create_supplement_cached(info, pool, mmz_name, pool_size);
}

static td_void vb_init_pool_common_attr(const vb_pool_common_info *info, vb_pool_attr *pool,
    vb_blk_info *blk, const vb_create_pool_size_set *pool_size_set)
{
    /* initialize this pool structure */
    pool->blk = blk;
    pool->blk_cnt = info->blk_cnt;
    pool->blk_size = pool_size_set->align_blk_size;
    pool->supplement_cached_size = pool_size_set->align_sup_cached_size;
    pool->supplement_no_cached_size = pool_size_set->align_sup_no_cache_size;
    pool->pool_size = pool_size_set->align_blk_size * info->blk_cnt;
    pool->free_blk_cnt = info->blk_cnt;
    pool->min_free_blk_cnt = info->blk_cnt;
    pool->vb_remap_mode = info->vb_remap_mode;
    (td_void)memset_s(pool->user_cnt_sum, sizeof(pool->user_cnt_sum), 0, sizeof(pool->user_cnt_sum));

    OSAL_INIT_LIST_HEAD(&pool->busy);
    OSAL_INIT_LIST_HEAD(&pool->free_list);

    pool->user          = (info->owner == OT_POOL_OWNER_USER);
    pool->is_comm_pool  = (info->owner == OT_POOL_OWNER_COMMON || info->owner == OT_POOL_OWNER_MODULE);
    pool->pool_owner = info->owner;
    pool->uid = info->uid;
}

static td_s32 vb_init_pool_attr(const vb_pool_common_info *info, vb_pool_attr *pool, vb_blk_info *blk,
    const td_char *pc_mmz_name, const vb_create_pool_size_set *pool_size_set)
{
    td_u32 j;
    vb_blk_info *blk_tmp = TD_NULL;

    /* initialize this pool structure */
    vb_init_pool_common_attr(info, pool, blk, pool_size_set);

    /* initialize all buffer block structure */
    blk_tmp = blk;

    for (j = 0; j < info->blk_cnt; j++, blk_tmp++) {
        osal_list_add_tail(&blk_tmp->list, &pool->free_list);

        blk_tmp->blk_id = j;
        blk_tmp->phys_addr = (td_phys_addr_t)(pool->pool_phy_addr + j * pool_size_set->align_blk_size);
        if (info->vb_remap_mode == OT_VB_REMAP_MODE_NONE) {
            blk_tmp->vir_addr = TD_NULL;
        } else {
            blk_tmp->vir_addr = (td_void *)((td_u8 *)pool->pool_vir_addr + j * pool_size_set->align_blk_size);
        }
        blk_tmp->sum_cnt = 0;
        (td_void)memset_s(blk_tmp->user_cnt, sizeof(blk_tmp->user_cnt), 0, sizeof(blk_tmp->user_cnt));
        blk_tmp->blk_size = pool_size_set->align_blk_size;

        (td_void)memset_s(&blk_tmp->supplement, sizeof(ot_video_supplement), 0, sizeof(ot_video_supplement));

        if (pool_size_set->align_sup_no_cache_size) {
            vb_value_supplement_no_cache_addr((td_phys_addr_t)(pool->supplement_no_cache_phy + j * (td_u64)
                pool_size_set->align_sup_no_cache_blk_size), (td_void *)((td_u8 *)pool->supplement_no_cache_vir +
                j * (td_u64)pool_size_set->align_sup_no_cache_blk_size), &blk_tmp->supplement, &g_supplement_conf);
        }

        if (pool_size_set->align_sup_cached_size) {
            vb_value_supplement_cached_addr((td_phys_addr_t)(pool->supplement_cached_phy + j *
                (td_u64)pool_size_set->align_sup_cached_blk_size), (td_void *)((td_u8 *)pool->supplement_cached_vir +
                j * (td_u64)pool_size_set->align_sup_cached_blk_size), &blk_tmp->supplement, &g_supplement_conf);
        }
    }

    if (pc_mmz_name != TD_NULL) {
        if (strncpy_s(pool->ac_pool_name, sizeof(pool->ac_pool_name), pc_mmz_name,
            sizeof(pool->ac_pool_name) - 1) != EOK) {
            ot_trace_vb(OT_DBG_ERR, "copy err!\n");
            return OT_ERR_VB_NO_MEM;
        }
    }

    pool->state = VB_POOL_IDLE;
    pool->async_flag = TD_FALSE;
    return TD_SUCCESS;
}

#ifdef CONFIG_OT_VB_LOG_SUPPORT
static td_void vb_log_record(const vb_blk_info *blk, const vb_pool_attr *pool_attr, td_u32 uid, td_bool opt, td_u32 chn)
{
    vb_analysisy_export_func *pfn_vb_analysisy_exp_func = TD_NULL;
    if (pool_attr->pool_owner == OT_POOL_OWNER_PRIVATE) {
        return;
    }
    pfn_vb_analysisy_exp_func = func_entry(vb_analysisy_export_func, OT_ID_VB_LOG);
    if (pfn_vb_analysisy_exp_func != TD_NULL && ckfn_vb_analysisy_vb_log_record()) {
        call_vb_analysisy_vb_log_record(pool_attr->pool_id, blk->blk_id, uid, opt, chn);
    }
}

static td_void vb_save_seg_log(const vb_blk_info *blk, td_s32 pool_owner, td_u32 pool_id)
{
    vb_analysisy_export_func *pfn_vb_analysisy_exp_func = TD_NULL;
    if (pool_owner == OT_POOL_OWNER_PRIVATE) {
        return;
    }
    pfn_vb_analysisy_exp_func = func_entry(vb_analysisy_export_func, OT_ID_VB_LOG);
    if (pfn_vb_analysisy_exp_func != TD_NULL && ckfn_vb_analysisy_vb_save_seg()) {
        call_vb_analysisy_vb_save_seg(pool_id, blk->blk_id);
    }
}

static td_void vb_create_pool_log(const vb_pool_info *pool_info)
{
    vb_analysisy_export_func *pfn_vb_analysisy_exp_func = func_entry(vb_analysisy_export_func, OT_ID_VB_LOG);
    if (pfn_vb_analysisy_exp_func != TD_NULL && ckfn_vb_analysisy_create_pool_log()) {
        call_vb_analysisy_create_pool_log(pool_info);
    }
}

static td_void vb_destroy_pool_log(td_u32 pool_id)
{
    vb_analysisy_export_func *pfn_vb_analysisy_exp_func = func_entry(vb_analysisy_export_func, OT_ID_VB_LOG);
    if (pfn_vb_analysisy_exp_func != TD_NULL && ckfn_vb_analysisy_destroy_pool_log()) {
        call_vb_analysisy_destroy_pool_log(pool_id);
    }
}
#endif

static td_s32 vb_search_and_set_pool(const vb_pool_common_info *info, vb_pool_attr *pool,
    vb_blk_info *blk, td_u32 *pool_id)
{
    td_u32 i;
    unsigned long flags;

    /* search a free_list pool */
    for (i = 0; i < g_vb_conf.max_pool_cnt; i++) {
        if (g_pools[i] == TD_NULL) {
            break;
        }
    }

    if (i >= g_vb_conf.max_pool_cnt) {
        ot_trace_vb(OT_DBG_ERR, "too many pools!\n");
        return OT_ERR_VB_NO_MEM;
    }

    if (info->owner == OT_POOL_OWNER_COMMON || info->owner == OT_POOL_OWNER_MODULE) {
        osal_list_add_tail(&pool->list, &g_comm_pools);
    }

    *pool_id = i;
    pool->pool_id = i;

    /* finally set g_pools and g_blks */
    vb_spin_lock(flags);
    g_pools[i] = pool;
    g_blks[i] = blk;
    vb_spin_unlock(flags);
    ot_trace_vb(OT_DBG_DEBUG, "created pool %u!\n", i);
    return TD_SUCCESS;
}

static td_s32 vb_creat_pool_process(const vb_pool_common_info *info, const td_char *mmz_name,
    vb_pool_attr *pool, vb_blk_info *blk)
{
    td_s32 ret;
    const td_char *pc_mmz_name = "\0";
    vb_create_pool_size_set pool_size_set = {0};

    /* alloc memory from MMZ */
    if (mmz_name == NULL) {
        pc_mmz_name = NULL;
    } else {
        if (osal_strcmp(pc_mmz_name, mmz_name) != 0) {
            pc_mmz_name = mmz_name;
        } else {
            pc_mmz_name = NULL;
        }
    }
    ret = vb_creat_common_vb_pool(info, pool, pc_mmz_name, &pool_size_set);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    ret = vb_create_supplement_vb_pool(info, pool, pc_mmz_name, &pool_size_set);
    if (ret != TD_SUCCESS) {
        goto mmz_failed_1;
    }

    ret = vb_init_pool_attr(info, pool, blk, pc_mmz_name, &pool_size_set);
    if (ret != TD_SUCCESS) {
        goto mmz_failed_2;
    }

    return TD_SUCCESS;

mmz_failed_2:
    cmpi_mmz_free(pool->supplement_cached_phy, pool->supplement_cached_vir);
    cmpi_mmz_free(pool->supplement_no_cache_phy, pool->supplement_no_cache_vir);
mmz_failed_1:
    cmpi_mmz_free(pool->pool_phy_addr, pool->pool_vir_addr);

    return ret;
}

static td_s32 create_pool(const vb_pool_common_info *info, td_u32 *pool_id, const td_char *mmz_name)
{
    td_s32 ret;
    vb_blk_info *blk = TD_NULL;
    vb_pool_attr *pool = TD_NULL;

    ret = vb_check_create_pool_param(info, pool_id);
    if (ret != TD_SUCCESS) {
        return ret;
    }

    /* first alloc resources for pool, then search a free_list pool */
    pool = (vb_pool_attr *)osal_kmalloc(sizeof(*pool), osal_gfp_kernel);
    blk = (vb_blk_info *)osal_kmalloc(sizeof(*blk) * info->blk_cnt, osal_gfp_kernel);
    if ((pool == TD_NULL) || (blk == TD_NULL)) {
        ot_trace_vb(OT_DBG_EMERG, "failed to malloc memory(pool=%p,blk=%p,info->blk_cnt=%u,size=%llu)\n",
                    pool, blk, info->blk_cnt, info->blk_size);
        ret = OT_ERR_VB_NO_MEM;
        goto malloc_failed;
    }

    (td_void)memset_s(pool, sizeof(*pool), 0, sizeof(*pool));
    (td_void)memset_s(blk, sizeof(*blk) * info->blk_cnt, 0, sizeof(*blk) * info->blk_cnt);

    /* alloc memory from MMZ */
    ret = vb_creat_pool_process(info, mmz_name, pool, blk);
    if (ret != TD_SUCCESS) {
        goto malloc_failed;
    }

    ret = vb_search_and_set_pool(info, pool, blk, pool_id);
    if (ret != TD_SUCCESS) {
        goto mmz_failed;
    }

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    {
        vb_pool_info pool_info = {0};
        pool_info.pool_id = pool->pool_id;
        pool_info.pool_info.blk_cnt = pool->blk_cnt;
        pool_info.pool_info.blk_size = pool->blk_size;
        pool_info.pool_info.pool_phy_addr = pool->pool_phy_addr;
        vb_create_pool_log(&pool_info);
    }
#endif
    return TD_SUCCESS;

mmz_failed:
    cmpi_mmz_free(pool->supplement_cached_phy, pool->supplement_cached_vir);
    cmpi_mmz_free(pool->supplement_no_cache_phy, pool->supplement_no_cache_vir);
    cmpi_mmz_free(pool->pool_phy_addr, pool->pool_vir_addr);
malloc_failed:
    if (pool != TD_NULL) {
        osal_kfree(pool);
    }
    if (blk != TD_NULL) {
        osal_kfree(blk);
    }

    return ret;
}

td_s32 vb_create_pool(td_u32 *pool_id, const td_char *pc_mmz_name, const vb_info *info)
{
    td_s32 ret;
    vb_pool_common_info common_info;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    if (info == TD_NULL) {
        ot_trace_vb(OT_DBG_EMERG, "vb info NULL!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }
    common_info.blk_cnt         = info->blk_cnt;
    common_info.blk_size        = info->blk_size;
    common_info.uid             = info->uid;
    common_info.vb_remap_mode   = info->vb_remap_mode;
    common_info.owner = OT_POOL_OWNER_PRIVATE;
    if (strncpy_s(common_info.buf_name, OT_MAX_MMZ_NAME_LEN, info->buf_name, OT_MAX_MMZ_NAME_LEN - 1) != EOK) {
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    ret = create_pool(&common_info, pool_id, pc_mmz_name);
    osal_up(&g_sema);

    return ret;
}

#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
static td_void vb_zombie_pool_list_add_tail(struct osal_list_head *list)
{
    osal_list_add_tail(list, &g_zombie_state_pools);
}

static td_void vb_async_destroy_thread_wakeup(td_void)
{
    osal_wakeup(&g_vb_async_destroy_wait);
}

static td_bool vb_is_zombie_pools_need_destroy(td_void)
{
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;

    vb_spin_lock(flags);
    osal_list_for_each_entry(pool, &g_zombie_state_pools, list) {
        if (pool->blk_cnt == pool->free_blk_cnt) {
            vb_spin_unlock(flags);
            return TD_TRUE;
        }
    }
    vb_spin_unlock(flags);
    return TD_FALSE;
}

static int vb_async_destroy_wait_condition(const void *param)
{
    ot_unused(param);
    return (osal_kthread_should_stop() || vb_is_zombie_pools_need_destroy() == TD_TRUE);
}
#endif

static td_void vb_reset_single_pools_and_blks(td_u32 pool_id)
{
    g_blks[pool_id] = TD_NULL;
    g_pools[pool_id] = TD_NULL;
}

static td_void vb_release_pool_resource(vb_pool_attr *pool)
{
    if (pool->is_comm_pool != TD_FALSE) {
        osal_list_del(&pool->list);
    }

    if (pool->supplement_cached_vir != TD_NULL) {
        osal_flush_dcache_area(pool->supplement_cached_vir, (td_ulong)pool->supplement_cached_phy,
            pool->supplement_cached_size);
    }
    cmpi_mmz_free(pool->supplement_cached_phy, pool->supplement_cached_vir);
    cmpi_mmz_free(pool->supplement_no_cache_phy, pool->supplement_no_cache_vir);
    cmpi_mmz_free(pool->pool_phy_addr, pool->pool_vir_addr);

    osal_kfree(pool->blk);
    osal_kfree(pool);

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_destroy_pool_log(pool_id);
#endif
}

static td_s32 destroy_pool(vb_pool_attr *pool)
{
    unsigned long flags;
    td_u32 pool_id = pool->pool_id;

    vb_spin_lock(flags);
    /* avoid destroying pool while someone using VB which belong to this pool */
    if (pool->blk_cnt != pool->free_blk_cnt) {
#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
        if (pool->async_flag && pool->state == VB_POOL_IDLE) {
            pool->state = VB_POOL_ZOMBIE;
            vb_zombie_pool_list_add_tail(&pool->list);
            ot_trace_vb(OT_DBG_DEBUG, "pool(%u) trigger vb async destroy.\n", pool_id);
            vb_spin_unlock(flags);
            return TD_SUCCESS;
        }
#endif

        ot_trace_vb(OT_DBG_ERR, "blk in this pool(%d) occupied by someone, please release first!\n", pool_id);

        if (!g_vb_force_exit) {
            vb_spin_unlock(flags);
            return OT_ERR_VB_BUSY;
        }
    }
    pool->async_flag = TD_FALSE;

    if (pool->state == VB_POOL_IDLE || pool->state == VB_POOL_ZOMBIE) {
        pool->state = VB_POOL_DEAD;
    } else if (pool->state == VB_POOL_DEAD) {
        /* avoid destroying the same pool in different process */
        vb_spin_unlock(flags);
        return TD_FAILURE;
    }
    vb_reset_single_pools_and_blks(pool_id);
    vb_spin_unlock(flags);

    vb_release_pool_resource(pool);
    ot_trace_vb(OT_DBG_DEBUG, "destroyed pool %u!\n", pool_id);
    return TD_SUCCESS;
}

td_s32 vb_destroy_pool(td_u32 pool_id)
{
    td_s32 ret;
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    ret = osal_down(&g_sema);
    if (ret) {
        return -ERESTARTSYS;
    }

    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "null ptr! pool %u\n", pool_id);
        return TD_FAILURE;
    }

    ret = destroy_pool(pool);
    osal_up(&g_sema);

    return ret;
}
#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
static int vb_async_destroy_thread(void *data)
{
    unsigned long flags;
    td_s32 ret;
    vb_pool_attr *pool = TD_NULL;
    vb_pool_attr *next_pool = TD_NULL;

    ot_unused(data);

    do {
        ret = osal_wait_event_interruptible(&g_vb_async_destroy_wait, vb_async_destroy_wait_condition, TD_NULL);
        if (ret == -ERESTARTSYS) {
            ot_trace_vb(OT_DBG_INFO, "interrupted by a signal!\n");
        } else if (ret != 0) {
            ot_trace_vb(OT_DBG_ERR, "wait interruptible failed!\n");
        } else {
            ot_trace_vb(OT_DBG_DEBUG, "wait success.\n");
        }

        vb_spin_lock(flags);
        osal_list_for_each_entry_safe(pool, next_pool, &g_zombie_state_pools, list) {
            if (pool->blk_cnt != pool->free_blk_cnt) {
                continue;
            }
            osal_list_del(&pool->list);
            pool->async_flag = TD_FALSE;
            pool->state = VB_POOL_DEAD;
            vb_reset_single_pools_and_blks(pool->pool_id);
            vb_spin_unlock(flags);
            ot_trace_vb(OT_DBG_DEBUG, "vb pool(%u) was asynchronously destroyed!\n", pool->pool_id);
            vb_release_pool_resource(pool);
            vb_spin_lock(flags);
        }
        vb_spin_unlock(flags);
    } while (!osal_kthread_should_stop());

    return TD_SUCCESS;
}

static td_s32 vb_async_destroy_ctx_init(td_void)
{
    if (osal_wait_init(&g_vb_async_destroy_wait) < 0) {
        ot_trace_vb(OT_DBG_ERR, "vb wait init failed!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    OSAL_INIT_LIST_HEAD(&g_zombie_state_pools);

    g_vb_async_destroy_td = osal_kthread_create(vb_async_destroy_thread, TD_NULL, "vb_thread");
    if (g_vb_async_destroy_td == TD_NULL) {
        osal_wait_destroy(&g_vb_async_destroy_wait);
        ot_trace_vb(OT_DBG_ERR, "create vb thread failed!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

static td_void vb_async_destroy_ctx_deinit(td_void)
{
    osal_kthread_destroy(g_vb_async_destroy_td, 1);

    osal_wait_destroy(&g_vb_async_destroy_wait);
}

td_s32 vb_async_destroy_pool(td_u32 pool_id)
{
    td_s32 ret;
    vb_pool_attr *pool = TD_NULL;
    unsigned long flags;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    ret = osal_down(&g_sema);
    if (ret) {
        return -ERESTARTSYS;
    }

    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "null ptr! pool %u\n", pool_id);
        return TD_FAILURE;
    }

    vb_spin_lock(flags);
    if (pool->state == VB_POOL_IDLE && pool->pool_owner == OT_POOL_OWNER_PRIVATE) {
        pool->async_flag = TD_TRUE;
        ot_trace_vb(OT_DBG_DEBUG, "pool(%u) set async destroy success.\n", pool_id);
    } else {
        vb_spin_unlock(flags);
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "async destroy pool %u must be init\n", pool_id);
        return TD_FAILURE;
    }
    vb_spin_unlock(flags);

    ret = destroy_pool(pool);
    osal_up(&g_sema);

    return ret;
}
#endif

static td_void vb_init_low_delay_info(vb_blk_info *blk)
{
    low_delay_info *info = NULL;

    if (blk->supplement.low_delay_virt_addr != TD_NULL) {
        info = (low_delay_info *)(blk->supplement.low_delay_virt_addr);
        info->tunl_frame = TD_FALSE;
        info->y_height = 0;
        info->c_height = 0;
        info->frame_finish = TD_FALSE;
        info->frame_ok = TD_FALSE;
        info->miss_vedu = 0;
        info->miss_jpege = 0;
        info->buf_line = 0;
        info->all_online = 0;
        info->csize = 0;
        info->ysize = 0;
    }

    return;
}

static td_void vb_init_mot_info(vb_blk_info *blk)
{
    if (blk->supplement.bnr_mot_virt_addr != TD_NULL) {
        (td_void)memset_s(blk->supplement.bnr_mot_virt_addr, sizeof(bnr_mot_info), 0, sizeof(bnr_mot_info));
    }
}

static td_void vb_init_misc_info(const vb_blk_info *blk)
{
    if (blk->supplement.misc_info_virt_addr != TD_NULL) {
        ot_video_supplement_misc *misc_info = (ot_video_supplement_misc *)blk->supplement.misc_info_virt_addr;
        (td_void)memset_s(misc_info, sizeof(ot_video_supplement_misc), 0, sizeof(ot_video_supplement_misc));
    }
}

static vb_blk_handle vb_get_blk_handle_by_pool_id(td_u32 pool_id, td_u32 uid, td_u32 chn)
{
    vb_blk_info *blk = TD_NULL;
    vb_pool_attr *pool = TD_NULL;

    vb_check_pool_id_return(pool_id, OT_VB_INVALID_HANDLE);

    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "pool %u is already destroyed !\n", pool_id);
        return OT_VB_INVALID_HANDLE;
    }
    if (pool->state == VB_POOL_DEAD) {
        ot_trace_vb(OT_DBG_ERR, "pool %u is dead !\n", pool_id);
        return OT_VB_INVALID_HANDLE;
    }

    if (osal_list_empty(&pool->free_list)) {
        ot_trace_vb(OT_DBG_WARN, "no free_list buffer in pool %u!\n", pool_id);
        return OT_VB_INVALID_HANDLE;
    }
    ot_assert(pool->free_blk_cnt != 0);
    pool->free_blk_cnt--;
    if (pool->free_blk_cnt < pool->min_free_blk_cnt) {
        pool->min_free_blk_cnt = pool->free_blk_cnt;
    }

    blk = osal_list_entry(pool->free_list.next, vb_blk_info, list);
    osal_list_del(pool->free_list.next);
    osal_list_add_tail(&blk->list, &pool->busy);

    vb_init_low_delay_info(blk);
    vb_init_mot_info(blk);
    vb_init_misc_info(blk);
    blk->user_cnt[uid]++;
    blk->sum_cnt++;
#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_log_record(blk, pool, uid, 1, chn);
#else
    ot_unused(chn);
#endif

    ot_trace_vb(OT_DBG_DEBUG, "Uid: [%u] blk [%u] in pool[%u]:is allocated!\n",
                uid, blk->blk_id, pool_id);

    return vb_mk_handle(pool_id, blk->blk_id);
}

vb_blk_handle vb_get_blk_by_pool_id(td_u32 pool_id, td_u32 uid)
{
    unsigned long flags;
    vb_blk_handle handle;

    vb_check_initialization_status_return(OT_VB_INVALID_HANDLE);
    vb_check_user_id_return(uid, OT_VB_INVALID_HANDLE);
    vb_check_pool_id_return(pool_id, OT_VB_INVALID_HANDLE);

    /* when called in process context, we do this */
    vb_spin_lock(flags);
    handle = vb_get_blk_handle_by_pool_id(pool_id, uid, CHN_INFO_NONE);
    vb_spin_unlock(flags);
    return handle;
}

static td_s32 vb_search_comm_pool_by_size(td_u32 *pool_id, td_u64 blk_size, const td_char *pc_mmz_name,
    td_s32 owner, ot_vb_uid uid)
{
    td_u64 min_size = -1ULL;
    const td_char *pc_ret_name = TD_NULL;
    vb_pool_attr *n = TD_NULL;
    vb_pool_attr *pool = TD_NULL;
    struct osal_list_head *pos = TD_NULL;
    td_bool blk_size_enough = TD_FALSE;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    if (pc_mmz_name == TD_NULL) {
        pc_ret_name = "\0";
    } else {
        pc_ret_name = pc_mmz_name;
    }

    osal_list_for_each(pos, &g_comm_pools) {
        n = osal_list_entry(pos, vb_pool_attr, list);
        ot_assert(n->is_comm_pool == TD_TRUE);

        if (n->pool_owner != owner || n->uid != uid) {
            continue;
        }

        if ((n->blk_size < blk_size) || (n->blk_size > min_size) ||
            ((pc_mmz_name != TD_NULL) && (osal_strcmp(n->ac_pool_name, pc_ret_name) != 0))) {
            continue;
        }
        blk_size_enough = TD_TRUE;

        if (n->free_blk_cnt == 0) {
            continue;
        }

        min_size = n->blk_size;
        pool = n;
        if (blk_size == min_size) {
            break;
        }
    }

    if (blk_size_enough == TD_FALSE) {
        *pool_id = OT_VB_INVALID_POOL_ID;
        ot_trace_vb(OT_DBG_ERR, "all of vb pool(owner %d) is smaller than the actual size %llu !\n", owner, blk_size);
        return OT_ERR_VB_SIZE_NOT_ENOUGH;
    }

    if (pool != TD_NULL) {
        *pool_id = pool->pool_id;
        return TD_SUCCESS;
    } else {
        ot_trace_vb(OT_DBG_DEBUG, "no fit common pool!\n");
        *pool_id = OT_VB_INVALID_POOL_ID;
        return OT_ERR_VB_NO_BUF;
    }
}

td_s32 vb_get_pool_id_by_size_and_module(td_u64 blk_size, td_u32 *pool_id, const td_char *pc_mmz_name, ot_vb_uid uid)
{
    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_user_id_return(uid, OT_ERR_VB_ILLEGAL_PARAM);
    ot_unused(pc_mmz_name);

    if (pool_id == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "null point !\n");
        return OT_ERR_VB_NULL_PTR;
    }

    return vb_search_comm_pool_by_size(pool_id, blk_size, TD_NULL, OT_POOL_OWNER_MODULE, uid);
}

td_s32 vb_get_pool_id(td_u64 blk_size, td_u32 *pool_id, const td_char *pc_mmz_name, ot_vb_uid uid)
{
    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_user_id_return(uid, OT_ERR_VB_ILLEGAL_PARAM);

    if (pool_id == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "null point !\n");
        return OT_ERR_VB_NULL_PTR;
    }

    if (uid != OT_VB_UID_USER) {
        return vb_search_comm_pool_by_size(pool_id, blk_size, TD_NULL, OT_POOL_OWNER_COMMON, OT_VB_UID_COMMON);
    } else {
        return vb_search_comm_pool_by_size(pool_id, blk_size, pc_mmz_name, OT_POOL_OWNER_COMMON, OT_VB_UID_COMMON);
    }
}

vb_blk_handle vb_get_blk_by_size_ex(td_u64 blk_size, ot_vb_uid uid, const td_char *pc_mmz_name, td_u32 chn)
{
    td_u32 pool_id;
    td_s32 ret;
    unsigned long flags;
    vb_blk_handle handle;

    vb_check_initialization_status_return(OT_VB_INVALID_HANDLE);
    vb_check_user_id_return(uid, OT_VB_INVALID_HANDLE);

    vb_spin_lock(flags);
    if (uid != OT_VB_UID_USER) {
        ret = vb_search_comm_pool_by_size(&pool_id, blk_size, TD_NULL, OT_POOL_OWNER_COMMON, OT_VB_UID_COMMON);
    } else {
        ret = vb_search_comm_pool_by_size(&pool_id, blk_size, pc_mmz_name, OT_POOL_OWNER_COMMON, OT_VB_UID_COMMON);
    }
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return OT_VB_INVALID_HANDLE;
    }

    handle = vb_get_blk_handle_by_pool_id(pool_id, uid, chn);
    vb_spin_unlock(flags);
    return handle;
}

vb_blk_handle vb_get_blk_by_size(td_u64 blk_size, ot_vb_uid uid, const td_char *pc_mmz_name)
{
    return vb_get_blk_by_size_ex(blk_size, uid, pc_mmz_name, CHN_INFO_NONE);
}

td_s32 vb_get_pool_info(vb_pool_info *info)
{
    td_u32 pool_id;
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;

    if (info == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "info is null!\n");
        return OT_ERR_VB_NULL_PTR;
    }

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_pool_id_return(info->pool_id, OT_VB_INVALID_POOL_ID);

    pool_id = info->pool_id;

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return OT_VB_INVALID_POOL_ID;
    }

    info->pool_info.pool_size = pool->pool_size;
    info->pool_info.pool_phy_addr = pool->pool_phy_addr;
    info->pool_info.blk_cnt = pool->blk_cnt;
    info->pool_info.blk_size = pool->blk_size;
    info->pool_info.remap_mode = pool->vb_remap_mode;
    info->is_user_vb = pool->user;
    vb_spin_unlock(flags);
    return TD_SUCCESS;
}

td_s32 vb_put_blk(td_u32 pool_id, td_phys_addr_t phys_addr)
{
    unsigned long flags;
    vb_blk_info *blk = TD_NULL;
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        vb_spin_unlock(flags);
        return TD_FAILURE;
    }
    if (pool->state == VB_POOL_DEAD) {
        vb_spin_unlock(flags);
        return TD_FAILURE;
    }

    if (vb_check_phy_addr(pool, phys_addr)) {
        vb_spin_unlock(flags);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    blk = pool->blk + osal_div64_u64(phys_addr - pool->pool_phy_addr, pool->blk_size);
    if (!blk->sum_cnt) {
        vb_spin_unlock(flags);
        ot_trace_vb(OT_DBG_WARN, "try to put a free_list buffer block!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    (td_void)memset_s(blk->user_cnt, sizeof(blk->user_cnt), 0, sizeof(blk->user_cnt));
    blk->sum_cnt = 0;
    pool->free_blk_cnt++;

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_save_seg_log(blk, OT_POOL_OWNER_COMMON, pool_id);
#endif

    osal_list_del(&blk->list);
    osal_list_add_tail(&blk->list, &pool->free_list);

    vb_spin_unlock(flags);
    return TD_SUCCESS;
}

vb_blk_handle vb_phy_to_handle(td_phys_addr_t phy_addr)
{
    td_u32 pool_id;
    td_u32 blk_id;
    vb_pool_attr *pool = TD_NULL;
    unsigned long flags;

    vb_check_initialization_status_return(OT_VB_INVALID_HANDLE);

    vb_spin_lock(flags);

    /* search the pool_id */
    for (pool_id = 0; pool_id < g_vb_conf.max_pool_cnt; pool_id++) {
        pool = g_pools[pool_id];
        if (pool == TD_NULL) {
            continue;
        }

        if ((pool->pool_phy_addr <= phy_addr) &&
            ((pool->pool_phy_addr + pool->pool_size) > phy_addr)) {
                /* caculte the blk_id */
            blk_id = (td_u32)osal_div64_u64(phy_addr - pool->pool_phy_addr, pool->blk_size);
            ot_assert(blk_id < pool->blk_cnt);
            vb_spin_unlock(flags);

            return vb_mk_handle(pool_id, blk_id);
        }
    }

    vb_spin_unlock(flags);

    return OT_VB_INVALID_HANDLE;
}

td_u32 vb_handle_to_pool_id(vb_blk_handle handle)
{
    vb_check_initialization_status_return(OT_VB_INVALID_POOL_ID);
    vb_check_pool_id_return(vb_hdle_to_pool_id(handle), OT_VB_INVALID_POOL_ID);

    return vb_hdle_to_pool_id(handle);
}

td_u32 vb_handle_to_blk_id(vb_blk_handle handle)
{
    vb_check_initialization_status_return(OT_VB_INVALID_POOL_ID);
    vb_check_pool_id_return(vb_hdle_to_pool_id(handle), OT_VB_INVALID_POOL_ID);

    return vb_hdle_to_blk_id(handle);
}

td_phys_addr_t vb_handle_to_phys(vb_blk_handle handle)
{
    td_s32 ret;
    unsigned long flags;
    td_phys_addr_t phys_addr;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    td_u32 blk_id = vb_hdle_to_blk_id(handle);
    vb_check_initialization_status_return(0);
    vb_check_pool_id_return(pool_id, 0);

    vb_spin_lock(flags);
    if (g_pools[pool_id] == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    if ((g_blks[pool_id] + blk_id) == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    ret = vb_check_block_id(pool_id, blk_id);
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return 0;
    }

    phys_addr = (td_phys_addr_t)(td_uintptr_t)((g_blks[pool_id] + blk_id)->phys_addr);
    vb_spin_unlock(flags);
    return phys_addr;
}

td_phys_addr_t vb_handle_to_kern(vb_blk_handle handle)
{
    td_s32 ret;
    unsigned long flags;
    td_phys_addr_t vir_addr;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    td_u32 blk_id = vb_hdle_to_blk_id(handle);
    vb_check_initialization_status_return(0);
    vb_check_pool_id_return(pool_id, 0);

    vb_spin_lock(flags);
    if (g_pools[pool_id] == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    if ((g_blks[pool_id] + blk_id) == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    ret = vb_check_block_id(pool_id, blk_id);
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return 0;
    }

    vir_addr = (td_ulong)(td_uintptr_t)((g_blks[pool_id] + blk_id)->vir_addr);
    vb_spin_unlock(flags);
    return vir_addr;
}

td_u64 vb_handle_to_blk_size(vb_blk_handle handle)
{
    td_s32 ret;
    unsigned long flags;
    td_u64 blk_size;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    td_u32 blk_id = vb_hdle_to_blk_id(handle);
    vb_check_initialization_status_return(0);
    vb_check_pool_id_return(pool_id, 0);

    vb_spin_lock(flags);
    if (g_pools[pool_id] == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    if ((g_blks[pool_id] + blk_id) == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    ret = vb_check_block_id(pool_id, blk_id);
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return 0;
    }

    blk_size = (td_ulong)(td_uintptr_t)((g_blks[pool_id] + blk_id)->blk_size);
    vb_spin_unlock(flags);
    return blk_size;
}

td_s32 vb_user_add(td_u32 pool_id, td_phys_addr_t phys_addr, td_u32 uid)
{
    unsigned long flags;
    vb_blk_info *blk = TD_NULL;
    const vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_user_id_return(uid, OT_ERR_VB_ILLEGAL_PARAM);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_NULL_PTR;
    }
    if (pool->state == VB_POOL_DEAD) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is dead !\n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_UNEXIST;
    }

    if (vb_check_phy_addr(pool, phys_addr)) {
        vb_spin_unlock(flags);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    blk = pool->blk + osal_div64_u64(phys_addr - pool->pool_phy_addr, pool->blk_size);
    if ((blk->sum_cnt <= 0)) {
        vb_spin_unlock(flags);
        ot_trace_vb(OT_DBG_WARN, "try to increase counter for a free_list buffer!\n");
        return OT_ERR_VB_NOT_PERM;
    }
    blk->user_cnt[uid]++;
    blk->sum_cnt++;

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_log_record(blk, pool, uid, 1, CHN_INFO_NONE);
#endif
    vb_spin_unlock(flags);
    return TD_SUCCESS;
}

static td_s32 vb_user_sub_process(td_u32 pool_id, vb_pool_attr *pool,
    td_phys_addr_t phys_addr, td_u32 uid)
{
    vb_blk_info *blk = TD_NULL;

    if (vb_check_phy_addr(pool, phys_addr)) {
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    blk = pool->blk + osal_div64_u64(phys_addr - pool->pool_phy_addr, pool->blk_size);
    if (((blk->sum_cnt <= 0) || (blk->user_cnt[uid] == 0))) {
        ot_trace_vb(OT_DBG_WARN, "try to sub user for a free_list buffer!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    ot_trace_vb(OT_DBG_DEBUG, "blk [%u] in pool[%u]:is subed!\n", blk->blk_id, pool_id);
    blk->user_cnt[uid]--;
    blk->sum_cnt--;

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_log_record(blk, pool, uid, 0, CHN_INFO_NONE);
#endif
    if (blk->sum_cnt) {
        return TD_SUCCESS;
    }

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_save_seg_log(blk, pool->pool_owner, pool_id);
#endif
    /* now this buffer is free_list. */
    osal_list_del(&blk->list);
    osal_list_add_tail(&blk->list, &pool->free_list);
    pool->free_blk_cnt++;
#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
    if (pool->state == VB_POOL_ZOMBIE && pool->blk_cnt == pool->free_blk_cnt) {
        vb_async_destroy_thread_wakeup();
    }
#endif

#ifdef OT_DEBUG
    {
        td_u32 i;
        for (i = 0; i < OT_VB_MAX_USER; i++) {
            ot_assert(blk->user_cnt[i] == 0);
        }
    }
#endif /* OT_DEBUG */
    return TD_SUCCESS;
}

static td_s32 vb_do_user_sub(td_u32 pool_id, td_phys_addr_t phys_addr, td_u32 uid, td_s32 milli_sec)
{
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;
    td_s32 ret;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_user_id_return(uid, OT_ERR_VB_ILLEGAL_PARAM);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_NULL_PTR;
    }
    if (pool->state == VB_POOL_DEAD) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is dead !\n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_UNEXIST;
    }

    ot_unused(milli_sec);

    ret = vb_user_sub_process(pool_id, pool, phys_addr, uid);
    vb_spin_unlock(flags);
    return ret;
}

td_s32 vb_user_sub(td_u32 pool_id, td_phys_addr_t phys_addr, td_u32 uid)
{
    return vb_do_user_sub(pool_id, phys_addr, uid, 0);
}

td_u32 vb_inquire_user_cnt(vb_blk_handle handle)
{
    td_s32 ret;
    td_u32 i;
    td_u32 cnt;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    vb_blk_info *blk = TD_NULL;
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(0);
    vb_check_pool_id_return(pool_id, 0);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    ret = vb_check_block_id(pool_id, vb_hdle_to_blk_id(handle));
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return 0;
    }

    blk = g_blks[pool_id] + vb_hdle_to_blk_id(handle);
    if (blk == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    for (i = 0, cnt = 0; i < OT_VB_MAX_USER; i++) {
        if (blk->user_cnt[i]) {
            cnt++;
        }
    }
    vb_spin_unlock(flags);
    return cnt;
}

td_u32 vb_get_one_user_cnt(vb_blk_handle handle, td_u32 uid)
{
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    vb_blk_info *blk = TD_NULL;

    blk = g_blks[pool_id] + vb_hdle_to_blk_id(handle);
    if (blk == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        return 0;
    }

    return blk->user_cnt[uid];
}

td_u32 vb_inquire_one_user_cnt(vb_blk_handle handle, td_u32 uid)
{
    td_s32 ret;
    td_u32 user_cnt;
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);

    vb_check_initialization_status_return(0);
    vb_check_pool_id_return(pool_id, 0);
    vb_check_user_id_return(uid, 0);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }
    ret = vb_check_block_id(pool_id, vb_hdle_to_blk_id(handle));
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return 0;
    }

    user_cnt = vb_get_one_user_cnt(handle, uid);
    vb_spin_unlock(flags);

    return user_cnt;
}

td_u32 vb_inquire_total_user_cnt(vb_blk_handle handle)
{
    td_s32 ret;
    td_u32 user_id;
    td_u32 total_user_cnt = 0;
    vb_pool_attr *pool = TD_NULL;
    unsigned long flags;

    td_u32 pool_id = vb_hdle_to_pool_id(handle);

    vb_check_initialization_status_return(0);
    vb_check_pool_id_return(pool_id, 0);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return 0;
    }

    ret = vb_check_block_id(pool_id, vb_hdle_to_blk_id(handle));
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return 0;
    }

    for (user_id = 0; user_id < OT_VB_MAX_USER; user_id++) {
        total_user_cnt += vb_get_one_user_cnt(handle, user_id);
    }
    vb_spin_unlock(flags);

    return total_user_cnt;
}

td_u32 vb_inquire_blk_cnt(td_u32 uid, td_bool is_comm_pool)
{
    td_u32 i;
    td_u32 j;
    td_u32 cnt_in_comm = 0;
    td_u32 cnt_in_all = 0;
    vb_blk_info *blk = TD_NULL;
    unsigned long flags;

    vb_check_initialization_status_return(0);
    vb_check_user_id_return(uid, 0);

    vb_spin_lock(flags);
    /* travel all pools */
    for (i = 0; i < g_vb_conf.max_pool_cnt; i++) {
        blk = g_blks[i];
        if (g_pools[i] == TD_NULL ||  blk == TD_NULL) {
            continue;
        }

        /* travel all blocks */
        for (j = 0; j < g_pools[i]->blk_cnt; j++, blk++) {
            if (!blk->user_cnt[uid]) {
                continue;
            }
            if (g_pools[i]->is_comm_pool != TD_FALSE) {
                cnt_in_comm++;
            }
            cnt_in_all++;
        }
    }
    vb_spin_unlock(flags);

    if (is_comm_pool != TD_FALSE) {
        return cnt_in_comm;
    }
    return cnt_in_all;
}

td_s32 vb_inquire_pool(td_u32 pool_id, ot_vb_pool_status *pool_status)
{
    vb_pool_attr *pool = TD_NULL;
    unsigned long flags;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);
    if (pool_status == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "null point!\n");
        return OT_ERR_VB_NULL_PTR;
    }

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool id [%u] does not existed! \n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_UNEXIST;
    }
    pool_status->is_common_pool = pool->is_comm_pool;
    pool_status->blk_cnt = pool->blk_cnt;
    pool_status->free_blk_cnt = pool->free_blk_cnt;
    vb_spin_unlock(flags);
    return TD_SUCCESS;
}

td_s32 vb_inquire_pool_user_cnt(td_u32 pool_id, td_u32 uid, td_u32 *cnt)
{
    td_u32 i;
    td_u32 count = 0;
    vb_blk_info *blk = TD_NULL;
    vb_pool_attr *pool = TD_NULL;
    unsigned long flags;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_user_id_return(uid, OT_ERR_VB_ILLEGAL_PARAM);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool id [%u] does not existed! \n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_UNEXIST;
    }

    blk = g_blks[pool_id];
    if (blk == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        vb_spin_unlock(flags);
        return OT_ERR_VB_UNEXIST;
    }

    for (i = 0; i < pool->blk_cnt; i++, blk++) {
        count += blk->user_cnt[uid];
    }

    *cnt = count;
    vb_spin_unlock(flags);

    return TD_SUCCESS;
}

td_s32 vb_inquire_pool_busy_blk_cnt(td_u32 pool_id, td_u32 *cnt)
{
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(OT_ERR_VB_NOT_READY);
    vb_check_pool_id_return(pool_id, OT_ERR_VB_ILLEGAL_PARAM);

    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool id [%u] does not existed! \n",
                    pool_id);
        return OT_ERR_VB_UNEXIST;
    }

    *cnt = pool->blk_cnt - pool->free_blk_cnt;
    ot_assert(*cnt <= pool->blk_cnt);

    return TD_SUCCESS;
}

td_bool vb_is_blk_valid(td_u32 pool_id, td_phys_addr_t phys_addr)
{
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(TD_FALSE);
    vb_check_pool_id_return(pool_id, TD_FALSE);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool id [%u] does not existed! \n", pool_id);
        vb_spin_unlock(flags);
        return TD_FALSE;
    }

    if ((phys_addr < pool->pool_phy_addr) || (phys_addr >= pool->pool_phy_addr + pool->pool_size)) {
        ot_trace_vb(OT_DBG_WARN, "id%d, phys_addr = %lx, pool_phys_addr = 0x%lx, pool_size = %llu\n",
                    pool_id, (td_ulong)phys_addr, (td_ulong)pool->pool_phy_addr, pool->pool_size);
        vb_spin_unlock(flags);
        return TD_FALSE;
    }
    vb_spin_unlock(flags);
    return TD_TRUE;
}

td_bool vb_is_pool_id_valid(td_u32 pool_id)
{
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(TD_FALSE);
    vb_check_pool_id_return(pool_id, TD_FALSE);

    vb_spin_lock(flags);
    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool id [%u] does not existed! \n",
                    pool_id);
        vb_spin_unlock(flags);
        return TD_FALSE;
    }
    vb_spin_unlock(flags);

    return TD_TRUE;
}

/* another kind of common pool which is created by a module. */
vb_blk_handle vb_get_blk_by_size_and_module_ex(td_u64 blk_size, ot_vb_uid uid, const td_char *pc_mmz_name, td_u32 chn)
{
    td_u32 pool_id;
    td_s32 ret;
    unsigned long flags;
    vb_blk_handle handle;

    ot_unused(pc_mmz_name);
    vb_check_initialization_status_return(OT_VB_INVALID_HANDLE);
    vb_check_user_id_return(uid, OT_VB_INVALID_HANDLE);

    vb_spin_lock(flags);
    ret = vb_search_comm_pool_by_size(&pool_id, blk_size, TD_NULL, OT_POOL_OWNER_MODULE, uid);
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return OT_VB_INVALID_HANDLE;
    }
    handle = vb_get_blk_handle_by_pool_id(pool_id, uid, chn);
    vb_spin_unlock(flags);
    return handle;
}

vb_blk_handle vb_get_blk_by_size_and_module(td_u64 blk_size, ot_vb_uid uid, const td_char *pc_mmz_name)
{
    return vb_get_blk_by_size_and_module_ex(blk_size, uid, pc_mmz_name, CHN_INFO_NONE);
}

static td_s32 vb_check_size_and_pool_id(td_u32 pool_id, td_u64 blk_size, const vb_pool_attr *pool)
{
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "pool %u is already destroyed !\n", pool_id);
        return TD_FAILURE;
    }
    if (pool->state == VB_POOL_DEAD) {
        ot_trace_vb(OT_DBG_ERR, "pool %u is dead !\n", pool_id);
        return TD_FAILURE;
    }
    if (pool->blk_size < blk_size) {
        ot_trace_vb(OT_DBG_ERR, "input blksize:%llu is larger than pool blk size:%llu !\n",
                    blk_size, pool->blk_size);
        return TD_FAILURE;
    }

    if (osal_list_empty(&pool->free_list)) {
        ot_trace_vb(OT_DBG_WARN, "no free_list buffer in pool %u!\n", pool_id);
        return TD_FAILURE;
    }

    if (pool->free_blk_cnt == 0) {
        ot_trace_vb(OT_DBG_ERR, "free_list blk cnt is 0, in pool %u!\n", pool_id);
        return TD_FAILURE;
    }
    return TD_SUCCESS;
}

/*
 * input: poolid, u32blksize, u32uid;
 * output: vb_handle;
 * description: search a free_list vb blk for poolid, and check the blksize must larger than the input
 *              blksize;
 *
 */
vb_blk_handle vb_get_blk_by_size_and_pool_id_ex(td_u32 pool_id, td_u64 blk_size, td_u32 uid, td_u32 chn)
{
    unsigned long flags;
    vb_blk_info *blk = TD_NULL;
    vb_pool_attr *pool = TD_NULL;

    vb_check_initialization_status_return(OT_VB_INVALID_HANDLE);
    vb_check_user_id_return(uid, OT_VB_INVALID_HANDLE);
    vb_check_pool_id_return(pool_id, OT_VB_INVALID_HANDLE);

    /* when called in process context, we do this */
    vb_spin_lock(flags);
    pool = g_pools[pool_id];

    if (vb_check_size_and_pool_id(pool_id, blk_size, pool) != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return OT_VB_INVALID_HANDLE;
    }

    pool->free_blk_cnt--;
    if (pool->free_blk_cnt < pool->min_free_blk_cnt) {
        pool->min_free_blk_cnt = pool->free_blk_cnt;
    }

    blk = osal_list_entry(pool->free_list.next, vb_blk_info, list);
    osal_list_del(pool->free_list.next);
    osal_list_add_tail(&blk->list, &pool->busy);

    vb_init_low_delay_info(blk);
    vb_init_mot_info(blk);
    vb_init_misc_info(blk);
    blk->user_cnt[uid]++;
    blk->sum_cnt++;

#ifdef CONFIG_OT_VB_LOG_SUPPORT
    vb_log_record(blk, pool, uid, 1, chn);
#else
    ot_unused(chn);
#endif

    ot_trace_vb(OT_DBG_DEBUG, "blk [%u] in pool[%u]:is allocated!\n", blk->blk_id, pool_id);

    vb_spin_unlock(flags);
    return vb_mk_handle(pool_id, blk->blk_id);
}

vb_blk_handle vb_get_blk_by_size_and_pool_id(td_u32 pool_id, td_u64 blk_size, td_u32 uid)
{
    return vb_get_blk_by_size_and_pool_id_ex(pool_id, blk_size, uid, CHN_INFO_NONE);
}

td_s32 vb_get_config(ot_vb_cfg *vb_config)
{
    if (vb_config == TD_NULL) {
        return OT_ERR_VB_NULL_PTR;
    }

    osal_down(&g_sema);
    if (g_is_conf == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "please configurate common VB first!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }
    if (memcpy_s(vb_config, sizeof(*vb_config), &g_vb_conf, sizeof(g_vb_conf)) != EOK) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    osal_up(&g_sema);

    return TD_SUCCESS;
}

td_s32 vb_check_config(const ot_vb_pool_cfg *pool_cfg)
{
    td_u64 align_blk_size;

    if (pool_cfg->blk_cnt > VB_MAX_BLK_CNT) {
        ot_trace_vb(OT_DBG_ERR, "illegal blk_cnt(%d), [1, %d]!\n", pool_cfg->blk_cnt, VB_MAX_BLK_CNT);
        return TD_FAILURE;
    }

    align_blk_size = vb_get_aligned_size(pool_cfg->blk_size);
    if ((align_blk_size > VB_MAX_POOL_SIZE) || (align_blk_size * pool_cfg->blk_cnt > VB_MAX_POOL_SIZE)) {
        ot_trace_vb(OT_DBG_ERR, "[size = %llu, cnt = %u]vb mmz alloc total size is larger than 4GB!\n",
            align_blk_size, pool_cfg->blk_cnt);
        return TD_FAILURE;
    }

    if (osal_strnlen(pool_cfg->mmz_name, OT_MAX_MMZ_NAME_LEN) >= OT_MAX_MMZ_NAME_LEN) {
        ot_trace_vb(OT_DBG_ERR, "mmz name len it's too long\n");
        return TD_FAILURE;
    }

    if (pool_cfg->remap_mode >= OT_VB_REMAP_MODE_BUTT) {
        ot_trace_vb(OT_DBG_ERR, "remap_mode %d is illegal!\n", pool_cfg->remap_mode);
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

td_s32 vb_set_config(const ot_vb_cfg *vb_config)
{
    td_s32 i;

    if (vb_config == TD_NULL) {
        return OT_ERR_VB_NULL_PTR;
    }

    for (i = 0; i < OT_VB_MAX_COMMON_POOLS; i++) {
        if (!vb_config->common_pool[i].blk_cnt || !vb_config->common_pool[i].blk_size) {
            continue;
        }

        if (vb_check_config(&vb_config->common_pool[i]) != TD_SUCCESS) {
            return OT_ERR_VB_ILLEGAL_PARAM;
        }
    }

    osal_down(&g_sema);

    if (g_pools != TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "VB is initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_BUSY;
    }

    if (memcpy_s(&g_vb_conf, sizeof(g_vb_conf), vb_config, sizeof(*vb_config)) != EOK) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    g_vb_conf.max_pool_cnt = OT_VB_MAX_POOLS; /* fix to OT_VB_MAX_POOLS */

    g_is_conf = TD_TRUE;
    osal_up(&g_sema);
    return TD_SUCCESS;
}

td_s32 vb_get_mod_pool_config(vb_ioc_cfg_arg *vb_ioc_cfg)
{
    td_u32 uid;

    if (vb_ioc_cfg == TD_NULL) {
        return OT_ERR_VB_NULL_PTR;
    }

    uid = vb_ioc_cfg->vb_uid;
    if (uid >= OT_VB_MAX_USER) {
        ot_trace_vb(OT_DBG_ERR, "invalid uid(%d) >= OT_VB_MAX_USER(%d)!\n", uid, OT_VB_MAX_USER);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    osal_down(&g_sema);
    if (g_is_mod_conf[uid] == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "invalid user id %d !\n", uid);
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }
    if (memcpy_s(&vb_ioc_cfg->vb_conf, sizeof(ot_vb_cfg), &g_vb_mod_conf[uid], sizeof(ot_vb_cfg)) != EOK) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    osal_up(&g_sema);
    return TD_SUCCESS;
}

td_s32 vb_get_mod_pool_vb_config(ot_vb_uid uid, ot_vb_cfg *vb_cfg)
{
    if (vb_cfg == TD_NULL) {
        return OT_ERR_VB_NULL_PTR;
    }

    if (uid >= OT_VB_MAX_USER) {
        ot_trace_vb(OT_DBG_ERR, "invalid uid(%d) >= OT_VB_MAX_USER(%d)!\n", uid, OT_VB_MAX_USER);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    osal_down(&g_sema);
    if (g_is_mod_conf[uid] == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "invalid user id %d !\n", uid);
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }
    if (memcpy_s(vb_cfg, sizeof(ot_vb_cfg), &g_vb_mod_conf[uid], sizeof(g_vb_mod_conf[uid])) != EOK) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    osal_up(&g_sema);
    return TD_SUCCESS;
}

td_s32 vb_set_mod_pool_config(const vb_ioc_cfg_arg *vb_ioc_cfg)
{
    td_u32 i;
    td_u32 uid;

    if (vb_ioc_cfg == TD_NULL) {
        return OT_ERR_VB_NULL_PTR;
    }

    uid = vb_ioc_cfg->vb_uid;
    if (uid >= OT_VB_MAX_USER) {
        ot_trace_vb(OT_DBG_ERR, "invalid uid(%d) >= OT_VB_MAX_USER(%d)!\n", uid, OT_VB_MAX_USER);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    for (i = 0; i < OT_VB_MAX_MOD_COMMON_POOLS; i++) {
        if (!vb_ioc_cfg->vb_conf.common_pool[i].blk_cnt ||
            !vb_ioc_cfg->vb_conf.common_pool[i].blk_size) {
                continue;
        }

        if (osal_strnlen(vb_ioc_cfg->vb_conf.common_pool[i].mmz_name, OT_MAX_MMZ_NAME_LEN) >= OT_MAX_MMZ_NAME_LEN) {
            ot_trace_vb(OT_DBG_ERR, "mmz name len it's too long\n");
            return OT_ERR_VB_ILLEGAL_PARAM;
        }

        if (vb_ioc_cfg->vb_conf.common_pool[i].remap_mode >= OT_VB_REMAP_MODE_BUTT) {
            ot_trace_vb(OT_DBG_ERR, "remap_mode %d is illegal!\n", vb_ioc_cfg->vb_conf.common_pool[i].remap_mode);
            return OT_ERR_VB_ILLEGAL_PARAM;
        }
    }

    osal_down(&g_sema);
    if (g_pools == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "VB is not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_BUSY;
    }

    if (g_is_mod_pool_init[uid] == TD_TRUE) {
        ot_trace_vb(OT_DBG_ERR, "Mod VB is initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_BUSY;
    }

    if (memcpy_s(&g_vb_mod_conf[uid], sizeof(ot_vb_cfg), &vb_ioc_cfg->vb_conf, sizeof(ot_vb_cfg)) != EOK) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    g_vb_mod_conf[uid].max_pool_cnt = OT_VB_MAX_POOLS; /* fix to OT_VB_MAX_POOLS */
    g_is_mod_conf[uid] = TD_TRUE;
    osal_up(&g_sema);

    return TD_SUCCESS;
}
static td_s32 vb_do_mod_vb_init_check(td_u32 uid, td_bool *need_return)
{
    *need_return = TD_TRUE;
    if (uid >= OT_VB_MAX_USER) {
        ot_trace_vb(OT_DBG_ERR, "uid should between [0, %d]! \n", OT_VB_MAX_USER);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    if (g_is_conf == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "please configurate common VB first! \n");
        return OT_ERR_VB_NOT_READY;
    }
    /* we should make sure common pool has initialized */
    if (g_pools == TD_NULL) {
        ot_trace_vb(OT_DBG_INFO, "please init common pools first!\n");
        return TD_SUCCESS;
    }
    if (g_is_mod_conf[uid] == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "please configurate mod common VB first! \n");
        return OT_ERR_VB_NOT_READY;
    }
    if (g_is_mod_pool_init[uid] == TD_TRUE) {
        ot_trace_vb(OT_DBG_INFO, "module %d common pool init again!\n", uid);
        return TD_SUCCESS;
    }
    *need_return = TD_FALSE;
    return TD_SUCCESS;
}
static td_s32 vb_do_mod_vb_init(td_u32 uid)
{
    td_s32 ret;
    td_u32 i;
    td_u32 pool_id;
    td_bool need_return = TD_FALSE;
    vb_pool_common_info common_info;

    if (osal_down(&g_sema)) {
        ot_trace_vb(OT_DBG_ERR, "osal_down error !\n");
        return -ERESTARTSYS;
    }
    ret = vb_do_mod_vb_init_check(uid, &need_return);
    if (need_return == TD_TRUE) {
        osal_up(&g_sema);
        return ret;
    }
    /* create module common pools */
    for (i = 0; i < OT_VB_MAX_MOD_COMMON_POOLS; i++) {
        if (!g_vb_mod_conf[uid].common_pool[i].blk_cnt || !g_vb_mod_conf[uid].common_pool[i].blk_size) {
            continue;
        }
        common_info.owner           = OT_POOL_OWNER_MODULE;
        common_info.uid             = uid;
        common_info.blk_cnt = g_vb_mod_conf[uid].common_pool[i].blk_cnt;
        common_info.blk_size = g_vb_mod_conf[uid].common_pool[i].blk_size;
        common_info.vb_remap_mode = g_vb_mod_conf[uid].common_pool[i].remap_mode;
        (td_void)strncpy_s(common_info.buf_name, OT_MAX_MMZ_NAME_LEN, "mod_vb", OT_MAX_MMZ_NAME_LEN - 1);
        ret = create_pool(&common_info, &pool_id, g_vb_mod_conf[uid].common_pool[i].mmz_name);
        if (ret != TD_SUCCESS) {
            goto create_fail;
        }
    }
    g_is_mod_pool_init[uid] = TD_TRUE;

    ot_trace_vb(OT_DBG_DEBUG, "vb init ok!\n");
    osal_up(&g_sema);
    return TD_SUCCESS;
create_fail:
    for (i = 0; i < g_vb_mod_conf[uid].max_pool_cnt; i++) {
        if (g_pools[i]) {
            vb_pool_attr *pool = g_pools[i];
            if (uid == pool->uid) {
                destroy_pool(g_pools[i]);
            }
        }
    }
    ot_trace_vb(OT_DBG_ERR, "init module(%d) common pool failed!\n", uid);
    osal_up(&g_sema);
    return OT_ERR_VB_NO_MEM;
}
static td_s32 vb_do_mod_vb_exit(td_u32 uid)
{
    td_u32 i;
    td_s32 ret;
    vb_pool_attr *pool = TD_NULL;
    td_u32 cnt;

    if (uid >= OT_VB_MAX_USER) {
        ot_trace_vb(OT_DBG_ERR, "invalid uid(%d) >= OT_VB_MAX_USER(%d)!\n", uid, OT_VB_MAX_USER);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    ret = osal_down(&g_sema);
    if (ret) {
        ot_trace_vb(OT_DBG_ERR, "osal_down error !\n");
        return -ERESTARTSYS;
    }
    if (g_pools == TD_NULL) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_INFO, "vb already exited!\n");
        return TD_SUCCESS;
    }
    for (i = 0; i < g_vb_mod_conf[uid].max_pool_cnt; i++) {
        if (g_pools[i] == TD_NULL) {
            continue;
        }
        pool = g_pools[i];
        if (uid != pool->uid) {
            continue;
        }
        if (vb_inquire_pool_busy_blk_cnt(pool->pool_id, &cnt) != TD_SUCCESS) {
            osal_up(&g_sema);
            return OT_ERR_VB_NOT_PERM;
        }
        if ((cnt != 0) && (!g_vb_force_exit)) {
            osal_up(&g_sema);
            ot_trace_vb(OT_DBG_ERR, "someone is using vb now, please make sure to release vb block first!\n");
            return OT_ERR_VB_NOT_PERM;
        }
        ret = destroy_pool(g_pools[i]);
    }
    g_is_mod_pool_init[uid] = TD_FALSE;
    g_is_mod_conf[uid] = TD_FALSE;

    osal_up(&g_sema);
    ot_trace_vb(OT_DBG_DEBUG, "common module(%d) vb exited!\n", uid);
    return ret;
}
#ifdef CONFIG_OT_VB_SUPPLEMENT_MASK_SUPPORT
td_s32 vb_set_supplement_conf(const ot_vb_supplement_cfg *supplement_conf)
{
    td_s32 ret;

    if (supplement_conf == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "virt addr is null.\n");
        return OT_ERR_VB_NULL_PTR;
    }
    ret = vb_check_supplement(supplement_conf);
    if (ret != TD_SUCCESS) {
        return ret;
    }
    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }
    if (g_pools != TD_NULL) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "VB is initialized!\n");
        return OT_ERR_VB_BUSY;
    }
    if (memcpy_s(&g_supplement_conf, sizeof(ot_vb_supplement_cfg), supplement_conf,
        sizeof(ot_vb_supplement_cfg)) != EOK) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_NOT_PERM;
    }
    osal_up(&g_sema);

    return TD_SUCCESS;
}
td_s32 vb_get_supplement_conf(ot_vb_supplement_cfg *supplement_conf)
{
    if (supplement_conf == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "virt addr is null.\n");
        return OT_ERR_VB_NULL_PTR;
    }
    if (memcpy_s(supplement_conf, sizeof(ot_vb_supplement_cfg), &g_supplement_conf,
        sizeof(ot_vb_supplement_cfg)) != EOK) {
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return OT_ERR_VB_NOT_PERM;
    }
    return TD_SUCCESS;
}
#endif

static td_void vb_common_copy_supplement_info(td_void *dst_virt_addr, td_u32 dst_size,
    const td_void *src_virt_addr, td_u32 src_size)
{
    if ((src_virt_addr == TD_NULL) || (dst_virt_addr == TD_NULL)) {
        ot_trace_vb(OT_DBG_WARN, "virt addr is null.\n");
        return;
    }
    if (memcpy_s(dst_virt_addr, dst_size, src_virt_addr, src_size) != EOK) {
        ot_trace_vb(OT_DBG_ERR, "memcpy_s fail!\n");
        return;
    }
}
td_void vb_copy_supplement_info(const ot_video_supplement *src_supp, const ot_video_supplement *dst_supp)
{
    td_void *dst_virt_addr = TD_NULL;
    td_void *src_virt_addr = TD_NULL;

    if (vb_is_supplement_support(OT_VB_SUPPLEMENT_JPEG_MASK)) {
        dst_virt_addr = dst_supp->jpeg_dcf_virt_addr;
        src_virt_addr = src_supp->jpeg_dcf_virt_addr;
        vb_common_copy_supplement_info(dst_virt_addr, sizeof(ot_jpeg_dcf), src_virt_addr, sizeof(ot_jpeg_dcf));
    }
    if (vb_drv_is_isp_frame_info_supported()) {
        dst_virt_addr = dst_supp->isp_info_virt_addr;
        src_virt_addr = src_supp->isp_info_virt_addr;
        vb_common_copy_supplement_info(dst_virt_addr, sizeof(ot_isp_frame_info),
            src_virt_addr, sizeof(ot_isp_frame_info));
    }
    if (vb_is_supplement_support(OT_VB_SUPPLEMENT_DNG_MASK)) {
        dst_virt_addr = dst_supp->frame_dng_virt_addr;
        src_virt_addr = src_supp->frame_dng_virt_addr;
        vb_common_copy_supplement_info(dst_virt_addr, sizeof(ot_dng_image_dynamic_info),
            src_virt_addr, sizeof(ot_dng_image_dynamic_info));
    }
    if (vb_drv_is_misc_supported(OT_VB_SUPPLEMENT_MISC_MASK) == TD_TRUE) {
        dst_virt_addr = dst_supp->misc_info_virt_addr;
        src_virt_addr = src_supp->misc_info_virt_addr;
        vb_common_copy_supplement_info(dst_virt_addr, sizeof(ot_video_supplement_misc),
            src_virt_addr, sizeof(ot_video_supplement_misc));
    }
    if (vb_is_supplement_support(OT_VB_SUPPLEMENT_BNR_MOT_MASK)) {
        dst_virt_addr = dst_supp->bnr_mot_virt_addr;
        src_virt_addr = src_supp->bnr_mot_virt_addr;
        vb_common_copy_supplement_info(dst_virt_addr, sizeof(bnr_mot_info), src_virt_addr, sizeof(bnr_mot_info));
    }
    if (vb_is_supplement_support(OT_VB_SUPPLEMENT_MOTION_DATA_MASK)) {
        dst_virt_addr = dst_supp->motion_data_virt_addr;
        src_virt_addr = src_supp->motion_data_virt_addr;
        vb_common_copy_supplement_info(dst_virt_addr, sizeof(dis_inner_motion_data_info),
            src_virt_addr, sizeof(dis_inner_motion_data_info));
    }
}
td_void vb_copy_supplement(ot_video_frame *dst_v_frame, const ot_video_frame *src_v_frame)
{
    vb_blk_handle handle;
    ot_video_supplement *src_supp = TD_NULL;
    ot_video_supplement *dst_supp = TD_NULL;

    handle = vb_phy_to_handle(dst_v_frame->phys_addr[0]);
    if (handle != OT_VB_INVALID_HANDLE) {
        dst_supp = vb_handle_to_supplement(handle);
        if (dst_supp == TD_NULL) {
            ot_trace_vb(OT_DBG_ERR, "dst_supp is null!\n");
            return;
        }
    } else {
        ot_trace_vb(OT_DBG_INFO, "dst VB handle is %d,phy_addr 0x%lx!!!\n", OT_VB_INVALID_HANDLE,
            (td_ulong)dst_v_frame->phys_addr[0]);
        return;
    }
    handle = vb_phy_to_handle(src_v_frame->phys_addr[0]);
    if (handle != OT_VB_INVALID_HANDLE) {
        src_supp = vb_handle_to_supplement(handle);
        if (src_supp == TD_NULL) {
            ot_trace_vb(OT_DBG_ERR, "src_supp is null!\n");
            return;
        }
    } else {
        dst_supp->jpeg_dcf_virt_addr = TD_NULL;
        dst_supp->jpeg_dcf_phys_addr = 0;
        ot_trace_vb(OT_DBG_INFO, "src VB handle is %d,phy_addr 0x%lx!!!\n", OT_VB_INVALID_HANDLE,
            (td_ulong)src_v_frame->phys_addr[0]);
        return;
    }
    vb_copy_supplement_info(src_supp, dst_supp);
    return;
}
ot_video_supplement *vb_handle_to_supplement(vb_blk_handle handle)
{
    td_s32 ret;
    unsigned long flags;
    ot_video_supplement *supplement = TD_NULL;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    td_u32 blk_id = vb_hdle_to_blk_id(handle);

    vb_check_initialization_status_return(TD_NULL);
    vb_check_pool_id_return(pool_id, TD_NULL);

    vb_spin_lock(flags);
    if (g_pools[pool_id] == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool %u is already destroyed !\n", pool_id);
        vb_spin_unlock(flags);
        return TD_NULL;
    }
    ret = vb_check_block_id(pool_id, blk_id);
    if (ret != TD_SUCCESS) {
        vb_spin_unlock(flags);
        return TD_NULL;
    }
    if ((g_blks[pool_id] + blk_id) == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "pool [%u] doesn't have blocks! \n", pool_id);
        vb_spin_unlock(flags);
        return TD_NULL;
    }
    supplement = &((g_blks[pool_id] + blk_id)->supplement);
    vb_spin_unlock(flags);
    return supplement;
}
td_bool vb_is_supplement_support(td_u32 mask)
{
    return (g_supplement_conf.supplement_cfg & mask);
}
static td_void vb_free_pools_and_blks_buf(td_void)
{
    if (g_blks != TD_NULL) {
        osal_kfree(g_blks);
    }
    if (g_pools != TD_NULL) {
        osal_kfree(g_pools);
    }
    g_blks = TD_NULL;
    g_pools = TD_NULL;
}
static td_s32 vb_init_pool_cfg(td_void)
{
    td_s32 ret;
    td_u32 i;
    td_u32 pool_id;
    vb_pool_common_info common_info;
    td_u32 buf_len = g_vb_conf.max_pool_cnt * sizeof(td_void *);

    OSAL_INIT_LIST_HEAD(&g_comm_pools);
    g_pools = (vb_pool_attr **)osal_kmalloc(buf_len, osal_gfp_kernel);
    g_blks = (vb_blk_info **)osal_kmalloc(buf_len, osal_gfp_kernel);
    if ((g_pools == TD_NULL) || (g_blks == TD_NULL)) {
        ot_trace_vb(OT_DBG_EMERG, "no memory!\n");
        goto no_memory;
    }
    (td_void)memset_s(g_pools, buf_len, 0, buf_len);
    (td_void)memset_s(g_blks, buf_len, 0, buf_len);

    /* create common pools */
    for (i = 0; i < OT_VB_MAX_COMMON_POOLS; i++) {
        if (!g_vb_conf.common_pool[i].blk_cnt || !g_vb_conf.common_pool[i].blk_size) {
            continue;
        }
        common_info.blk_cnt = g_vb_conf.common_pool[i].blk_cnt;
        common_info.blk_size = g_vb_conf.common_pool[i].blk_size;
        common_info.owner = OT_POOL_OWNER_COMMON;
        common_info.uid         = OT_VB_UID_COMMON;
        common_info.vb_remap_mode = g_vb_conf.common_pool[i].remap_mode;
        (td_void)strncpy_s(common_info.buf_name, OT_MAX_MMZ_NAME_LEN, "vb_pool", OT_MAX_MMZ_NAME_LEN - 1);

        ret = create_pool(&common_info, &pool_id, g_vb_conf.common_pool[i].mmz_name);
        if (ret) {
            goto create_fail;
        }
    }
    return TD_SUCCESS;

create_fail:
    for (i = 0; i < g_vb_conf.max_pool_cnt; i++) {
        if (g_pools[i] != TD_NULL) {
            destroy_pool(g_pools[i]);
        }
    }
no_memory:
    vb_free_pools_and_blks_buf();
    return TD_FAILURE;
}
static td_s32 vb_do_vb_init(td_void)
{
    /* just for supplement configure */
    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }
    if (g_is_conf == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "please configurate VB first! \n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }
    if (g_pools != TD_NULL) {
        ot_trace_vb(OT_DBG_INFO, "vb_init again!\n");
        osal_up(&g_sema);
        return TD_SUCCESS;
    }
    if (vb_init_pool_cfg() != TD_SUCCESS) {
        osal_up(&g_sema);
        return OT_ERR_VB_NO_MEM;
    }
    ot_trace_vb(OT_DBG_DEBUG, "vb init ok!\n");

    osal_up(&g_sema);

    return TD_SUCCESS;
}
static td_s32 vb_do_vb_exit(td_void)
{
    td_u32 i;
    td_s32 ret;
    ret = osal_down(&g_sema);
    if (ret) {
        ot_trace_vb(OT_DBG_ERR, "osal_down error !\n");
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_INFO, "vb already exited!\n");
        return TD_SUCCESS;
    }

    for (i = 0; i < OT_VB_MAX_USER; i++) {
        if (g_is_mod_pool_init[i] != TD_FALSE) {
            osal_up(&g_sema);
            ot_trace_vb(OT_DBG_ERR, "mod[%d] vb should be exit first!\n", i);
            return OT_ERR_VB_NOT_PERM;
        }
    }

    if (g_vb_init) {
        osal_up(&g_sema);
        ot_trace_vb(OT_DBG_ERR, "please exit sys first!\n");
        return OT_ERR_VB_NOT_PERM;
    }

    for (i = 0; i < g_vb_conf.max_pool_cnt; i++) {
        if (g_pools[i] != TD_NULL) {
            g_pools[i]->async_flag = TD_FALSE;
            ret = destroy_pool(g_pools[i]);
            if (ret != TD_SUCCESS) {
                osal_up(&g_sema);
                return ret;
            }
        }
    }

    ret = osal_list_empty(&g_comm_pools);

    ot_assert(ret == TD_TRUE);

    osal_kfree(g_pools);
    osal_kfree(g_blks);
    g_blks = TD_NULL;
    g_pools = TD_NULL;
    g_is_conf = TD_FALSE;
    (td_void)memset_s(&g_vb_conf, sizeof(ot_vb_cfg), 0, sizeof(ot_vb_cfg));
    osal_up(&g_sema);
    ot_trace_vb(OT_DBG_DEBUG, "vb exited!\n");
    return TD_SUCCESS;
}

td_s32 vb_do_create_pool(vb_ioc_arg *ioc_arg)
{
    td_s32 ret;
    vb_pool_common_info common_info;

    if (ioc_arg->blk_size == 0 || ioc_arg->blk_cnt == 0) {
        ot_trace_vb(OT_DBG_ERR, "blk_size(%llu) or blk_cnt(%u) illegal, should not be 0!\n",
            ioc_arg->blk_size, ioc_arg->blk_cnt);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (ioc_arg->remap_mode >= OT_VB_REMAP_MODE_BUTT) {
        ot_trace_vb(OT_DBG_ERR, "illegal parameter remap_mode %u!\n", ioc_arg->remap_mode);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        OT_TRACE(OT_DBG_ERR, OT_ID_VB, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    common_info.blk_cnt = ioc_arg->blk_cnt;
    common_info.blk_size = ioc_arg->blk_size;
    common_info.owner           = OT_POOL_OWNER_USER;
    common_info.uid             = OT_VB_UID_USER;
    common_info.vb_remap_mode = ioc_arg->remap_mode;
    (td_void)strncpy_s(common_info.buf_name, OT_MAX_MMZ_NAME_LEN, "user_pool", OT_MAX_MMZ_NAME_LEN - 1);
    ret = create_pool(&common_info, &ioc_arg->pool_id, ioc_arg->mmz_name);
    osal_up(&g_sema);
    return ret;
}

td_s32 vb_do_destroy_pool(td_u32 pool_id)
{
    vb_pool_attr *pool = TD_NULL;
    td_u32 cnt = 0;
    td_s32 ret;

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        OT_TRACE(OT_DBG_WARN, OT_ID_VB, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    if (pool_id >= g_vb_conf.max_pool_cnt) {
        ot_trace_vb(OT_DBG_ERR, "invalid pool ID [%u]!\n", pool_id);
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    pool = g_pools[pool_id];
    if (pool == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "try to destroy a NOT existed pool!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_UNEXIST;
    }
    if (pool->user == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "try to destroy a kernel pool!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_PERM;
    }

    if (vb_inquire_pool_busy_blk_cnt(pool_id, &cnt) != TD_SUCCESS) {
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_PERM;
    } else {
        if ((cnt != 0) && (!g_vb_force_exit)) {
            osal_up(&g_sema);
            ot_trace_vb(OT_DBG_ERR, "someone is using vb now, please make sure to release vb block first!\n");
            return OT_ERR_VB_NOT_PERM;
        }
    }

    ret = destroy_pool(pool);
    osal_up(&g_sema);

    return ret;
}

td_s32 vb_check_blk_cfg(const vb_ioc_arg *ioc_arg)
{
    /* check pool ID */
    if (ioc_arg->pool_id >= g_vb_conf.max_pool_cnt) {
        ot_trace_vb(OT_DBG_ERR, "invalid pool id[%u]!\n", ioc_arg->pool_id);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (!g_pools[ioc_arg->pool_id]) {
        ot_trace_vb(OT_DBG_ERR, "pool id[%u] is not existed!\n",  ioc_arg->pool_id);
        return OT_ERR_VB_UNEXIST;
    }

    if (ioc_arg->blk_size > g_pools[ioc_arg->pool_id]->blk_size) {
        ot_trace_vb(OT_DBG_ERR, "the size(%llu) is larger than the pool block size(%llu)!\n",
                    ioc_arg->blk_size, g_pools[ioc_arg->pool_id]->blk_size);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    return TD_SUCCESS;
}

td_s32 vb_do_get_blk(vb_ioc_arg *ioc_arg)
{
    td_s32 ret;

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    /* get block from common pools */
    if (ioc_arg->pool_id == OT_VB_INVALID_POOL_ID) {
        ioc_arg->blk_handle = (td_u32)vb_get_blk_by_size(ioc_arg->blk_size, OT_VB_UID_USER, ioc_arg->mmz_name);
        if (ioc_arg->blk_handle == OT_VB_INVALID_HANDLE) {
            ot_trace_vb(OT_DBG_WARN, "no buffer block!\n");
            osal_up(&g_sema);
            return OT_ERR_VB_NO_BUF;
        }
        osal_up(&g_sema);
        return TD_SUCCESS;
    }

    ret = vb_check_blk_cfg(ioc_arg);
    if (ret != TD_SUCCESS) {
        ioc_arg->blk_handle = OT_VB_INVALID_HANDLE;
        osal_up(&g_sema);
        return ret;
    }

    /* get block from specific pool */
    ioc_arg->blk_handle = (td_u32)vb_get_blk_by_pool_id(ioc_arg->pool_id, OT_VB_UID_USER);
    if (ioc_arg->blk_handle == OT_VB_INVALID_HANDLE) {
        ot_trace_vb(OT_DBG_ERR, "no buffer block!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NO_BUF;
    }
    osal_up(&g_sema);

    return TD_SUCCESS;
}

static td_s32 vb_do_release_blk(vb_blk_handle handle, td_s32 milli_sec, td_bool is_delete_buf)
{
    td_s32 ret;
    td_u32 pool_id = vb_hdle_to_pool_id(handle);
    td_u32 blk_id = vb_hdle_to_blk_id(handle);

    if (milli_sec < -1) {
        ot_trace_vb(OT_DBG_ERR, "invalid milli_sec %d!\n", milli_sec);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }
    if (pool_id >= g_vb_conf.max_pool_cnt) {
        ot_trace_vb(OT_DBG_ERR, "invalid handle %u!\n", handle);
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }
    if (g_pools[pool_id] == TD_NULL) {
        ot_trace_vb(OT_DBG_ERR, "pool id[%u] is not existed!\n", pool_id);
        osal_up(&g_sema);
        return OT_ERR_VB_UNEXIST;
    }
    if (blk_id >= g_pools[pool_id]->blk_cnt) {
        ot_trace_vb(OT_DBG_ERR, "invalid handle!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    ot_unused(is_delete_buf);
    ret = vb_do_user_sub(pool_id, (g_blks[pool_id] + blk_id)->phys_addr, OT_VB_UID_USER, milli_sec);
    osal_up(&g_sema);
    return ret;
}

td_s32 vb_do_handle_to_phys_addr(ot_vb_blk handle, td_phys_addr_t *phys_addr)
{
    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        *phys_addr = TD_NULL;
        ot_trace_vb(OT_DBG_WARN, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    if (vb_hdle_to_pool_id(handle) >= g_vb_conf.max_pool_cnt) {
        *phys_addr = TD_NULL;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if ((g_pools[vb_hdle_to_pool_id(handle)] == TD_NULL) ||
        (vb_hdle_to_blk_id(handle) >= g_pools[vb_hdle_to_pool_id(handle)]->blk_cnt)) {
        *phys_addr = TD_NULL;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    *phys_addr = vb_handle_to_phys(handle);

    osal_up(&g_sema);

    return TD_SUCCESS;
}

td_s32 vb_do_handle_to_pool_id(ot_vb_blk handle, ot_vb_pool *pool_id)
{
    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        *pool_id = OT_VB_INVALID_POOL_ID;
        ot_trace_vb(OT_DBG_WARN, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    if (vb_hdle_to_pool_id(handle) >= g_vb_conf.max_pool_cnt) {
        *pool_id = OT_VB_INVALID_POOL_ID;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if ((g_pools[vb_hdle_to_pool_id(handle)] == TD_NULL) ||
        (vb_hdle_to_blk_id(handle) >= g_pools[vb_hdle_to_pool_id(handle)]->blk_cnt)) {
        *pool_id = OT_VB_INVALID_POOL_ID;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    *pool_id = vb_handle_to_pool_id(handle);

    osal_up(&g_sema);

    return TD_SUCCESS;
}

td_s32 vb_get_supplement_addr(video_supplement_info *supplement_info)
{
    ot_video_supplement *supplement_tmp = TD_NULL;
    ot_vb_blk handle = supplement_info->block;

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    if (vb_hdle_to_pool_id(handle) >= g_vb_conf.max_pool_cnt) {
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if ((g_pools[vb_hdle_to_pool_id(handle)] == TD_NULL) ||
        (vb_hdle_to_blk_id(handle) >= g_pools[vb_hdle_to_pool_id(handle)]->blk_cnt)) {
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_UNEXIST;
    }

    supplement_tmp = vb_handle_to_supplement(handle);

    if (write_user_linear_space_valid((td_u8 *)(supplement_info->supplement),
        sizeof(ot_video_supplement)) == TD_FALSE) {
        ot_trace_vb(OT_DBG_ERR, "supplement address is invalid!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    if (supplement_tmp == NULL || osal_copy_to_user(supplement_info->supplement,
        supplement_tmp, sizeof(ot_video_supplement))) {
        osal_printk("user copy supplement_tmp failed!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    osal_up(&g_sema);

    return TD_SUCCESS;
}

td_s32 vb_do_inquire_user_cnt(ot_vb_blk handle, td_s32 *user_cnt)
{
    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        ot_trace_vb(OT_DBG_WARN, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    if (vb_hdle_to_pool_id(handle) >= g_vb_conf.max_pool_cnt) {
        *user_cnt = OT_VB_INVALID_HANDLE;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_VB_INVALID_HANDLE;
    }

    if ((g_pools[vb_hdle_to_pool_id(handle)] == TD_NULL) ||
        (vb_hdle_to_blk_id(handle) >= g_pools[vb_hdle_to_pool_id(handle)]->blk_cnt)) {
        *user_cnt = OT_VB_INVALID_HANDLE;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_VB_INVALID_HANDLE;
    }

    *user_cnt = vb_inquire_user_cnt(handle);
    osal_up(&g_sema);

    return TD_SUCCESS;
}

static td_s32 base_ioctl_pool(unsigned int cmd, unsigned long arg, td_bool *handled)
{
    *handled = TD_TRUE;
    switch (cmd) {
        case VB_CREATE_CTRL: {
            vb_ioc_arg *ioc_arg = (vb_ioc_arg *)(td_uintptr_t)arg;
            vb_check_null_ptr_return(ioc_arg);
            return vb_do_create_pool(ioc_arg);
        }
        case VB_DESTROY_CTRL: {
            td_u32 *p_id = (td_u32 *)(td_uintptr_t)arg;
            vb_check_null_ptr_return(p_id);
            return vb_do_destroy_pool(*p_id);
        }
        case VB_GETBLK_CTRL: {
            vb_ioc_arg *ioc_arg = (vb_ioc_arg *)(td_uintptr_t)arg;
            vb_check_null_ptr_return(ioc_arg);
            return vb_do_get_blk(ioc_arg);
        }
        case VB_RELBLK_CTRL: {
            vb_blk_handle *handle = (vb_blk_handle *)(td_uintptr_t)arg;
            vb_check_null_ptr_return(handle);
            return vb_do_release_blk(*handle, 0, TD_FALSE);
        }
        default:
            *handled = TD_FALSE;
            return TD_FAILURE;
    }
}
static td_s32 base_ioctl_handle(unsigned int cmd, unsigned long arg, td_bool *handled)
{
    *handled = TD_TRUE;
    switch (cmd) {
        case VB_PHYS2H_CTRL: {
            td_phys_addr_t *phy_addr = (td_phys_addr_t *)(td_uintptr_t)arg;
            vb_check_null_ptr_return(phy_addr);
            osal_down(&g_sema);
            *phy_addr = vb_phy_to_handle(*phy_addr);
            osal_up(&g_sema);
            return TD_SUCCESS;
        }
        case VB_H2PHYS_CTRL: {
            ot_vb_blk handle;
            vb_check_null_ptr_return(arg);
            handle = *(ot_vb_blk *)(td_uintptr_t)arg;
            return vb_do_handle_to_phys_addr(handle, (td_phys_addr_t *)(td_uintptr_t)arg);
        }
        case VB_H2POOL_CTRL: {
            ot_vb_blk handle;
            vb_check_null_ptr_return(arg);
            handle = *(td_u32 *)(td_uintptr_t)arg;
            return vb_do_handle_to_pool_id(handle, (ot_vb_pool *)(td_uintptr_t)arg);
        }
        case VB_H2USERCNT_CTRL: {
            vb_ioc_user_cnt *vb_user_cnt = TD_NULL;
            vb_check_null_ptr_return(arg);
            vb_user_cnt = (vb_ioc_user_cnt *)(td_uintptr_t)arg;
            return vb_do_inquire_user_cnt(vb_user_cnt->vb_blk, &vb_user_cnt->user_cnt);
        }
        default:
            *handled = TD_FALSE;
            return TD_FAILURE;
    }
}

static td_s32 base_ioctl_config(unsigned int cmd, unsigned long arg, td_bool *handled)
{
    *handled = TD_TRUE;
    switch (cmd) {
        case VB_GETSUPPLEMENTADDR_CTRL: {
            video_supplement_info *supplement_info = (video_supplement_info *)(td_uintptr_t)arg;
            vb_check_null_ptr_return(supplement_info);
            return vb_get_supplement_addr(supplement_info);
        }
        case VB_GET_CONF_CTRL:
            return vb_get_config((ot_vb_cfg *)(td_uintptr_t)arg);
        case VB_SET_CONF_CTRL:
            return vb_set_config((ot_vb_cfg *)(td_uintptr_t)arg);
        case VB_GET_MOD_CONF_CTRL:
            return vb_get_mod_pool_config((vb_ioc_cfg_arg *)(td_uintptr_t)arg);
        case VB_SET_MOD_CONF_CTRL:
            return vb_set_mod_pool_config((vb_ioc_cfg_arg *)(td_uintptr_t)arg);
        default:
            *handled = TD_FALSE;
            return TD_FAILURE;
    }
}

static td_s32 base_ioctl_get_info(unsigned long arg)
{
    td_s32 ret;
    vb_pool_info *info = (vb_pool_info *)(td_uintptr_t)arg;

    vb_check_null_ptr_return(info);
    osal_down(&g_sema);
    ret = vb_get_pool_info(info);
    osal_up(&g_sema);
    return ret;
}

static td_s32 base_ioctl_qry_user(unsigned long arg)
{
    td_s32 ret;
    td_u32 pool_id;

    vb_check_null_ptr_return(arg);
    osal_down(&g_sema);
    if (g_pools == TD_NULL) {
        OT_TRACE(OT_DBG_WARN, OT_ID_VB, "VB not initialized!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_NOT_READY;
    }

    pool_id = *(td_u32 *)(td_uintptr_t)arg;
    if ((pool_id >= g_vb_conf.max_pool_cnt) || !g_pools[pool_id]) {
        *(td_u32 *)(td_uintptr_t)arg = TD_NULL;
        ot_trace_vb(OT_DBG_ERR, "bad argument!\n");
        osal_up(&g_sema);
        return OT_ERR_VB_ILLEGAL_PARAM;
    }

    ret = vb_inquire_pool_user_cnt(pool_id, OT_VB_UID_USER, (td_u32 *)(td_uintptr_t)arg);
    osal_up(&g_sema);
    return ret;
}

static td_s32 base_ioctl_mod_pool(unsigned int cmd, unsigned long arg, td_bool *handled)
{
    *handled = TD_TRUE;
    switch (cmd) {
        case VB_INIT_POOL_CTRL:
            return vb_do_vb_init();
        case VB_EXIT_POOL_CTRL:
            return vb_do_vb_exit();
        case VB_INIT_MOD_POOL_CTRL: {
            ot_vb_uid uid;
            vb_check_null_ptr_return(arg);
            uid = *(ot_vb_uid *)(td_uintptr_t)arg;
            return vb_do_mod_vb_init(uid);
        }
        case VB_EXIT_MOD_POOL_CTRL: {
            td_u32 uid;
            vb_check_null_ptr_return(arg);
            uid = *(td_u32 *)(td_uintptr_t)arg;
            return vb_do_mod_vb_exit(uid);
        }
        default:
            *handled = TD_FALSE;
            return TD_FAILURE;
    }
}

static long base_ioctl(unsigned int cmd, unsigned long arg, td_void *private_data)
{
    td_s32 ret;
    td_bool handled;

    ot_unused(private_data);
    ret = base_ioctl_pool(cmd, arg, &handled);
    if (handled == TD_TRUE) {
        return ret;
    }
    ret = base_ioctl_handle(cmd, arg, &handled);
    if (handled == TD_TRUE) {
        return ret;
    }
    ret = base_ioctl_config(cmd, arg, &handled);
    if (handled == TD_TRUE) {
        return ret;
    }
    ret = base_ioctl_mod_pool(cmd, arg, &handled);
    if (handled == TD_TRUE) {
        return ret;
    }

    switch (cmd) {
        case VB_GET_INFO_CTRL:
            return base_ioctl_get_info(arg);
        case VB_QRY_USER_CTRL:
            return base_ioctl_qry_user(arg);
#ifdef CONFIG_OT_VB_SUPPLEMENT_MASK_SUPPORT
        case VB_SET_SUPPLEMENT_CONF_CTRL:
            return vb_set_supplement_conf((ot_vb_supplement_cfg *)(td_uintptr_t)arg);

        case VB_GET_SUPPLEMENT_CONF_CTRL:
            return vb_get_supplement_conf((ot_vb_supplement_cfg *)(td_uintptr_t)arg);
#endif

        default: {
            ot_trace_vb(OT_DBG_ERR, "ioctl cmd does NOT exist!\n");
            ret = TD_FAILURE;
        }
    }
    return ret;
}

#ifdef CONFIG_COMPAT
static long base_compat_ioctl(unsigned int cmd, unsigned long arg, td_void *private_data)
{
    switch (cmd) {
        case VB_GETSUPPLEMENTADDR_CTRL: {
            video_supplement_info *supplement_info = (video_supplement_info *)(td_uintptr_t)arg;

            OT_COMPAT_POINTER(supplement_info->supplement, ot_video_supplement *);
            break;
        }

        default: {
            break;
        }
    }

    return base_ioctl(cmd, arg, private_data);
}
#endif

static td_s32 open(td_void *data)
{
    ot_unused(data);
    return 0;
}

static td_s32 close(td_void *data)
{
    ot_unused(data);
    return 0;
}

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT

static void vb_proc_show_common_pool(struct osal_proc_dir_entry *s)
{
    td_u32 i;

    osal_seq_printf(s, "----------------------------------------common pool config"
        "--------------------------------------------------------------\n");
    osal_seq_printf(s, "pool_id");
    for (i = 0; i < OT_VB_MAX_COMMON_POOLS; i++) {
        osal_seq_printf(s, "%10u", i);
    }
    osal_seq_printf(s, "\nsize   ");
    for (i = 0; i < OT_VB_MAX_COMMON_POOLS; i++) {
        osal_seq_printf(s, "%10llu", vb_get_aligned_size(g_vb_conf.common_pool[i].blk_size));
    }
    osal_seq_printf(s, "\ncount  ");
    for (i = 0; i < OT_VB_MAX_COMMON_POOLS; i++) {
        osal_seq_printf(s, "%10u", g_vb_conf.common_pool[i].blk_cnt);
    }
    osal_seq_printf(s, "\n");
}

static void vb_proc_module_pool_cfg(struct osal_proc_dir_entry *s)
{
    td_u32 i;
    td_u32 m;

    for (m = 0; m < OT_VB_MAX_USER; m++) {
        if (g_is_mod_conf[m] == TD_FALSE) {
            continue;
        }
        if (g_is_mod_pool_init[m] == TD_FALSE) {
            continue;
        }

        osal_seq_printf(s, "----------------------------------------module common pool "
            "config of vb_uid <%d>-----------------------------------------\n", m);

        osal_seq_printf(s, "pool_id");
        for (i = 0; i < OT_VB_MAX_MOD_COMMON_POOLS; i++) {
            osal_seq_printf(s, "%10u", i);
        }
        osal_seq_printf(s, "\nsize   ");
        for (i = 0; i < OT_VB_MAX_MOD_COMMON_POOLS; i++) {
            osal_seq_printf(s, "%10llu", g_vb_mod_conf[m].common_pool[i].blk_size);
        }
        osal_seq_printf(s, "\ncount  ");
        for (i = 0; i < OT_VB_MAX_MOD_COMMON_POOLS; i++) {
            osal_seq_printf(s, "%10u", g_vb_mod_conf[m].common_pool[i].blk_cnt);
        }
        osal_seq_printf(s, "\n");
    }
}
static void vb_show_blk_by_module_pool(struct osal_proc_dir_entry *s, vb_pool_attr *pool)
{
    td_u32 k;
    vb_blk_info *blk = TD_NULL;
    struct osal_list_head *pos = TD_NULL;

    /* OT_VB_UID_COMMON not print! */
    td_char *apc_user_name[OT_VB_MAX_USER] = {
        "vi    ", "vo    ", "vgs   ", "venc  ", "vdec  ", "h265e ", "h264e ", "jpege ",
        "jpegd ", "vpss  ", "dis   ", "user  ", "pciv  ", "ai    ", "aenc  ", "rc    ",
        "vfmw  ", "gdc   ", "avs   ", "rect  ", "match ", "mcf   ", "vda   ", "vpp   ",
        TD_NULL, "uvc   ", "vdec_adapt   "
    };

    osal_seq_printf(s, "blk   ");
    for (k = 0; k < OT_VB_MAX_USER; k++) {
        if (!apc_user_name[k]) {
            continue;
        }
        osal_seq_printf(s, "%6s", apc_user_name[k]);
    }
    osal_seq_printf(s, "\n");

    osal_list_for_each(pos, &pool->busy) {
        blk = osal_list_entry(pos, vb_blk_info, list);
        osal_seq_printf(s, "%-6u", blk->blk_id);

        for (k = 0; k < OT_VB_MAX_USER; k++) {
            if (k == OT_VB_UID_COMMON) {
                continue;
            }
            osal_seq_printf(s, "%-6u", blk->user_cnt[k]);
            if (blk->user_cnt[k] > 0) {
                pool->user_cnt_sum[k]++;
            }
        }
        osal_seq_printf(s, "\n");
    }

    osal_seq_printf(s, "sum   ");
    for (k = 0; k < OT_VB_MAX_USER; k++) {
        if (k == OT_VB_UID_COMMON) {
            continue;
        }
        osal_seq_printf(s, "%-6u", pool->user_cnt_sum[k]);
        pool->user_cnt_sum[k] = 0;
    }
    osal_seq_printf(s, "\n");
}

static void vb_proc_show_module_pool(struct osal_proc_dir_entry *s)
{
    td_u32 i;
    td_u32 j;
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;
    struct osal_list_head *pos = TD_NULL;

    /* module common pool configure proc show */
    vb_proc_module_pool_cfg(s);

    for (i = 0, j = 0; i < g_vb_conf.max_pool_cnt; i++) {
        vb_spin_lock(flags);
        if (g_pools[i] == TD_NULL) {
            vb_spin_unlock(flags);
            continue;
        }

        pool = g_pools[i];
        j = 0;
        osal_list_for_each(pos, &pool->free_list) {
            j++;
        }
        osal_seq_printf(s, "----------------------------------------%s----------------"
            "----------------------------------------------------------------\n", pool->ac_pool_name);
        osal_seq_printf(s, "%-8s" "%-20s" "%-20s" "%-8s" "%-7s" "%-15s" "%-8s" "%-8s" "%-8s" "\n",
            "pool_id", "phys_addr", "virt_addr", "is_comm", "owner", "blk_sz", "blk_cnt", "free_list", "min_free");
        osal_seq_printf(s, "%-8u" "0x%-18lx" "0x%-18lx" "%-8d" "%-7d" "%-15llu" "%-8d" "%-8u" "%-8u" "\n",
            pool->pool_id, (td_ulong)pool->pool_phy_addr, (td_ulong)(td_uintptr_t)pool->pool_vir_addr,
            pool->is_comm_pool, pool->pool_owner, pool->blk_size, pool->blk_cnt,
            pool->free_blk_cnt, pool->min_free_blk_cnt);

        if (j == pool->blk_cnt) {
            vb_spin_unlock(flags);
            continue;
        }

        vb_show_blk_by_module_pool(s, pool);

        vb_spin_unlock(flags);
    }
}

static td_s32 vb_proc_show(struct osal_proc_dir_entry *s)
{
    td_u32 i;
    unsigned long flags;
    vb_pool_attr *pool = TD_NULL;
    td_u32 cnt = 0;

    osal_seq_printf(s, "\n[VB] Version: [" OT_MPP_VERSION "], Build Time["__DATE__", "__TIME__"]\n");
    osal_seq_printf(s, "\n");

    osal_seq_printf(s, "----------------------------------------vb pub config---------------------"
        "----------------------------------------------\n");
    osal_seq_printf(s, "max_pool_cnt\n");
    osal_seq_printf(s, "%10d\n", g_vb_conf.max_pool_cnt);

    osal_seq_printf(s, "----------------------------------------vb supplement attr---------------"
        "-----------------------------------------------\n");
    osal_seq_printf(s, "%8s" "%8s" "%9s" "\n", "config", "size", "vb_cnt");

    osal_seq_printf(s, "%8d%8d", g_supplement_conf.supplement_cfg,
                    vb_get_aligned_size(vb_get_supplement_size(&g_supplement_conf, TD_TRUE)));

    if (osal_down(&g_sema)) {
        return -ERESTARTSYS;
    }

    if (g_pools == TD_NULL) {
        osal_up(&g_sema);
        osal_seq_printf(s, "%9d\n", cnt);
        return 0;
    }

    vb_spin_lock(flags);
    for (i = 0; i < g_vb_conf.max_pool_cnt; i++) {
        if (g_pools[i] == TD_NULL) {
            continue;
        }
        pool = g_pools[i];
        cnt += pool->blk_cnt;
    }
    vb_spin_unlock(flags);
    osal_seq_printf(s, "%9d\n", cnt);

    vb_proc_show_common_pool(s);
    vb_proc_show_module_pool(s);

    osal_up(&g_sema);
    osal_seq_printf(s, "\n");

    return 0;
}
#endif

static struct osal_fileops g_file_op = {
    .open = open,
    .unlocked_ioctl = base_ioctl,
    .release = close,
#ifdef CONFIG_COMPAT
    .compat_ioctl = base_compat_ioctl,
#endif
};

static osal_dev_t *g_device;

td_s32 vb_mod_init(td_void *p)
{
    ot_unused(p);
    g_vb_init = TD_TRUE;
    return TD_SUCCESS;
}

td_void vb_mod_exit(td_void)
{
    g_vb_init = TD_FALSE;
    (td_void)memset_s(&g_supplement_conf, sizeof(g_supplement_conf), 0, sizeof(g_supplement_conf));
    return;
}

static vb_export_func g_export_funcs = {
    .pfn_vb_create_pool = vb_create_pool,
    .pfn_vb_destroy_pool = vb_destroy_pool,
    .pfn_vb_get_blk_by_pool_id = vb_get_blk_by_pool_id,
    .pfn_vb_get_blk_by_size = vb_get_blk_by_size,
    .pfn_vb_get_blk_by_size_and_module = vb_get_blk_by_size_and_module,
    .pfn_vb_get_blk_by_size_and_pool_id = vb_get_blk_by_size_and_pool_id,
    .pfn_vb_get_pool_id_by_size_and_module = vb_get_pool_id_by_size_and_module,
    .pfn_vb_get_pool_id = vb_get_pool_id,
    .pfn_vb_get_pool_info = vb_get_pool_info,
    .pfn_vb_put_blk = vb_put_blk,
    .pfn_vb_phy_to_handle = vb_phy_to_handle,
    .pfn_vb_handle_to_pool_id = vb_handle_to_pool_id,
    .pfn_vb_handle_to_blk_id = vb_handle_to_blk_id,
    .pfn_vb_handle_to_kern = vb_handle_to_kern,
    .pfn_vb_handle_to_phys = vb_handle_to_phys,
    .pfn_vb_handle_to_blk_size = vb_handle_to_blk_size,
    .pfn_vb_user_add = vb_user_add,
    .pfn_vb_user_sub = vb_user_sub,
    .pfn_vb_inquire_user_cnt = vb_inquire_user_cnt,
    .pfn_vb_inquire_one_user_cnt = vb_inquire_one_user_cnt,
    .pfn_vb_inquire_total_user_cnt = vb_inquire_total_user_cnt,
    .pfn_vb_inquire_blk_cnt = vb_inquire_blk_cnt,
    .pfn_vb_is_pool_id_valid = vb_is_pool_id_valid,
    .pfn_vb_is_blk_valid = vb_is_blk_valid,
    .pfn_vb_inquire_pool = vb_inquire_pool,
    .pfn_vb_inquire_pool_user_cnt = vb_inquire_pool_user_cnt,
    .pfn_vb_handle_to_supplement = vb_handle_to_supplement,
    .pfn_vb_is_supplement_support = vb_is_supplement_support,
    .pfn_vb_copy_supplement = vb_copy_supplement,
    .pfn_vb_get_mod_pool_vb_config = vb_get_mod_pool_vb_config,
#ifdef CONFIG_OT_VB_LOG_SUPPORT
    .pfn_vb_get_blk_by_size_ex = vb_get_blk_by_size_ex,
    .pfn_vb_get_blk_by_size_and_module_ex = vb_get_blk_by_size_and_module_ex,
    .pfn_vb_get_blk_by_size_and_pool_id_ex = vb_get_blk_by_size_and_pool_id_ex,
#endif
#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
    .pfn_vb_async_destroy_pool = vb_async_destroy_pool,
#endif
};

static umap_module g_vb_module = {
    .mod_id = OT_ID_VB,
    .mod_name = OT_MPP_MOD_VB,

    .pfn_init = vb_mod_init,
    .pfn_exit = vb_mod_exit,
    .pfn_ver_checker = TD_NULL,

    .export_funcs = &g_export_funcs,
    .data = TD_NULL,
};

static td_s32 vb_default_init(td_void)
{
    (td_void)memset_s(&g_vb_conf, sizeof(g_vb_conf), 0, sizeof(g_vb_conf));
    g_is_conf = TD_FALSE;
    g_pools = TD_NULL;
    g_blks = TD_NULL;

    if (osal_sema_init(&g_sema, 1) < 0) {
        ot_trace_vb(OT_DBG_EMERG, "sema init failed!\n");
        return TD_FAILURE;
    }

    if (osal_spin_lock_init(&g_vb_spin_lock) < 0) {
        osal_sema_destroy(&g_sema);
        ot_trace_vb(OT_DBG_EMERG, "spinlock init failed!\n");
        return TD_FAILURE;
    }

#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
    if (vb_async_destroy_ctx_init() != TD_SUCCESS) {
        osal_spin_lock_destroy(&g_vb_spin_lock);
        osal_sema_destroy(&g_sema);
        return TD_FAILURE;
    }
#endif

    return TD_SUCCESS;
}

td_s32 vb_init(td_void)
{
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_proc_entry_t *proc = TD_NULL;
#endif
    g_device = osal_createdev(UMAP_DEVNAME_VB_BASE);
    if (g_device == NULL) {
        ot_trace_vb(OT_DBG_EMERG, "VB createdev failed!\n");
        goto fail0;
    }
    g_device->fops = &g_file_op;
    g_device->minor = UMAP_VB_MINOR_BASE;
    if (osal_registerdevice(g_device)) {
        ot_trace_vb(OT_DBG_EMERG, "VB register device failed!\n");
        goto fail1;
    }
    if (cmpi_register_module(&g_vb_module)) {
        ot_trace_vb(OT_DBG_EMERG, "vb register module failed!\n");
        goto fail2;
    }

#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    /* create proc interface */
    proc = osal_create_proc_entry(PROC_ENTRY_VB, NULL);
    if (proc == TD_NULL) {
        ot_trace_vb(OT_DBG_EMERG, "vb register module failed!\n");
        goto fail3;
    }

    proc->read = vb_proc_show;
#endif
    if (vb_default_init() != TD_SUCCESS) {
        goto fail4;
    }

    ot_trace_vb(OT_DBG_INFO, "VB init OK!\n");
    return TD_SUCCESS;

fail4:
#ifdef CONFIG_OT_PROC_SHOW_SUPPORT
    osal_remove_proc_entry(PROC_ENTRY_VB, NULL);
fail3:
#endif
    cmpi_unregister_module(OT_ID_VB);
fail2:
    osal_deregisterdevice(g_device);
fail1:
    osal_destroydev(g_device);
    g_device = TD_NULL;
fail0:
    osal_printk("load base.ko ...fail!\n");
    return TD_FAILURE;
}

td_s32 vb_exit(td_void)
{
    cmpi_unregister_module(OT_ID_VB);
    osal_remove_proc_entry(PROC_ENTRY_VB, NULL);
    osal_deregisterdevice(g_device);
    osal_destroydev(g_device);

    if (vb_do_vb_exit() != TD_SUCCESS) {
        ot_trace_vb(OT_DBG_ERR, "VB exit FAILED!\n");
    } else {
        ot_trace_vb(OT_DBG_INFO, "VB exit OK!\n");
    }

#ifdef CONFIG_OT_VB_ASYNC_SUPPORT
    vb_async_destroy_ctx_deinit();
#endif

    osal_sema_destroy(&g_sema);
    osal_spin_lock_destroy(&g_vb_spin_lock);

    return TD_SUCCESS;
}

#endif /* VIDEO_BUF_BODY_INC */
