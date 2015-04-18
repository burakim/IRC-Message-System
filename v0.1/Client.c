/*
	Computer Networks Term Project Client Application
	Writers:
		Halim Burak Yesilyurt
		Emrah Akgul
		Umut Pilge
		This document's rights are protected by MIT licence.
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
#include "SystemCodes.h"

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

struct compacket
{
	int senderfd;
	char message[1024];
	int isConsumed;
	int SystemCode;

};
int main(int argc, char **argv){
	if(argc<=2){printf("Please give IP address and port number of server to connect\n");exit(1);}	
	char send_buf[BUFSIZE];
	char buf[BUFSIZE];
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
	printf("%s\n","Client sends connection request to server..." );
	if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr))==-1) {//connect request is send to server
		printf("Connection cannot be done!!!\n");
		exit(1);
	}
	else
		printf("%s\n","Connection established" );

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
							struct compacket compac;			

					int return1 =fgets(compac.message,BUFSIZE,stdin);


					
                   
					if(strcmp(send_buf,"quit\n")==0){
						exit(0);
						
						compac.SystemCode = NORMAL_MESSAGE;

					}else if(send(sockfd,&compac,sizeof(compac),0))//data is sent to server
						 printf(">message is sended\n");

						
					
						
				}
				int nbytes=0;
				struct compacket packet;
				 if((nbytes=recv(i,&packet,sizeof(packet),0))>0){//from i. connection,socket	

                    packet.message[nbytes]='\0';
                    printf("%s%d\n","Received packet error code = ",packet.SystemCode);
                    
                    	switch (packet.SystemCode)
                    	{
                    		case LOGIN_REQUEST:
                    		{
                    			struct compacket compac;
                    			char username[256];
                    			username[0] = '\0';
                    			char password[256];
                    			password[0] = '\0';

                    			printf("%s\n","Enter username password");
                    			scanf("%s%s",&username,&password);
                    			printf("%s%s\n",username ," pass" );
                    			
                    			compac.message[0] = '\0';
                    			strcat(compac.message,username);
                    			strcat(compac.message," ");
                    			strcat(compac.message,password);
                    			//gathered[512] = '\0';
                    				
                    			
                    			compac.SystemCode = NORMAL_MESSAGE;

                    			if(send(sockfd,&compac,sizeof(compac),0))//data is sent to server
									 printf(">user creditendals sended\n");
									break;
                    		}
                    		case WRONG_CREDENTIAL:
                    		{
                    			printf("%s\n", "Wrong username and password entered");
                    		break;
                    		}
                    		case NORMAL_MESSAGE:
                    		{
                    			printf("%d\n", packet.SystemCode);
                    			  printf(" %d%s%s\n",packet.senderfd," says: ",packet.message);

                    			break;
                    		}
                    		case SESION_LIST_SEND:
                    		{
                    			printf("%s\n","Authetication succesfull. Please join one of sessions that listed under this line by entering session number" );
                    			int j;
                    			int maxSession = atoi(packet.message);
                    			printf("%s\n","Session List:" );
                    			for(j=0;j<maxSession;j++)
                    			{
                    				printf("%s%d%s\n", " - ",(j+1)," th session");
                    			}
                    			struct compacket compac;
                    			scanf("%s",compac.message);
                    			compac.SystemCode =SESSION_JOIN_REQUEST;

                    			if(send(sockfd,&compac,sizeof(compac),0))//data is sent to server
									 printf(">user creditendals sended\n");

                    			break;
                    		}
                    		default:
                    		{

                    		}
                    	}
                    
				    		// printf("Socket closed...\n");
					   		// close(i);//connection,socket closed						    		
					   		// FD_CLR(i,&master);//it is removed from master set
                    
					}
					
				
        }

         
			
	}
	printf("client-quited\n");
	close(sockfd);//socket is closed
	return 0;
}

