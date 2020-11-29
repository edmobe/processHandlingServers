#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <sys/types.h>

#include <unistd.h>   //file close
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>    //socket close     

// For image processing
// #include "macros.h"
// #include "sobel.h"
// #include "file_operations.h"

#define MAX 80
#define TRUE   1  
#define FALSE  0
#define NO_INPUT 2
#define SA struct sockaddr 
#define NET_BUF_SIZE 32 
#define ENOUGH ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)

union LongChar {
    char myByte[4];
    long mylong;
};

// int readcolor(char *image);

// Function designed for file(picture) transfer between client and server. 
void func(int sockfd);

// Read
int buff_read(int sockfd, char buff[MAX]);

int receive_image(int sockfd);

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Driver function 
int main(int argc , char *argv[]) 
{ 
	int sockfd, connfd, len, nBytes; 
	struct sockaddr_in servaddr, cli;
    struct in_addr **addr_list;
    // int addrlen = sizeof(servaddr);
    // char net_buf[NET_BUF_SIZE]; 

    if (argc != NO_INPUT) {
        printf("ERROR: num of param #%d, param needed #%d\n", argc, NO_INPUT);
        exit(1);
    }

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("ERROR: Socket creation failed...\n"); 
		exit(1); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 


    // assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port =htons(atoi(argv[1]));

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("ERROR: Socket bind failed...\n"); 
		exit(1); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 10)) != 0) { 
		printf("ERROR: Listen failed...\n"); 
		exit(1); 
	} 

    len = sizeof(cli); 
    
    // char *client_ip = inet_ntoa(cli.sin_addr);
    
    // func(connfd);

    char buff[MAX+1];
    char *name;
    char *msg_rcv = "Image processed";
	int n;
    FILE *image;
    int first = 0;
    int counter = 0;
    ssize_t nread;

    int pid;
    // while (1) {
    //      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    //      if (newsockfd < 0)
    //             error("ERROR on accept");
    //      //fork new process
    //      pid = fork();
    //      if (pid < 0) {
    //           error("ERROR in new process creation");
    //      }
    //      if (pid == 0) {
    //         //child process
    //         close(sockfd);
    //         //do whatever you want
    //         bzero(buffer, 256);
    //         n = read(newsockfd, buffer, 255);
    //         if (n < 0)
    //             error("ERROR reading from socket");
    //         printf("Here is the message: %s\n", buffer);
    //         n = write(newsockfd, "I got your message", 18);
    //         if (n < 0)
    //              error("ERROR writing to socket");
    //         close(newsockfd);
    //       } else {
    //          //parent process
    //          close(newsockfd);
    //       }
    // }

    while (TRUE)
    {
        printf("Server listening..\n"); 
        // Accept the data packet from client and verification 
        connfd = accept(sockfd, (SA*)&cli, &len); 
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n");

        //fork new process
        pid = fork();
        if (pid < 0) {
            error("ERROR in new process creation");
        }
        if (pid == 0) {

            //Somebody connected , get his details and print  
            getpeername(connfd, (SA*)&cli,(socklen_t*)&len); 

            printf("Host connected , ip %s , port %d \n" , 
                inet_ntoa(cli.sin_addr) , ntohs(cli.sin_port));

            while (TRUE) {

                char temp_dir[100] = "../imgs/";

                //Read Picture Size
                printf("Reading Picture Size\n");
                bzero(buff, sizeof(buff));

                long size;
                if (read(connfd, &size, sizeof(long)) == 0) {
                    break;
                }

                // if ((strncmp(buff, "exit", 5)) == 0) { 
                //     printf("Client Exit...\n"); 
                //     exit(0); 
                // }

                // union LongChar myUnion;
                // myUnion.myByte[0] = buff[0];
                // myUnion.myByte[1] = buff[1];
                // myUnion.myByte[2] = buff[2];
                // myUnion.myByte[3] = buff[3];


                // long size = myUnion.mylong;
                printf("Size: %ld\n", size);


                //Read Picture Name
                printf("Reading Picture Name\n");
                bzero(buff, sizeof(buff));
                
                if (read(connfd, buff, MAX) == 0) {
                    break;
                }

                char *token = strtok(buff, "/");

                do {   
                    name = token;
                    token = strtok(NULL, "/");
                } while (token != NULL);

                printf("Name: %s\n", name);

                // break;
                char counter_str[4];
                sprintf(counter_str, "%d", counter);
                strcat(temp_dir, counter_str);
                strcat(temp_dir, name);
                strcat(temp_dir,"\0");
                printf("%s\n",temp_dir);
                
                /*
                //Read Picture Byte Array
                printf("Reading Picture Byte Array\n");
                char p_array[size+1];
                if((nread = read(connfd, p_array, size)) == 0) {
                    fclose(image);
                    break;
                }
                if (counter < 100) {
                    image = fopen(temp_dir, "w");

                    if (image == NULL) {
                        printf("Image didn't open\n");
                        exit(0);
                    }


                    fwrite(p_array, 1, size, image); 
                    fclose(image);
                }*/
                // long counter = 0L;
                // while(counter < size) {
                //     bzero(buff, sizeof(buff));
                //     ssize_t nread = read(connfd, buff, 256);
                //     fwrite(buff, 1, nread, image);
                //     counter += sizeof(buff);
                // }

                printf("Responding to client\n");
                bzero(buff, sizeof(buff)); 
                write(connfd, msg_rcv, MAX);
                
                if (counter < 100) {
                    counter ++;
                }

            }
            
            close(connfd);
            close(sockfd);
            exit(0);
        } else {
            //parent process
            close(connfd);
        }
    }

    close(sockfd);

    return 0;
}
// int main(int argc, char *argv[]) {
//     int sockfd, newsockfd, portno;
//     socklen_t clilen;
//     char buffer[256];
//     struct sockaddr_in serv_addr, cli_addr;
//     int n;
//     if (argc < 2) {
//          fprintf(stderr, "ERROR, no port provided\n");
//          exit(1);
//     }
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0)
//          error("ERROR opening socket");
//     bzero((char *) &serv_addr, sizeof(serv_addr));
//     portno = atoi(argv[1]);
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_addr.s_addr = INADDR_ANY;
//     serv_addr.sin_port = htons(portno);
//     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
//         error("ERROR on binding");
//     listen(sockfd, 5);
//     clilen = sizeof(cli_addr);
//     //Below code is modified to handle multiple clients using fork
//     //------------------------------------------------------------------
//     int pid;
//     while (1) {
//          newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
//          if (newsockfd < 0)
//                 error("ERROR on accept");
//          //fork new process
//          pid = fork();
//          if (pid < 0) {
//               error("ERROR in new process creation");
//          }
//          if (pid == 0) {
//             //child process
//             close(sockfd);
//             //do whatever you want
//             bzero(buffer, 256);
//             n = read(newsockfd, buffer, 255);
//             if (n < 0)
//                 error("ERROR reading from socket");
//             printf("Here is the message: %s\n", buffer);
//             n = write(newsockfd, "I got your message", 18);
//             if (n < 0)
//                  error("ERROR writing to socket");
//             close(newsockfd);
//           } else {
//              //parent process
//              close(newsockfd);
//           }
//     }
//     //-------------------------------------------------------------------
//    return 0;
// }