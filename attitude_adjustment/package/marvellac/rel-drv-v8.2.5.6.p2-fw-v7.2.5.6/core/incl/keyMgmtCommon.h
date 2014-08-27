/*
*                Copyright 2002-2014, Marvell Semiconductor, Inc.
* This code contains confidential information of Marvell Semiconductor, Inc.
* No rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
*/

#ifndef _KEYMGMT_COMMON_H_
#define _KEYMGMT_COMMON_H_

#define NONCE_SIZE          32
#define EAPOL_MIC_KEY_SIZE  16
#define EAPOL_MIC_SIZE      16
#define EAPOL_ENCR_KEY_SIZE 16
#define MAC_ADDR_SIZE       6
#define TK_SIZE             16
#define HDR_8021x_LEN       4
#define KEYMGMTTIMEOUTVAL 1000

typedef PACK_START struct
{
    UINT8 protocol_ver;
    UINT8 pckt_type;
    UINT16 pckt_body_len;
}
PACK_END Hdr_8021x_t;

typedef PACK_START struct
{ /*don't change this order */
UINT16 desc_ver :
    3;
UINT16 key_type :
    1;
UINT16 key_index :
    2;
UINT16 install :
    1;
UINT16 key_ack :
    1;
UINT16 key_MIC :
    1;
UINT16 secure :
    1;
UINT16 error :
    1;
UINT16 request :
    1;
/* AP_WPA2 */
UINT16 encryptedKeyData :   //WPA2
    1;
UINT16 rsvd :
    3;
/* AP_WPA2 */
}
PACK_END key_info_t;

#define ENCRYPTEDKEYDATA (1<<12)

/* AP_WPA2 */
// WPA2 GTK IE
typedef PACK_START struct
{
    UINT8 keyID_Tx;
    UINT8 rsvd;
    UINT8 GTK[16];
}
PACK_END EAPOL_WPA2_GTK_IE_t;

// WPA2 Key Data
typedef PACK_START struct
{
    UINT8 type;
    UINT8 length;
    UINT8 OUI[3];
    UINT8 dataType;
    UINT8 data[1];
    //EAPOL_WPA2_GTK_IE_t GTK_IE;
}
PACK_END EAPOL_KeyDataWPA2_t;
/* AP_WPA2 */

typedef PACK_START struct
{
    ether_hdr_t Ether_Hdr;
    Hdr_8021x_t hdr_8021x;
    UINT8 desc_type;
    PACK_START union
    {
        key_info_t key_info;
        UINT16 key_info16; /*key_info as UINT16 */
    }PACK_END k;
    UINT16 key_length;
    UINT32 replay_cnt[2];
    UINT8 key_nonce[NONCE_SIZE]; /*32 bytes */
    UINT8 EAPOL_key_IV[16];
    UINT8 key_RSC[8];
    UINT8 key_ID[8];
    UINT8 key_MIC[EAPOL_MIC_KEY_SIZE];
    UINT16 key_material_len;
    UINT8 key_data[1];
}
PACK_END EAPOL_KeyMsg_t;

#endif
