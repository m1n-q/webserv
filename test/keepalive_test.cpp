#include <iostream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <fcntl.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 8000
#define BUFSIZE 10000

int main()
{
    int             sock;
    bool            keep;
    sockaddr_in     info;
    std::string     sendBuf = "";
    std::string     readBuf = "";     
    int             byte;
    int             fd;
    char            buf[BUFSIZE];
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&info, sizeof(sockaddr_in));
    info.sin_family = PF_INET;
    info.sin_addr.s_addr = inet_addr(SERVERIP);
    info.sin_port = htons(SERVERPORT);
    if (connect(sock, (struct sockaddr *)&info, sizeof(sockaddr_in)) != 0)
    {
        std::cout << "Error connect" << std::endl;
        return 1;
    }

    sendBuf.append("GET / HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: localhost\r\n\r\n");    
    byte = write(sock, sendBuf.c_str(), sendBuf.length());
    // sleep(1);
    sendBuf.clear();
    sendBuf.append("GET / HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: localhost\r\n\r\n");    
    byte = write(sock, sendBuf.c_str(), sendBuf.length());
    // sleep(1);
    sendBuf.clear();
    sendBuf.append("GET / HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: localhost\r\n\r\n");    
    byte = write(sock, sendBuf.c_str(), sendBuf.length());
    
    std::cout << "=============================" << std::endl;
    std::cout << sendBuf;
    std::cout << "=============================" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << sendBuf;
    std::cout << "=============================" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << sendBuf;
    std::cout << "=============================" << std::endl;
    sleep(5);
    bzero(buf, BUFSIZE);
    byte = read(sock, buf, BUFSIZE);
    std::cout << buf << std::endl;
    std::cout << "=============================" << std::endl;
    return (0);
}

