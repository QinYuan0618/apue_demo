/*
    socket_server服务器端
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <getopt.h>

#define MSG_STR "Hello Qinyuan client!"


void print_usage(char *progname)
{
    printf("%s usage: \n", progname);
    printf("-p(--port):specify server port.\n");
    printf("-h(--help):print this help information.\n");
    return ;
 }


int main(int argc, char **argv)
{   
    int                 sockfd = -1;
    int                 clifd;                 //after accept successfully
    int                 rv = -1;            
    struct sockaddr_in  servaddr;              //IPv4
    struct sockaddr_in  cliaddr;               //IPv4
    socklen_t           len;                   //栈中变量，随机值，肯恶搞>=sizeof(cli_addr)
    //len = sizeof(cliaddr);
    int                 port = 0;           
    char                buf[1024];             
    struct option       opts[] = {
        {"ipaddr", required_argument, NULL, 'i'},
        {"port", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    int                 opt;
    int                 idx;
    int                 ch;
    int                 on = 1;

    while((ch = getopt_long(argc, argv, "p:h", opts, NULL)) != -1)
    {
            switch(ch)
            {
                case 'p':
                    port = atoi(optarg);
                    break;

                case 'h':
                    print_usage(argv[0]);
                    return 0;
            }
    }

    // have not Change servip or port
    if( !port )
    {
        print_usage(argv[0]);
        return 0;
    }

    /*
    if(argc < 3)
    {
        printf("program usage: %s [ServerIP] [Port]\n", argv[0]);
        return -1;
    }
    servip = argv[1];
    port = atoi(argv[2]);
    */

    //1.socket()
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) 
    {
        printf("Create socket failure:%s\n", strerror(errno));
        return -1;
    }
    printf("Create socket[%d] successfully!\n", sockfd);    

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    //2.bind()
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET;               //协议
    servaddr.sin_port = htons(port);             //端口
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//listen all ip
    //inet_aton("192.168.2.240", &servaddr.sin_addr); 
    rv = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(rv < 0)
    {
        printf("socket[%d] bind on port[%d] failure: %s\n", sockfd, port, strerror(errno)); 
        return -2;
    }

    //3.listen()
    listen(sockfd, 13);
    printf("start to listen on port[%d]\n", port);

    while (1)
    {
        printf("start accept new client in coming...\n");

        //4.accept()
        clifd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        if(clifd < 0)
        {
            printf("accept new client failure: %s\n",strerror(errno));
            continue; //Skip the remaining code of the current loop and proceed directly to the next iteration of the loop.
        }
        printf("accept new client[%s:%d] successfully\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port)); // printf the ip and port from the accepted client

        //5.read()
        memset(buf, 0, sizeof(buf)); 
        rv = read(clifd, buf, sizeof(buf));
        if(rv < 0)
        {
            printf("read data from client clifd[%d] failure:%s\n", clifd, strerror(errno));
            close(clifd);
            continue;
        }
        else if (rv == 0)    
        {
            printf("socket[%d] get disconnected\n",clifd);
            continue;
        }
        else if(rv > 0)
        {
            printf("Read %d bytes data from client: %s\n", rv, buf);
        }

        //6.write()
        rv = write(clifd, MSG_STR, strlen(MSG_STR));
        if(rv < 0) 
        {
            printf("write to server by  clifd[%d] failure:%s\n", clifd, strerror(errno));
            close(clifd);
            continue;
        }
        printf("close client socket[%d]\n", clifd);
        close(clifd);
    }

    close(sockfd);
}