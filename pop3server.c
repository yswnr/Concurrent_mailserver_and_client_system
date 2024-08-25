
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_USERNAME_LENGTH 200
#define MAX_PASSWORD_LENGTH 100
#define USER_FILE "user.txt"

#define MAX_LINE_LENGTH 100
void send_response(int sockfd, const char *response) {
  //  printf("Entered\n");
   // printf("%s\n",response);
    send(sockfd, response, strlen(response), 0);
 //   printf("Exited\n");
}

void send_mails(int sockfd,const char *username)
{   
    char filename[100]; // Adjust the size as needed
    sprintf(filename, "%s/mailbox.txt", username);

    //FILE *file = fopen(filename, "r");
    int source = open(filename, O_RDONLY);
     char buffer[1000];
     int bytesRead;
    while ((bytesRead = read(source, buffer, sizeof(buffer))) > 0) {
        send(sockfd, buffer, bytesRead, 0);
    }
 printf("END\n");
}

void list_mails(int sockfd, const char *username) {
    char filename[100]; // Adjust the size as needed
    sprintf(filename, "%s/mailbox.txt", username);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening mailbox file");
        exit(EXIT_FAILURE);
    }
    
   // printf("Entered LIst mails\n");

    char line[MAX_LINE_LENGTH];
    int mail_number = 1;
    int in_email = 0; 

    send_response(sockfd, "+OK Listing emails\r\n");

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "-------------Received Email--------------")) {
            in_email = 1; 
            char response[MAX_LINE_LENGTH];
            sprintf(response, "Mail %d:\r\n", mail_number++);
            send_response(sockfd, response);
            continue;
        }

        if (in_email) {
            if (strcmp(line, ".\n") == 0) {
                send_response(sockfd, ".\n");
                in_email = 0; 
                send_response(sockfd, "\r\n");
                continue;
            }

            send_response(sockfd, line);
        }
    }

    fclose(file);
   //send_response(sockfd, ".\r\n");
}


void delete_mail(const char *username, int serial_number,int sockfd) {
    char filename[100]; 
    sprintf(filename, "%s/mailbox.txt", username);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening mailbox file");
        exit(EXIT_FAILURE);
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        fclose(file);
        perror("Error creating temporary file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int mail_number = 1;
    int in_email = 0; 

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "-------------Received Email--------------")) {
            if (mail_number != serial_number) {
                fprintf(temp_file, "%s", line); 
            }
            in_email = 1; // Start of a new email
            continue;
        }

        if (in_email) {
            if (strcmp(line, ".\n") == 0) {
             if(mail_number != serial_number)
             {
                fprintf(temp_file, "%s", line);
             }
                in_email = 0; // End of email
                mail_number++;
            } else if (mail_number != serial_number) {
                fprintf(temp_file, "%s", line); // Write the email content to the temporary file
            }
        }

        /*if (in_email && strstr(line, "-------------Received Email--------------")) {
            mail_number++;
        }*/
    }

    fclose(file);
    fclose(temp_file);

    if (rename("temp.txt", filename) != 0) {
        perror("Error replacing mailbox file");
        exit(EXIT_FAILURE);
    }
    send_response(sockfd, "+OK Deleted successfully\r\n");
}





