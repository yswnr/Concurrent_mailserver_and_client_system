#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define domain "iitkgp.edu"
#define USER_FILE "user.txt"

int main(int argc, char *argv[])
{
   
    int sockfd, newsockfd, n, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buff[10240];          
    char command[50];          
    char username[1000];
    char password[1000];
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd < 0)
    {
        perror("\nError occured while creating the socket!\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
         perror("\nSocket binding failed!\n");
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
    {
    perror("\nError occured while accepting on socket!\n");
    }

   
    do
    {
        char mailbox[1000];
        bzero(buff,10240);
        n = read(newsockfd,buff,10239);
        if (n < 0)
        {
            printf("\nError occured while reading from socket!\n");
            break;
        }
        else
        {
            buff[n] = '\0';
            if(strstr(buff,"HELO")!=NULL)
            {
                printf("RECEIVED : %s",buff);
                bzero(buff,10240);
                strcpy(buff,"250 Hello ");
                strcat(buff, domain);
                printf("SENT : %s\n\n",buff);
                n = write(newsockfd,buff,strlen(buff));
                if (n<0) {  perror("Error occured while writing to socket!"); }
            }
            else if(strstr(buff,"MAIL FROM")!=NULL)
            {
                printf("RECEIVED : %s",buff);
                bzero(buff,10240);
                strcpy(buff,"250 OK");
                printf("SENT : %s\n\n",buff);
                n = write(newsockfd,buff,strlen(buff));
                if (n<0) {  perror("Error occured while writing to socket!"); }
            }
            else if(strstr(buff,"RCPT TO")!=NULL)
            {
                printf("RECEIVED : %s",buff);
          
                char mailbox[100]; 

		  
		    char *start = strchr(buff, '<');
		    char *end = strchr(buff, '@');

		    
		    if (start != NULL && end != NULL && start < end) {
			
			size_t length = end - (start + 1);

			strncpy(mailbox, start + 1, length);
			mailbox[length] = '\0'; 
		
		    } 
		
                bzero(buff,10240);
                strcpy(buff,"250 OK");
                printf("SENT : %s\n\n",buff);
                n = write(newsockfd,buff,strlen(buff));
                if (n<0) {  perror("Error occured while writing to socket!"); }
            }
            else if(strstr(buff,"DATA")!=NULL)
            {
                printf("RECEIVED : %s",buff);
                bzero(buff,10240);
                strcpy(buff,"354 Send message content; end with <CRLF>.<CRLF>");
                printf("SENT : %s\n\n",buff);
                n = write(newsockfd,buff,strlen(buff));
                if (n < 0) {  perror("Error occured while writing to socket!"); }

                bzero(buff,10240);
                n = read(newsockfd,buff,10239);
                if (n < 0){ printf("\nError occured while reading from socket!\n"); break; }
                char filepath[100]; 
               
		    sprintf(filepath, "%s/%s", mailbox, "mailbox.txt");

		    FILE *file = fopen(filepath, "a"); 
		    if (file == NULL) {
			perror("Error opening file for appending");
			return 1;  
		    }

		 
		    fprintf(file, "-------------Received Email--------------\n%s\n", buff);
               printf("\n-------------Received Email----------\n%s\n",buff);
               printf("-------------------------------------------------------\n");
                                       
                bzero(buff,10240);
                n = read(newsockfd,buff,10239);
                if (n < 0){ printf("\nError occured while reading from socket!\n"); break; }
           
                if(strstr(buff,".")!=NULL){
                    printf("RECEIVED : %s",buff);
                    bzero(buff,10240);
                 
                    strcpy(buff,"250 OK, message accepted for delivery.");
                    printf("SENT : %s\n\n",buff);
                    n = write(newsockfd,buff,strlen(buff));
                    if (n < 0) {  perror("Error occured while writing to socket!"); }
                }
            }
           
            else if(strstr(buff,"QUIT") != NULL)
            {
                break;
            }  
        }
    }while(strcmp(buff,"QUIT") != 0);

    printf("RECEIVED : %s",buff);
    bzero(buff,10240);
    strcpy(buff,"221 Bye");
    printf("SENT : %s\n\n",buff);
    n = write(newsockfd,buff,strlen(buff));
    if (n<0) {  perror("Error occured while writing to socket!"); }
    printf("\nConnection closed successfully with the client!\n\n");
    close(sockfd);
    return 0;
}

