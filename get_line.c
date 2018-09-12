#include <stdio.h>
#include "get_line.h"

int get_line(int sock,char line[],int size)
{
    int c = 'a';
    int i = 0;
    ssize_t s = 0;
    while(i<size-1 && c != '\n')
    {
        s = recv(sock,&c,1,0);
        if(s > 0)
        {
            if(c == '\r')
            {
                read(sock,&c,1,MSG_PEEX);
                if(c != '\n')
                {
                    c = '\n';
                }
                else
                {
                    recv(sock,&c,1,0);
                }
            }
            line[i++] = c;
        }
        else
        {
            break;
        }
    }
    line[i] = '\0';
    return i;
}