void print_mail(const char *username, int serial_number,char* buff) {
    char filename[100]; // Adjust the size as needed
    sprintf(filename, "%s/mailbox.txt", username);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening mailbox file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int mail_number = 1;
    int in_email = 0; 
    int found_mail = 0; 

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "-------------Received Email--------------")) {
            in_email = 1; // Start of a new email
            if (mail_number == serial_number) {
                found_mail = 1;
            }
            continue;
        }

        if (in_email) {
            if (strcmp(line, ".\n") == 0) {
                in_email = 0; // End of email
                 mail_number++;
                if (found_mail) {
                    strcat(buff,"\n");
                    break; 
                }
                continue;
            }

            if (found_mail) {
                strcat(buff, line); // Print the line of the email
            }
        }

     /*   if (in_email && strstr(line, "-------------Received Email--------------")) {
            mail_number++;
        }*/
    }

    fclose(file);

    if (!found_mail) {
        printf("Mail with serial number %d not found.\n", serial_number);
    }
}
/*void list_mails(const char *username) {
    char filename[100]; // Adjust the size as needed
    sprintf(filename, "%s/mailbox.txt", username);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening mailbox file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int mail_number = 1;
    int in_email = 0; // Flag to indicate if we are inside an email

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "-------------Received Email--------------")) {
            in_email = 1; // Start of a new email
            printf("Sl.No.%d: ", mail_number++);
            continue;
        }

        if (in_email) {
            if (strcmp(line, ".\n") == 0) {
                in_email = 0; // End of email
                printf("\n");
                continue;
            }

      
            char *token;
            if ((token = strstr(line, "FROM:")) != NULL) {
               // token += strlen("FROM:");
                printf("%s", token);
            } else if ((token = strstr(line, "RECEIVED:")) != NULL) {
                //token += strlen("RECEIVED:");
                printf("%s", token);
            } else if ((token = strstr(line, "Subject:")) != NULL) {
                //token += strlen("Subject:");
                printf("%s", token);
            }
        }
    }

    fclose(file);
}*/
int validate_credentials(const char *username, const char *password) {
    // Open user file for reading
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        perror("Error opening user file");
        exit(EXIT_FAILURE);
    }

   
    char line[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2]; 
    while (fgets(line, sizeof(line), file)) {
        // Extract username and password from the line
        char file_username[MAX_USERNAME_LENGTH];
        char file_password[MAX_PASSWORD_LENGTH];
        if (sscanf(line, "%s %s", file_username, file_password) != 2) {
            fprintf(stderr, "Invalid line in user file: %s\n", line);
            continue;
        }

        // Compare with provided credentials
        if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
            fclose(file);
            return 1; // Credentials match
        }
    }

    fclose(file);
    return 0; // Credentials do not match
}


void handle_user_command(int sockfd, const char *username) {
    // Print received username
    printf("Received username: %s\n", username);
    send_response(sockfd, "+OK User accepted\r\n");
}

void handle_pass_command(int sockfd, const char *username,const char *password) {
    // Print received password
    printf("Received password: %s\n", password);
    if (validate_credentials(username, password)) {
        send_response(sockfd, "Login Successful\r\n");
    } else {
        send_response(sockfd, "-ERR Invalid username or password\r\n");
        
        close(sockfd);
    }
}

int main(int argc, char *argv[]) {
    int server_sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char command[250];
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    
    int SERVER_PORT = atoi(argv[1]);
    // Create socket
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind socket to address and port
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sockfd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("POP3 server listening on port %d\n", SERVER_PORT);
    
    addr_size = sizeof(client_addr);
    client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_size);
    
    if (client_sockfd == -1) {
            perror("Accept failed");
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
         bzero(command,250);
               recv(client_sockfd, command, sizeof(command), 0);

          //  if (strncmp(command, "USER", 4) == 0) {
                sscanf(command, "USER %s", username);
                //aprintf("%s %s\n",username,command);
                handle_user_command(client_sockfd, username);
             
           // if (strncmp(command, "PASS", 4) == 0) {
               bzero(command,250);
               recv(client_sockfd, command, sizeof(command), 0);
                sscanf(command, "PASS %s", password);
                printf("%s\n",password);
                handle_pass_command(client_sockfd, username,password);
             
         do
         {
             
               
             char mail_number[50];
        
            char buff[250];
           bzero(command,250);
           bzero(buff,250);
           bzero(mail_number,50);
            recv(client_sockfd, command, sizeof(command), 0);
            printf("Command:%s\n",command);
            if(strstr(command,"LIST")!=NULL){
             printf("if block\n");
             // send_mails(client_sockfd,username);
             list_mails(client_sockfd,username);
              printf("Exited list\n");
            }
            else if(strstr(command,"RETR")!=NULL){
             sscanf(command, "RETR %s", mail_number);
             print_mail(username,atoi(mail_number),buff);
           //  delete_mail(username,2);
             send(client_sockfd,buff,sizeof(buff),0);
             }
             else if(strstr(command,"DELE")!=NULL){
              sscanf(command, "DELE %s", mail_number);
              delete_mail(username,atoi(mail_number),client_sockfd);
             }
         }while(strcmp(command,"QUIT")!=0);
 return 0;
 }
