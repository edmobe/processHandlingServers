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
#include "macros.h"
#include "sobel.h"
#include "file_operations.h"

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

// Driver function 
int main(int argc , char *argv[]) 
{ 
	int sockfd, connfd, len, nBytes;
    int width, height; 
	struct sockaddr_in servaddr, cli;
    struct in_addr **addr_list;
    // int addrlen = sizeof(servaddr);
    // char net_buf[NET_BUF_SIZE]; 

    byte *rgb, *gray, *sobel_h_res, *sobel_v_res, *contour_img;
    
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
    char *msg_rcv1 = "Metadata processed";
    char *msg_rcv2 = "Image processed";
	int n;
    FILE *image, *fp;
    int first = 0;
    int counter = 0;
    ssize_t nread;

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

        //Somebody connected , get his details and print  
        getpeername(connfd, (SA*)&cli,(socklen_t*)&len); 

        printf("Host connected , ip %s , port %d \n" , 
            inet_ntoa(cli.sin_addr) , ntohs(cli.sin_port));
        
        int counter1 = 0;
        while (counter1<1) {

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

            // First response message
            printf("Responding 1 to client\n");
            bzero(buff, sizeof(buff)); 
            write(connfd, msg_rcv1, MAX);
            
            //Read Picture Byte Array
            printf("Reading Picture Byte Array\n");
            char p_array[size + 1];
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

                // Read file to rgb and get size
                readFile(temp_dir, &rgb, size);

                if (size == 786432L) {
                    width = 512;
                    height = 512;
                } else if (size == 6220800L) {
                    width = 1920;
                    height = 1080;
                } else {
                    width = 512;
                    height = 512;
                }

                int gray_size = sobelFilter(p_array, &gray, &sobel_h_res, &sobel_v_res, &contour_img, width, height);

                // Write sobel img to a file
                strcat(temp_dir,".gray\0");
                writeFile(temp_dir, contour_img, gray_size);

                char path[100] = "convert -size ";

                char size_str[7];
                sprintf(size_str, "%dx%d ", width, height);
                strcat(path, size_str);
                strcat(path, "-depth 8 ");
                
                strcat(path, temp_dir);
                strcat(temp_dir,".png\0");
                strcat(path, " ");
                strcat(path, temp_dir);
                /* Open the command for reading. */
                
                fp = popen(path, "r");
                if (fp == NULL) {
                    printf("ERROR: Failed to run command\n" );
                    exit(1);
                }

                // close 
                pclose(fp);

                counter ++;

            } else {
                int gray_size = sobelFilter(p_array, &gray, &sobel_h_res, &sobel_v_res, &contour_img, width, height);
            }
            // long counter = 0L;
            // while(counter < size) {
            //     bzero(buff, sizeof(buff));
            //     ssize_t nread = read(connfd, buff, 256);
            //     fwrite(buff, 1, nread, image);
            //     counter += sizeof(buff);
            // }

            printf("Responding 2 to client\n");
            bzero(buff, sizeof(buff));
            write(connfd, msg_rcv2, MAX);
            
            counter1 ++;
        }

        close(connfd);
    }

    close(sockfd);

    return 0;
}