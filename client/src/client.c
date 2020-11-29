#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>  
#include <sys/types.h>
#include <sys/ioctl.h>

#include <unistd.h>   //file close
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>    //socket close     

#define SA struct sockaddr
#define NO_INPUT 6
#define TRUE   1
#define FALSE  0  

// Simple client with only one thread and process, TCP type.
// For using it the parameters are like described in the project specification
// but without the threads and times.

// Ex.: ./client <IP> <PORT> <RELATIVE_DIR_IMG> <NUM_THREADS> <NUM_ITERATIONS>
// How it works: ./client 127.0.0.1 9090 ../imgs/img.png 0 1

//This function is to be used once we have confirmed that an image is to be sent
//It should read and output an image file

int send_image(int socket, char *filename)
{

    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;

    picture = fopen(filename, "r");

    if (picture == NULL)
    {
      	puts("ERROR: Opening Image File");
    }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);

    //Send Picture Size
    write(socket, (void *)&size, sizeof(int));

    //Send Picture as Byte Array

    do
    { //Read while we get errors that are due to signals.
      	stat = read(socket, &read_buffer, 255);

    } while (stat < 0);

    while (!feof(picture))
    {
		//while(packet_index = 1){
		//Read from the file into our send buffer
		read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, picture);

		//Send data through our socket
		do
		{
			stat = write(socket, send_buffer, read_size);
		} while (stat < 0);

		packet_index++;

		//Zero out our send buffer
		bzero(send_buffer, sizeof(send_buffer));
    }
}

void client_send(char *ip, int port, char *filename)
{

	int socket_desc;
	struct sockaddr_in server;
	char *parray;

	//************Socket connection***************
    //      Socket creation and varification
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_desc == -1) {
        puts("ERROR: socket creation failed...\n"); 
        exit(1); 
    } 

	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(socket_desc, (SA *)&server, sizeof(server)) != 0)
	{
		close(socket_desc);
		puts("ERROR: connection with the server failed...\n"); 
        exit(1);
	}

	puts("Connected\n");

	send_image(socket_desc, filename);

	close(socket_desc);
}

int main(int argc, char *argv[]) { 

    // FILE *fp, *picture;
    // int counter = 0;

    if (argc != NO_INPUT) {
        printf("ERROR: num of param #%d, param needed #%d\n", argc, NO_INPUT);
        exit(1);
    }

    if(access(argv[3], F_OK) == -1) {
        puts("ERROR: Image doesn't exits\n");
        exit(1);
    }

	int name_len = strlen(argv[3]);

	char img_ext[4];
	char name_str[name_len+1];

    for (int i = 0; i < 3; i ++) {
        img_ext[2-i] = argv[3][name_len-i];
    }
	img_ext[3] = '\0';

	for (int i = 0; i < name_len; i ++) {
		name_str[i] = argv[3][i];
	}
	name_str[name_len] = '\0';

    if((img_ext[1]!='p' || img_ext[1]!='e') && img_ext[2]!='g') {

		FILE *fp;
        char path[100] = "convert ";

		name_str[name_len-1] = 'g';
		name_str[name_len-2] = 'p';
		name_str[name_len-3] = 'j';

        strcat(path, argv[3]);
        strcat(path, " ");

        strcat(path, name_str);

        /* Open the command for reading. */
        printf("%s\n", path);
        
        fp = popen(path, "r");
        if (fp == NULL) {
            printf("ERROR: Failed to run command\n" );
            exit(1);
        }

        // close 
        pclose(fp);
    }

	// ----------ATENTION-----------------
	// This makes iterations for the same image
	// We need threads implementation
	// Use the argv[4] for the number of threads
	for (int i = 0; i < atoi(argv[5]); i++) {
		client_send(argv[1], atoi(argv[2]), name_str);
	}


    return 0;
}
