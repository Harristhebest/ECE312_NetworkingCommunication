   /* project 1 sockets programming
   File name: chatServer.c
   Author: Harris Wu
   Date: 9/18/2022
   File Description: this program sets a communication channel between two devices, this file is the Server side.

   Function description:
                As the Server, the listenning process will begin once the username has been typed into the prompt
                once the connection request is recieved.
    Note: Typing "quit" will terminate the program

    Important Variables:
        sockfd: integer that identifies the socket
        portno: port number
        n: error indicator
        serv_addr: address of the server
        cli_addr: address of the client
        buffer: buffer for the communication info
        username: username of the other end
        localname: username of the local end

        struct sockaddr_in:
        //this struct basically includes all the information of the socket address. Here's its structure:
        //sin_family: address family TCP/IP
        //sin_port: port number
        //in_addr sin_addr: internet address
   */
  #include <stdio.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

    void error(char *msg)
  {
      perror(msg);
      exit(1);
  }


   int main(int argc, char *argv[])
   {    

        int sockfd, newsockfd, portno, clilen;  //see the description of the struct sockaddr_in in the header
        char buffer[256];
        char* username = malloc(256);     //other end 
        char local_name[256];   //local end
        bzero(local_name,256);
        pid_t pid = -1;
        struct sockaddr_in serv_addr, cli_addr;

        int n;
        if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }
        //initiate program
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
           error("ERROR opening socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);                             //convert port number to int
        serv_addr.sin_family = AF_INET;                     //domain
        serv_addr.sin_port = htons(portno);                 //port
        serv_addr.sin_addr.s_addr = INADDR_ANY;             //address

        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0) 
                 error("ERROR on binding");
        
        listen(sockfd,5);                                   //listening initiated
        clilen = sizeof(cli_addr);

        printf("waiting for connection...\n");
        //finding the request, accept the request once found
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
             error("ERROR on accept");

        //get the username
        printf("please enter your user name:");
        bzero(local_name,strlen(local_name));
        fgets(local_name,256,stdin);
        local_name[strcspn(local_name,"\n")] = '\0';


        //give client the username
        n = write(newsockfd,local_name,255);
        if (n < 0) error("ERROR writing to socket");
        printf("usernamer transmmited to the client\n");

        printf("now waiting for the other end to provide the ip addr...\n");    
        //wait for the client to provide the username and ip address
        char newbuffer[256];
        bzero(newbuffer,256);
        n = read(newsockfd,newbuffer,255);
        if (n < 0) error("ERROR reading from socket");
        char* ip_addr = strtok(newbuffer,",");    
        memset(username,0,256);    
        username = strtok(NULL,",");

        printf("Connection established with: %s , username : %s\n",ip_addr,username);



        
        //multi-thread begins here to allow reading/writing running at the same time
        pid = fork();
        
        //write messages @child
        if(pid==0){
            /*
            this while loop will end once either side typed "quit" into the propt, same for the 
            while loop in the child process
            */

            while (strcmp(buffer,"quit\n")!=0){
                printf("\n<%s>:",local_name);
                bzero(buffer,256);
                fgets(buffer,255,stdin);
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0) 
                    error("ERROR writing to socket");  
            }
        } 
        //read messages @parent
        else{
            while (strcmp(buffer,"quit\n")!=0){
                bzero(buffer,256);
                n = read(newsockfd,buffer,255);
                if (n < 0) error("ERROR reading from socket");
                printf("\n<%s>:%s",username, buffer);

            }
        }
        kill(pid, SIGKILL); //kill the process
        printf("program terminated\n");
        close(newsockfd);   //close the socket channel
        return 0; 
   }

    