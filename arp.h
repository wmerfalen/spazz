#ifndef __SPAZZ_ARP_HEADER__
#define __SPAZZ_ARP_HEADER__
#if (HAVE_CONFIG_H)
#if ((_WIN32) && !(__CYGWIN__)) 
#include "../include/win32/config.h"
#else
#include "../include/config.h"
#endif
#endif
#include "libnet.h"
#include "util.h"

int build_broadcast_arp(
    libnet_t * arp_handle,
    uint8_t* target_ip,       /* IP Address */
    uint8_t* is_at,           /* HW Address */
    uint8_t* tell_hw,         /* HW Address */
    uint8_t* tell_ip          /* IP Address */
);

int start_arp_spoof(char* device,char* target_ip,char* gateway_ip);
void cleanup_arp_spoof();

#endif
