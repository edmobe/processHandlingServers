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
#include "../../filter/src/macros.h"
#include "../../filter/src/sobel.h"
#include "../../filter/src/file_operations.h"

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
int seq_server(int port) 
{ 
	int sockfd, connfd, len, read_size, buffer = 0;
    int width, height;
	struct sockaddr_in servaddr, cli;
    struct in_addr **addr_list;
    char *readin;

	//************Socket connection***************
    //      Socket creation and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		puts("ERROR: Socket creation failed...\n"); 
		return 1; 
	}
	bzero(&servaddr, sizeof(servaddr)); 

    // assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port =htons(port);

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		puts("ERROR: Socket bind failed...\n"); 
		return 1;
	} 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 10)) != 0) { 
		puts("ERROR: Listen failed...\n"); 
		return 1; 
	} 

    len = sizeof(cli);

    // char buff[MAX+1];
    char *name;
    char *msg_rcv2 = "Got it";
	int n;
    FILE *image, *fp;
    // int first = 0;
    int counter = 0;
    // ssize_t nread;

    while (TRUE)
    {
        puts("Server listening..\n"); 

        // Accept the data packet from client and verification 
        if ((connfd = accept(sockfd, (SA*)&cli, &len)) < 0) { 
            puts("ERROR: Server acccept failed...\n"); 
            return 1; 
        } 

        //Somebody connected , get his details and print  
        getpeername(connfd, (SA*)&cli, (socklen_t*)&len);

        printf("- Host connected , ip %s , port %d -\n", 
            inet_ntoa(cli.sin_addr) , ntohs(cli.sin_port));

        fflush(stdout);
        
        // int counter1 = 0;
        // while (counter1<1) {

        char temp_dir[20] = "../imgs/img_";
        char rgb_temp_dir[20] = "../imgs/img_";
        char result_dir[20] = "../imgs/sobel_";
        char gray_dir[20] = "../imgs/img_";
        int buffersize = 0, recv_size = 0, size = 0, 
            read_size, write_size, packet_index = 1, stat;
        
        char imagearray[10241], verify = '1';

        // Read Picture Size
        do
        {
            stat = read(connfd, &size, sizeof(int));
        } while (stat < 0);

        printf("Size: %d\n", size);
        puts(" ");


        //Send our verification signal
        do
        {
            stat = write(connfd, msg_rcv2, sizeof(int));
        } while (stat < 0);

        puts("Reply sent\n");

        // bzero(buff, sizeof(buff));
        // printf("Reading Picture Size\n");

        // long size;
        // if (read(connfd, &size, sizeof(long)) == 0) {
        //     break;
        // }

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

        // Read Picture Name
        // **In the new client, there won't be send name
        // printf("Reading Picture Name\n");
        // bzero(buff, sizeof(buff));
        
        // if (read(connfd, buff, MAX) == 0) {
        //     break;
        // }

        // char *token = strtok(buff, "/");

        // do {   
        //     name = token;
        //     token = strtok(NULL, "/");
        // } while (token != NULL);

        // printf("Name: %s\n", name);

        char counter_str[4];
        sprintf(counter_str, "%d", counter);

        strcat(temp_dir, counter_str);
        strcat(temp_dir, ".jpg");
        strcat(temp_dir, "\0");

        strcat(rgb_temp_dir, counter_str);
        strcat(rgb_temp_dir, ".rgb");
        strcat(rgb_temp_dir, "\0");

        strcat(result_dir, counter_str);
        strcat(result_dir, ".jpg");
        strcat(result_dir, "\0");

        strcat(gray_dir, counter_str);
        strcat(gray_dir, ".gray");
        strcat(gray_dir, "\0");

        printf("Dir to image: %s\n",temp_dir);

        // First response message
        // printf("Responding 1 to client\n");
        // bzero(buff, sizeof(buff)); 
        // write(connfd, msg_rcv1, MAX);
        
        //Read Picture Byte Array
        puts(" ");
        puts("Reading Picture Byte Array");
        int need_exit = 0;
        struct timeval timeout = {10, 0};

        fd_set fds;
        int buffer_fd, buffer_out;

        image = fopen(temp_dir, "w");

        if (image == NULL) {
            printf("ERROR: Image didn't open\n");
            fclose(image);
            return 1;
        }

        while (recv_size < size)
        {

            FD_ZERO(&fds);
            FD_SET(connfd, &fds);

            buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

            if (buffer_fd < 0) {
                puts("ERROR: bad file descriptor set.");
                return 1;
            }

            if (buffer_fd == 0) {
                puts("ERROR: buffer read timeout expired.");
                return 1;
            }

            // if (buffer_fd > 0) {
            do {
                read_size = read(connfd, imagearray, 10241);
            } while (read_size < 0);

            //Write the currently read data into our image file
            write_size = fwrite(imagearray, 1, read_size, image);

            if (read_size != write_size)
            {
                puts("ERROR: In read write");
                return 1;
            }

            //Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
            // }
        }
        fclose(image);
        
        byte *rgb, *gray, *sobel_h_res, *sobel_v_res, *contour_img;

        char path[50] = "convert ";

        strcat(path, temp_dir);
        strcat(path, " ");
        strcat(path, rgb_temp_dir);
        // strcat(path, ".rgb");

        // Open the command for reading.
        printf("%s\n", path);
        
        fp = popen(path, "r");
        if (fp == NULL) {
            printf("ERROR: Failed to run command\n" );
            return 1;
        }
        
        // close 
        pclose(fp);

        char path2[50] = "identify -format '%wx%h' ";
        strcat(path2, temp_dir);
        fp = popen(path2, "r");
        if (fp == NULL) {
            printf("ERROR: Failed to run command\n" );
            return 1;
        }

        char buf_temp[100];
        char *str = NULL;
        char *temp = NULL;
        unsigned int size1 = 1;  // start with size of 1 to make room for null terminator
        unsigned int strlength;

        while (fgets(buf_temp, sizeof(buf_temp), fp) != NULL) {
            strlength = strlen(buf_temp);
            temp = realloc(str, size1 + strlength);  // allocate room for the buf that gets appended
            if (temp != NULL) {
                str = temp;
            }
            strcpy(str + size1 - 1, buf_temp);     // append buffer to str
            size1 += strlength; 
        }
        // close
        pclose(fp);

        int offset1 = 0;
        int offset2 = 0;
        int post = 0;
        while (*(str+offset1) != '\0') {
            if (*(str+offset1) != 'x') {
                buf_temp[offset2] = *(str+offset1);
                offset1++;
                offset2++;
            } else {
                buf_temp[offset2] = '\0';
                sscanf(buf_temp, "%d", &width);
                offset1++;
                offset2 = 0;
            }
        }

        buf_temp[offset2] = '\0';
        sscanf(buf_temp, "%d", &height);

        // if (size == 75441) {
        //     width = 512;
        //     height = 512;
        // } else if (size == 6220800) {
        //     width = 1920;
        //     height = 1080;
        // } else {
        //     width = 512;
        //     height = 512;
        // }

        // Read file to rgb and get size
        readFile(rgb_temp_dir, &rgb, width*height*3);

        puts("Applying filter");
        int gray_size = sobelFilter(rgb, &gray, &sobel_h_res, &sobel_v_res, &contour_img, width, height);

        if (counter < 100) {

            // Write sobel img to a file
            writeFile(gray_dir, contour_img, gray_size);

            char path1[100] = "convert -size ";

            char size_str[7];
            sprintf(size_str, "%dx%d ", width, height);
            strcat(path1, size_str);
            strcat(path1, "-depth 8 ");
            
            strcat(path1, gray_dir);
            strcat(path1, " ");
            strcat(path1, result_dir);

            // Open the command for reading. 
            fp = popen(path1, "r");
            if (fp == NULL) {
                printf("ERROR: Failed to run command\n" );
                exit(1);
            }

            // close 
            pclose(fp);

            counter ++;

        } 
        // else {
        //     int gray_size = sobelFilter(rgb, &gray, &sobel_h_res, &sobel_v_res, &contour_img, width, height);
        // }

        close(connfd);
    }

    close(sockfd);
    fflush(stdout);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != NO_INPUT) {
        printf("ERROR: num of param #%d, param needed #%d\n", argc, NO_INPUT);
        exit(1);
    }

    return seq_server(atoi(argv[1]));
}