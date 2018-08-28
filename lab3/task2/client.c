#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    int connected=1;
    int recvBuff[10];
    int setpointbuffer[2];    
    struct sockaddr_in serv_addr;
    int request_cmd, newsetpoint=0;

    if(argc != 2) 
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }
            
    memset(recvBuff, '0',sizeof(recvBuff));
    //AF_INET (IPv4 protocol)                                                      
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_port = htons(5000);//port
    // Convert IPv4 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0){
        printf("\n check server IP address \n");
        return 1;
    }
    // make a request            
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){                                                                                          
       printf("\n Error : Connect Failed \n");
       return 1;
    }
    // specify the command type by user
    while(1){
    printf("what is your need?\n type 1 for setpoint\n type 2 for displaying position of the motor\n type 3 will end the connection\n");
    if (scanf("%d", &request_cmd)==1){
		if(request_cmd==1){
    			setpointbuffer[0]=1;
    			printf("Please enter your setpoint(0-359):");
    			scanf("%d",&newsetpoint);
    			setpointbuffer[1]=newsetpoint;
    			int write_result =write(sockfd,setpointbuffer,sizeof(setpointbuffer));
    			if(write_result<0){
    				printf("ERROR writing setpoint to socket\n");
    			}else {
    				printf("New setpoint is sent!!!\n");
    			}
    		}else if(request_cmd==2){
    			if((write(sockfd,&request_cmd,sizeof(request_cmd)))<0){
                    		printf("Error requesting display position\n");  
                	}else {
                    		printf("position request is sent\n");
                	}
                	while( (n = read(sockfd, recvBuff, sizeof(recvBuff))) > 0){
                    		int i;
                    		for(i=0;i<10;i++){
                        		printf("Position: %d \n",recvBuff[i]);	 
							}
							//write(sockfd,&connected,sizeof(connected));
							//printf("2222222\n");
							sleep(1);	  
    				}
    		}else if(request_cmd==3){
    			write(sockfd,&request_cmd,sizeof(request_cmd));
    			printf("Connection is closing\n");
    			return 1;
    		}else{
    			printf("Request error, please try other cmd\n");
    		}

    	}else {
    		perror("Scanf error occured\n");
    	}
    }
    close(sockfd);
    return 0;
}

         
