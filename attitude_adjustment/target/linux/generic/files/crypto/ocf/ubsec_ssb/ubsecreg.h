
/*
 * Copyright (c) 2008 Daniel Mueller (daniel@danm.de)
 * Copyright (c) 2000 Theo de Raadt
 * Copyright (c) 2001 Patrik Lindergren (patrik@ipunplugged.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Effort sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F30602-01-2-0537.
 *
 */

/*
 * Register definitions for 5601 BlueSteel Networks Ubiquitous Broadband
 * Security "uBSec" chip.  Definitions from revision 2.8 of the product
 * datasheet.
 */

#define BS_BAR          0x10    /* DMA base address register */
#define BS_TRDY_TIMEOUT     0x40    /* TRDY timeout */
#define BS_RETRY_TIMEOUT    0x41    /* DMA retry timeout */

#define UBS_PCI_RTY_SHIFT           8
#define UBS_PCI_RTY_MASK            0xff
#define UBS_PCI_RTY(misc) \
    (((misc) >> UBS_PCI_RTY_SHIFT) & UBS_PCI_RTY_MASK)

#define UBS_PCI_TOUT_SHIFT          0
#define UBS_PCI_TOUT_MASK           0xff
#define UBS_PCI_TOUT(misc) \
    (((misc) >> PCI_TOUT_SHIFT) & PCI_TOUT_MASK)

/*
 * DMA Control & Status Registers (offset from BS_BAR)
 */
#define BS_MCR1     0x20    /* DMA Master Command Record 1 */
#define BS_CTRL     0x24    /* DMA Control */
#define BS_STAT     0x28    /* DMA Status */
#define BS_ERR      0x2c    /* DMA Error Address */
#define BS_DEV_ID   0x34    /* IPSec Device ID */

/* BS_CTRL - DMA Control */
#define BS_CTRL_RESET       0x80000000  /* hardware reset, 5805/5820 */
#define BS_CTRL_MCR2INT     0x40000000  /* enable intr MCR for MCR2 */
#define BS_CTRL_MCR1INT     0x20000000  /* enable intr MCR for MCR1 */
#define BS_CTRL_OFM     0x10000000  /* Output fragment mode */
#define BS_CTRL_BE32        0x08000000  /* big-endian, 32bit bytes */
#define BS_CTRL_BE64        0x04000000  /* big-endian, 64bit bytes */
#define BS_CTRL_DMAERR      0x02000000  /* enable intr DMA error */
#define BS_CTRL_RNG_M       0x01800000  /* RNG mode */
#define BS_CTRL_RNG_1       0x00000000  /* 1bit rn/one slow clock */
#define BS_CTRL_RNG_4       0x00800000  /* 1bit rn/four slow clocks */
#define BS_CTRL_RNG_8       0x01000000  /* 1bit rn/eight slow clocks */
#define BS_CTRL_RNG_16      0x01800000  /* 1bit rn/16 slow clocks */
#define BS_CTRL_SWNORM      0x00400000  /* 582[01], sw normalization */
#define BS_CTRL_FRAG_M      0x0000ffff  /* output fragment size mask */
#define BS_CTRL_LITTLE_ENDIAN   (BS_CTRL_BE32 | BS_CTRL_BE64)

/* BS_STAT - DMA Status */
#define BS_STAT_MCR1_BUSY   0x80000000  /* MCR1 is busy */
#define BS_STAT_MCR1_FULL   0x40000000  /* MCR1 is full */
#define BS_STAT_MCR1_DONE   0x20000000  /* MCR1 is done */
#define BS_STAT_DMAERR      0x10000000  /* DMA error */
#define BS_STAT_MCR2_FULL   0x08000000  /* MCR2 is full */
#define BS_STAT_MCR2_DONE   0x04000000  /* MCR2 is done */
#define BS_STAT_MCR1_ALLEMPTY   0x02000000  /* 5821, MCR1 is empty */
#define BS_STAT_MCR2_ALLEMPTY   0x01000000  /* 5821, MCR2 is empty */

/* BS_ERR - DMA Error Address */
#define BS_ERR_ADDR     0xfffffffc  /* error address mask */
#define BS_ERR_READ     0x00000002  /* fault was on read */

