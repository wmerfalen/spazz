#ifndef __SPAZZ_UTIL_C__
#define __SPAZZ_UTIL_C__
#include "util.h"

int strtomac(char* mac,unsigned char* hw_addr){
    int maclen,a=0,b=0,i,val=0;
    maclen = strlen(mac);
    if( maclen != 17 ){
        return -1;
    }
    for(i = 0 ; i < 17; i++){
        if( *mac == ':' ){
            b = 0;
            mac++;
            continue;
        }
        if( (a=ishexalpha(*mac)) || isdigit(*mac) ){
            if( b ){
                if( a ){
                    val += a;
                }else{
                    val += chrtoi(*mac);
                }
                *hw_addr++ = val;
                b = 0;
                val = 0;
            }else{
                if( a ){
                    val += a * 16;
                }else{
                    val += chrtoi(*mac) * 16;
                }
                b++;
            }
        }else{
            return -2;
        }
        mac++;
    }
    return 0;
}

int ishexalpha(int a){
    if( a >= 'a' && a <= 'f' ){
        return a - 87 ;
    }
    if( a >= 'A' && a <= 'F' ){
        return a - 55;
    }
    return 0;
}
int chrtoi(char a){
    if( a >= '0' && a <= '9' ){
        return a - 48;
    }
}
#endif
