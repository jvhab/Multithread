#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int Socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12345);
    SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(Socket, (struct sockaddr*)& SockAddr, sizeof(SockAddr));

    char Buffer[] = "PING111222";
    

    send(Socket, Buffer, 10, MSG_NOSIGNAL);
    recv(Socket, Buffer, 10, MSG_NOSIGNAL);

    shutdown(Socket, SHUT_RDWR);


    std::cout << Buffer << std::endl;

    return 0;
}
