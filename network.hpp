#ifndef __SPAZZ_NETWORK__
#define __SPAZZ_NETWORK__
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define SPAZZ_SOCKET_ERROR -1
#define SPAZZ_DNS_ERROR -2
#define SPAZZ_CONNECT_ERROR -3

namespace network {

int connect(std::string host,short portno);
int serveJpeg(int socket, const std::string file);

}

#endif
