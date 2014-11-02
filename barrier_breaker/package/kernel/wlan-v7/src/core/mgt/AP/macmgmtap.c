/** 
  * Copyright (C) 2008-2014, Marvell International Ltd. 
  * 
  * This software file (the "File") is distributed by Marvell International 
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991 
  * (the "License").  You may use, redistribute and/or modify this File in 
  * accordance with the terms and conditions of the License, a copy of which 
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
  * this warranty disclaimer.
  *
  */


/******************** (c) Marvell Semiconductor, Inc., 2001 *******************
*
* Purpose: 
*    This file contains the implementations of the function prototypes given 
*    in the associated header file for the MAC Management Service Task. 
* 
* Public Procedures: 
*    macMgmtAp_Init       Initialzies all MAC Management Service Task and 
*                           related components 
*    macMgmtAp_Start      Starts running the MAC Management Service Task 
* 
* Private Procedures: 
*    MacMgmtApTask            The actual MAC Management Service Task 
*    RegisterRead           Reads a value from a specified register 
*    RegisterWrite          Writes a value to a specified register 
*    SignalStrengthRead     Reads signal strength 
*    StationInfoRead        Reads staton info 
*    StationInfoWrite       Writes station info 
*    StationListRead        Reads the list of known stations 
*    StatisticsRead         Reads accumulated statistics 
* 
* Notes: 
*    None. 
* 
*****************************************************************************/

/*============================================================================= */
/*                               INCLUDE FILES */
/*============================================================================= */
#include "ap8xLnxIntf.h"
#include "wltypes.h"
#include "IEEE_types.h"

#include "mib.h"
#include "ds.h"
#include "keyMgmtCommon.h"
#include "keyMgmt.h"
#include "tkip.h"
#include "StaDb.h"
#include "macmgmtap.h"
#include "macMgmtMlme.h"
#include "timer.h"
#include "wldebug.h"
#include "wlvmac.h"
#include "mlmeApi.h"
/*============================================================================= */
/*                                DEFINITIONS */
/*============================================================================= */
#define MAC_MGMT_MAIN_EVENT_TRIGGERS macMgmtMain_802_11_MGMT_MSG_RCVD | \
	macMgmtMain_PWR_MODE_CHANGE_RCVD | \
	macMgmtMain_SME_MSG_RCVD  | \
	macMgmtMain_TIMER_CALLBACK | \
	macMgmtMain_TIMER_EXPIRED


/*============================================================================= */
/*                             GLOBAL VARIABLES */
/*============================================================================= */
extern UINT8 StopWirelessflag;

/* */
/* State that the MAC Management Service Task is in */
/* */

extern void macMgtSyncSrvInit(vmacApInfo_t *vmacSta_p);
extern SINT8 evtSmeCmdMsg(vmacApInfo_t *vmacSta_p,UINT8 *);
extern SINT8 evtDot11MgtMsg(vmacApInfo_t *vmacSta_p, UINT8 *, struct sk_buff *skb);
extern void RxBeacon(vmacApInfo_t *vmacSta_p , void *BssData_p, UINT16 len, UINT32 rssi);	

extern int wlMgmtTx(struct sk_buff *skb, struct net_device *netdev);
extern int wlDataTx(struct sk_buff *skb, struct net_device *netdev);
extern int wlDataTxUnencr(struct sk_buff *skb, struct net_device *netdev, extStaDb_StaInfo_t *pStaInfo);
/*============================================================================= */
/*                   PRIVATE PROCEDURES (ANSI Prototypes) */
/*============================================================================= */

/*============================================================================= */
/*                         CODED PUBLIC PROCEDURES */
/*============================================================================= */
#ifdef SOC_W8764
void dispCSIorNonCompMatrix(UINT8 *pData, UINT8 Nb, UINT8 Nr, UINT8 Nc, UINT8 Ng, UINT8 type);
void dispCompressedCode(UINT8 *pCodeData, UINT8 code, UINT8 numAngles, UINT8 Ng);
void dispTxBf(macmgmtQ_MgmtMsg3_t *MgmtMsg_p);
#endif

