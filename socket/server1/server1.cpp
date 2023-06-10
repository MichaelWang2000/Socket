#include <stdio.h>  
#include <stdlib.h>     
#include <string.h>  
#include <errno.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
#include <sys/types.h>  
#include <unistd.h> 
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>


#define  PORT      5168
#define  MSGLEN    1024

int main(int argc, char **argv)
{
    int severFd, clientFd;
    int fp,flags;
    socklen_t addrlen;
    struct sockaddr_in severAddr, clientAddr;
    char recvBuff[MSGLEN];
    char filename[100];
    int recv_len;

    if((severFd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("sockst() error");
        exit(-1);
    }

    severAddr.sin_family = AF_INET;
    severAddr.sin_port   = htons(PORT);
    severAddr.sin_addr.s_addr = htons(INADDR_ANY);
    bzero(&severAddr.sin_zero, 8);

    if(bind(severFd, (struct sockaddr*)&severAddr, sizeof(struct sockaddr)) == -1) {
        perror("bind() error");
        exit(-1);
    }

    if(listen(severFd, 1) == -1) {
        perror("listen() error");
        exit(-1);
    }
    
    addrlen =sizeof(struct sockaddr);
    while(1) {
        flags = 0;
        if((clientFd = accept(severFd, (struct sockaddr*)&clientAddr, &addrlen)) == -1){
            perror("accept() error");
            exit(-1);
        }
        printf("recv file fome ip:%s  port: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        if(recv(clientFd, filename, sizeof(filename), 0) < 0){
            perror("recv filename error");
            break; //less
        }
        
        fp = open(filename, O_RDWR | O_CREAT, 777);
        while((recv_len = recv(clientFd, recvBuff, MSGLEN, 0)) > 0) {
            flags++;
            if(flags == 1) {
                printf("recv file start");
            }
            else {
                printf(".");
            }
            if(write(fp, recvBuff, recv_len)) {
                bzero(&recvBuff, MSGLEN);    
            }else {
                perror("write() error");
                break;
            }
        } 
        if(flags == 0) 
             perror("recv() error");
        if(flags > 0){
               printf("\nrecv success\n");
             close(clientFd);
        }
    }

    close(severFd);
    return 0;
}
