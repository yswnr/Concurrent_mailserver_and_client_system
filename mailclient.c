#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define domain "server.smtp.com"
char mail_from[100];
char mail_to[100];  
#define MAX_LINE_LENGTH 100
void receive_response(int sockfd) {
    char response[1000];
    ssize_t num_bytes;
  // printf("Start\n");
     
       while((num_bytes = recv(sockfd, response, sizeof(response),0))>0)
      {
       // printf("%ld\n",num_bytes);
        response[num_bytes] = '\0';
       // printf("IN-LOOP\n");
        printf("%s", response);
        bzero(response,1000);
    }
    printf("Stop\n");
}
void handle_list_command(int sockfd) {
    
    char response[1024];
    ssize_t num_bytes;

    send(sockfd, "LIST\r\n", strlen("LIST\r\n"), 0);

    while ((num_bytes = recv(sockfd, response, sizeof(response)-1, 0)) > 0) {
        response[num_bytes] = '\0';
        printf("%s", response);
    }
}


void handle_retrieve_command(int sockfd, int mail_number) {
   
    char command[20];
    char buffer[1024];
    ssize_t num_bytes;

    sprintf(command, "RETR %d\r\n", mail_number);

    send(sockfd, command, strlen(command), 0);

    while ((num_bytes = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[num_bytes] = '\0';
        printf("%s", buffer);
    }
}


void handle_delete_command(int sockfd, int mail_number) {
  
    char command[20];
    char buffer[1024];
    ssize_t num_bytes;

    
    sprintf(command, "DELE %d\r\n", mail_number);

    send(sockfd, command, strlen(command), 0);

    while ((num_bytes = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[num_bytes] = '\0';
        printf("%s", buffer);
    }
}




char *Mail_Header(char *from, char *to, char *sub, char *content) {
    time_t t;
    time(&t);

    struct tm *tm_info;
    tm_info = localtime(&t);
    char date[26];
    strftime(date, sizeof(date), "%a %d %b %Y %H:%M:%S", tm_info);

    char *DATE_h = (char *)malloc(strlen(date) + 50); 
    char *sender = (char *)malloc(strlen(from) + 50); 
    char *recep = (char *)malloc(strlen(to) + 50);
    char *subject = (char *)malloc(strlen(sub) + 50); 
    char *content_a = (char *)malloc(strlen(content) + 50); 
    
    if (!DATE_h || !sender || !recep || !subject || !content_a) {
      
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    bzero(DATE_h,sizeof(DATE_h));
    bzero(sender,sizeof(sender));
    bzero(recep,sizeof(recep));
    bzero(subject,sizeof(subject));
    bzero(content_a,sizeof(content_a));
    
    sprintf(DATE_h, "RECEIVED: %s\r\n", date);
    sprintf(sender, "FROM: %s\r\n", from);
    sprintf(recep, "TO: %s\r\n", to);
    sprintf(subject, "Subject: %s\r\n", sub);
    sprintf(content_a, "%s\r\n", content);

    int header_length = strlen(sender) + strlen(recep) + strlen(subject) + strlen(DATE_h) + strlen(content_a);

    char *header = (char *)malloc((header_length + 1) * sizeof(char)); 

    if (!header) {
        
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    snprintf(header, header_length + 1, "%s%s%s%s%s", sender, recep, DATE_h, subject, content_a);

    free(DATE_h);
    free(sender);
    free(recep);
    free(subject);
    free(content_a);

    return header;
}


int main(int argc, char *argv[])
{
    int socket_id,n;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char cname[256];
    char buff[10240];
    char username[100], password[100];
     bzero(buff, 10240);
     int val;
       if(argc < 4)
		    {
			perror("\nPlease enter the hostname and port number.\n");
			exit(0);
		    }
   do
   {
       
       printf("********************\n");
       printf("Make sure the user.txt file exists and if the mail id is X@Y then the directory name should be X ");
       printf("\n********************\n");
       
       printf("1. Manage Mail : Shows the stored mails of the logged in user only\n");
       printf("2. Send Mail : Allows the user to send a mail\n");
       printf("3. Quit : Quits the program.\n");
       printf("Enter your choice : ");
       char command[250];
       char response[250];
       scanf("%d",&val);
	 if(val == 1)
	    {               
	                   
			     portno = atoi(argv[2]);
			     

			    socket_id = socket(AF_INET,SOCK_STREAM, 0);

			    if(socket_id < 0)
			    {
				perror("\nError occured while opening the socket!\n");
				exit(0);
			    }
			    server = gethostbyname(argv[1]);

			    if(server == NULL)
			    {
				perror("\nError: No such host found!\n");
				exit(0);
			    }

			    bzero((char *) &serv_addr, sizeof(serv_addr));
			    serv_addr.sin_family = AF_INET;
			    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
			    serv_addr.sin_port = htons(portno);
			   
			    //connect to the server
			    int connect_id;
			    connect_id = connect(socket_id,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
			    if(connect_id < 0)
			    {
				perror("Error occured while connecting to server...\n");
			    }
		           printf("Enter user name: ");
			    scanf("%s",username);
			    printf("Enter password: ");
			    scanf("%s",password);
			    
			    
			    // Send USER command
			    bzero(command,250);
			     strcpy(command,"USER ");
			     strcat(command,username);
			   // snprintf(command, "USER %s\r\n",username);
			    send(socket_id, command, strlen(command), 0);
			    // Receive response from server
			   // printf("%s\n",command); 
			    bzero(response,250);
			    recv(socket_id, response, sizeof(response), 0);
			    printf("%s", response);

			    // Send PASS command
			    //snprintf(command, "PASS %s\r\n",password);
			    bzero(response,250);
			    bzero(command,250);
			     strcpy(command,"PASS ");
			     strcat(command,password);
			    send(socket_id, command, strlen(command), 0);
			    // Receive response from server
			   // printf("%s\n",command);
			    recv(socket_id, response, sizeof(response), 0);
			    printf("%s", response);
			    
			    
			    printf("Commands\n");
			    printf("LIST: Listing all mails in the mailbox of the logged in user(It will list all the emails correclty but it's not exiting te while loop)\n");           
			    printf("RETR: The entered serial number mail is printed\n");                         
			    printf("DELE: Delete a particular mail\n");
			    printf("QUIT: Quit\n");
			    
			  

			    
			    do {
			        int mail_number;
				bzero(command,250);
				bzero(response,250);
				printf("Enter the command: ");
				scanf("%s",command);
				if(strcmp(command,"LIST")==0){
				  strcat(command,"\r\n");
				  send(socket_id, command, strlen(command), 0);
				  
                                   receive_response(socket_id);

				   }
								  
				
				else if(strcmp(command,"RETR")==0){
				  strcpy(command,"RETR ");
				  printf("Enter the mail number to be retrieved: ");
				  scanf("%d",&mail_number);
				  char temp[50];
				  sprintf(temp, "%d", mail_number);
				  strcat(command,temp);
				  strcat(command,"\r\n");
				  send(socket_id, command, strlen(command), 0);
				  bzero(response,250);
				  recv(socket_id, response, sizeof(response), 0);
			          printf("%s", response);
		
				}
				else if(strcmp(command,"DELE")==0){
				  strcpy(command,"DELE ");
				  printf("Enter the mail number to be deleted: ");
				  scanf("%d",&mail_number);
				  char temp[50];
				  sprintf(temp, "%d", mail_number);
				  strcat(command,temp);
				  strcat(command,"\r\n");
				  send(socket_id, command, strlen(command), 0);
				  bzero(response,250);
				  recv(socket_id, response, sizeof(response), 0);
			          printf("%s", response);
				}
			    }while (strcmp(command, "QUIT") != 0);
	       }        
		   
	   else if(val == 2)
           {
    
		     printf("SMTP PROTOCOLS\n");
		     printf("'HELO'      : Initiating conversation with the server via smtp protocols\n");
		     printf("'MAIL FROM' : Enter the from mail address in the format X@Y\n");
		     printf("'RCPT TO'   : Enter the to mail address in the format X@Y\n");
		     printf("'DATA'      : Enter the subject of only one line and content until only a '.' is entered in a new line\n");
		     printf("'QUIT'      : Once this is entered all the mails sent will be appended to the to address's mail box\n");
		   
		 
		    portno = atoi(argv[3]);

		    socket_id = socket(AF_INET,SOCK_STREAM, 0);

		    if(socket_id < 0)
		    {
			perror("\nError occured while opening the socket!\n");
			exit(0);
		    }
		    server = gethostbyname(argv[1]);

		    if(server == NULL)
		    {
			perror("\nError: No such host found!\n");
			exit(0);
		    }

		    bzero((char *) &serv_addr, sizeof(serv_addr));
		    serv_addr.sin_family = AF_INET;
		    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
		    serv_addr.sin_port = htons(portno);
		   
		    //connect to the server
		    int connect_id;
		    connect_id = connect(socket_id,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
		    if(connect_id < 0)
		    {
			perror("Error occured while connecting to server...\n");
		    }
	       
	  
		    do
		    {
			
			printf("Enter the command : ");
			intake: gets(cname);
		       
			char code[4];       
		       
			if(strcasecmp(cname,"HELO") == 0)
			{
			    bzero(buff,10240);
			    strcpy(buff,"HELO ");
			    strcat(buff, domain);
			    strcat(buff,"\r\n");
			    n = write(socket_id,buff,strlen(buff));
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			    printf("\nSENT : %s",buff);  
			    bzero(buff,10240);
			    n = read(socket_id,buff,10239);
			    if(n < 0)
			    {
				printf("\nError occured while reading from socket!\n");
			    }
			    printf("RECEIVED : %s\n",buff);    

			 
			    code[0] = buff[0];
			    code[1] = buff[1];
			    code[2] = buff[2];
			    code[3] = '\0';

			    if(strcmp(code,"250") == 0)
			    {
				printf("\nGo to next command...\n\n");
			    }
			    else{
				printf("\nError occured!\n\n");
			    }
			    fflush(stdin);
			}
			else if(strcasecmp(cname,"MAIL FROM") == 0)
			{
			    bzero(buff,10240);
			    printf("\nEnter Sender Email id : ");
			    scanf("%s",mail_from);
			    strcpy(buff,"MAIL FROM:<");
			    strcat(buff,mail_from);
			    strcat(buff,">");
			    strcat(buff,"\r\n");
			    n = write(socket_id,buff,strlen(buff));
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			    printf("\nSENT : %s",buff); 
			    bzero(buff,10240);
			    n = read(socket_id,buff,10239);
			    if(n < 0)
			    {
				printf("\nError occured while reading from socket!\n");
			    }
			    printf("RECEIVED : %s\n",buff); 

			    code[0] = buff[0];
			    code[1] = buff[1];
			    code[2] = buff[2];
			    code[3] = '\0';

			    if(strcmp(code,"250") == 0)
			    {
				printf("\nGo to next command...\n\n");
			    }
			    else{
				printf("\nError occured!\n\n");
			    }
			    fflush(stdin);
			}
			else if(strcasecmp(cname,"RCPT TO") == 0)
			{
			    bzero(buff,10240);
			    printf("\nEnter Recipient Email id : ");
			    scanf("%s",mail_to);
			    strcpy(buff,"RCPT TO:<");
			    strcat(buff,mail_to);
			    strcat(buff,">");
			    strcat(buff,"\r\n");
			    n = write(socket_id,buff,strlen(buff));
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			    printf("\nSENT : %s",buff);  
			    bzero(buff,10240);
			    n = read(socket_id,buff,10239);
			    if(n < 0)
			    {
				printf("\nError occured while reading from socket!\n");
			    }
			    printf("RECEIVED : %s\n",buff);  

			    code[0] = buff[0];
			    code[1] = buff[1];
			    code[2] = buff[2];
			    code[3] = '\0';

			    if(strcmp(code,"250") == 0)
			    {
				printf("\nGo to next command...\n\n");
			    }
			    else{
				printf("\nError occured!\n\n");
			    }
			    fflush(stdin);
			}
			else if(strcasecmp(cname,"DATA") == 0)          
			{
			    bzero(buff,10240);
			    strcpy(buff,"DATA");
			    strcat(buff,"\r\n");
			    n = write(socket_id,buff,strlen(buff));
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			    printf("\nSENT : %s",buff); 
			    bzero(buff,10240);
			    n = read(socket_id,buff,10239);
			    if(n < 0)
			    {
				printf("\nError occured while reading from socket!\n");
			    }
			    printf("RECEIVED : %s\n",buff);  
			 
			    code[0] = buff[0];
			    code[1] = buff[1];
			    code[2] = buff[2];
			    code[3] = '\0';

			    if(strcmp(code,"354") == 0)
			    {
				printf("\nReady to send header data!\n\n");
			    }
			    else{
				printf("\nError occured!\n\n");
			    }

			
			    char sub[150];
			    char content[10000];
			    printf("\nEnter Subject : ");
			    scanf("%[^\n]",sub);
			   char line[1000]; 
			    printf("\nEnter content : (Press '.' and Enter Key to end in a new line)\n");
		      
			    while (1) {
			      fgets(line, sizeof(line), stdin); 

			      size_t length = strlen(line);
			      if (length > 0 && line[length - 1] == '\n') {
				  line[length - 1] = '\0';
			}

			strcat(content, line);
			strcat(content, "\n"); 
			 if (strcmp(line, ".") == 0) {
			    
			    break;
			}
		     }
		     
		      printf("\nEntire message with newline characters:\n%s", content);

			    bzero(buff,10240);
			  // printf("mail from:%s\n",mail_from);
			   //printf("mail to:%s\n",mail_to);
			   //printf("subj:%s\n",sub);
			   printf("content:%s\n",content);
			    strcpy(buff, Mail_Header(mail_from,mail_to,sub,content)); 
			    
			    n = write(socket_id,buff,strlen(buff));                    
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			    printf("\n =======| Mail |=======\n%s\n",buff);  
			   
			    bzero(buff,10240);
			    strcpy(buff,".\r\n");
			    n = write(socket_id,buff,strlen(buff));                    
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			   
			    bzero(buff,10240);
			    n = read(socket_id,buff,10239);  
			    if(n < 0)
			    {
				printf("\nError occured while reading from socket!\n");
			    }
			    printf("RECEIVED : %s\n",buff);    

			    code[0] = buff[0];
			    code[1] = buff[1];
			    code[2] = buff[2];
			    code[3] = '\0';

			    if(strcmp(code,"250") == 0)
			    {
				printf("\nGo to next command...\n\n");
			    }
			    else{
				printf("\nError occured!\n\n");
			    }
			    fflush(stdin);  
			     bzero(content,10000);                                
			}
		       
			else if(strcasecmp(cname,"QUIT") == 0)
			{
			    bzero(buff,10240);
			    strcpy(buff,"QUIT");
			    strcat(buff,"\r\n");
			    n = write(socket_id,buff,strlen(buff));
			    if(n < 0)
			    {
				printf("\nError occured while writing to socket!\n");
			    }
			    printf("\nSENT : %s",buff);  
			    bzero(buff,10240);
			    n = read(socket_id,buff,10239);
			    if(n < 0)
			    {
				printf("\nError occured while reading from socket!\n");
			    }
			    printf("RECEIVED : %s\n",buff);   

			    code[0] = buff[0];
			    code[1] = buff[1];
			    code[2] = buff[2];
			    code[3] = '\0';

			    if(strcmp(code,"221") == 0)
			    {
				printf("\nConnection closed successfully with SMTP Server!\n\n");
			    }
			    else{
				printf("\nError occured!\n\n");
			    }
			    fflush(stdin);
			}
			else
			{
			strcpy(cname,"");
			goto intake;
			}
		       
		    }while(strcmp(cname,"QUIT") != 0); 
	    
           }
}while(val != 3);  
   
    close(socket_id);
    return 0;
}