/******************************************************************************
* 
* Name: macMgmtAp_Init 
* 
* Description: 
*   This routine is called to initialize the the MAC Management Service Task 
*   and related components. 
* 
* Conditions For Use: 
*   None. 
* 
* Arguments: 
*   None. 
* 
* Return Value: 
*   Status indicating success or failure 
* 
* Notes: 
*   None. 
* 
* PDL: 
*   Create 802_11 receive queue and sme msg receive queue
*   If the queue was successfully initialized Then 
*      Create the MAC Management Task by calling os_TaskCreate() 
*      If creating the MAC Management Task succeeded Then 
*         Set the MAC Management Service State to IDLE 
*         Set the power mode to active 
*         Return OS_SUCCESS 
*      End If 
*   End If 
* 
*   Return OS_FAIL 
* END PDL 
* 
*****************************************************************************/

extern WL_STATUS macMgmtAp_Init( vmacApInfo_t *vmacSta_p,UINT32 maxStns, IEEEtypes_MacAddr_t *stnMacAddr)
{
	/* Init the AP Synchronization State Machine Service */
	macMgtSyncSrvInit(vmacSta_p);
	macMgmtMlme_Init(vmacSta_p, maxStns, stnMacAddr);
#if !defined(CONDOR2)&&defined(HARRIER)
	/* Init RF calibration timer */
	TimerInit(&rfCalTimer);
#endif

	return (OS_SUCCESS);
}


/******************************************************************************
*
* Name: macMgmtQ_SmeWriteNoBlock
*
* Description:
*   This routine is called to write a message to the queue where messages
*   from the SME task are placed for the MAC Management Service Task. If
*   writing to the queue cannot immediately occur, then the routine returns
*   with a failure status (non-blocking).
*
* Conditions For Use:
*   The queue has been initialized by calling macMgmtQ_Init().
*
* Arguments:
*   Arg1 (i  ): SmeCmd_p - a pointer to the message to be placed on
*               the queue
*
* Return Value:
*   Status indicating success or failure
*
* Notes:
*   None.
*
* PDL:
*   Call os_QueueWriteNoBlock() to write SmeCmd_p to the SME message queue
*   If the message was successfully placed on the queue Then
*      Return OS_SUCCESS
*   Else
*      Return OS_FAIL
*   End If
* END PDL
*
*****************************************************************************/
extern WL_STATUS macMgmtQ_SmeWriteNoBlock(vmacApInfo_t *vmacSta_p, macmgmtQ_SmeCmd_t *SmeCmd_p )
{
	//	WL_STATUS status;
	evtSmeCmdMsg(vmacSta_p,(UINT8 *)SmeCmd_p); 
	return(OS_SUCCESS);
}

#ifdef CLIENT_SUPPORT
WLAN_RX_INFO curRxInfo_g;
#endif /* CLIENT_SUPPORT */

/*============================================================================= */
/*                         CODED PRIVATE PROCEDURES */
/*============================================================================= */

