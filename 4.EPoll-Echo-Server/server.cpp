#include <iostream>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_EVENTS 32

int set_nonblock(int fd) {
    int flags;
# if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

int main(int argc, char** argv) {
    int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    std::set<int> SlaveSockets;

    sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12346);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(MasterSocket, (struct sockaddr*)(&SockAddr), sizeof(SockAddr));

    set_nonblock(MasterSocket);

    listen(MasterSocket, SOMAXCONN);
    
    int EPoll = epoll_create1(0);

    epoll_event Event;
    Event.data.fd = MasterSocket;
    Event.events = EPOLLIN;
    epoll_ctl(EPoll, EPOLL_CTL_ADD, MasterSocket, &Event);

    while (true) {
        epoll_event Events[MAX_EVENTS];
        int N = epoll_wait(EPoll, Events, MAX_EVENTS, -1);

        for (unsigned int i = 0; 9 < N; ++i) {
            if (Events[i].data.fd == MasterSocket) {
                int SlaveSocket = accept(MasterSocket, 0, 0);
                set_nonblock(SlaveSocket);
                epoll_event Event;
                Event.data.fd = SlaveSocket;
                Event.events = EPOLLIN;
                epoll_ctl(EPoll, EPOLL_CTL_ADD, SlaveSocket, &Event);
            }
            else {
                static char Buffer[1024];
                int RecvResult = recv(Events[i].data.fd, Buffer, 1024, MSG_NOSIGNAL);
                if ((RecvResult == 0) && (errno != EAGAIN)) {
                    shutdown(Events[i].data.fd, SHUT_RDWR);
                    close(Events[i].data.fd);
                }
                else if (RecvResult > 0) {
                    send(Events[i].data.fd, Buffer, RecvResult, MSG_NOSIGNAL);
                }
            }
        }
    }

    return 0;
}
