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

/*******************************************************************************************
*
* File: mlmeApi.h
*        AP MLME Events Module
* Description:  Handle all the events coming in and out of the MLME State Machines
*******************************************************************************************/
#ifndef HDR_STATION
#define HDR_STATION

#include "wltypes.h"
//#include "HSM.h"


#include "macMgmtMain.h"

#define PACK   __attribute__ ((packed))

#define uint8   UINT8
#define uint16  UINT16
#define uint32  UINT32
#define uint64  UINT64
//#define Boolean BOOLEAN
#define int8    SINT8


///////////////////////////////////////////////////////////////
//For Station.h

#if 1
//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================

// rate options
#define STA_BONLY_MODE 0
#define STA_GONLY_MODE 1
#define STA_MIXED_MODE 2


//=============================================================================
//                          PUBLIC TYPE DEFINITIONS
//=============================================================================
typedef struct sta_MacCtrl_t
{
   uint16  TxOn:1;
   uint16  RxOn:1;
   uint16  LoopBackOn:1;
   uint16  WepEn:1;
   uint16  IntrEn:1;
   uint16  MultiCastEn:1;
   uint16  BroadCastEn:1;
   uint16  PromiscuousEn:1;
   uint16  AllMultiCastEn:1;
   uint16  No11gProtection:1;
   uint16  Reserved:5;
   uint16  WepType:1;
} PACK sta_MacCtrl_t;
   //
   // The general control field for Mac
   //

typedef struct sta_RFCtrl_t
{
   uint16  rfOn:1;         // 1=on, 0=off
   uint16  shortPream:1;   // 1=short preamble, 0=long pream
   uint16  AutoPream:1;     // 0=use fix preamble settin, 1=auto
   uint16  antenaSel:1;
   uint16  antenaDiver:1;  // 1=diversify
   uint16  Reserved:11;
} PACK sta_RFCtrl_t;

typedef struct sta_CBProcCtrl_t
{
   uint16  ScanCmdRcvd:1;
   uint16  AutoAssoc:1;
   uint16  ResetInProgress:1;
   uint16  AllMCPatchOn:1;
   uint16  Reserved:12;

} PACK sta_CBProcCtrl_t;

typedef struct sta_CondorCtrl_t
{
   uint16  dma0_Busy:2;
   uint16  dma1_Busy:1;
   uint16  TxWeb_Busy:1;
   uint16  staFreeze:1;
   uint16  PSMode:1;
   uint16  FixRate:1;
   uint16  Protection4g:1;
   uint16  GreenField:1;
   uint16  GreenFieldSet:1;
   uint16  Reserved:6;
} PACK sta_CondorCtrl_t;

typedef struct sta_NetworkCtrl_t
{
   uint16  RateOptions:3;
   uint16  isB_Network:1;
   uint16  Reserved:12;
} PACK sta_NetworkCtrl_t;

typedef struct sta_RfTxPwr_t
{
uint16 SupportLevel;      /* Total levels (1 to 8) */
uint16 CurrentLevel;      /* Current level (1 to 8) */
uint16 PowerLevelList[8]; /* level 1 = 4dBm,... level 8 = 24dBm */
} PACK sta_RfTxPwr_t;

typedef struct SpiCsPaOpt_t
{
   uint16  IntPaCalTabOpt:1;  /* 0 = Delta Method, 1 = Absolute Method */
   uint16  ant2NotCalSep:1;
   uint16  ExtPaNegPol:1;     /* Ext PA polarization. 0 = Positive, 1 = Negative */
   uint16  ExtPaUsed:2;
   uint16  Reserved:3;
   uint16  Revision:8;
} PACK SpiCsPaOpt_t;

typedef struct SpiCsExtPa_t
{
   uint8  rfReg26:3;         /* RF register x26 bit[6:4] */
   uint8  reserved:1;
   uint8  ExtPaVendor:4;
} PACK SpiCsExtPa_t;

typedef struct SpiCsAnt_t
{
   uint8   Tx:2;        /* AntDivModeTx[7:6](BBP:0x38),  00 = ant0; 01 = ant1; 1x = diversity */
   uint8   Rx:2;        /* AntDivModeRx[7:6](BBP:0x37),  00 = ant0; 01 = ant1; 1x = diversity */
   uint8   Reserved:4;
} PACK SpiCsAnt_t;

typedef struct Delta_t_station
{
   uint16 Reg21_22:4;
   uint16 Reg20:4;
   uint16 Ant2:4;
   uint16 Reserved:4;
} PACK Delta_t_station;

typedef struct Absolute_t_station
{
   uint16 Reg22:3;
   uint16 Reg21:7;
   uint16 Reg20:6;
} PACK Absolute_t_station;

typedef union SpiCsPa_t
{
   Delta_t_station    Delta;
   Absolute_t_station Absolute;
} PACK SpiCsPa_t;

typedef struct SpiCardSpec_t
{
   SpiCsPaOpt_t   SpiCsPaOpt;
   SpiCsExtPa_t   SpiCsExPa;
   SpiCsAnt_t     SpiCsAnt;
   SpiCsPa_t      SpiCsPa[14];
   uint16         SpiCsCountry;
   uint16         SpiCsCustmrOpt;
} PACK SpiCardSpec_t;

typedef struct Station_t
{
   sta_MacCtrl_t        MacCtrl;
   sta_RFCtrl_t         RFCtrl;
   sta_CBProcCtrl_t     CBProcCtrl;
   sta_CondorCtrl_t     ChipCtrl;
   sta_NetworkCtrl_t    NetworkCtrl;
   uint8                KeyId;
   uint8                rfch;
   uint32               IV;
   uint8                ChipRev;
   //uint8                FirmwareRev;
   uint8                DataRate;
   uint8                RSSI;
   uint8                HostIf;
   uint8                Reserved2;
   uint8                Reserved3;
   sta_RfTxPwr_t        RfTx;
   SpiCardSpec_t        *SpiCardSpecPtr;
} PACK Station_t;

////////////////////////////////////////////
///////////////////////////
/*-------------------------------------------*/
/* PHY Direct Sequence Spread Spectrum Table */
/*-------------------------------------------*/
//typedef enum
//{
//   RSSI_ONLY              = 1,
//   SIG1_AND_RSSI_A        = 2,
//   RSSI_AND_POS_SIG1      = 3,
//   SIG1_AND_SIG2_AND_RSSI = 4,
//   SIG1_AND_RSSI_B        = 5
//} snmp_CcaMode_e;
#endif

#endif /* HDR_STATION */