void receiveWlanMsg(struct net_device *dev, struct sk_buff *skb, UINT32 rssi, BOOLEAN stationpacket)
{
	struct wlprivate *priv = NETDEV_PRIV_P(struct wlprivate, dev);
	vmacApInfo_t *vmacSta_p = priv->vmacSta_p;
	//	MIB_802DOT11 *mib=vmacSta_p->Mib802dot11;
	//	macmgmtQ_SmeCmd_t *SmeCmd_p;
	//	struct sk_buff *mgmtBuff_p;
	IEEEtypes_Frame_t *wlanMsg_p;

	wlanMsg_p = (IEEEtypes_Frame_t *) ((UINT8 *)skb->data -2);

	wlanMsg_p->Hdr.FrmBodyLen = skb->len;

#ifdef SOC_W8764
    dispTxBf((macmgmtQ_MgmtMsg3_t *)wlanMsg_p);
#endif

	switch (wlanMsg_p->Hdr.FrmCtl.Type)
	{
	case IEEE_TYPE_DATA:
		break;
	case IEEE_TYPE_MANAGEMENT:
		{
			WLDBG_INFO(DBG_LEVEL_11, "IEEE_TYPE_MANAGEMENT message received. \n");
			switch (wlanMsg_p->Hdr.FrmCtl.Subtype)
			{
			case 0xf:	//fw debug type
				printk("FW: %s\n", (UINT8 *)((UINT32)wlanMsg_p+6));
				/*WLDBG_INFO(DBG_LEVEL_11,"%s\n", (UINT8 *)((UINT32)wlanMsg_p+2+sizeof(IEEEtypes_MgmtHdr2_t)));*/
				break;
#ifndef INTEROP
			case IEEE_MSG_QOS_ACTION:

				WLDBG_INFO(DBG_LEVEL_11, "IEEE_MSG_QOS_ACTION message received. \n");
				break;
#endif
			case IEEE_MSG_PROBE_RQST:
#if 0//def SOC_W8764
                {
                    char ssid[32] = {0};
                    macmgmtQ_MgmtMsg3_t *probe_p = (macmgmtQ_MgmtMsg3_t *)wlanMsg_p;
                    memcpy(ssid, probe_p->Body.ProbeRqst.SsId.SsId,probe_p->Body.ProbeRqst.SsId.Len);  
                    printk("IEEE_MSG_PROBE_RQST from station %s for SSID = %s \n", mac_display(wlanMsg_p->Hdr.Addr2), ssid);
                }
#endif
				macMgmtMlme_ProbeRqst(vmacSta_p,(macmgmtQ_MgmtMsg3_t *)wlanMsg_p);
				break;
#ifdef CLIENT_SUPPORT
				/* Intercept beacon here for AP.
				* Client beacon will be handled later.
				*/
			case IEEE_MSG_BEACON:
				RxBeacon(vmacSta_p,wlanMsg_p, skb->len, rssi);		
#endif //CLIENT_SUPPORT
			default:
				/* 802.11 Management frame::feed MLME State Machines */
#ifdef CLIENT_SUPPORT
				{
#define SK_BUF_RESERVED_PAD     6
					if(stationpacket)
					{
						vmacEntry_t *targetVMacEntry_p = NULL;
						IEEEtypes_MgmtHdr_t *Hdr_p;

						/*Have to handle broadcast deauth pkt too. Checking whether deauth pkt bssid is for station is done at later part*/
						if( (wlanMsg_p->Hdr.FrmCtl.Subtype == IEEE_MSG_BEACON )
                            	|| (wlanMsg_p->Hdr.FrmCtl.Subtype == IEEE_MSG_QOS_ACTION )
                            	|| (wlanMsg_p->Hdr.FrmCtl.Subtype == IEEE_MSG_DEAUTHENTICATE && IS_GROUP((UINT8 *) &(wlanMsg_p->Hdr.Addr1))))						
{
							targetVMacEntry_p = sme_GetParentVMacEntry(vmacSta_p->VMacEntry.phyHwMacIndx) ;
						}
						else
						{
							targetVMacEntry_p = vmacGetVMacEntryByAddr((UINT8 *)&wlanMsg_p->Hdr.Addr1) ;
						}
						if( targetVMacEntry_p )
						{
							curRxInfo_g.RSSI = (UINT8)rssi;
							//printk("****** client process dot11 rssi=%d\n",curRxInfo_g.RSSI);
							//skb_push(skb, SK_BUF_RESERVED_PAD);
							skb_push(skb, 2); /* For UINT16 FrmBodyLen */
							Hdr_p = (IEEEtypes_MgmtHdr_t *)(skb->data);
							Hdr_p->FrmBodyLen = skb->len;
							skb_push(skb, 4); /* For UINT32 priv_p pointer */
							targetVMacEntry_p->dot11MsgEvt(skb->data, (UINT8 *)&curRxInfo_g, targetVMacEntry_p->info_p);
							skb_pull(skb, SK_BUF_RESERVED_PAD);
						}
					}
					else
					{
						evtDot11MgtMsg(vmacSta_p,(UINT8 *)wlanMsg_p, skb);
					}
				}
#else /* CLIENT_SUPPORT */
				evtDot11MgtMsg(vmacSta_p,(UINT8 *)wlanMsg_p, skb);
#endif /* CLIENT_SUPPORT */
				break;
			}
			break;
		}
	case IEEE_TYPE_CONTROL:
		{
			WLDBG_INFO(DBG_LEVEL_11, "IEEE_TYPE_CONTROL message received. \n");
			{
				switch (wlanMsg_p->Hdr.FrmCtl.Subtype)
				{
				case PS_POLL:
				case RTS:
				case CTS:
				case ACK:
				case CF_END:
				case CF_END_CF_ACK:
					break;
				default:
					break;
				}
			} /* end if */

			break;
		}
	default:
		/* Uknown Type */
		break;
	}
	dev_kfree_skb_any(skb);    
}


