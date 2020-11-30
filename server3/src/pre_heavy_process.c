#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include "../../general/queue.c"
#include "constant.h"

int countFiles()
{
  int file_count = 0;
  DIR *dirp;
  struct dirent *entry;
  dirp = opendir("../imgs");
  while ((entry = readdir(dirp)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      file_count++;
    }
  }
  closedir(dirp);
  return file_count;
}

int receive_image(int socket)
{ // Start function
  printf("Attending client %d with process %d\n", socket, getpid());
  int file_count = countFiles();
  int buffersize = 0, recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;
  char imagearray[10241], verify = '1';
  FILE *image;
  char path[30] = "../imgs/image_";
  char extension[5] = ".jpg";
  char number_str[10];
  char pid[5];
  sprintf(number_str, "%d", file_count);
  sprintf(pid, "%d", getpid());
  strcat(path, number_str);
  strcat(path, "_");
  strcat(path, pid);
  strcat(path, extension);

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

  image = fopen(path, "w");

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
  //enqueue(path);
  fclose(image);

  return 1;
}

int server(int port)
{
  int processes = 10, procn;
  int pictures = 0;
  int socket_desc, new_socket, c, read_size, buffer = 0;
  struct sockaddr_in server, client;
  int fd[2];
  pipe(fd);
  char *readin;
  pid_t pid[processes];
  //Create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1)
  {
    printf("Could not create socket");
  }

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  printf("%d\n", server.sin_addr.s_addr);
  //Bind
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    puts("bind failed");
    return 1;
  }

  puts("bind done");

  //Listen
  listen(socket_desc, 10);

  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
  printf("Creating %d processes\n", processes);

  for (procn = 0; procn < processes; procn++)
  {
    pid[procn] = fork();
    if (pid[procn] == 0)
    {
      break;
    }
  }

  while (1)
  {

    if (pid[procn] == 0)
    {
      if ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
      {
        puts("Connection accepted");
      }
      if (new_socket < 0)
      {
        perror("Accept Failed");
        return 1;
      }
      receive_image(new_socket);
    }
    else
    {
      close(socket_desc);
    }

    //receive_image(new_socket, inet_ntoa(client.sin_addr));
  }
  close(socket_desc);
  fflush(stdout);
  return 0;
}

int main(int argc, char *argv[])
{
  server(portConst);
}