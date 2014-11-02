/*
 * Copyright (c) 2000-2005 by Marvell International Ltd.
*
* If you received this File from Marvell, you may opt to use, redistribute 
* and/or modify this File in accordance with the terms and conditions of the 
* General Public License Version 2, June 1991 (the “GPL License?, a copy of 
* which is available along with the File in the license.txt file or by writing 
* to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
* MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 
*
* THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE 
* EXPRESSLY DISCLAIMED. The GPL License provides additional details about 
* this warranty disclaimer.
*
*/
#ifndef WL_KERNEL_26
#include <linux/config.h>
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/random.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,5,44))
#include <linux/tqueue.h>
#else
#include <linux/workqueue.h>
#endif
#include <linux/kmod.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27))
#include <asm/arch/irqs.h>
#endif

#include "ewb_packet.h"
#include "ewb_hash.h"
#include "wltypes.h"
#include "ewb_hash.h"

#include "linux/udp.h"
#include <linux/skbuff.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#define SKB_IPHDR(skb) ((struct iphdr*)skb->network_header)
#define SKB_NHDR(skb) skb->network_header
#define SKB_MACHDR(skb) skb->mac_header
#else
#define SKB_IPHDR(skb) skb->nh.iph
#define SKB_NHDR(skb) skb->nh.raw
#define SKB_MACHDR(skb) skb->mac.raw
#endif

/* Global Variables */
/* Import Variables */
#define BOOTP_REQUEST   1
#define BOOTP_REPLY     2

#define DHCPDISCOVER    1
#define DHCPOFFER       2
#define DHCPREQUEST     3
#define DHCPDECLINE     4
#define DHCPACK         5
#define DHCPNAK         6
#define DHCPRELEASE     7
#define DHCPINFORM      8
static const UINT8 ic_bootp_cookie[4] = { 99, 130, 83, 99 };

struct bootp_pkt {              /* BOOTP packet format */
         struct iphdr iph;       /* IP header */
         struct udphdr udph;     /* UDP header */
         UINT8 op;                  /* 1=request, 2=reply */
         UINT8 htype;               /* HW address type */
         UINT8 hlen;                /* HW address length */
         UINT8 hops;                /* Used only by gateways */
         UINT32 xid;             /* Transaction ID */
         UINT16 secs;            /* Seconds since we started */
         UINT16 flags;           /* Just what it says */
         UINT32 client_ip;               /* Client's IP address if known */
         UINT32 your_ip;         /* Assigned IP address */
         UINT32 server_ip;               /* (Next, e.g. NFS) Server's IP address */
         UINT32 relay_ip;                /* IP address of BOOTP relay */
         UINT8 hw_addr[16];         /* Client's HW address */
         UINT8 serv_name[64];       /* Server host name */
         UINT8 boot_file[128];      /* Name of boot file */
         UINT8 exten[312];          /* DHCP options / BOOTP vendor extensions */
 };
 