struct ubsec_pktctx {
    u_int32_t   pc_deskey[6];       /* 3DES key */
    u_int32_t   pc_hminner[5];      /* hmac inner state */
    u_int32_t   pc_hmouter[5];      /* hmac outer state */
    u_int32_t   pc_iv[2];       /* [3]DES iv */
    u_int16_t   pc_flags;       /* flags, below */
    u_int16_t   pc_offset;      /* crypto offset */
} __attribute__ ((packed));

#define UBS_PKTCTX_ENC_3DES 0x8000      /* use 3des */
#define UBS_PKTCTX_ENC_AES  0x8000      /* use aes */
#define UBS_PKTCTX_ENC_NONE 0x0000      /* no encryption */
#define UBS_PKTCTX_INBOUND  0x4000      /* inbound packet */
#define UBS_PKTCTX_AUTH     0x3000      /* authentication mask */
#define UBS_PKTCTX_AUTH_NONE    0x0000      /* no authentication */
#define UBS_PKTCTX_AUTH_MD5 0x1000      /* use hmac-md5 */
#define UBS_PKTCTX_AUTH_SHA1    0x2000      /* use hmac-sha1 */
#define UBS_PKTCTX_AES128   0x0         /* AES 128bit keys */
#define UBS_PKTCTX_AES192   0x100       /* AES 192bit keys */
#define UBS_PKTCTX_AES256   0x200       /* AES 256bit keys */

struct ubsec_pktctx_des {
    volatile u_int16_t  pc_len;     /* length of ctx struct */
    volatile u_int16_t  pc_type;    /* context type */
    volatile u_int16_t  pc_flags;   /* flags, same as above */
    volatile u_int16_t  pc_offset;  /* crypto/auth offset */
    volatile u_int32_t  pc_deskey[6];   /* 3DES key */
    volatile u_int32_t  pc_iv[2];   /* [3]DES iv */
    volatile u_int32_t  pc_hminner[5];  /* hmac inner state */
    volatile u_int32_t  pc_hmouter[5];  /* hmac outer state */
} __attribute__ ((packed));

struct ubsec_pktctx_aes128 {
    volatile u_int16_t  pc_len;         /* length of ctx struct */
    volatile u_int16_t  pc_type;        /* context type */
    volatile u_int16_t  pc_flags;       /* flags, same as above */
    volatile u_int16_t  pc_offset;      /* crypto/auth offset */
    volatile u_int32_t  pc_aeskey[4];   /* AES 128bit key */
    volatile u_int32_t  pc_iv[4];       /* AES iv */
    volatile u_int32_t  pc_hminner[5];  /* hmac inner state */
    volatile u_int32_t  pc_hmouter[5];  /* hmac outer state */
} __attribute__ ((packed));

struct ubsec_pktctx_aes192 {
    volatile u_int16_t  pc_len;         /* length of ctx struct */
    volatile u_int16_t  pc_type;        /* context type */
    volatile u_int16_t  pc_flags;       /* flags, same as above */
    volatile u_int16_t  pc_offset;      /* crypto/auth offset */
    volatile u_int32_t  pc_aeskey[6];   /* AES 192bit key */
    volatile u_int32_t  pc_iv[4];       /* AES iv */
    volatile u_int32_t  pc_hminner[5];  /* hmac inner state */
    volatile u_int32_t  pc_hmouter[5];  /* hmac outer state */
} __attribute__ ((packed));

struct ubsec_pktctx_aes256 {
    volatile u_int16_t  pc_len;         /* length of ctx struct */
    volatile u_int16_t  pc_type;        /* context type */
    volatile u_int16_t  pc_flags;       /* flags, same as above */
    volatile u_int16_t  pc_offset;      /* crypto/auth offset */
    volatile u_int32_t  pc_aeskey[8];   /* AES 256bit key */
    volatile u_int32_t  pc_iv[4];       /* AES iv */
    volatile u_int32_t  pc_hminner[5];  /* hmac inner state */
    volatile u_int32_t  pc_hmouter[5];  /* hmac outer state */
} __attribute__ ((packed));

#define UBS_PKTCTX_TYPE_IPSEC_DES   0x0000
#define UBS_PKTCTX_TYPE_IPSEC_AES   0x0040

