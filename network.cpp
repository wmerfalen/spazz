#include "network.hpp"

namespace network {

int connect(std::string host,short portno){
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        return SPAZZ_SOCKET_ERROR; 
    }
    server = gethostbyname(host.c_str());
    if (server == NULL) {
        return SPAZZ_DNS_ERROR;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        return SPAZZ_CONNECT_ERROR;
    }
    return sockfd;
}

int serveJpeg(int socket,const std::string fileName){
    std::string headers = "HTTP/1.1 200 OK\r\n";
    std::string buf;
    std::string strSize;
    int ret = 0;
    int size = 0;
    std::cout << "Building headers\n";
    headers += "Content-Type: image/jpeg\r\n";
    //headers += "Accept-Range: bytes\r\n";

    std::cout << "Headers sent: " << ret << "\n";
    std::fstream fs{fileName.c_str(),std::ios::in | std::ios::binary};
    std::cout << "Attempting to open jpeg\n";
    if( !fs.is_open() ){
        std::cout << "Failed opening jpeg file\n";
        return -2;
    }
    //Get file size
    fs.seekg(0,std::ifstream::end);
    size = fs.tellg();
    fs.seekg(-size,std::ifstream::end);
    headers += "Content-Length: ";
    headers += std::to_string(size);
    headers += "\r\n";
    headers += "Connection: close\r\n";
    headers += "\r\n";

    std::cout << "Sending headers\n";
    ret = send(socket,headers.c_str(),headers.length(),0);
    if( ret == -1 ){
        std::cerr << "Headers: send error\n";
        return -1;
    }
    std::cout << headers.c_str();

    std::cout << "Jpeg opened\n";
    buf.resize(size);
    //while(
    fs.read((char*)&buf[0],size);
        std::cout << "Sending jpeg contents...\n";
        std::cout << "GCOUNT: " << fs.gcount() << "\n";
        ret = send(socket,(char*)&buf[0],size,0);
        std::cout << "Send: " << ret << "\n";
        if( ret == -1 ){
            std::cerr << "Send error!\n";
            fs.close();
            return -3;
        }
    //}
    fs.close();
    return 0;
}

}