void printMAC(unsigned char* mac)
{
    printk("%x %x %x %x %x %x ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);	
}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbIpToDs(unsigned char *packet, unsigned char *rfAddr)
{
	eth_hdr_t *eth;
	ewb_ip_hdr  *ip;
	UINT32 srcAddr;

	if (packet == NULL)
	{
		return (-1);
	}
	eth = (eth_hdr_t *)packet;
	ip = (ewb_ip_hdr *)(packet + ETH_HDR_LEN);
	
	memcpy(&srcAddr, ip->srcAddr, 4);
    wetUpdateHashEntry(srcAddr, eth->src);
    
    /* The original IP may have roamed! */
	memcpy(eth->src, rfAddr, HW_ADDR_LEN);
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbIpFromDs(unsigned char *packet)
{
	eth_hdr_t *eth;
	ewb_ip_hdr  *ip;
	unsigned char *clntMac;
	UINT32 destAddr, srcAddr;
	
	eth = (eth_hdr_t *)packet;
	ip = (ewb_ip_hdr *)(packet + ETH_HDR_LEN);
	
	memcpy(&srcAddr, ip->srcAddr, 4);
   	wetClearHashEntry(srcAddr); /* The original IP may have roamed! */

	memcpy(&destAddr, ip->destAddr, 4);    
	if ((clntMac = wetGetHashEntryValue(destAddr)) == NULL)
    {
        return (-1);
    }
	memcpy(eth->dest, clntMac, HW_ADDR_LEN);
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbArpToDs(unsigned char *packet, unsigned char *rfAddr)
{
	eth_hdr_t *eth;
	arp_eth_ip_addr *arpAddr;
	UINT32 sndIpAddr;	

	if(packet == NULL)
	{
		return -1;
	}
	eth = (eth_hdr_t *)packet;
	arpAddr = (arp_eth_ip_addr *)(packet + ETH_HDR_LEN + ARP_HDR_LEN);
	memcpy(&sndIpAddr, arpAddr->sndIpAddr, 4);
    
    //printk("\nSend IP %x mac:: ",sndIpAddr);	
    //printMAC(eth->src);
    
    wetUpdateHashEntry(sndIpAddr, eth->src);
    
    /* The original IP may have roamed! */
	memcpy(eth->src, rfAddr, HW_ADDR_LEN);
	memcpy(arpAddr->sndHwAddr, rfAddr, HW_ADDR_LEN);
	return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
int ewbArpFromDs(unsigned char *packet)
{
	eth_hdr_t *eth;
	arp_eth_ip_addr *arpAddr;
	unsigned char *clntMac;
	UINT32 sndIpAddr, trgtIpAddr;	
	
	eth = (eth_hdr_t *)packet;
	arpAddr = (arp_eth_ip_addr *)(packet + ETH_HDR_LEN + ARP_HDR_LEN);
	memcpy(&sndIpAddr, arpAddr->sndIpAddr, 4);
    
	wetClearHashEntry(sndIpAddr); /* The original IP may have roamed! */
    
	memcpy(&trgtIpAddr, arpAddr->trgtIpAddr, 4);
    
    //printk("ewbArpFromDs: %x %x ",sndIpAddr,trgtIpAddr);

    if ((clntMac = wetGetHashEntryValue(trgtIpAddr)) == NULL)
    {
        printk("\nCould not find entry ");
        return -1;
    }
	memcpy(eth->dest, clntMac, HW_ADDR_LEN);
	memcpy(arpAddr->trgtHwAddr, clntMac, HW_ADDR_LEN);
	return 0;
}

int ewbDhcpFromDs(unsigned char *packet, struct udphdr *udphp)
{
  eth_hdr_t *eth =  (eth_hdr_t *)packet;
  char *clntMac;

  clntMac = (char *)udphp + 28;     /* client MAC address */

  //L2: Modify dest mac in MAC header
  memcpy(eth->dest, clntMac, HW_ADDR_LEN);
 
  return 0;

}


/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern int ewbWlanRecv(struct sk_buff *skb,unsigned char *rfAddr)
{
	struct eth_hdr_t *eth;
    unsigned short etherType;
    struct iphdr *iphp;
    struct udphdr * udphp;

	eth = (eth_hdr_t *)skb->data;

	if(IS_BROADCAST_HWADDR(eth->dest))
        return 0;

    /* MULTI_CAST_SUPPORT*/
	/* Check to if it's MultiCast */
	if(eth->dest[0] == 0x01)
        return 0;

    etherType = ntohs(eth->type);
    //printk("\nP WL R %x %x  ",etherType,skb->protocol);

	switch(etherType)
	{
	    case ETH_IP_TYPE:
            iphp = (struct iphdr *) (eth + 1);
            udphp = (struct udphdr *) (iphp+1);
            if( !(memcmp(eth->dest, rfAddr, HW_ADDR_LEN))
                && iphp->protocol == IPPROTO_UDP && 
                udphp->source == htons(67) && (udphp->dest == htons(67) || udphp->dest == htons(68))) 
            {
                if(ewbDhcpFromDs(skb->data, udphp) < 0) 
                    return -1;
            }
            else
            {
                if(ewbIpFromDs(skb->data) < 0)
    		        return -1;
            }
		    break;

	    case ETH_ARP_TYPE:
    		if(ewbArpFromDs(skb->data) < 0)
    		    return -1;    		
		    break;

	    default:
		    return 0;
	}  

    return 0;
}

/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern int ewbLanRecv(struct sk_buff *skb,unsigned char *rfAddr)
{
	eth_hdr_t *eth;
	unsigned short etherType;
    struct iphdr *iphp;
    struct udphdr * udphp;
	struct bootp_pkt *bootp;
        
	eth = (eth_hdr_t *)skb->data;
    etherType = ntohs(eth->type);
    
	switch (etherType)
	{
        case ETH_IP_TYPE:
            iphp = (struct iphdr *) (eth + 1);
            udphp = (struct udphdr *) (iphp+1);
			bootp = (struct bootp_pkt *)(eth + 1);
            if( iphp->protocol == IPPROTO_UDP && bootp->op == BOOTP_REQUEST
			&& (bootp->exten[0] ==ic_bootp_cookie[0])
			&& (bootp->exten[1] ==ic_bootp_cookie[1])
			&& (bootp->exten[2] ==ic_bootp_cookie[2])
			&& (bootp->exten[3] ==ic_bootp_cookie[3])
			) 
			{
				if(bootp->exten[6] == DHCPDISCOVER || bootp->exten[6] == DHCPREQUEST ) {
					bootp->flags = 0x0080;
					skb_pull(skb, 14);
					udphp->check=0;
					skb->csum = skb_checksum(skb, iphp->ihl*4,
							skb->len - iphp->ihl*4, 0);
					udphp->check  = csum_tcpudp_magic(iphp->saddr, iphp->daddr,
							skb->len - iphp->ihl*4,
							IPPROTO_UDP,
							skb->csum);
					skb_push(skb, 14);
				}
			}
    		if (ewbIpToDs(skb->data, rfAddr) < 0)
    		{
    			goto dropPacket;
    		}
		    break;

	    case ETH_ARP_TYPE:
    		if (ewbArpToDs(skb->data, rfAddr) < 0)
    		{
    			goto dropPacket;
    		}
    		break;

	    case EAPOL_TYPE:
            {
                goto sendToWLAN;
	        }
        default:
            /* MULTI_CAST_SUPPORT*/
            /* Check if MultiCast */
            if(eth->dest[0] == 0x01)
            {
                memcpy(eth->src, rfAddr, HW_ADDR_LEN);
                goto sendToWLAN;
            }
    
		    goto dropPacket;    
	}

sendToWLAN:	
    return 0;

dropPacket:
    //panic("Fatal Error! Fix me");
    return (-1);
}

    
/*************************************************************************
* Function:
*
* Description:
*
* Input:
*
* Output:
*
**************************************************************************/
extern int ewbInit(void)
{   
    wetHashInit();
    return 0;
}
