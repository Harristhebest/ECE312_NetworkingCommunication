   // From page 58, Figure 2.14 of Stallings
   
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
     int sockfd, portno, n;

     struct sockaddr_in serv_addr;
     struct hostent *server;
     char buffer[256];
     char username[256];
    char local_name[256]="";
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
     bzero(local_name,256);
     fgets(local_name,256,stdin);
    local_name[strcspn(local_name, "\n")] = '\0';



    printf("ip address: %s\n", argv[1]);





    bzero(username,255);
    n = read(sockfd,username,255);
    if (n < 0) error("ERROR reading from socket");
    printf("server username recieved\n");     

    n = write(sockfd,argv[1],strlen(argv[1]));  //ip addr
    if (n < 0) 
        error("ERROR writing to socket");
    printf("client ip transmitted\n");

    n = write(sockfd,local_name,strlen(local_name));        //username
    if (n < 0) 
        error("ERROR writing to socket");
    printf("client username transmitted\n");



    bzero (buffer,256);
    pid_t pid = fork();
    //writing
    if(pid!=0){
        printf("now writting starts\n");
        while (strcmp(buffer,"quit\n")!=0){
            printf("%s->",local_name);
            bzero(buffer,256);
            fgets(buffer,256,stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            n = write(sockfd,buffer,strlen(buffer));            
            if (n < 0) 
                error("ERROR writing to socket");   
        }
    }

    else{    
        
        while (strcmp(buffer,"quit\n")!=0){
            bzero(buffer,255);
            n = read(sockfd,buffer,255);
            if (n < 0) 
                error("ERROR reading from socket");     
            printf("\n %s-> %s",username,buffer);
        }
    }
    kill(pid,SIGKILL);
    close(sockfd);
    printf("Client Program ended\n");
     return 0;
   }

