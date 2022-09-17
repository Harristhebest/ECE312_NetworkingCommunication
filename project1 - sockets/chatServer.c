// From page 57, Figure 2.13 of Stallings

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

        int sockfd, newsockfd, portno, clilen;
        char buffer[256];
        char username[256];

        char quit_msg [256] = "quit";
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
        }
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
           error("ERROR opening socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(portno);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0) 
                 error("ERROR on binding");
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        printf("waiting for connection...\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        printf("please enter your user name:");
        bzero(username,256);
        fgets(username,256,stdin);
        username[strcspn(username,"\n")] = 0;     
        if (newsockfd < 0) 
             error("ERROR on accept");
        int pid = fork();
        printf("pid:%d\n",pid);
        if(pid!=0){
            printf("connection established\n");

            while (strcmp(buffer,quit_msg)!=0){
                printf("<%s>",username);
                bzero(buffer,256);
                fgets(buffer,255,stdin);
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0) 
                    error("ERROR writing to socket");  
                buffer[strcspn(buffer, "\n")] = '\0';
            }
            kill(pid, SIGTERM);
        }     

        else{
            while (strcmp(buffer,quit_msg)!=0){
                bzero(buffer,255);
                char client [256] = "<client>";
                n = read(newsockfd,buffer,255);
                if (n < 0) error("ERROR reading from socket");
                strcat(client,buffer);
                printf("%s\n",client);
                bzero(client,255);
                buffer[strcspn(buffer, "\n")] = '\0';

            }
        }
        printf("Server Program ended   pid id:%d\n",pid);
        close(newsockfd);
        return 0; 
   }
