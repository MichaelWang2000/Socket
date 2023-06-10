// g++ -m32 -o test test.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "pfile.h"
#include <sys/stat.h>

#define PATH ""
int main(int argc, char*argv[])
{
	int sockfd;
	struct sockaddr_in addr;
	char wholepath[256] = {0};

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error :%s", perror);
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3333);
	addr.sin_addr.s_addr = inet_addr("192.168.203.58");
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("connect error :%s\n", perror);
        return -1;
	}


	char buffer[1024] = {0};
	pfilehead_t head;
	bzero(&head, sizeof(head));
	if(argv[1] == NULL)
	{
		printf("run as:./test [filename]\n");
		return 0;
	}
	strcpy(head.filename, argv[1]);
	//strcpy(head.filename, "hfudshfiudhf.mp3");
	sprintf(wholepath, "%s%s", PATH, head.filename);
	FILE *fp = fopen(wholepath, "r");
	if(fp == NULL)
	{
		printf("fopen %s error\n", head.filename);
		return -1;
	}
	struct stat statbuf;
	stat(wholepath, &statbuf);
	head.datalen = statbuf.st_size;
	printf("filelen=%d\n", head.datalen);
	memcpy(buffer+2, &head, sizeof(head));
	int len = sizeof(head)+3;
	int translen = htons(len);
	memcpy(buffer, &translen, 2);
	printf("len=%d,buffer=%s\n", len, buffer+2);
	int ret = send(sockfd, buffer, len, 0);

	int readlen = 0;
	bzero(buffer, sizeof(buffer));
	while(readlen = fread(buffer, sizeof(char), sizeof(buffer), fp))
	{
		if(readlen < 0)
			break;
		if(send(sockfd, buffer, readlen, 0) < 0)
		{
			printf("send error\n");
			break;
		}
		//printf("send %d\n", readlen);
		bzero(buffer, sizeof(buffer));
	}
	fclose(fp);
	printf("close\n");
	close(sockfd);
	return 0;
}
