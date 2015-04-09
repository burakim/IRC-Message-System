/*
	Computer Networks Term Project Server Application
	Writers:
		Halim Burak Yesilyurt
		Emrah Akgul
		Umut Pilge
		This document's rights are protected by MIT licence.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>  

struct compacket
{
	int senderfd;
	char message[1024];
	int isConsumed;
};

int main(int argc, char **argv){
	if(argc<=1){
		printf("Please give port number to listen\n");exit(1);
	}   

	fd_set master; //master file descriptor list	   
	fd_set read_fds; //copy of master file descriptor list which is temp
	char send_buf[1024];

	struct sockaddr_in serveraddr;//server address
	struct sockaddr_in clientaddr;//client address
	   
	int fdmax; //biggest file descriptor number	    
	int listener;//socket descriptor which listens client
	int newfd; //new socket which is accepted
	    
 	char buf[1024];//buffer for data
	int nbytes;//number of received bytes
	int PORT=atoi(argv[1]);
	    /* for setsockopt() SO_REUSEADDR, below */
	int addrlen;
    int i;
    int yes=1;
    
	FD_ZERO(&master);//master and temp is cleared
	FD_ZERO(&read_fds);
	   
	if((listener=socket(AF_INET,SOCK_STREAM,0))==-1){ //socket is defined TCP, and return socket descriptor for listening
                                                    // for UDP, use DATAGRAM instead of STREAM
 		printf("Socket cannot be created!!!\n");   	
		exit(1);//EXIT FAILURE
 	}
	printf("Socket is created...\n");

	    /*if there exist such situation which is "address already in use" error message, even so use this socket */
    if(setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){//socket,level,optname,optval,optlen are set
        printf("Socket cannot be used!!!\n");
        exit(1);//EXIT FAILURE
    }
	printf("Socket is being used...\n");
		    
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_addr.s_addr=INADDR_ANY;//use my IP address
	serveraddr.sin_port=htons(PORT);
	memset(&(serveraddr.sin_zero),'\0',8);	    
	
	if(bind(listener,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1){//socket and port is bind
		printf("Binding cannot be done!!!\n");
		exit(1);//EXIT FAILURE
 	}
	printf("Binding is done...\n");	   
	
	if(listen(listener,45)==-1){//listening, 45 connection is let in buffer
		printf("");
		exit(1);//EXIT FAILURE
 	}	
	printf("Server is listening...\n");	   
	
	FD_SET(listener,&master);//add listener to master set	    
	fdmax=listener; //biggest is listener now	    
	struct compacket packet;
packet.isConsumed = -1;

	for(;;){
    	read_fds=master;//copy of master
    	if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1){ //track ready sockets to read,write, supports multiplexing, read_fds is updated
			exit(1);//EXIT FAILURE
    	}
    	else if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==1)
    	{
    		printf("%s\n","There is only one connection..." );
    	}
    	else
    	{
    		printf("%s\n", "Server multiplex sockets...");
    	}
    	for(i=0;i<=fdmax;i++){//all list is controlled	
			if(FD_ISSET(i,&read_fds)){ //if it is in temp list		
				printf("%s%d%s%d\n","Current i val = ",i," fdmax val= ",fdmax );
				if(i==listener){ //if it is listener, there is new connection
					addrlen=sizeof(clientaddr);
                    
                    newfd=accept(listener,(struct sockaddr *)&clientaddr,&addrlen);//accept connect request of client and return new socket for this connection
                    if(newfd>0)
                    printf("%s\n","Server accepts connection request of Client..." );
                    FD_SET(newfd,&master); // add new socket to master set
					char ipAddres[INET_ADDRSTRLEN];
                    if(inet_ntop(AF_INET, &(clientaddr.sin_addr), ipAddres, INET_ADDRSTRLEN)== NULL)
                    {
                    	printf("%s\n","Long unsugned to ip adess conversion failed" );
                    }

                    printf("%s%s%s%hu\n", "New connection from ",ipAddres," on socket ",ntohs(clientaddr.sin_port));
                    
                    if(newfd>fdmax)
                        fdmax=newfd;//it is biggest socket number
                   
				}else{
				
					if((nbytes=recv(i,buf,sizeof(buf),0))<=0){//from i. connection,socket				
                    
				    		printf("Socket closed...\n");
					   		close(i);//connection,socket closed						    		
					   		FD_CLR(i,&master);//it is removed from master set
						
                   
					
                    
					}else{	//if data is received from a client
						packet.message[nbytes]='\0';
						int j;
						for(j=0;j<nbytes;j++)
						{
							packet.message[j] = buf[j];
						}
						
						packet.senderfd = i;
						packet.isConsumed = 0;
                        printf("%d says: %s \n",i,buf);
                        for(j=0;j<=fdmax;j++)
                        {
                       
                        if(packet.isConsumed != -1)
                        {
					if(send(j,&packet,sizeof(packet),0))//data is sent to server
						 printf(">message is sended\n"); //if it is not listener, there is data from client
						packet.isConsumed++;
					}
					

				
			}
                       
				    }
				}
			
		    }
		    else
		    {
		    		
				}
					
		    }
		    // printf("%s\n","Gonder0" );
		}
       //  for(j=0;j<=fdmax;j++)
       //  {
       //  	// printf("%s\n","Gonder1" );
       //  	if(FD_ISSET(j,&read_fds))
       //  	{
       //  		// printf("%s\n","Gonder2" );
       //  		if(j!=listener&& j!= i)
       //  		{
       //  			// printf("%s\n",buf );
						
       //  			if(send(i,buf,strlen(buf),0))//data is sent to server
						 // // printf(">message is sended\n");
       //  		}
       //  	}
       //  }
	
	return 0;
}

 // int k;
 //                        for(k=0;k<fdmax;k++)
	// 				if(send(k,buf,strlen(buf),0))//data is sent to server
	// 					 printf(">message is sended\n"); //if it is not listener, there is data from client
