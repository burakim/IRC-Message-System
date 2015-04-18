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
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>  
#include "SystemCodes.h"

struct compacket
{
	int senderfd;
	char message[1024];
	int isConsumed;
	int SystemCode;

};
void setfdAuthStatus(int index, int status);
char* getTimeStamp();
int isValidUser(char* uname_pass);
 bool initalizeSessions();
 bool addSession(fd_set fdSet);
 bool handleNormalMessage(int i,struct compacket compacMost);
// Remember 0 represents no auth
//			1 represents there is auth


int* authStatus;
int authSize;
fd_set* sessions;
int sessionsSize;
int sessionsCurrentIndex;
	int fdmax; //biggest file descriptor number	 
int main(int argc, char **argv){
	if(argc<=1){
		printf("Please give port number to listen\n");exit(1);
	}   
	authSize = 8;
	authStatus = (int*)malloc(sizeof(int)*authSize);


	fd_set master; //master file descriptor list	   
	fd_set read_fds; //copy of master file descriptor list which is temp
	char send_buf[1024];

	struct sockaddr_in serveraddr;//server address
	struct sockaddr_in clientaddr;//client address
	   
   
	int listener;//socket descriptor which listens client
	int newfd; //new socket which is accepted
	    
	int nbytes;//number of received bytes
	int PORT=atoi(argv[1]);
	    /* for setsockopt() SO_REUSEADDR, below */
	int addrlen;
    int i;
    int yes=1;
    
	FD_ZERO(&master);//master and temp is cleared
	FD_ZERO(&read_fds);
	   if(initalizeSessions())
	   {
	   	// printf("%s\n","Low memory error occured. Because try again under restful memory." );
	   	// return -2;
	   }
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
int isOnlyOnce = 0;
	for(;;){
    	read_fds=master;//copy of master


    	if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==-1){ //track ready sockets to read,write, supports multiplexing, read_fds is updated
			exit(1);//EXIT FAILURE
    	}
    		int p;
   //  for(p=0;p<=sessionsCurrentIndex;p++){
   //  	if(select(fdmax+1,&sessions[p],NULL,NULL,NULL)==-1){ //track ready sockets to read,write, supports multiplexing, read_fds is updated
   //  		printf("%s%d\n","exit",p );
			// exit(1);//EXIT FAILURE
   //  	}
   //  }
    	// else if(select(fdmax+1,&read_fds,NULL,NULL,NULL)==1)
    	// {
    	// 	printf("%s\n","There is only one connection..." );
    	// }
    	// else
    	// {
    	// 	printf("%s\n", "Server multiplex sockets...");
    	// }
    	for(i=0;i<=fdmax;i++){//all list is controlled	
			if(FD_ISSET(i,&read_fds)){ //if it is in temp list		
				printf("%s%d%s%d\n","Current i val = ",i," fdmax val= ",fdmax );
				if(i==listener){ //if it is listener, there is new connection
					addrlen=sizeof(clientaddr);
                    
                    newfd=accept(listener,(struct sockaddr *)&clientaddr,&addrlen);//accept connect request of client and return new socket for this connection
                    setfdAuthStatus(newfd,0);
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

                    struct packet;

                    packet.SystemCode = LOGIN_REQUEST; // means you shall not pass, you shall login first :)
					

                    if(authStatus[newfd] ==0)
                    {
                    	printf("%s","Auth requested");
                    	if(send(newfd,&packet,sizeof(packet),0))//data is sent to server
						 printf(">Login Request sended\n"); //if it is not listener, there is data from client
                    }
                   
				}else{
					struct  compacket compacMost;
						memset(&(compacMost.message[0]),0,sizeof(compacMost.message));
						
					if((nbytes=recv(i,&compacMost,sizeof(compacMost),0))<=0){//from i. connection,socket				
                    
				    		printf("Socket closed...\n");
					   		close(i);//connection,socket closed						    		
					   		FD_CLR(i,&master);//it is removed from master set
						
                   
					
                    
					}else{	//if data is received from a client
printf("%s\n","NORMAL_MESSAGE-2" );
if(authStatus[i] == 1)
						{	
														// printf("%s",compacMost.message);
printf("%s\n","NORMAL_MESSAGE-1" );
							compacMost.message[sizeof(compacMost.message)/sizeof(char)-1]='\0';
							// printf("%s",compacMost.message);
printf("%d\n",compacMost.SystemCode );
							switch(compacMost.SystemCode)
							{
								case NORMAL_MESSAGE:
								{
									printf("%s\n","NORMAL_MESSAGE2" );
									handleNormalMessage(i,compacMost);
		       
									break;
								}

								case SESSION_JOIN_REQUEST:
								{
									printf("Socket closed4\n");
									printf("%d%s%s%s\n",i, " th user want to join " ,compacMost.message,"session");
									//if() -- TODO -- atoi exception control is need.
									int convertedValue =atoi(compacMost.message);
									if(convertedValue<= sessionsCurrentIndex){
printf("%d%s%d\n",convertedValue, " sessiona eklendi ve eklenen deger = ",i );
										FD_SET(i,&sessions[convertedValue]);
									//	FD_CLR(i,&master);//it is removed from master set
									}
									
									break;
								}
								default:{
									break;
								}
							}
						
				    }

				    else
				{
					
						if(isValidUser(compacMost.message)>0)
						{
							authStatus[i] = 1;
								// printf("%s\n","Ben gonderdim kanka0" );
							//printf("%s%s\n","user auth ok at time => ",getTimeStamp() );
								// printf("%s\n","Ben gonderdim kanka1" );
							struct compacket sessionListPacket;
								// printf("%s\n","Ben gonderdim kanka2" );
							packet.SystemCode = SESION_LIST_SEND;
	// printf("%s\n","Ben gonderdim kanka3" );
							//itoa(sessionsCurrentIndex,packet.message,10);
							sessionsCurrentIndex =4;
							sprintf(packet.message,"%d",sessionsCurrentIndex);
	printf("%s\n","Ben gonderdim kanka4" );

							if(send(i,&packet,sizeof(packet),0))//data is sent to server
						 printf(">err code is sended\n"); //if it is not listener, there is data from client
						}
						else
						{
							struct compacket packet;
							packet.SystemCode = WRONG_CREDENTIAL;
							if(send(i,&packet,sizeof(packet),0))//data is sent to server
						 printf(">err code is sended\n"); //if it is not listener, there is data from client
							
						}
				}
				}
				
				}
			
		    }
		    
		    	
		//     		int k;
		//     		for(k=0; k< sessionsCurrentIndex;k++)
		//     		{
		//     			struct  compacket compacMost2;
		//     			 printf("%s%d%s%d\n","i val = ",i,"session k val = ",k);
		//     			if(FD_ISSET(i,&sessions[k]))
		//     			{
		//     				printf("%s\n","Session bulundu" );
		//     				// printf("Socket 2132\n");
		    				
		//     				if(authStatus[i] == 1)
		// 				{	
		// 					printf("%s\n","Auth olan userdan geldi" );
		// 					int nbytes =-1;
							
		// 				memset(&(compacMost2.message[0]),0,sizeof(compacMost2.message));
						
		// 			if((nbytes=recv(i,&compacMost2,sizeof(compacMost2),0))<=0){//from i. connection,socket				
                    
		// 		    		printf("Socket closed...\n");
		// 			   		close(i);//connection,socket closed						    		
		// 			   		FD_CLR(i,&master);//it is removed from master set
						
                   
					
                    
		// 			}else{
		// 				struct compacket packet2;
		// 				printf("%s\n","Veri geldi" );
		// 					//compacMost.message[sizeof(compacMost.message)/sizeof(char)-1]='\0';
		// 					printf("%d%s\n",compacMost2.SystemCode ,compacMost2.message);
		// 					//printf("%s",compacMost.message);

		// 					switch(compacMost2.SystemCode)
		// 					{
		// 						case NORMAL_MESSAGE:
		// 						{
		// 							printf("Socket dasdasd\n");
		// 							packet2.message[nbytes]='\0';
		// 				int j;
		// 				for(j=0;j<sizeof(compacMost2.message)/sizeof(char)-1;j++)
		// 				{
		// 					packet2.message[j] = compacMost2.message[j];
		// 				}
						
		// 				packet2.senderfd = i;
		// 				packet2.isConsumed = 0;
  //                       printf("%d says: %s \n",i,compacMost2.message);
  //                       for(j=0;j<=fdmax;j++)
  //                       {
  //                      	if(authStatus[j] ==1)
  //                      	{
  //                       if(packet2.isConsumed != -1)
  //                       {
  //                       	if(FD_ISSET(j,&sessions[k]))
		//     			{
  //                       	packet2.SystemCode = NORMAL_MESSAGE;
		// 			if(send(j,&packet2,sizeof(packet2),0))//data is sent to server
		// 				 printf(">message is sended\n"); //if it is not listener, there is data from client
		// 				packet2.isConsumed++;
		// 			}
		// 		}
					

				
		// 	}
		//     			}
		//     		}
		// 		}
		// 		}	
		//     }
		//     else
		//     {
		//     	printf("%s\n","Veri geliyor ama auth yok" );
		//     }
		//     // printf("%s\n","Gonder0" );
		// }

  //      //  for(j=0;j<=fdmax;j++)
  //      //  {
  //      //  	// printf("%s\n","Gonder1" );
  //      //  	if(FD_ISSET(j,&read_fds))
  //      //  	{
  //      //  		// printf("%s\n","Gonder2" );
  //      //  		if(j!=listener&& j!= i)
  //      //  		{
  //      //  			// printf("%s\n",buf );
						
  //      //  			if(send(i,buf,strlen(buf),0))//data is sent to server
		// 				 // // printf(">message is sended\n");
  //       		}
        	
        }
    }
	free(sessions);
	return 0;
}
  