extern WL_STATUS txMgmtMsg(struct net_device *dev,struct sk_buff *skb)
{
	//	WL_STATUS status = FAIL;
	WLDBG_INFO(DBG_LEVEL_11,"IEEE_TYPE_MGMT_CONTROL message txed skb = %x \n", skb);
	if(wlMgmtTx(skb, dev))
		dev_kfree_skb_any(skb);    
	return(SUCCESS);
}

extern WL_STATUS txDataMsg(struct net_device *dev,struct sk_buff *skb)
{
	//	WL_STATUS status = FAIL;
	WLDBG_INFO(DBG_LEVEL_11,"IEEE_TYPE_Data message txed skb = %x \n", skb);
	if(wlDataTx(skb, dev))
		dev_kfree_skb_any(skb);    
	return(SUCCESS);
}

extern WL_STATUS txDataMsg_UnEncrypted(struct net_device *dev,struct sk_buff *skb, extStaDb_StaInfo_t *pStaInfo)
{
	//	WL_STATUS status = FAIL;
	WLDBG_INFO(DBG_LEVEL_11,"IEEE_TYPE_Data message txed skb = %x \n", skb);
	if(wlDataTxUnencr(skb, dev, pStaInfo))
		dev_kfree_skb_any(skb);
	return(SUCCESS);
}

#ifdef SOC_W8764

/* defines for CSI, non-compressed, and compressed Tx Beamforming Matrices */
UINT8 csiBits[4] = {4,5,6,8};
UINT8 nonCompBits[4] = {4,2,6,8};
UINT8 numSubCarriers[2][3] = {{ 56, 30, 16},
                              {114, 58, 30}};
UINT8 compBits[4][2]= { {1,3},
                        {2,4},
                        {3,5},
                        {4,6}};

/* Number of Angles, based on Nr and Nc : only defined for 2x1, 2x2, 3x1, 3x2, 3x3, 4x1, 4x2, 4x3, 4x4 - leave 
   others set to 0. */
UINT8 numAnglesNa[4][4] = {{0,  0,  0,  0},
                           {2,  2,  0,  0},
                           {4,  6,  6,  0},
                           {6, 10, 12, 12}};


#define DWORDBITSIZE        8*sizeof(UINT32)
#define TXBF_BITSIZE        DWORDBITSIZE - sizeof(UINT8)
#define TXBF_BYTEINC        sizeof(UINT32) - sizeof(UINT8)
#define TXBF_CSIAMPBITMASK  7
#define TXBF_CSIAMPBITSIZE  3
/* Work with 24 bits at a time and check for overlap.  Always use memcpy to cross byte boundaries. */
UINT8 txBfMatrix[58][4][3][2]; /* For debug use max of 2 (I,J) x 58 subtones * 4 paths *3 streams max */
UINT8 txBfCsiAmp[58] = {0};


