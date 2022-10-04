/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // From: https://linux.die.net/man/3/inet_addr
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define _OPEN_SYS_ITOA_EXT

#define SERVER "137.112.38.183"
#define PORT 1874
#define BUFSIZE 1024
#define COMMID 312

#define RHP_TYPE_SIZE 1
#define RHP_COMMID_SIZE 2
#define RHP_LENGTH_SIZE 2
#define RHP_CHECKSUM_SIZE 2

#define RHP_CONTROL_MESSAGE 2
#define RHP_RHMP_MESSAGE 4

#define MESSAGE_1_SIZE 5

#define MESSAGE_1 "hello"
#define MESSAGE_2 "hi"



int send_config(int clientSocket,void *buffer,int size,struct sockaddr_in serverAddr){
    if (sendto(clientSocket,buffer, size, 0,
            (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0) {
        perror("sendto failed");
        return 0;
    }
    memset(buffer,'\0',sizeof(buffer));
    recvfrom(clientSocket, buffer, BUFSIZE, 0, NULL, NULL);  
    printf("Received from server: %s\n", (char*)buffer);
    return atoi(buffer);
}

void printHeader(uint8_t* data){
    printf("\nprint function: \n");
    int type = data[0];
    int comID = data[1]<<8 | data[2];
    int paylength =data[4]<<8|data[3];
    int checksum = data[sizeof(data)-1];
    int i;
    printf("type: %d\n",type);
    printf("comID: %d\n",comID);
    printf("paylength: %d\n",paylength);
    printf("message:");
    for(i =5;i<=4+MESSAGE_1_SIZE;i++){
        printf("%c",data[i]);
    }
    printf("\nBytes checked:%d \nchecksum:",i);
    for(i=i+1;i<=12;i++) printf("%x",data[i]);
    printf("\n");

}


int main() {
    int clientSocket, nBytes;
    char buffer[BUFSIZE];
    struct sockaddr_in clientAddr, serverAddr;
    uint8_t data [BUFSIZE];
    /*Create UDP socket*/
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket");
        return 0;
    }

    /* Bind to an arbitrary return address.
     * Because this is the client side, we don't care about the address 
     * since no application will initiate communication here - it will 
     * just send responses 
     * INADDR_ANY is the IP address and 0 is the port (allow OS to select port) 
     * htonl converts a long integer (e.g. address) to a network representation 
     * htons converts a short integer (e.g. port) to a network representation */
    memset((char *) &clientAddr, 0, sizeof (clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(0);




    if (bind(clientSocket, (struct sockaddr *) &clientAddr, sizeof (clientAddr)) < 0) {
        perror("bind failed");
        return 0;
    }

    /* Configure settings in server address struct */
    memset((char*) &serverAddr, 0, sizeof (serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    memset(buffer,'\0',BUFSIZE);
    memset(data,'\0',10);
    int position_indicator = 0;
    /* TYPE */
    data[position_indicator++] = RHP_CONTROL_MESSAGE;

    /* commID dec 312 hex 0x138*/
    data[position_indicator++] = 0x01;
    data[position_indicator++] = 0x38;



    /*LENGTH*/
    data[position_indicator++] = 5;
    data[position_indicator++] = 0;



    /*PAYLOAD*/
    char message_1[MESSAGE_1_SIZE] = MESSAGE_1;
    for(int i =0;i<=MESSAGE_1_SIZE;i++){
        data[position_indicator++] = message_1[i];
    }
    //finish at data [9]


    if((position_indicator+1)%2!=0){
        data[position_indicator++] = 0;       
        printf("buffer added");
        }
    uint32_t checks_result = 0;
    /*CHECKSUM COMPUTE*/
    for (int i = 0;i<position_indicator;i++){
        checks_result = checks_result + data[i];
        if(checks_result>0xff) {
            checks_result += 1;
            checks_result = checks_result & 0xFF;
            }
    }
    checks_result = ~(checks_result & 0xFF);

    data[position_indicator++] = checks_result & 0xFF;
    data[position_indicator++] = (checks_result>>8) & 0xFF;
    /* CHECKSUM */
    printHeader(data);

    close(clientSocket);
    return 0;
}



