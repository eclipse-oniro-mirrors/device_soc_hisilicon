/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef CRYP_SYMC_H_
#define CRYP_SYMC_H_

#include "hi_cipher_compat.h"
#include "drv_cipher_kapi.h"
#include "drv_osal_lib.h"
#include "drv_symc.h"

/*
 * brief          Initialize crypto of symc
 */
hi_s32 cryp_symc_init(hi_void);

/*
 * brief          Deinitialize crypto of symc
 */
hi_void cryp_symc_deinit(hi_void);

/*
 * brief          Create symc handle
 *
 * param handle   symc handle to be initialized
 * param chn      symc channel
 */
typedef hi_void *(*func_symc_create)(hi_u32 hard_chn);

/**
 * brief          Clear symc context
 *
 * param handle      symc handle to be destroy
 */
typedef hi_s32 (*func_symc_destroy)(hi_void *ctx);

/**
 * brief          symc key schedule
 *
 * param handle   symc handle
 * param[in]      fkey first  key buffer, default
 * param[in]      skey second key buffer, expand
 * param hisi_klen input key type, output key length in bytes
 *
 * return         0 if successful, or HI_SYMC_ERR_KEY_LEN_INVALID
 */
typedef hi_s32 (*func_symc_setkey)(hi_void *ctx, const hi_u8 *fkey, const hi_u8 *skey, hi_u32 *hisi_klen);

/**
 * brief          Symc iv schedule
 *
 * param handle   symc handle
 * param IV       Symc IV
 * param ivlen    length of iv
 *
 * return         0 if successful.
 */
typedef hi_s32 (*func_symc_setiv)(hi_void *ctx, const hi_u8 *iv, hi_u32 ivlen, hi_u32 usage);

/**
 * brief          Symc iv schedule
 *
 * param handle   symc handle
 * param IV       Symc IV
 * param ivlen    must be 128, 192 or 256
 *
 * return         0 if successful.
 */
typedef hi_void (*func_symc_getiv)(const hi_void *ctx, hi_u8 *iv, hi_u32 *ivlen);

/**
 * brief          SYMC alg and mode
 *
 * param handle   SYMC handle
 * param alg      Symmetric cipher alg
 * param mode     Symmetric cipher mode
 * param keybits  must be 128, 192 or 256
 *
 * return         0 if successful.
 */
typedef hi_void (*func_symc_setmode)(hi_void *ctx, symc_alg alg, symc_mode mode, symc_width width);

/**
 * brief          SYMC wait done
 *
 * param ctx      SYMC handle
 * return         0 if successful.
 */
typedef hi_s32 (*func_symc_wait_done)(hi_void *ctx, hi_u32 timeout);

/**
 * brief          SYMC alg and mode
 *
 * param handle   SYMC handle
 * param round    SM1 round number
 *
 * return         0 if successful.
 */
typedef hi_s32 (*func_symc_sm1_setround)(hi_void *ctx, hi_u32 round);

/**
 * brief          symc  buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * param ctx       symc ctx
 * param operation decrypt or encrypt
 * param pack     package for encrypt or decrypt.
 * param wait     last or not
 *
 * return         0 if successful
 */
typedef hi_s32 (*func_symc_crypto)(hi_void *ctx, hi_u32 operation, symc_multi_pack *pack, hi_u32 wait);

/**
 * brief          CCM/GCM set Associated Data
 *
 * param ctx      SYMC handle
 * param aad      Associated Data
 * param alen     Associated Data Length
 * param tlen     Tag length
 *
 * return         0 if successful.
 */
typedef hi_s32 (*func_aead_set_aad)(hi_void *ctx, compat_addr aad, hi_u32 alen, hi_u32 tlen);

/**
 * brief          SYMC multiple buffer encryption/decryption.
 * param[in]  id The channel number.
 * param[in]  tag tag data of CCM/GCM
 * param uuid uuid The user identification.
 *
 * return         0 if successful
 */
typedef hi_s32 (*func_aead_get_tag)(hi_void *ctx, hi_u32 tag[AEAD_TAG_SIZE_IN_WORD], hi_u32 *taglen);

/* struct of Symmetric cipher function template */
typedef struct {
    hi_u32 valid;               /* valid or not */
    symc_alg alg;               /* Alg of Symmetric cipher */
    symc_mode mode;             /* Mode of Symmetric cipher */
    func_symc_setmode setmode;  /* Set mode function */
    func_symc_sm1_setround setround; /* SM1 set round function */
    func_symc_create  create;   /* Create function */
    func_symc_destroy destroy;  /* destroy function */
    func_symc_setkey  setkey;   /* setkey function */
    func_symc_setiv   setiv;    /* setiv function */
    func_symc_getiv   getiv;    /* getiv function */
    func_aead_set_aad setadd;   /* setadd function */
    func_aead_get_tag gettag;   /* get tag function */
    func_symc_crypto  crypto;   /* crypto function */
    func_symc_wait_done waitdone; /* wait done */
} symc_func;

/*
 * brief  symc alloc channel.
 * param[out]  hard_chn symc channel.
 * retval     On success, func is returned.  On error, HI_NULL is returned.
 */
hi_s32 cryp_symc_alloc_chn(hi_u32 *hard_chn);

/*
 * brief  symc free channel.
 * param[in]  hard_chn symc channel.
 * retval     On success, func is returned.  On error, HI_NULL is returned.
 */
hi_void cryp_symc_free_chn(hi_u32 hard_chn);

/*
 * brief  Clone the function from template of aes engine.
 * param[in]  alg The alg of Symmetric cipher.
 * param[in]  mode The work mode.
 * retval     On success, func is returned.  On error, HI_NULL is returned.
 */
symc_func *cryp_get_symc_op(hi_cipher_alg alg, hi_cipher_work_mode mode);
#endif
