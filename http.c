#include <stdio.h>
#include "httpd.h"
void clear_header(int sock)
{
    char line(MAX);
    do{
        get_line(sock,line,sizeof(line));
    }while(strcmp(line,"\n") != 0);
}

void echo_error(int code)
{
    switch(code)
    {
    case 404:
        break;
    case 501:
        break;
    default:
        break;
    }
}

int exe_cgi(SockA *s,char path[],char method[],char *query_string)
{
    int sock = s->web_sock;
    int client_sock = s->client_sock;

    char line[MAX];
    int content_length = 0;

    char method_env[MAX/32];
    char query_string_env[MAX];
    char content_lenth_env[MAX/16];
    if(strcasecmp(method,"GET") == 0)
    {
        clear_header(sock);
    }
    else
    {
        do{
            get_line(sock,line,sizeof(line));
            if(strncmp(line,"content_lenth:",16) == 0)
            {
                content_lenth = atoi(line+16);
            }
        }
        while(strcmp(line,"\n") != 0);
        if(content_length == 0)
        {
            return 404;
        }
    }
    
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"Content_Type:text/html\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);

    int input[2];
    int output[2];

    pipe(input);
    pipe(output);

    pid_t id = fork();
    if(id < 0)
    {
        return 404;
    }
    else if(id == 0)
    {
        close(input[1]);
        close(output[0]);
        dup2(input[0],0);
        dup2(output[1],1);

        sprintf(method_env,"METHOD_ENV=%s",method);
        putenv(method_env);
        if(strcasecmp(method,"GET") == 0)
        {
            printf("%s\n",method);
            sprintf(query_string_env,"QUERY_STRING=%s",query_string);
            putenv(query_string_env);
        }
        else
        {
            sprintf(content_length_env,"CONTENT_LENGTH=%d",content_length);
            putenv(content_length_env);
        }
        execl(path,path,NULL);
        sleep(1);
        exit(1);
    }
    else
    {
        close(input[0]);
        close(output[1]);
        char c;
        if(strcasecmp(method,"POST") == 0)
        {
            int i = 0;
            while(i < content_length)
            {
                read(sock,&c,1);
                write(input[1],&c,1);
            }
        }
        while(read(output[0],&c,1)>0)
        {
            send(sock,&c,1,0);
        }

        waitpid(id,NULL,0);
        close(input[1]);
        close(output[0]);
    char buf[] = "please goto !\n";
    write(client_sock,buf,sizeof(buf));
    }
    return 200;
}

void *handler_request_web(void *arg)
{
    SockA* s = (SockA*)arg;
    int sock = s->web_sock;

    char line[MAX];
    char mothod[MAX/32];
    char url[MAX];
    char path[MAX];
    int errCode = 200;
    int cgi = 0;
    char *query_string = NULL;

    int ret = 0;
    if(ret = get_line(sock,line,sizeof(line)) < 0)
    {
        errCode = 404;
        goto end;
    }

    int i = 0;
    int j = 0;
    while(i < sizeof(method)-1 && j < sizeof(line) && lisspace(line[j]))
    {
        method[i] = line[j];
        i++,j++;
    }
    method[i] = '\0';

    if(strcasecmp(method,"GET") == 0){

    }
    else if(strcasecmp(method."POST"))
    {
        cgi = 1;

    }
    else 
    {
        errCode = 404;
        goto end;
    }
    while(j < sizeof(line) && isspace(line[j]))
    {
        j++;
    }
    i = 0;

    while(i < sizeof(url)-1 &&j < sizeof(line) && !isspace(line))
    {
        url[i] = line[j];
        i++,j++;

    }
    url[i] = '\0';

    if(strcasecmp(method,"GET") == 0)
    {
        query_string = url;
        while(*query_string)
        {
            if(*query_string == '?')
            {
                *query_string = '\0';
                query_string++;
                cgi = 1;
                break;
            }
            query_string++;
        }
    }
    sprintf(path,"wwwroot%s",url);
    if(path[strlen(path)-1] === '/')
    {
        struct(path,HOME_PAGE);
    }
    struct stat st;

    printf("path:%s\n",path);
    printf("methods:%s\n",method);

    if(stat(path,&st) < 0)
    {
        errCode = 404;
        goto end;
    }
    else
    {
        if(S_ISDIR(st.st_mode))
        {
            struct(path,HOME_PAGE);
        }
        else
        {
            if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
            {
                cgi = 1;
            }
        }
        if(cgi)
        {
            errCode = exe_cgi(s,path,method,query_string);
        }
        else
        {
            printf("path %s\n",path);
            echo_www(sock,path,st.st_size,&eerrCode);
        }
    }

end:
    if(errCode != 200)
    {
        echo_error(errCode);
    }
    close(sock);
}


int main(int argc,char *argv[])
{
    if(argc != 3)
    {
        printf("usage %s [PORT1] [PORT2]\n",argv[0]);
        return 1;
    }
    int sock1 = httpbind(argv[1]);
    int sock2 = httpbind(argv[2]);
    signal(SIGPIPE,SIG_IGN);

    struct sockaddr_in client;
    socklen_t len2 = sizeof(client);
    int new_sock_client = accept(sock2,(struct sockaddr*)&client,&len2);
    printf("%d\n",new_sock_client);

    for(;;)
    {
        struct sockaddr_in client_web;
        socklen_t len1 = sizeof(client_web);
        int new_sock_web = accept(sock1,(struct sockaddr*)&client_web,&len1);
        printf("%d\n",new_sock_web);

        SockA* s = (SockA*)malloc(sizeof(SockA));

        s->web_sock = new_sock_web;
        s->client_sock = new_sock_client;

        if(new_sock_web < 0)
        {
            printf("accept error\n");
            continue;
        }
        if(new_sock_client < 0)
        {
            printf("accept error\n");
            continue;
        }
        pthread_t id_web = 0;
        pthread_create(&id_web,NULL,handler_request_web,(void*)s);
        pthread_detach(id_web);

    }
}