struct ubsec_pktbuf {
    volatile u_int32_t  pb_addr;    /* address of buffer start */
    volatile u_int32_t  pb_next;    /* pointer to next pktbuf */
    volatile u_int32_t  pb_len;     /* packet length */
} __attribute__ ((packed));
#define UBS_PKTBUF_LEN      0x0000ffff  /* length mask */

struct ubsec_mcr {
    volatile u_int16_t  mcr_pkts;   /* #pkts in this mcr */
    volatile u_int16_t  mcr_flags;  /* mcr flags (below) */
    volatile u_int32_t  mcr_cmdctxp;    /* command ctx pointer */
    struct ubsec_pktbuf mcr_ipktbuf;    /* input chain header */
    volatile u_int16_t  mcr_reserved;
    volatile u_int16_t  mcr_pktlen;
    struct ubsec_pktbuf mcr_opktbuf;    /* output chain header */
} __attribute__ ((packed));

struct ubsec_mcr_add {
    volatile u_int32_t  mcr_cmdctxp;    /* command ctx pointer */
    struct ubsec_pktbuf mcr_ipktbuf;    /* input chain header */
    volatile u_int16_t  mcr_reserved;
    volatile u_int16_t  mcr_pktlen;
    struct ubsec_pktbuf mcr_opktbuf;    /* output chain header */
} __attribute__ ((packed));

#define UBS_MCR_DONE        0x0001      /* mcr has been processed */
#define UBS_MCR_ERROR       0x0002      /* error in processing */
#define UBS_MCR_ERRORCODE   0xff00      /* error type */

struct ubsec_ctx_keyop {
    volatile u_int16_t  ctx_len;    /* command length */
    volatile u_int16_t  ctx_op;     /* operation code */
    volatile u_int8_t   ctx_pad[60];    /* padding */
} __attribute__ ((packed));
#define UBS_CTXOP_DHPKGEN   0x01        /* dh public key generation */
#define UBS_CTXOP_DHSSGEN   0x02        /* dh shared secret gen. */
#define UBS_CTXOP_RSAPUB    0x03        /* rsa public key op */
#define UBS_CTXOP_RSAPRIV   0x04        /* rsa private key op */
#define UBS_CTXOP_DSASIGN   0x05        /* dsa signing op */
#define UBS_CTXOP_DSAVRFY   0x06        /* dsa verification */
#define UBS_CTXOP_RNGBYPASS 0x41        /* rng direct test mode */
#define UBS_CTXOP_RNGSHA1   0x42        /* rng sha1 test mode */
#define UBS_CTXOP_MODADD    0x43        /* modular addition */
#define UBS_CTXOP_MODSUB    0x44        /* modular subtraction */
#define UBS_CTXOP_MODMUL    0x45        /* modular multiplication */
#define UBS_CTXOP_MODRED    0x46        /* modular reduction */
#define UBS_CTXOP_MODEXP    0x47        /* modular exponentiation */
#define UBS_CTXOP_MODINV    0x48        /* modular inverse */

struct ubsec_ctx_rngbypass {
    volatile u_int16_t  rbp_len;    /* command length, 64 */
    volatile u_int16_t  rbp_op;     /* rng bypass, 0x41 */
    volatile u_int8_t   rbp_pad[60];    /* padding */
} __attribute__ ((packed));

/* modexp: C = (M ^ E) mod N */
struct ubsec_ctx_modexp {
    volatile u_int16_t  me_len;     /* command length */
    volatile u_int16_t  me_op;      /* modexp, 0x47 */
    volatile u_int16_t  me_E_len;   /* E (bits) */
    volatile u_int16_t  me_N_len;   /* N (bits) */
    u_int8_t        me_N[2048/8];   /* N */
} __attribute__ ((packed));

struct ubsec_ctx_rsapriv {
    volatile u_int16_t  rpr_len;    /* command length */
    volatile u_int16_t  rpr_op;     /* rsaprivate, 0x04 */
    volatile u_int16_t  rpr_q_len;  /* q (bits) */
    volatile u_int16_t  rpr_p_len;  /* p (bits) */
    u_int8_t        rpr_buf[5 * 1024 / 8];  /* parameters: */
                        /* p, q, dp, dq, pinv */
} __attribute__ ((packed));
