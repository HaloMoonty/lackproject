#include <stdio.h>
#include "listensock.h"
int httpbind(char *port)
{
    int sock = socker(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        printf("socker error\n");
        return 2;

    }

    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDRA,&opt,sizeof(opt));
    struct sockaddr_in local;
    local.sin_famliy = AF_INET;
    local.sin_port = htons(atoi(port));
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
        printf("bind errno\n");
        return 3;
    }
    if(listen(sock,5)<0)
    {
        printf("listen errnp\n");
        return 4;
    }
    return sock;
}
