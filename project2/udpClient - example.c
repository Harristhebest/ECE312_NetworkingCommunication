/************* UDP CLIENT CODE *******************/

#include "pro2Header.h"
/*SENDDING FUNCTION*/
/*
This function simply send the implemented packet to the server and recieves the data returned 
from the server.
*/
void send_config(int clientSocket,uint8_t *buffer,int size,struct sockaddr_in serverAddr){

    if (sendto(clientSocket,buffer, size, 0,
            (struct sockaddr *) &serverAddr, sizeof (serverAddr)) < 0) {
        perror("sendto failed");
    }
    memset(buffer,'\0',sizeof(buffer));
    recvfrom(clientSocket, buffer, BUFSIZE, 0, NULL, NULL);  
    printf("Received from server: %s\n", (char*)(&buffer[5]));
}
/*PRINT FUNCTION*/
/*
This function prints the information about the packet, specifically each field of the packet,
which includes the following:
Type:           Instruction
ComID:          Fixed Const.
PayLoadLength:  Length of message(in Bytes)  
Message:        Message
Bytes Checked (to determine if buffer is needed)
CheckSum:       Checksum
INPUT:      uint8_t* data:      Packet Buffer
            char*    message:   message string
            uint16_t checksum:  checksum
*/
void printHeader(uint8_t* data,char* message,uint16_t checksum){
    int type = data[0];
    int comID = data[COMMID_SIZE]<<8 | data[COMMID_SIZE-1];
    int paylength =data[COMMID_SIZE+TYPE_SIZE+LENGTH_SIZE-1]<<8|data[TYPE_SIZE+COMMID_SIZE];
    int i;
    printf("type: %d\n",type);
    printf("comID: %d\n",comID);
    printf("paylength: %d\n",paylength);
    printf("message:");
    for(i =TYPE_SIZE+COMMID_SIZE+LENGTH_SIZE;i<TYPE_SIZE+COMMID_SIZE+LENGTH_SIZE+paylength;i++){
        printf("%c",data[i]);
    }
    printf("\nBytes checked:%d \nchecksum:",i+NULLBIT_SIZE);
    i += ((strlen(message)+NULLBIT_SIZE)%2==0?1:0); 
    int checksum1 = data[i+1]<<8 | data[i];
    printf("%x\n",checksum1);

}
/*CHECKSUM COMPUTE*/
/*
data:                       buffer got sent to the server
position_indicator:         records the size of the buffer before the checksum field (in Bytes)
*/
uint16_t checksum_Compute(uint8_t* data,int position_indicator){

    uint32_t checks_result=0;
    for (int i = 0;i<position_indicator;i+=2){
        checks_result = checks_result + (((data[i]<<8))|(data[i+1]&BIT_MASK_8));
        if(checks_result>BIT_MASK_16) {                  //if carryout is found
            checks_result = checks_result & BIT_MASK_16;
            checks_result += 1;
            printf("ones complement added\n");
            }
    }

    checks_result = (~(checks_result)&  BIT_MASK_16) ;   //one's complement operation
    return checks_result;

}
/*BUFFER MANIPULATION*/
/*
This function fills the actual data into the packet
INPUT:  char* message: message we want to send to the server 
        uint8_t data:  packet buffer
*/
void config_data(char* message,uint8_t* data){
    int position_indicator = 0; //store the current position

    /* TYPE */
    data[position_indicator++] = RHP_CONTROL_MESSAGE;
    uint16_t id= COMMID;
    /* commID dec 312 hex 0x138*/
    data[position_indicator++] = id & BIT_MASK_8;
    data[position_indicator++] = (id >>8)&BIT_MASK_8;
    int strlength = sizeof(message);
    /*LENGTH*/
    data[position_indicator++] = (strlength)&BIT_MASK_8;
    data[position_indicator++] = ( strlength>>8)&BIT_MASK_8;
    /*PAYLOAD*/
    for(int i =0;i<strlength;i++){
        data[position_indicator++] = message[i];
    }

    if((position_indicator)%2!=0){
        data[position_indicator++] = 0;       
        }


    /* CHECKSUM */
    uint16_t checks_result = checksum_Compute(data,position_indicator);
    data[position_indicator++] = ((checks_result>>8) & BIT_MASK_8);
    data[position_indicator++] =( (checks_result) & BIT_MASK_8);
    printHeader(data,message,checks_result);

}
/*MAIN FUNCTION*/
int main() {
    char message_hello[MESSAGE_1_SIZE] = MESSAGE_1;
    char message_hi[MESSAGE_2_SIZE] = MESSAGE_2;
    int clientSocket, nBytes;
    struct sockaddr_in clientAddr, serverAddr;
    int position_indicator = 0;

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


    /*sending message hello*/
    memset(data,'\0',BUFSIZE);
    //fill in the packet
    config_data(message_hello,data);    
    //calculate the total size of the packet
    int size = (strlen(MESSAGE_1)+NULLBIT_SIZE)+
        TYPE_SIZE+COMMID_SIZE+LENGTH_SIZE+(MESSAGE_1_SIZE%2==0?1:0)+CHECKSUM_SIZE;

    //send the message to the server
    send_config(clientSocket,data,size,serverAddr);
    printf("\n");



    /*message hi*/
    //fill in the buffer
    config_data(message_hi,data);   
    //calculate the size of the packet
    size = (strlen(MESSAGE_2)+NULLBIT_SIZE)+TYPE_SIZE+COMMID_SIZE+
        LENGTH_SIZE+(MESSAGE_2_SIZE%2==0?1:0)+CHECKSUM_SIZE;
    //send to the server
    send_config(clientSocket,data,size,serverAddr);
    //close socket
    close(clientSocket);


    return 0;
}



