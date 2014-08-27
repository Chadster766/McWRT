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


#ifndef _WPA_H_
#define _WPA_H_
typedef enum
{
    KEYMGMT_ERROR,
    KEYMGMTTIMEOUTEVENT, //key management timeout
    GRPKEYTIMEOUTEVENT
}keymgmt_timout_msg_type_e;
typedef UINT8 keymgmt_timout_msg_type_t;

//Rahul
typedef struct
{
    //timer_Data_t timerData;
    //uint32   Id;
    IEEEtypes_MacAddr_t StnAddr;
    keymgmt_timout_msg_type_t type;
}
dist_PendingData_t;

/*----------------*/
/* Timer Messages */
/*----------------*/

typedef struct
{
    dist_PendingData_t PendingData_p;
    UINT8 Id;
}
distQ_TimerMsg_t;

typedef enum{
    WPAEVT_STA_AUTHENTICATED,
    WPAEVT_STA_ASSOCIATED,
    WPAEVT_STA_DEAUTHENTICATED,
    WPAEVT_STA_DISASSOCIATED,
    WPAEVT_STA_AUTHENTICATE_FAIL,
}WPA_ASSOC_TYPE;
typedef UINT8 WPA_ASSOC_TYPE_t;

typedef struct _StaAssocStateMsg
{
    unsigned char staMACAddr[6];
    WPA_ASSOC_TYPE_t assocType;
}
StaAssocStateMsg_t;

typedef enum
{
    STA_ASSOMSGRECVD,
    TIMERMSGRECVD,
    KEYMGMTINITMSGRECVD
}MsgType_e;
typedef UINT8 MsgType_t;

typedef struct
{
    MsgType_t MsgType;
    union
    {
        StaAssocStateMsg_t StaAssocStateMsg;
        distQ_TimerMsg_t distQ_TimerMsg;
    }msg;
}
DistTaskMsg_t;

#endif
