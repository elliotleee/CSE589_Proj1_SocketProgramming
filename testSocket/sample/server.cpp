/**
 * @server
 * @author  Swetank Kumar Saha <swetankk@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains the server init and main while loop for tha application.
 * Uses the select() API to multiplex between network I/O and STDIN.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
  if(argc != 2) {
    printf("Usage:%s [port]\n", argv[0]);
    exit(-1);
  }

  int port, server_socket, head_socket, selret, sock_index, fdaccept=0;
  struct sockaddr_in server_addr, client_addr;
  fd_set master_list, watch_list;
  socklen_t caddr_len;

  /* Socket */
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(server_socket < 0)
    perror("Cannot create socket");

  /* Fill up sockaddr_in struct */
  port = atoi(argv[1]);
  bzero(&server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);

  /* Bind */
  if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
    perror("Bind failed");

  /* Listen */
  if(listen(server_socket, BACKLOG) < 0)
    perror("Unable to listen on port");

  /* ---------------------------------------------------------------------------- */

  /* Zero select FD sets */
  FD_ZERO(&master_list);
  FD_ZERO(&watch_list);

  /* Register the listening socket */
  FD_SET(server_socket, &master_list);
  /* Register STDIN */
  FD_SET(STDIN, &master_list);

  head_socket = server_socket;

  while(TRUE){
    memcpy(&watch_list, &master_list, sizeof(master_list));

    //printf("\n[PA1-Server@CSE489/589]$ ");
    //fflush(stdout);

    /* select() system call. This will BLOCK */
    selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
    if(selret < 0)
      perror("select failed.");

    /* Check if we have sockets/STDIN to process */
    if(selret > 0){
      /* Loop through socket descriptors to check which ones are ready */
      for(sock_index = 0; sock_index <= head_socket; sock_index += 1){

        if(FD_ISSET(sock_index, &watch_list)){

          /* Check if new command on STDIN */
          if (sock_index == STDIN){
            char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

            memset(cmd, '\0', CMD_SIZE);
            if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
              exit(-1);

            printf("\nI got: %s\n", cmd);

            //Process PA1 commands here ...

            free(cmd);
          }
          /* Check if new client is requesting connection */
          else if(sock_index == server_socket){
            caddr_len = sizeof(client_addr);
            fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
            if(fdaccept < 0)
              perror("Accept failed.");

            printf("\nRemote Host connected!\n");

            /* Add to watched socket list */
            FD_SET(fdaccept, &master_list);
            if(fdaccept > head_socket) head_socket = fdaccept;
          }
          /* Read from existing clients */
          else{
            /* Initialize buffer to receieve response */
            char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
            memset(buffer, '\0', BUFFER_SIZE);

            if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
              close(sock_index);
              printf("Remote Host terminated connection!\n");

              /* Remove from watched list */
              FD_CLR(sock_index, &master_list);
            }
            else {
              //Process incoming data from existing clients here ...

              printf("\nClient sent me: %s\n", buffer);
              printf("ECHOing it back to the remote host ... ");
              if(send(sock_index, buffer, strlen(buffer), 0) == strlen(buffer))
                printf("Done!\n");
              fflush(stdout);
            }

            free(buffer);
          }
        }
      }
    }
  }

  return 0;
}
