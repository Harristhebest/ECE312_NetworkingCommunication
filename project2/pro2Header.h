// #define SERVER "137.112.38.183"
 #define SERVER "localhost"

#define _OPEN_SYS_ITOA_EXT
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>      // From: https://linux.die.net/man/3/inet_addr
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
 #define PORT 1874

#define BUFSIZE 1024
#define COMMID 312
#define DESTPORT 0x0122
#define SRCPORT 0x1B8E

#define RHP_CONTROL_MESSAGE 2
#define RHP_RHMP_MESSAGE 4

#define MESSAGE_1_SIZE 6
#define MESSAGE_2_SIZE 3
#define CHECKSUM_SIZE 2
#define TYPE_SIZE 1
#define COMMID_SIZE 2
#define LENGTH_SIZE 2
#define NULLBIT_SIZE 1
#define ID_SIZE 32
#define RHMP_REQUEST_FIELD_SIZE 4   //no length and payload


#define MESSAGE_REQUEST_TYPE 2
#define ID_REQUEST_TYPE 6



#define BIT_MASK_8 0XFF
#define BIT_MASK_16 0XFFFF

#define MESSAGE_1 "hello\0"
#define MESSAGE_2 "hi\0"

#define BUFFER_INITIAL 5

/*project 2 part a done here*/
