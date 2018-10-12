
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

#define PORT2 "3491"
#define MSG_SIZE 256
#define PORT "3490"
#define MAXDATASIZE 100
using namespace std;
// 取得 IPv4 或 IPv6 的 sockaddr：
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main()
{
   int sockfd, numbytes;
   char buf[MAXDATASIZE];
   struct addrinfo hints, *servinfo,*clientinfo, *p;
   int rv;
   char s[INET6_ADDRSTRLEN];

   memset(&hints, 0, sizeof hints); // 确保 struct 为空
   hints.ai_family = AF_INET; // good for ipv4 and ipv6
   hints.ai_socktype = SOCK_STREAM;
// int getaddrinfo(const char *node, // 例如： "www.example.com" 或 IP
// const char *service, // 例如： "http" 或 port number
// const struct addrinfo *hints,
// struct addrinfo **res);
   if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) { // return not zero value when error happen
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); // print error with gai_strerror
        return 1;
    }
    if ((rv = getaddrinfo(NULL, PORT2, &hints, &clientinfo)) != 0) { // return not zero value when error happen
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); // print error with gai_strerror
      return 1;
      }
    for(p = clientinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype, 
          p->ai_protocol)) == -1) {
          perror("client: socket"); 
          continue;
        }
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
         close(sockfd);
         continue;
      }
    }
    for(p = servinfo; p != NULL; p = p->ai_next) {
      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("client: connect");
        continue;
      }
      break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // 全部皆以这个 structure 完成

   
char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
    memset(msg, '\0', MSG_SIZE);
        if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
            exit(-1);


        printf("\nSENDing it to the remote server ... ");
        if(send(sockfd, msg, strlen(msg), 0) == strlen(msg))
            printf("Done!\n");

   if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
       perror("recv");
       exit(1);
    }
    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);
    close(sockfd);
    return 0;
  }