void dispTxBf(macmgmtQ_MgmtMsg3_t *MgmtMsg_p)
{
#if 0
    UINT8 dispMgmt[256];
    UINT32 i;
    UINT32 numBits = 0;
    UINT32 matrixSizeBits = 0;
    UINT32 reportSizeBits = 0;
    UINT32 Ns = 0;
    UINT32 b_Psi = 0;
    UINT32 b_Phi = 0;
    UINT32 Na = 0;

    printk("--------------------------------------------------------------------------------\n");
    printk("receiveWlanMsg Rx Managment Messsage \n");
    memcpy((UINT8 *)dispMgmt, (UINT8 *)MgmtMsg_p, 128);
    for (i=0; i < 128; i++)
    {
        printk(" 0x%02x", dispMgmt[i]);
        if ((i+1)%16 == 0) printk("\n");
    }
    printk("\n");
    
    if ((MgmtMsg_p->Hdr.FrmCtl.Type == IEEE_TYPE_MANAGEMENT) && 
        (MgmtMsg_p->Hdr.FrmCtl.Subtype == IEEE_MSG_ACTION_NO_ACK))
    {
        UINT8 dispData[64];
        printk("---------------------------IEEE_MSG_ACTION_NO_ACK__-----------------------------\n");
        if(MgmtMsg_p->Body.CsiReport.Category == HT_CATEGORY)
        {
            switch (MgmtMsg_p->Body.CsiReport.Action)
            {
            case ACTION_MIMO_CSI_REPORT:
                {
                    printk("Rx ACTION Frame CSI Report received. \n");
                    numBits = csiBits[MgmtMsg_p->Body.CsiReport.Mimo.CoeffSizeNb];
                    Ns = numSubCarriers[MgmtMsg_p->Body.CsiReport.Mimo.MimoBw][MgmtMsg_p->Body.CsiReport.Mimo.GroupingNg];
                    dispCSIorNonCompMatrix((UINT8 *)&MgmtMsg_p->Body.CsiReport.CSI.Data[0], 
                                           numBits, 
                                           MgmtMsg_p->Body.CsiReport.Mimo.NrIndex+1, 
                                           MgmtMsg_p->Body.CsiReport.Mimo.NcIndex+1, 
                                           Ns, MgmtMsg_p->Body.CsiReport.Action);
                    break;
                }
            case ACTION_MIMO_COMP_REPORT:
                {
                    #if 1
                    b_Psi = compBits[MgmtMsg_p->Body.CsiReport.Mimo.CodeBookInfo][0];
                    b_Phi = compBits[MgmtMsg_p->Body.CsiReport.Mimo.CodeBookInfo][1];
                    
                    Ns = numSubCarriers[MgmtMsg_p->Body.CsiReport.Mimo.MimoBw][MgmtMsg_p->Body.CsiReport.Mimo.GroupingNg];
                    Na = numAnglesNa[MgmtMsg_p->Body.CsiReport.Mimo.NrIndex][MgmtMsg_p->Body.CsiReport.Mimo.NcIndex];
                    printk("Rx ACTION Frame Compressed Report received b_Psi = %d b_Phi = %d Na = %d. \n",
                           b_Psi, b_Phi, Na);
                    dispCompressedCode((UINT8 *)&MgmtMsg_p->Body.CsiReport.CSI.Data[0], 
                                       MgmtMsg_p->Body.CsiReport.Mimo.CodeBookInfo, 
                                       Na, 
                                       Ns);
                    #endif
                    break;
                }
            case ACTION_MIMO_NONCOMP_REPORT:
                {                    
                    numBits = nonCompBits[MgmtMsg_p->Body.CsiReport.Mimo.CoeffSizeNb];
#ifdef SC_DEBUG
                    printk("Rx ACTION Frame Non-Compressed Report received. \n");
                    printk("CSI/Non-compressed i+j coefficient bit size = %d \n", numBits);
#endif
                    matrixSizeBits = 3 + 2*(MgmtMsg_p->Body.CsiReport.Mimo.NcIndex+1)*
                                           (MgmtMsg_p->Body.CsiReport.Mimo.NrIndex+1)* numBits;
        
                    Ns = numSubCarriers[MgmtMsg_p->Body.CsiReport.Mimo.MimoBw][MgmtMsg_p->Body.CsiReport.Mimo.GroupingNg];
                    reportSizeBits = Ns*matrixSizeBits;

                    printk("CSI/Non-compressed matrix size in bits = 3+2× Nb×Nc×Nr = %d \n", matrixSizeBits );
                    printk("CSI/Non-compressed Ns = %d \n", Ns );
                    printk("CSI/Non-compressed Report size in bits = %d bytes = %d\n", reportSizeBits,  reportSizeBits/8);

                    //dispCSIorNonCompMatrix(UINT8 *pData, UINT8 Nb, UINT8 Nr, UINT8 Nc, UINT8 Ng, UINT8 type)
                    dispCSIorNonCompMatrix((UINT8 *)&MgmtMsg_p->Body.CsiReport.CSI.Data[0], 
                                           numBits, 
                                           MgmtMsg_p->Body.CsiReport.Mimo.NrIndex+1, 
                                           MgmtMsg_p->Body.CsiReport.Mimo.NcIndex+1, 
                                           Ns, MgmtMsg_p->Body.CsiReport.Action);
                    break;
                }
            default:
                break;
            }

        }

        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Category = %x \n", MgmtMsg_p->Body.CsiReport.Category);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Action   = %x \n", MgmtMsg_p->Body.CsiReport.Action);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.NcIndex       = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.NcIndex);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.NrIndex       = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.NrIndex);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.MimoBw        = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.MimoBw);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.GroupingNg    = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.GroupingNg);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.CoeffSizeNb   = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.CoeffSizeNb);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.CodeBookInfo  = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.CodeBookInfo);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.RemMatrixSeg  = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.RemMatrixSeg);
        printk("IEEE_MSG_ACTION_NO_ACK CsiReport.Mimo.SoundingTmStp = %x \n", MgmtMsg_p->Body.CsiReport.Mimo.SoundingTmStp);
        printk("SNR for %d receive paths \n", MgmtMsg_p->Body.CsiReport.Mimo.NrIndex+1);


        printk("\nRemaining CSI Data \n");
        memcpy((UINT8 *)&dispData, (UINT8 *)&MgmtMsg_p->Body.CsiReport.CSI.Data, 64);
        for (i=0; i < (MgmtMsg_p->Body.CsiReport.Mimo.NrIndex+1); i++)
        {
            printk("SNR for %d receive path = %x \n", i, dispData[i]); 
        }        
        for ( ; i < reportSizeBits/8; i++)
        {
            printk(" 0x%02x", dispData[i]);
            if ((i+1)%16 == 0) printk("\n");
        }
        printk("\n\n");
    }

