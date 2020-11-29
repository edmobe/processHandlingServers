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

#define MAX 80
#define SA struct sockaddr 
#define OK       0
#define NO_INPUT 4
#define TRUE   1
#define FALSE  0  

// static int getLine (char *prmpt, char *buff, size_t sz);

// Simple client with only one thread and process, TCP type.
// For using it the parameters are like described in the project specification
// but without the threads and times.

// Ex.: ./client <IP> <PORT> <RELATIVE_DIR_IMG>
// How it works: ./client 127.0.0.1 9090 ../imgs/img.png

// *.rgb are temporals for image transference without compresion.

int main(int argc, char *argv[]) { 

    FILE *fp, *picture;
    int counter = 0;
    char *exitmsg = "exit\0";

    if (argc != NO_INPUT) {
        printf("ERROR: num of param #%d, param needed #%d\n", argc, NO_INPUT);
        exit(1);
    }

    if(access(argv[3], F_OK) == -1) {
        printf("ERROR: Image doesn't exits\n");
        exit(1);
    }

    int name_len = strlen(argv[3]);
    printf("Len:%d\n", name_len);
    char name_str[name_len+1];

    for (int i = 0; i < name_len-4; i ++) {
        name_str[i] = argv[3][i];
    }
    name_str[name_len-4] = '\0';
    strcat(name_str, ".rgb\0");

    if(access( name_str, F_OK ) == -1) {

        char path[100] = "convert ";

        strcat(path, argv[3]);
        strcat(path, " ");
        
        
        strcat(path, name_str);
        // strcat(path, ".rgb");

        /* Open the command for reading. */
        printf("%s\n", path);
        
        fp = popen(path, "r");
        if (fp == NULL) {
            printf("ERROR: Failed to run command\n" );
            exit(1);
        }

        // close 
        pclose(fp);
    } else {
        printf("Image format present\n");
    }

    int sockfd; 
    struct sockaddr_in servaddr, cli; 

    picture = fopen(name_str, "r");

    if(!picture) {
        printf("ERROR: opening image format\n");
        exit(1);
    }

    //Get Picture Size
    printf("Getting Picture Size\n");
    // printf(buff);

    long size;
    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);

    printf("Img size: %ld\n", size);

    bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
    servaddr.sin_port = htons(atoi(argv[2]));

    //************Socket connection***************
    //      Socket creation and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("ERROR: socket creation failed...\n"); 
        exit(1); 
    } 
    else
        printf("Socket successfully created..\n"); 
    
    char buff[MAX+1];

    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("ERROR: connection with the server failed...\n"); 
        exit(1); 
    } 
    else
        printf("connected to the server..\n"); 

    int first = 0;

    // This while is the times that the client comunicates with the server
    // after the handshake. Disconnects if the server leaves the handshake.
    while (counter < 1)
    {

        //Send Picture Size
        printf("Sending Picture Size\n");
        bzero(buff, sizeof(buff));
        long temp_size = size;
        // _itoa_s(size,buff,sizeof(buff),10);
        write(sockfd, &temp_size, sizeof(long));

        // if (first) {
        //     break;
        // } else {
        //     first = 1;
        // }

        //Send Picture Name
        printf("Sending Picture Name\n");
        bzero(buff, sizeof(buff));
        write(sockfd, name_str, MAX);
        
        /*
        //Send Picture as Byte Array (without need of a buffer as large as the image file)
        printf("Sending Picture as Byte Array\n");
        // bzero(buff, sizeof(buff));
        char img_array[size+1];
        bzero(img_array, sizeof(img_array));
        // char send_buffer[MAX]; // no link between BUFSIZE and the file size
        int nb = fread(img_array, 1, size, picture);
        img_array[size] = '\0';
        // while(!feof(picture)) {
        write(sockfd, img_array, size);
        fseek(picture, 0, SEEK_SET);*/
            // nb = fread(buff, 1, sizeof(buff), picture);
            // no need to bzero
        // }
        
        // break;
        
        // Recieves answer from server
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, MAX); 
        printf("From Server : %s \n", buff);

        // // Sends zeroes
        // bzero(buff, sizeof(buff));
        // write(sockfd, NULL, MAX);
        

        // This is a termination code, but it could be done more fancy and not server
        // dependant.
        // if ((strncmp(buff, "exit", 4)) == 0) { 
        //     printf("Client Exit...\n"); 
        //     break; 
        // } */

        counter++;

        // sleep(0.01);


    }
    
    fclose(picture);
    // close the socket
    close(sockfd);
    

    // printf("Client Exit...\n"); 
    // bzero(buff, sizeof(buff));
    // write(sockfd, "exit", sizeof(buff));

    return 0;
}

// ****************************************************************
// Code for recieving inputs from terminal, not used in thi project
// ****************************************************************
// static int getLine (char *prmpt, char *buff, size_t sz) {
//     int ch, extra;

//     // Get line with buffer overrun protection.
//     if (prmpt != NULL) {
//         printf ("%s", prmpt);
//         fflush (stdout);
//     }
//     if (fgets (buff, sz, stdin) == NULL)
//         return NO_INPUT;

//     // If it was too long, there'll be no newline. In that case, we flush
//     // to end of line so that excess doesn't affect the next call.
//     if (buff[strlen(buff)-1] != '\n') {
//         extra = 0;
//         while (((ch = getchar()) != '\n') && (ch != EOF))
//             extra = 1;
//         return (extra == 1) ? TOO_LONG : OK;
//     }

//     // Otherwise remove newline and give string back to caller.
//     buff[strlen(buff)-1] = '\0';
//     return OK;
// }