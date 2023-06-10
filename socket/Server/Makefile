Server:Server.o ProcessData.o checksum.o AES.o
	gcc -o Server Server.o ProcessData.o checksum.o AES.o -lm 

Server.o: Server.c ProcessData.h 
	gcc -c Server.c 

ProcessData.o: ProcessData.c ProcessData.h checksum.h AES.h
	gcc -c ProcessData.c -lm

checksum.o: checksum.c checksum.h AES.h
	gcc -c checksum.c

AES.o: AES.c AES.h
	gcc -c AES.c

clean:
	rm *.o
