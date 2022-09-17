   // From page 58, Figure 2.14 of Stallings
   
   #include <stdio.h>
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netdb.h> 
   
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
     int sockfd, portno, n;

     struct sockaddr_in serv_addr;
     struct hostent *server;
     char buffer[256];
     char username[256]="";
     char quit_msg [256] = "quit";

     if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
     }
     portno = atoi(argv[2]);
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
         error("ERROR opening socket");
     server = gethostbyname(argv[1]);
     if (server == NULL) {
         fprintf(stderr,"ERROR, no such host\n");
         exit(0);
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
     serv_addr.sin_port = htons(portno);
     if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
         error("ERROR connecting");
     printf("please enter your username:");
     bzero(username,256);
     fgets(username,256,stdin);
    username[strcspn(username, "\n")] = 0;

    printf("connection success...\n");
    printf("ip address: %s\n", argv[1]);

    if(fork()==0){
        
        while (strcmp(buffer,"quit\n")!=0){
            printf("now writting starts");
            strcpy(buffer, "");
            printf("<%s>: ",username);
            fgets(buffer,256,stdin);
            bzero(buffer,256);
            n = write(sockfd,buffer,strlen(buffer));

            if (n < 0) 
                error("ERROR writing to socket");   
        }
    }

    else{    
        while (strcmp(buffer,quit_msg)!=0){
            bzero(buffer,255);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                error("ERROR reading from socket");
            printf("\n<server>: %s",buffer);
            buffer[strcspn(buffer, "\n")] = '\0';
            
        }
    }
    close(sockfd);
    printf("Client Program ended\n");
     return 0;
   }

