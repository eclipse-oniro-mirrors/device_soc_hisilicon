
CIPHER_DRV_CFLAGS += -I$(CIPHER_DIR)/drv/drivers/core/include
CIPHER_DRV_CFLAGS += -I$(CIPHER_DIR)/drv/drivers/core/rsa/include
CIPHER_DRV_CFLAGS += -I$(CIPHER_DIR)/drv/drivers/crypto/include
CIPHER_DRV_CFLAGS += -I$(CIPHER_DIR)/drv/drivers/extend/include
CIPHER_DRV_CFLAGS += -I$(CIPHER_DIR)/../include

CIPHER_DRV_OBJS +=  drv/drivers/core/drv_symc_v300.o          \
                    drv/drivers/core/drv_hash_v300.o          \
                    drv/drivers/core/drv_pke_v200.o           \
                    drv/drivers/core/drv_trng_v200.o          \
                    drv/drivers/core/drv_lib.o

CIPHER_DRV_OBJS +=  drv/drivers/core/rsa/crypto_drv_common.o      \
                    drv/drivers/core/rsa/drv_pke_priv.o           \
                    drv/drivers/core/rsa/drv_pke_rsa_pub_enc.o    \
                    drv/drivers/core/rsa/drv_pke_rsa_priv_dec.o   \
                    drv/drivers/core/rsa/drv_pke_rsa_sign.o       \
                    drv/drivers/core/rsa/drv_pke_rsa_v15.o        \
                    drv/drivers/core/rsa/drv_pke_rsa_v21.o        \
                    drv/drivers/core/rsa/drv_pke_rsa_verify.o

CIPHER_DRV_OBJS += drv/drivers/crypto/cryp_symc.o             \
                   drv/drivers/crypto/cryp_hash.o             \
                   drv/drivers/crypto/cryp_trng.o             \
                   drv/drivers/crypto/cryp_rsa.o              \
                   drv/drivers/crypto/cryp_sm2.o

CIPHER_DRV_OBJS += drv/drivers/kapi_symc.o                    \
                   drv/drivers/kapi_hash.o                    \
                   drv/drivers/kapi_rsa.o                     \
                   drv/drivers/kapi_trng.o                    \
                   drv/drivers/kapi_sm2.o                     \
                   drv/drivers/ot_drv_cipher.o                \
                   drv/drivers/kapi_dispatch.o

CIPHER_DRV_OBJS += drv/drivers/common_check_param.o