#endif

}

void dispCSIorNonCompMatrix(UINT8 *pData, UINT8 Nb, UINT8 Nr, UINT8 Nc, UINT8 Ng, UINT8 type)
{
    UINT32 subCar_cnt;
    UINT32 i = 0, j = 0, k = 0;
    UINT32 reportSizePerSS = 2*Nb*Nr*Nc;
    UINT32 bit = 0;
    UINT32 bitMask = (1 << Nb) - 1;
    UINT32 tmpData = 0; 
    memcpy((UINT8 *)tmpData, pData, sizeof(UINT32));
    pData+=TXBF_BYTEINC;
    printk("Non compressed report size per stream = %d number of tones = %d\n",  (int) reportSizePerSS, (int) Ng);
    for (subCar_cnt=0; subCar_cnt < Ng ; subCar_cnt++)
    {
        printk("tone %d \n", (int) subCar_cnt);
        if (type == ACTION_MIMO_CSI_REPORT)
        {
            if ((bit + TXBF_CSIAMPBITSIZE) >= DWORDBITSIZE)
            {
                memcpy((UINT8 *)tmpData, pData, sizeof(UINT32));
                bit = bit % TXBF_BITSIZE;
                pData+=TXBF_BYTEINC;
            }
            txBfCsiAmp[subCar_cnt] =  tmpData & TXBF_CSIAMPBITMASK;
            printk("CSI tone Amplitude = %d \n", txBfCsiAmp[subCar_cnt]);
            tmpData >>= TXBF_CSIAMPBITSIZE;
            bit+=TXBF_CSIAMPBITSIZE;
        }
        for (i = 0; i < Nr; i++)
        {
            for (j = 0; j < Nc; j++)
            {
                while(k <= 1)
                {                    
                    if ((bit + Nb) < DWORDBITSIZE)
                    {
                        txBfMatrix[subCar_cnt][i][j][k++] = tmpData & bitMask;
                        tmpData >>=Nb;
                        bit+=Nb;
                    }
                    else
                    {
                        memcpy((UINT8 *)tmpData, pData, sizeof(UINT32));
                        bit = bit % TXBF_BITSIZE;
                        pData+=TXBF_BYTEINC;
                    }                    
                }
                k = 0;
                printk("(%d,%d) I=0x%2x J=0x%2x ", (int) i, (int) j, txBfMatrix[subCar_cnt][i][j][0], txBfMatrix[subCar_cnt][i][j][1]);
            }
            printk("\n");
        }
        printk("\n");
    }
}



