#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include "constant.h"
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
    printf("Error Opening Image File");
  }

  fseek(picture, 0, SEEK_END);
  size = ftell(picture);
  fseek(picture, 0, SEEK_SET);

  //Send Picture Size
  puts("Sending Picture Size");
  write(socket, (void *)&size, sizeof(int));

  //Send Picture as Byte Array
  printf("Sending Picture Byte Array");
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

  //Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }

  memset(&server, 0, sizeof(server));
  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  //Connect to remote server
  if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    close(socket_desc);
    puts("Connect Error");
    exit(1);
  }

  puts("Connected\n");

  send_image(socket_desc, filename);

  close(socket_desc);
}

int main(int argc, char const *argv[])
{
  for (int i = 0; i < 20; i++)
  {
    char str[3];
    sprintf(str, "%d", i % 10);
    char path[30] = "imagesToSend/imagen";
    strcat(path, str);
    strcat(path, ".jpg");
    printf("Sending %s\n", path);
    client_send("127.0.0.1", portConst, path);
  }

  return 0;
}
