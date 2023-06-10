#include "main.h"
#include "pfile.h"

#define FILE_PATH "/home/ulp/"	
#define MAX_CONNECT 128
static int lindex = 0;
pthread_t tid;

int addEvent(int epollfd, int fd, int state)
{
	struct epoll_event ev;

	ev.events = state;
	ev.data.fd = fd;
	return epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int delEvent(int epollfd, int fd, int state)
{
	struct epoll_event ev;

	ev.events = state;
	ev.data.fd = fd;
	return epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}


int serListen(int domain, char *servicename,int *pfd)
{
	int listen_sockfd;
	struct sockaddr_in seraddr;
	int ret_code=0;
	short port;
	int reuse_addr=1;
	struct servent *service=0;


	if((port=atoi(servicename))==0) {
		if((service=getservbyname(servicename,"tcp"))==NULL)
		{
			return -1;
		}
		else port=service->s_port;
	}

	if((listen_sockfd=socket(domain,SOCK_STREAM,0))<0) {
		printf("socket error\n"); 
		return -1;
	}

	bzero(&seraddr,sizeof(seraddr));
	seraddr.sin_family=domain;
	seraddr.sin_addr.s_addr=htonl(INADDR_ANY);
	seraddr.sin_port=htons(port);

	ret_code=setsockopt(listen_sockfd,SOL_SOCKET,SO_REUSEADDR,(const char *)&reuse_addr,sizeof(reuse_addr));
	if(ret_code<0) {
		printf("setsockopt.reuse_addr error\n");
		close(listen_sockfd);
		return -1;
	}

	ret_code=bind(listen_sockfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
	if(ret_code<0) {
		printf("bind error\n");
		close(listen_sockfd);
		return -1;
	}

	ret_code=listen(listen_sockfd,1000);
	if(ret_code<0) {
		printf("listen error\n");
		close(listen_sockfd);
		return -1;
	}

	*pfd=listen_sockfd;
	return 0;
}

void * cit(void *arg)
{
	int len = 0;
	int recvlen = 0;
	int writelen = 0;
	int sockfd;
	char buff[STR_LEN_2048] = {0};	
	FILE *fp = NULL;
	pfilehead_t head;
	char wholepath[STR_LEN_256] = {0};

	sockfd = (int )arg;
	recvlen = recv(sockfd, buff, 2, 0);
	memcpy(&len, buff, 2);
	len = ntohs(len);
	recvlen = recv(sockfd, buff+2, len-2, 0);
	pfilehead_t *tmp = (pfilehead_t *)(buff+2);

	sprintf(wholepath, "%s%s", FILE_PATH, tmp->filename);	
	int filelen = tmp->datalen;
	fp = fopen(wholepath, "w");	
	if(fp == NULL)
	{
		printf("fopen %s failed\n", wholepath);
		return NULL;
	}
	printf("writeFile:%s, time:%d\n", wholepath, time(NULL));
	bzero(buff, sizeof(buff));	
	len = 0;
	while(len = recv(sockfd, buff, sizeof(buff), 0))
	{
		if(len < 0)
		{
			break;
		}
		writelen = fwrite(buff, 1, len, fp);
		if(writelen < len)
		{
			printf("write to file %s error\n", wholepath);
			break;
		}
		//printf("writelen=%d\n", writelen);
		bzero(buff, sizeof(buff));
	}
	fclose(fp);
	printf("write %s finish. time:%d\n", wholepath, time(NULL));

	//jiaoyan
	struct stat statbuf;
	stat(wholepath, &statbuf);
	if(statbuf.st_size == filelen)
		printf("%s filelen = head.datalen(%d)\n", wholepath, filelen);
	else
		printf("%s filelen(%d) != head.datalen(%d)\n", wholepath, statbuf.st_size, filelen);
	close(sockfd);
	return NULL;
}

int handleNewConnect(int epollfd, int sockfd)
{
	int clifd = -1;
	struct sockaddr_in cliaddr;
	socklen_t addrlen;
	int ret = 0;

	addrlen = sizeof(cliaddr);
	clifd =  accept(sockfd, (sockaddr *)&cliaddr, &addrlen);
	if(clifd < 0)
	{
		printf("handleNewConnect() accept(sockfd:%d) is failed! err:%d,%s", sockfd, errno, strerror(errno));
		return -1;
	}
	getpeername(clifd, (struct sockaddr *)&cliaddr, &addrlen);
	printf("accept(sockfd:%d), clifd:%d, cli_ip:%s, cli_port:%d.\n", sockfd, clifd, inet_ntoa(cliaddr.sin_addr),  ntohs(cliaddr.sin_port));


	ret = pthread_create(&tid, NULL, cit, (void *)clifd);
	lindex++;
	return ret;
	//return addEvent(epollfd, clifd, EPOLLIN);

}


int main(int argc, char* argv[])
{
	char rbuff[STR_LEN_4096];
	int ret = -1;
	int sockfd = -1;
	int epollfd;
	int len = 0;
	int i = 0;
	struct epoll_event events[MAX_CONNECT];
	int nfds = 0;

	char *tport = "3333";
	ret = serListen(AF_INET, tport, &sockfd);
	if(ret < 0)
	{
		sockfd = -1;
		printf("serListen(tport:%s) failed!", tport);
		return NULL;
	}
	epollfd = epoll_create(MAX_CONNECT);
	if(epollfd < 0)
	{
		//appSendLog(CurTv, ERROR, (char *)"%s:%d sRoutine() epoll_create failed", __FILE__, __LINE__);
		return NULL;
	}
	ret = addEvent(epollfd, sockfd, EPOLLIN);
	if(ret == -1)
	{
		//appSendLog(CurTv, ERROR, (char *)"%s:%d sRoutine() addEvent failed", __FILE__, __LINE__);
		return NULL;
	}
	for(;;)
	{
		nfds = epoll_wait(epollfd, events, MAX_CONNECT, -1);
		if(nfds == -1)
		{
			if(errno == 4)
				continue;
			//appSendLog(CurTv, ERROR, (char *)"%s:%d sRoutine() epoll_wait failed", __FILE__, __LINE__);
			return NULL;
		}
		for(i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == sockfd && events[i].events & EPOLLIN)//new connect
			{
				handleNewConnect(epollfd, sockfd);
				continue;
			}
		}
	}
	close(epollfd);
	return 0;
}