void dispCompressedCode(UINT8 *pCodeData, UINT8 code, UINT8 numAngles, UINT8 Ng)
{
    UINT32 val = 0;
    UINT32 i = 0;
    UINT32 byteCount = 0;
    UINT8 subCar_cnt = 0;
    UINT8 *pByte = pCodeData;
    switch(code)
    {
        case 0:  /* psi = 1 bit, phi 3 bits */
            {
                IEEEtypes_CompBeamReportCode0_t *pCode0;
                pCode0 = (IEEEtypes_CompBeamReportCode0_t *) pCodeData;
                for (subCar_cnt=0; subCar_cnt < Ng ; subCar_cnt++)
                {
                    printk("Angles for tone index %d \n", subCar_cnt);
                    for (i = 0; i < numAngles; i++)
                    {                     
                        printk(" %x %x", pCode0->psi, pCode0->phi);
                        if (i%2)
                            pCode0++;
                        else
                            *((UINT8 *)pCode0) = ((UINT8)*((UINT8 *)pCode0)) >> 4;

                    }
                }
            }
            break;
        case 1:  /* psi = 2 bit, phi 4 bits */
            {
                IEEEtypes_CompBeamReportCode1_t *pCode1;

                val = (UINT32 ) *pByte;
                pCode1 = (IEEEtypes_CompBeamReportCode1_t *) &val;
                byteCount = 0;
                for (subCar_cnt=0; subCar_cnt < Ng ; subCar_cnt++)
                {
                    printk("Angles for tone index %d \n", subCar_cnt);
                    for (i = 0; i < numAngles; i++)
                    {   
                        if (byteCount > 2)
                        {
                            byteCount = 0;
                            pByte+=3;
                            memcpy((UINT8 *) &val, pByte, sizeof(UINT32));
                        }
                        printk(" %x %x", pCode1->psi, pCode1->phi);
                        val = val >> 6;
                        byteCount++;
                    }
                }
            }
            break;
        case 2:  /* psi = 3 bits, phi 5 bits */
            {
                IEEEtypes_CompBeamReportCode2_t *pCode2 = (IEEEtypes_CompBeamReportCode2_t *) pCodeData;
                for (subCar_cnt=0; subCar_cnt < Ng ; subCar_cnt++)
                {
                    printk("Angles for tone index %d \n", subCar_cnt);
                    for (i = 0; i < numAngles; i++)
                    {                     
                        printk(" %x %x", pCode2->psi, pCode2->phi);            
                        pCode2++;
                    }
                }
            }
            break;
        case 3:  /* psi = 4 bits, phi 6 bits */
            {
                IEEEtypes_CompBeamReportCode3_t *pCode3;
                memcpy((UINT8 *)&val, pByte, sizeof(UINT32));
                pCode3 = (IEEEtypes_CompBeamReportCode3_t *) &val;
                byteCount = 0;
                for (subCar_cnt=0; subCar_cnt < Ng ; subCar_cnt++)
                {
                    printk("Angles for tone index %d \n", subCar_cnt);
                    while (i < numAngles)
                    {
                        printk(" %x %x", pCode3->psi, pCode3->phi);
                        val= val >> 10;
                        i++;
                        byteCount++;
                        if ((byteCount > 2) && (i < numAngles))
                        {
                            byteCount = 0;
                            pByte+=4;
                            val = ((val & 0x3) | (((UINT32)(*pByte) >> 2))) & 0x3FF;
                            printk(" %x %x", pCode3->psi, pCode3->phi);
                            memcpy((UINT8 *)&val, pByte+1, sizeof(UINT32));
                            i++;
                        }                    
                    }
                    i=0;
                }
            }
            break;
        default:
            printk("Invalid Code = %d \n", code);
            break;
    }

}

#endif
