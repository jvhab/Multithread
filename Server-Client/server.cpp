#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
int main()
{
    // Дескриптор сокета
    int Socket_server = socket(AF_INET, SOCK_STREAM, 0);

    // Связка сокета с адресом
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(Socket_server, (struct sockaddr*)& addr, sizeof(addr));

    // Ожидание запросов
    listen(Socket_server, SOMAXCONN);
    

    while(true)
    {
        int SlaveSocket = accept(Socket_server, 0, 0);
        char buffer[5] = {0, 0, 0, 0, 0};
        recv(SlaveSocket, buffer, 5, MSG_NOSIGNAL);
        shutdown(SlaveSocket, SHUT_RDWR);
        //close(SlaveSocket);
        std::cout << buffer << std::endl;
        break;
         
    }


    return 0;
}
