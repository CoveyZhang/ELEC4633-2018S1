#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>


int main(int argc, char *argv[])
{
    int *mem1;
    int *buffer;
    int *finalset;
    int serverfd = 0, clientfd = 0;
    int n=2;
    struct sockaddr_in serv_addr;
    int sendBuff[10];
    int recvBuff[2];
    //int heartbeat_compare=0;
    buffer= rtai_malloc(nam2num("shmem0"), sizeof(int)*10);
    mem1 = rtai_malloc(nam2num("shmem1"), sizeof(int));
    finalset = rtai_kmalloc(nam2num("shmem2"), sizeof(int));
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));   
    memset(sendBuff, '0', sizeof(sendBuff));     
    serv_addr.sin_family = AF_INET;
    //INADDR_ANY local host
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);
    bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //puts the server socket in a passive mode waiting for the client to make a connection   
    listen(serverfd, 5);                         
    while(1){
    	clientfd = accept(serverfd, (struct sockaddr*)NULL, NULL);
    	
        while( (n = read(clientfd, recvBuff, sizeof(recvBuff))) > 0){
            if(recvBuff[0]==1){
                printf("changing setpoint to %d\n",recvBuff[1] );;
                *finalset= recvBuff[1];
            }else if(recvBuff[0]==2){
				//int connected=1;
				while(1){
					//connected=0;
            	if(*mem1==9){
						int loop;
						for(loop = 0; loop < 10; loop++){
               		sendBuff[loop]= buffer[loop];
						}
					}
            	write(clientfd,sendBuff,sizeof(sendBuff));
					// if disconneted, the value of 'connected' will be 0
					//fault detection every one second
					//close(clientfd);
					//clientfd = accept(serverfd, (struct sockaddr*)NULL, NULL);
					//read(clientfd,connected,sizeof(connected));
					//printf("flag connected is %d\n",connected);
            				sleep(1);
					}
					//printf("connnection is shut down due to the error on the client side\n");
            				//return 1;
            }else if(recvBuff[0]==3){
            	*finalset=0;
            	printf("shut down the connnection by the cilent\n");
            	return 1;
            }
            
         }
         if(n<1){
    			*finalset=0;
            printf("disconnnection due to the error of the cilent\n");
            return 1;
           }
         	
            
        close(clientfd);
    }
   rtai_free(nam2num("shmem0"), *buffer);
	rtai_free(nam2num("shmem1"), *mem1);
	rtai_free(nam2num("shmem2"), *finalset);
}

