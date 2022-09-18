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
        char username[256]; //other end 
        char local_name[256];
        pid_t pid = -1;
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
        if (newsockfd < 0) 
             error("ERROR on accept");


        printf("please enter your user name:");
        bzero(local_name,strlen(local_name));
        fgets(local_name,256,stdin);
        local_name[strcspn(local_name,"\n")] = '\0';


        n = write(newsockfd,local_name,255);
        if (n < 0) error("ERROR writing to socket");
        printf("usernamer transmmited to the client\n");

        printf("now waiting for the other end to provide the ip addr...\n");    



    





        bzero(username,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("username recieved from client\n");



        bzero(buffer,256);
        n = read(newsockfd,username,255);
        if (n < 0) error("ERROR reading from socket");
        printf("id addr recieved from client\n");

        printf("Connection established with: %s , username : %s\n",buffer,username);

        pid = fork();
        // printf("multi thread creating... pid = %d\n",pid);
        


        //write messages 
        if(pid==0){
            // printf("writting successfully initiated\n");
            while (strcmp(buffer,"quit\n")!=0){
                printf("\n%s->",local_name);
                bzero(buffer,256);
                fgets(buffer,255,stdin);
                username[strcspn(username, "\n")] = '\0';
                n = write(newsockfd,buffer,strlen(buffer));
                if (n < 0) 
                    error("ERROR writing to socket");  
            }
        } 
        //read messages
        else{
            // printf("reading successfully initiated\n");
            while (strcmp(buffer,"quit\n")!=0){
                printf("\n%s->",local_name);

                bzero(buffer,256);
                n = read(newsockfd,buffer,255);
                if (n < 0) error("ERROR reading from socket");
                printf("\n%s -> %s",username, buffer);
            }
        }
        kill(pid, SIGKILL);
        printf("program terminated");
        close(newsockfd);
        return 0; 
   }

    