#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "pre_heavy_process.h"


int main(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-p") == 0)
    {
      if (argv[i + 1] != NULL)
      {
        printf("Port %s\n", argv[i + 1]);
        i++;
      }
      else
      {
        printf("You need to set a port\n");
      }
    }
    else if (strcmp(argv[i], "-ip") == 0)
    {
      if (argv[i + 1] != NULL)
      {
        printf("IP %s\n", argv[i + 1]);
        i++;
      }
      else
      {
        printf("You need to set a ip\n");
      }
    }

    else if (strcmp(argv[i], "-proc") == 0)
    {
      if (argv[i + 1] != NULL)
      {
        printf("# Process %s\n", argv[i + 1]);
        i++;
      }
      else
      {
        printf("You need to set a number of process\n");
      }
    }
  }
}



int receive_image(int socket, char *ip_client)
{ // Start function

  int buffersize = 0, recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;

  char imagearray[10241], verify = '1';
  FILE *image;

  //Find the size of the image
  do
  {
    stat = read(socket, &size, sizeof(int));
  } while (stat < 0);

  char buffer[] = "Got it";

  //Send our verification signal
  do
  {
    stat = write(socket, &buffer, sizeof(int));
  } while (stat < 0);

  printf("Reply sent\n");
  printf(" \n");

  image = fopen("t.png", "w");

  if (image == NULL)
  {
    printf("Error has occurred. Image file could not be opened\n");
    return -1;
  }

  //Loop while we have not received the entire file yet

  int need_exit = 0;
  struct timeval timeout = {10, 0};

  fd_set fds;
  int buffer_fd, buffer_out;

  while (recv_size < size)
  {
    //while(packet_index < 2){

    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

    if (buffer_fd < 0)
      printf("error: bad file descriptor set.\n");

    if (buffer_fd == 0)
      printf("error: buffer read timeout expired.\n");

    if (buffer_fd > 0)
    {
      do
      {
        read_size = read(socket, imagearray, 10241);
      } while (read_size < 0);

      //Write the currently read data into our image file
      write_size = fwrite(imagearray, 1, read_size, image);

      if (read_size != write_size)
      {
        printf("error in read write\n");
      }

      //Increment the total number of bytes read
      recv_size += read_size;
      packet_index++;
    }
  }

  fclose(image);

  return 1;
}

int server()
{
  int socket_desc, new_socket, c, read_size, buffer = 0;
  struct sockaddr_in server, client;
  char *readin;

  //Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8080);
  //Bind
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    puts("bind failed");
    return 1;
  }

  puts("bind done");

  //Listen
  listen(socket_desc, 3);

  //Accept and incoming connection
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  while (1)
  {

    if ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
      puts("Connection accepted");
    }

    fflush(stdout);

    if (new_socket < 0)
    {
      perror("Accept Failed");
      return 1;
    }

    receive_image(new_socket, inet_ntoa(client.sin_addr));
  }
  close(socket_desc);
  fflush(stdout);
  return 0;
}

