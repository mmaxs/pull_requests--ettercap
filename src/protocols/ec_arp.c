/*
    ettercap -- ARP decoder module

    Copyright (C) ALoR & NaGA

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Header: /home/drizzt/dev/sources/ettercap.cvs/ettercap_ng/src/protocols/ec_arp.c,v 1.4 2003/04/15 07:57:37 alor Exp $
*/

#include <ec.h>
#include <ec_decode.h>

/* globals */

struct arp_header {
   u_int16  ar_hrd;          /* Format of hardware address.  */
   u_int16  ar_pro;          /* Format of protocol address.  */
   u_int8   ar_hln;          /* Length of hardware address.  */
   u_int8   ar_pln;          /* Length of protocol address.  */
   u_int16  ar_op;           /* ARP opcode (command).  */
#define ARPOP_REQUEST   1    /* ARP request.  */
#define ARPOP_REPLY     2    /* ARP reply.  */
#define ARPOP_RREQUEST  3    /* RARP request.  */
#define ARPOP_RREPLY    4    /* RARP reply.  */
};

struct arp_eth_header {
   u_int8   arp_sha[ETH_ADDR_LEN];     /* sender hardware address */
   u_int8   arp_spa[IP_ADDR_LEN];      /* sender protocol address */
   u_int8   arp_tha[ETH_ADDR_LEN];     /* target hardware address */
   u_int8   arp_tpa[IP_ADDR_LEN];      /* target protocol address */
};

/* protos */

FUNC_DECODER(decode_arp);
void arp_init(void);

/*******************************************/

/*
 * this function is the initializer.
 * it adds the entry in the table of registered decoder
 */

void __init arp_init(void)
{
   add_decoder(NET_LAYER, LL_TYPE_ARP, decode_arp);
}


FUNC_DECODER(decode_arp)
{
   struct arp_header *arp;

   arp = (struct arp_header *)DECODE_DATA;

   /*
    * size of the whole packet is the size of the
    * header + size of the hard address + proto address
    * for the sender and the target
    */
   
   DECODED_LEN = sizeof(struct arp_header) +
                 2 * (arp->ar_hln + arp->ar_pln);

   /*
    * XXX - TODO
    *
    *    1) differentiate between request and reply
    *
    *    2) implement a NDIS decoder of IPv6
    */

   PACKET->L3.len = DECODED_LEN;
   PACKET->L3.header = (u_char *)DECODE_DATA;
   PACKET->L3.options = NULL;
   
   PACKET->L3.proto = htons(LL_TYPE_ARP);
   PACKET->L3.ttl = 1;
   
   if (arp->ar_hln == ETH_ADDR_LEN && arp->ar_pln == IP_ADDR_LEN) {
   
      struct arp_eth_header *earp;
      earp = (struct arp_eth_header *)(arp + 1);
      
      ip_addr_init(&PACKET->L3.src, AF_INET, (char *)&earp->arp_spa);
      ip_addr_init(&PACKET->L3.dst, AF_INET, (char *)&earp->arp_tpa);
           
      /* HOOK POINT:  PACKET_ARP */
      hook_point(PACKET_ARP, po);
   }
   
   return NULL;
}

/* EOF */

// vim:ts=3:expandtab

