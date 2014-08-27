/*******************************************************************************************
*
* File: linkmgt.h
*
*        Client Link Management Module Module Header
*        Copy Right(2006) Marvell Semiconductors, Inc.
*
*                                                                          
* Date:         2006-10-10
* Description:  Implementation of the Client Link Management Module Services
*
*******************************************************************************************/

#ifndef LINK_MGT
#define LINK_MGT

/* Definition of virtual mac entry for each instance */
#define SELECT_FOR_BSSID    (1)
#define SELECT_FOR_SSID     (1<<1)
#define LNK_MGT_POLL_TICK   50
#define LNK_MGT_RESTART_WAIT_TICK   10
#define LNK_MGT_SCAN_IE_BUF_LEN 256
#define LNK_MGT_BSS_PROFILE_BUF_LEN 256

typedef struct linkMgtEntry_t
{
	UINT8				    active;  	
	phyMacId_t			    phyHwMacIndx;
    Timer 				    linkTimer;
    UINT8                   searchBitMap;
    UINT8	                bssid[IEEEtypes_ADDRESS_SIZE];
    IEEEtypes_SsIdElement_t ssidIE;
    UINT8                   scanIeBufLen;
    UINT8                   scanIeBuf[LNK_MGT_SCAN_IE_BUF_LEN];
	vmacEntry_t             *vMac_p;
}linkMgtEntry_t;

extern void linkMgtInit(UINT8 phyIndex);
extern SINT32 linkMgtParseScanResult(UINT8 phyIndex);
extern SINT32 linkMgtStop(UINT8 phyIndex);
extern SINT32 linkMgtStart(UINT8 phyIndex, 
                           UINT8 *prefBSSID_p, 
                           UINT8 *ieBuf_p, 
                           UINT8 ieBufLen);
extern SINT32 linkMgtReStart(UINT8 phyIndex, vmacEntry_t  *vmacEntry_p);
extern IEEEtypes_RSN_IE_t *linkMgtParseWpaIe(UINT8 *ieBuf_p, UINT16 ieBufLen);
extern IEEEtypes_RSN_IE_t *linkMgtParseWpsIe(UINT8 *ieBuf_p, UINT16 ieBufLen);
extern IEEEtypes_Generic_HT_Element_t *linkMgtParseHTGenIe(UINT8 *ieBuf_p, UINT16 ieBufLen);
extern WSC_HeaderIE_t *linkMgtParseWpsInfo(UINT16 wpsInfoID, UINT8 *ieBuf_p, UINT16 ieBufLen);
#endif /* LINK_MGT */





