# Socket
自己弄的一个服务器，实现了从客户端传给sever文件。
文件说明
“AES.h” 
Responsible for encrypting and decrypting files
‘Checksum.h’
The calculation method of checksum and test method are defined
‘ProcessData.h’
Integrated data processing mode, can centrally process data return message
Checksum.o ProcessData.o AES.o server.o
Created to make makefile operations easier on the Server side as a whole
客户端程序设计结构说明
Initialize socket resources and obtain the server IP address and port number.Create a socket and connect to the server.TCP packets processed by the data processing function are sent to the serve.If successful, the encrypted message sent by the server is received and stored in a file.
客户端运行结果截图
The content is “My test, good luck!”
client客户端程序 ./client 运行

server服务器端运行结果



客户端发送事例

服务器接收
