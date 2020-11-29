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
// #define PORT 9090
#define TRUE   1  
#define FALSE  0
#define NO_INPUT 2
#define R 2
#define G 3
#define B 4
#define SA struct sockaddr 
#define NET_BUF_SIZE 32 

// int readcolor(char *image);

// Function designed for file(picture) transfer between client and server. 
void func(int sockfd);

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
	if ((listen(sockfd, 5)) != 0) { 
		printf("ERROR: Listen failed...\n"); 
		exit(1); 
	} 

    while (TRUE)
    {
        printf("Server listening..\n"); 
    
        len = sizeof(cli); 

        // Accept the data packet from client and verification 
        connfd = accept(sockfd, (SA*)&cli, &len); 
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
            printf("server acccept the client...\n"); 

        // Last project ips acceptance configuration
        // 
        // const char* trusted = getenv("TRUSTED_HOSTS");
        // const char* not_trusted = getenv("NOT_TRUSTED_HOSTS");

        // printf("Are the ips ok?\n");
        // if (trusted == NULL || trusted == "" || not_trusted == NULL) {
        //     printf("Didn't get the ips correctly.\n");
        //     exit(1);
        // }

        // printf("%s\n", trusted);
        char *client_ip = inet_ntoa(cli.sin_addr);

        // char *istrusted = strstr(trusted, client_ip);
        // char *isnottrusted = strstr(not_trusted, client_ip);

        // printf("%s\n", client_ip);

        // if (istrusted != NULL && isnottrusted == NULL) {
        //     func(connfd, 0);
        // } else if (istrusted == NULL && isnottrusted != NULL) {
        //     func(connfd, 1);
        // } else {
        //     printf("Rejected: ip %s\n", client_ip);
        // }
        func(connfd);



    }
    close(sockfd);
}

// int readcolor(char *image) {
  
//     FILE *fp;
//     char path[100] = "convert ";
//     char msg[20];
//     strcat(path, image);
//     strcat(path, " -scale 1x1\\! -format '%[pixel:s]\n' info:-");

//     /* Open the command for reading. */
//     printf("%s\n", path);
//     fp = popen(path, "r");
//     if (fp == NULL) {
//         printf("Failed to run command\n" );
//         exit(1);
//     }

//     /* Read the output a line at a time - output it. */
//     // while (fgets(msg, sizeof(msg), fp) != NULL) {
//     //     printf("%s\n", msg);
//     // }

//     if (fgets(msg, sizeof(msg), fp) == NULL) {
//         printf("Bad reading in cmd\n");
//         exit(1);
//     }

//     /* close */
//     pclose(fp);

//     // Function to convert a string to
//     // integer array
//     int arr[3]; 
  
//     int j = 0, i, sum = 0; 
  
//     // Traverse the string 
//     for (i = 0; msg[i] != '\0'; i++) { 
        
//         if (j >= 3) {
//             break;
//         }

//         if (i < 5)
//             continue;
//         // if str[i] is ', ' then split 
//         else if (msg[i] == ',' || msg[i] == ')') {
//             arr[j] = sum;
//             sum = 0;
//             j++;
//         }
//         // ignore others char
//         else if (msg[i] == ' ' || msg[i] == '(' || msg[i] == 'b' || msg[i] == '.')
//             continue;
//         else
//             // subtract str[i] by 48 to convert it to int 
//             // Generate number by multiplying 10 and adding 
//             // (int)(str[i]) 
//             sum = sum * 10 + (msg[i] - '0'); 
//     } 
    
//     // printf("R:%d G:%d B:%d\n", arr[0], arr[1], arr[2]);

//     if (arr[0] > arr[1] && arr[0] > arr[2]) {
//         return R;
//     } else if (arr[1] > arr[2]) {
//         return G;
//     } else {
//         return B;
//     }

//     // return 0;
// }

void func(int sockfd)
{ 
	char buff[MAX];
    char *name;
	int n;
    FILE *image;

    // char n_dir[100] = "/volume/Not_trusted/";
    // char r_dir[100] = "/volume/R/";
    // char g_dir[100] = "/volume/G/";
    // char b_dir[100] = "/volume/B/";
    char temp_dir[100] = "../imgs/";
    char *msg_rcv = "Image processed!\0";

    //Read Picture Size
    printf("Reading Picture Size\n");
    int size;
    // bzero(size, sizeof(size));
    read(sockfd, &size, sizeof(int));

    //Read Picture Name
    printf("Reading Picture Name\n");
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));

    char *token = strtok(buff, "/");

    do {   
        name = token;
        token = strtok(NULL, "/");
    } while (token != NULL);

    //Read Picture Byte Array
    printf("Reading Picture Byte Array\n");
    char p_array[size];
    bzero(p_array, sizeof(p_array));
    read(sockfd, p_array, size);

    // if (type) {
    //     //Convert it Back into Picture
    //     printf("Converting Byte Array to Picture Not Trusted\n");
    //     strcat(n_dir, name);
    //     strcat(n_dir,"\0");
    //     printf("%s\n",n_dir);
    //     image = fopen(n_dir, "w");

    //     if (image == NULL) {
    //         printf("Image didn't open\n");
    //         exit(0);
    //     }
    //     fwrite(p_array, 1, sizeof(p_array), image);
    //     fclose(image);
    // } else {
        //Convert it Back into Picture
    printf("Converting Byte Array to Picture Trusted\n");

    strcat(temp_dir, name);
    strcat(temp_dir,"\0");
    printf("%s\n",temp_dir);
    image = fopen(temp_dir, "w");

    if (image == NULL) {
        printf("Image didn't open\n");
        exit(0);
    }
    fwrite(p_array, 1, sizeof(p_array), image);
    fclose(image);

    // bzero(buff, sizeof(buff)); 
    write(sockfd, msg_rcv, sizeof(msg_rcv));

    /*int cond = readcolor(temp_dir);

    int del = remove(temp_dir);
    if (!del)
        printf("The file is Deleted successfully\n");
    else {
        printf("the file is not Deleted\n");
        exit(1);
    }

    if (cond == R) {
        strcat(r_dir, name);
        strcat(r_dir,"\0");
        image = fopen(r_dir, "w");
    } else if (cond == G) {
        strcat(g_dir, name);
        strcat(g_dir,"\0");
        image = fopen(g_dir, "w");
    } else {
        strcat(b_dir, name);
        strcat(b_dir,"\0");
        image = fopen(b_dir, "w");
    }

    if (image == NULL) {
        printf("Image didn't open\n");
        exit(0);
    }
    fwrite(p_array, 1, sizeof(p_array), image);
    fclose(image);*/
    // }
}