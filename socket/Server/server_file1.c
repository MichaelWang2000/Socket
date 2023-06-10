/*************************************************************************
	> File Name: Server.c
	> Author: SongLee
	> E-mail: lisong.shine@qq.com 
	> Created Time: 2014年03月13日 星期四 22时17分43秒
    > Personal Blog: http://songlee24.github.io/
 ************************************************************************/
 
#include<netinet/in.h>  // sockaddr_in
#include<sys/types.h>   // socket
#include<sys/socket.h>  // socket
#include<stdio.h>       // printf
#include<stdlib.h>      // exit
#include<string.h>      // bzero
 
#define SERVER_PORT 8000
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
 
int main(void)
{
    // 声明并初始化一个服务器端的socket地址结构
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
 
    // 创建socket，若成功，返回socket描述符
    int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(server_socket_fd < 0)
    {
        perror("Create Socket Failed:");
        exit(1);
    }
    int opt = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
    // 绑定socket和socket地址结构
    if(-1 == (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))))
    {
        perror("Server Bind Failed:");
        exit(1);
    }
    
    // socket监听
    if(-1 == (listen(server_socket_fd, LENGTH_OF_LISTEN_QUEUE)))
    {
        perror("Server Listen Failed:");
        exit(1);
    }
 
    while(1)
    {
        // 定义客户端的socket地址结构
        struct sockaddr_in client_addr;
        socklen_t client_addr_length = sizeof(client_addr);
 
        // 接受连接请求，返回一个新的socket(描述符)，这个新socket用于同连接的客户端通信
        // accept函数会把连接到的客户端信息写到client_addr中
        int new_server_socket_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_addr_length);
        if(new_server_socket_fd < 0)
        {
            perror("Server Accept Failed:");
            break;
        }
 
        // recv函数接收数据到缓冲区buffer中
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
    
        // 打开文件，准备写入
        FILE *fp = fopen("rec_file", "w");
        if(NULL == fp)
        {
            printf("File:\t%s Can Not Open To Write\n", "rec_file");
            exit(1);
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            int length = 0;
            // 从服务器接收数据到buffer中
            // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止   
            while((length = recv(new_server_socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
            {
                if(fwrite(buffer, sizeof(char), length, fp) < length)
                {
                    printf("File:\t%s Write Failed\n", "rec_file");
                    break;
                }
                bzero(buffer, BUFFER_SIZE);
            }
 
            // 关闭文件
            fclose(fp);
            printf("Receive File:\t%s From  Client Successful! \n", "rec_file");
        }
        // 关闭与客户端的连接
        close(new_server_socket_fd);
    }
    // 关闭监听用的socket
    close(server_socket_fd);
    return 0;
}

