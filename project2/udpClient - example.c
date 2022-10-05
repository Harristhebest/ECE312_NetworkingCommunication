/************* UDP CLIENT CODE *******************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // From: https://linux.die.net/man/3/inet_addr
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define _OPEN_SYS_ITOA_EXT

// #define SERVER "137.112.38.183"
#define SERVER "localhost"

#define PORT 1874
#define BUFSIZE 1024
#define COMMID 312

#define RHP_CONTROL_MESSAGE 2
#define RHP_RHMP_MESSAGE 4

#define MESSAGE_1_SIZE 5
#define MESSAGE_2_SIZE 2
#define MESSAGE_1 "hello"
#define MESSAGE_2 "hi"



int send_config(int clientSocket,uint8_t *buffer,int size,struct sockaddr_in serverAddr){

    if (sendto(clientSocket,buffer, size, 0,
            (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0) {
        perror("sendto failed");
        return 0;
    }
    memset(buffer,'\0',sizeof(buffer));
    recvfrom(clientSocket, buffer, BUFSIZE, 0, NULL, NULL);  
    printf("Received from server: %s\n", (char*)(&buffer[5]));
    return atoi(buffer);
}

//10878     2A7E
void printHeader(uint8_t* data,char* message){
    printf("print function: \n");
    int type = data[0];
    int comID = data[2]<<8 | data[1];
    int paylength =data[4]<<8|data[3];
    int i;
    printf("type: %d\n",type);
    printf("comID: %d\n",comID);
    printf("paylength: %d\n",paylength);
    printf("message:");
    for(i =5;i<=4+paylength;i++){
        printf("%x ",data[i]);
    }
    printf("\nBytes checked:%d \nchecksum:",i);
    int checksum = data[i+1]<<8 | data[i];
    
    printf("%x\n",checksum);

}

uint16_t checksum_Compute(uint8_t* data,int position_indicator){
    /*CHECKSUM COMPUTE*/
    /*
    data:                       buffer got sent to the server
    position_indicator:         records the size of the buffer before the checksum field (in Bytes)
    */
    uint32_t checks_result=0;
    for (int i = 0;i<position_indicator;i+=2){
        checks_result = checks_result + (((data[i]<<8))|(data[i+1]&0xFF));
        if(checks_result>0xffff) {
            checks_result = checks_result & 0xFFFF;
            checks_result += 1;
            printf("ones complement added\n");
            }
    }

    checks_result = (~(checks_result)&  0xFFFF) ;
    printf("%x\n",(checks_result));
    return checks_result;

}

void config_data(char* message,uint8_t* data){
    int position_indicator = 3;
    /*LENGTH*/
    data[position_indicator++] = (strlen(message))&0xff;
    data[position_indicator++] = ( strlen(message)>>8)&0xff;
    /*PAYLOAD*/
    for(int i =0;i<strlen(message);i++){
        data[position_indicator++] = message[i];
    }

    if((position_indicator)%2!=0){
        data[position_indicator++] = 0;       
        printf("buffer added\n");
        }


    /* CHECKSUM */
    uint16_t checks_result = checksum_Compute(data,position_indicator);
    data[position_indicator++] = (checks_result & 0xFF);
    data[position_indicator++] =( (checks_result>>8) & 0xFF);
    memset(message,0,sizeof(message));
}

int main() {
    char message_hello[MESSAGE_1_SIZE] = MESSAGE_1;
    char message_hi[MESSAGE_2_SIZE] = MESSAGE_2;
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
    memset(data,'\0',BUFSIZE);
    int position_indicator = 0;
    /* TYPE */
    data[position_indicator++] = RHP_CONTROL_MESSAGE;
    uint16_t id= COMMID;
    /* commID dec 312 hex 0x138*/
    data[position_indicator++] = id & 0xFF;
    data[position_indicator++] = (id >>8)&0xFF;



    // /*LENGTH*/
    // data[position_indicator++] = sizeof(message_hello);
    // data[position_indicator++] = 0;



    // /*PAYLOAD*/
    // for(int i =0;i<MESSAGE_1_SIZE;i++){
    //     data[position_indicator++] = message_hello[i];
    // }

    // if((position_indicator)%2!=0){
    //     data[position_indicator++] = 0;       
    //     printf("buffer added");
    //     }


    // /* CHECKSUM */
    // uint16_t checks_result = checksum_Compute(data,position_indicator);

    // data[position_indicator++] = (checks_result & 0xFF);
    // data[position_indicator++] =( (checks_result>>8) & 0xFF);

    config_data(message_hello,data);

    printHeader(data,message_hello);
    for(int i = 0;i<12;i++){
        printf("%x ",data[i]);
    }
        printf("\n");

    // send_config(clientSocket,data,position_indicator,serverAddr);
    
        printf("\n");
        printf("\n");
        printf("\n");


    config_data(message_hi,data);    
    printHeader(data,message_hi);
    for(int i = 0;i<10;i++){
        printf("%x ",data[i]);
    }
        printf("\n");
    send_config(clientSocket,data,MESSAGE_2_SIZE+8,serverAddr);
    close(clientSocket);


    return 0;
}



