#include <iostream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 8000
#define BUFSIZE 10000

int main(int argc, char *argv[])
{
    int             sock;
    short           port;
    bool            keep;
    sockaddr_in     info;
    std::string     sendBuf = "";
    std::string     readBuf = "";     
    int             byte;
    int             fd;
    std::ifstream   fin;
    char            buf[BUFSIZE];
    
    if (argc != 3)
    {
        std::cout << "useage : port add file path" << std::endl;
        return 1;
    }
    port = atoi(argv[1]);
    std::cout << "port : " << port << std::endl;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&info, sizeof(sockaddr_in));
    info.sin_family = PF_INET;
    info.sin_addr.s_addr = inet_addr(SERVERIP);
    info.sin_port = htons(port);
    if (connect(sock, (struct sockaddr *)&info, sizeof(sockaddr_in)) != 0)
    {
        std::cout << "Error connect" << std::endl;
        return 1;
    }

    fin.open(argv[2]);
    if (!fin.is_open())
        return 1;
    
    
    while (getline(fin, readBuf))
    {
        sendBuf.append(readBuf);
        sendBuf.append("\r\n");
    }
    sendBuf.append("\r\n");
    fin.close();

    std::cout << "=============================" << std::endl;
    std::cout << sendBuf;
    std::cout << "=============================" << std::endl;
    byte = write(sock, sendBuf.c_str(), sendBuf.length());
    sleep(1);
    bzero(buf, BUFSIZE);
    byte = read(sock, buf, BUFSIZE);
    std::cout << buf << std::endl;
    std::cout << "=============================" << std::endl;
    return (0);
}

