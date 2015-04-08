/*
	Computer Networks Term Project Client Application
	Writers:
		Halim Burak Yesilyurt
		Emrah Akgul
		Umut Pilge
		This document's rights is protected by MIT licence.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>	

#define BUFSIZE 1024

/*
struct sockaddr_in{
	short int sin_family;
	unsigned short int sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero[8];
};

struct in_addr{
	unsigned long s_addr;
};
*/

int main(int argc, char **argv){
	if(argc<=2){printf("Please give IP address and port number of server to connect\n");exit(1);}	
	char send_buf[BUFSIZE];
	int sockfd, fdmax, i;

	struct sockaddr_in server_addr;

	fd_set master;
	fd_set read_fds;

	if ((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) {//socket(domain,type,protocol) is defined by TCP (SOCK_STREAM) and returned to socket descriptor
		printf("Socket cannot be created!!!\n");
		exit(1);
	}

	server_addr.sin_family=AF_INET;//network family
	server_addr.sin_port=htons(atoi(argv[2]));//host to network short changes is done
	server_addr.sin_addr.s_addr=inet_addr(argv[1]);//address is put to sin_addr in server
	memset(server_addr.sin_zero,'\0',sizeof server_addr.sin_zero);//sin_zero is fulled by 0

	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))==-1) {//connect request is send to server
		printf("Connection cannot be done!!!\n");
		exit(1);
	}

	FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0,&master);//0 is added to master file
	FD_SET(sockfd,&master);//sockfd is added to master file

	fdmax=sockfd;

	while(1){
		read_fds=master;
		if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1){//track ready sockets to read,write, supports multiplexing, read_fds is updated
			printf("Sockets cannot be multiplexed!!!\n");
			exit(4);
		}
		for(i=0;i<=fdmax;i++)
			if(FD_ISSET(i,&read_fds)){		
				if(i==0){//ready for write
					fgets(send_buf,BUFSIZE,stdin);
                   
					if(strcmp(send_buf,"quit\n")==0){
						exit(0);
					}else if(send(sockfd,send_buf,strlen(send_buf),0))//data is sent to server
						 printf(">message is sended\n");
				}
        }
			
	}
	printf("client-quited\n");
	close(sockfd);//socket is closed
	return 0;
}