void setfdAuthStatus(int index, int status)
{
	if(authSize==index || authSize<index)
	{
		int * tempLoc = (int*)malloc(sizeof(int)*(2*index));
		int j;
		for(j=0; j<authSize; j++)
		{
			tempLoc[j]=authStatus[j];
		}
		tempLoc[j] = status;
		free(authStatus);
		authStatus = tempLoc;
		return;
	}
	else
	{

		authStatus[index] = status;
		return;
	}

}
int isValidUser(char* uname_pass)
{
	char *fname="users.txt";   // dosya adi
	FILE *fp;
	char temp[512];
	
	if((fp = fopen(fname, "r")) == NULL) {
		return(-1);
	}
	while(fgets(temp, 512, fp) != NULL) {
		if((strstr(temp, uname_pass)) != NULL) {
			return 1;
			
			
		}
	}

	
	
	if(fp) {
		fclose(fp);
	}
	return -1;

}
char* getTimeStamp()
{
	time_t clk = time(NULL);
return ctime(&clk);
}
bool initalizeSessions()
{
	sessionsSize = 8;
	sessionsCurrentIndex = -1;
	sessions = (fd_set*) malloc(sizeof(fd_set)*8);
	if(sessions != NULL)
		return true;
	else
		return false;
}

bool addSession(fd_set fdSet)
{
	sessionsCurrentIndex++;
	if(sessionsCurrentIndex<=sessionsSize)
	{
		sessions[sessionsCurrentIndex] = fdSet;
	}
	else
	{
		fd_set* tempPointer;
		fd_set* newArray = (fd_set*) malloc(sizeof(fd_set)*2*sessionsSize);
		if(newArray == NULL)
			return false;
		tempPointer = sessions;
		int i;
		for(i=0;i<sessionsSize;i++)
		{
			newArray[i] = sessions[i];
		}
		tempPointer = sessions;
		sessions = newArray;
		free(tempPointer);
	}
	return true;
}

bool handleNormalMessage(int i,struct compacket compacMost)
{
	printf("%d%s\n",i, " adli kullanici paket gonderdi dagitalim onu" );
	int j;
	int whichsession;
	for(j=0;j<=sessionsCurrentIndex;j++)
	{
		if(FD_ISSET(i,&sessions[j]))
		{
			whichsession = j;
		}

	}
	
	compacMost.senderfd = i;
	for(j=0;j<=fdmax;j++)
	{
		if(FD_ISSET(j,&sessions[whichsession]))
		{
			if(send(j,&compacMost,sizeof(compacMost),0))//data is sent to server
			{
				printf("%s%s%d%s%d\n", compacMost.message, " -> ",j, " | session id = ",whichsession );
			}
		}
	}
}