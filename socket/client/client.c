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


#define MSGLEN 1024

struct ADDR {
    int port;
    char name[32];
};

void file_send(struct ADDR addr, char *filename)
{
    int sockfd;
    FILE* fp;
    struct sockaddr_in sevrAddr;
    struct hostent *host;
    char readBuff[MSGLEN];
    int len;

    //host = gethostbyname(addr.name);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket() error");
        exit(-1);
    }else
        printf("socket ok\n");

    sevrAddr.sin_family = AF_INET;
    sevrAddr.sin_port   = htons(addr.port);
    //sevrAddr.sin_addr   = *((struct in_addr*)host->h_addr);
    bzero(&sevrAddr.sin_zero, 8);
//    printf("ok2\n");
    if(connect(sockfd, (struct sockaddr*)&sevrAddr, sizeof(struct sockaddr)) == -1){
        perror("connect() error");
        exit(-1);
    }else
        printf("connect ok\n");

    //fp = open(filename, O_RDONLY);
    if(send(sockfd, filename, sizeof(filename), 0) < 0) {
        perror("send filename error");
        exit(-1);
    }
    


    fp = fopen(filename, "rb");
    if(fp != NULL){
        printf("send file");
        while(1) {
            if((len = fread(readBuff, 1, MSGLEN, fp)) > 0) {
                if(send(sockfd, readBuff, len, 0) < 0){
                    perror("send() error");
                    exit(-1);
                } else {
                    printf(".");
                    bzero(&readBuff, MSGLEN);    
                }                
            } else if(len == 0){  //等于0表示文件已到末尾
    //            send(sockfd, readBuff, strlen(readBuff), 0);
                printf("\nfile send success\n");
                break;
            } else {
                perror("read() error");
                exit(-1);
            }
        }
    }else {
        printf("open file failed\n");
        exit(-1);
    }
    fclose(fp);
    close(sockfd);
}


int main(int argc, char **argv) 
{
    struct ADDR useraddr;
    char filename[100]; 

    if(argc != 8) {
        printf("Usage: input error");
        exit(-1);
    }

    //strcpy(useraddr.name, argv[1]);
    useraddr.port = atoi(argv[2]);
	
    while(1) {
        printf("please input filename of you want send\n");
		//char* filename = (char*)argv[4];
		fgets(filename, 20, stdin);
        filename[strlen(filename)-1] = 0;
//        printf("ok1\n");
        file_send(useraddr, filename);
    }

	//password 

    return 0;
}
