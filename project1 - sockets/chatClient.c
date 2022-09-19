   /* project 1 sockets programming
   File name: chatClient.c
   Author: Harris Wu
   Date: 9/18/2022
   Description: 
                This program sets a communication channel between two devices, this file is the client side.

    Function Description: 
                As the client, once the server starts the listening process, we will need to provide the username
                 as well as the ip address of the server in order to set up the channel.

    Note: Typing "quit" will terminate the program

    Important Variables:
        sockfd: integer that identifies the socket
        portno: port number 
        n: error indicator
        serv_addr: address of the server
        buffer: buffer for the communication info
        username: username of the other end
        localname: username of the local end
   */
   #include <stdio.h>
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h> 
   #include <signal.h>
   #include <stdlib.h>
   #include <string.h>
   #include <unistd.h>

   void error(char *msg)
   {
       perror(msg);
       exit(0);
   }



   int main(int argc, char *argv[])
   {
    //variables declearation
     int sockfd, portno, n;
     struct sockaddr_in serv_addr;
     struct hostent *server;
     char buffer[256];
     char username[256];
    char local_name[256]="";
     if (argc < 3) {                            //if the argument is not enough
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
     }

     portno = atoi(argv[2]);                    //port number assignment
     sockfd = socket(AF_INET, SOCK_STREAM, 0);  //get the socket identification 

     if (sockfd < 0)                            //if socket fails
         error("ERROR opening socket");

     server = gethostbyname(argv[1]);           //get the host 
     if (server == NULL) {
         fprintf(stderr,"ERROR, no such host\n");
         exit(0);
     }

     bzero((char *) &serv_addr, sizeof(serv_addr)); //clean the memory space for the variables 
     serv_addr.sin_family = AF_INET;                //assign the struct variables AF_INET specifies the domain
     bcopy((char *)server->h_addr,                  //address assignment
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
     serv_addr.sin_port = htons(portno);            //port number
     
     
     if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
         error("ERROR connecting");



     printf("please enter your username:");         //ask for the client to provide the username
     bzero(local_name,256);                         
     fgets(local_name,256,stdin);
     local_name[strcspn(local_name, "\n")] = '\0';

    printf("ip address: %s\n", argv[1]);
    printf("waiting for connection...\n");
    bzero(username,255);                            //get the username of the other end
    n = read(sockfd,username,255);
    if (n < 0) error("ERROR reading from socket");
    printf("server username recieved\n");    


    bzero(buffer,256);
    strcpy(buffer,argv[1]);
    char info [] = ",";
    strcat(buffer,info);
    strcat(buffer,local_name);
    n = write(sockfd,buffer,strlen(buffer));        //send the username to the server end
    if (n < 0) 
        error("ERROR writing to socket");
    printf("client username transmitted\n");



    bzero (buffer,256);
    /*
    multi-process begines here
    Child: listening to the server
    Parent: writing to the server
    The reason why I use multi threads here is to ensure listenning and reading runs at the same time
    */
    pid_t pid = fork();

    //writing @parent 
    if(pid!=0){
        /*
        this while loop will end once either side typed "quit" into the propt, same for the 
        while loop in the child process
        */
        while (strcmp(buffer,"quit\n")!=0){
            printf("%s->",local_name);  
            bzero(buffer,256);          //clean the buffer space
            fgets(buffer,256,stdin);    //get the info from the prompt
            n = write(sockfd,buffer,strlen(buffer));  //send info          
            if (n < 0) 
                error("ERROR writing to socket");   
        }
    }
    //reading @child
    else{    
        while (strcmp(buffer,"quit\n")!=0){ 
            bzero(buffer,255);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                error("ERROR reading from socket");     
            printf("\n %s-> %s",username,buffer);
        }
    }
    kill(pid,SIGKILL);  //kill the process
    close(sockfd);      //close the socket
    printf("Client Program ended\n");
     return 0;
   }

