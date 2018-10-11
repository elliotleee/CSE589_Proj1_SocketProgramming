/*
/*
** client.c -- 一个 stream socket client 的 demo
*/
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
#include "../include/global.h"
#include "../include/logger.h"
using namespace std;



#define PORT "3490" // Client 所要连接的 port
#define MAXDATASIZE 100 // 我们一次可以收到的最大字节数量（number of bytes）

// 取得 IPv4 或 IPv6 的 sockaddr：
void *get_in_addr(struct sockaddr *sa) // sockaddr for sa_family and sa_data[14]->destination and port number
{
　　if (sa->sa_family == AF_INET) {
　　　　return &(((struct sockaddr_in*)sa)->sin_addr);
　　}

　　return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
　　int sockfd, numbytes;
　　char buf[MAXDATASIZE];
　　struct addrinfo hints, *servinfo, *p;
　　int rv;
　　char s[INET6_ADDRSTRLEN];
    char lines[MAX_BUF_LEN];
    char *command;

　　if (argc != 2) {
　　　　fprintf(stderr,"usage: client hostname\n");
　　　　exit(1);
　　}

　　memset(&hints, 0, sizeof hints); // 确保 struct 为空
　　hints.ai_family = AF_UNSPEC; // good for ipv4 and ipv6
　　hints.ai_socktype = SOCK_STREAM;

// int getaddrinfo(const char *node, // 例如： "www.example.com" 或 IP
// const char *service, // 例如： "http" 或 port number
// const struct addrinfo *hints,
// struct addrinfo **res);

　　if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) { // return not zero value when error happen
　　　　fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); // print error with gai_strerror
　　　　return 1;
　　}
    // if success, serinfo指向一个struct addrinfos 的链表
　　// 用循环取得全部的结果，并先连接到能成功连接的
　　for(p = servinfo; p != NULL; p = p->ai_next) {
　　　　if ((sockfd = socket(p->ai_family, p->ai_socktype, // int socket(int domain, int type, int protocol) return socket descriptor 
　　　　　　p->ai_protocol)) == -1) {
　　　　　　perror("client: socket");
        //    cse4589_print_and_log('Can not create a socket.')
　　　　　　continue;
　　　　}

　　　　if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
　　　　　　close(sockfd);
　　　　　　perror("client: connect");
        //    cse4589_print_and_log('Connection failed.')
　　　　　　continue;
　　　　}

　　　　break;
　　}

　　if (p == NULL) {
　　　　fprintf(stderr, "client: failed to connect\n");
       cse4589_print_and_log('Connection failed. No connection')
　　　　return 2;
　　}

　　inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

　　printf("client: connecting to %s\n", s);

　　freeaddrinfo(servinfo); // 全部皆以这个 structure 完成

    fgets(lines, MAX_BUF_LEN, cin)

    command = strtok(lines, "\n");

　　if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
　　　　perror("recv");
       cse4589_print_and_log('Received data out of range')
　　　　exit(1);
　　}

　　buf[numbytes] = '\0';
　　printf("client: received '%s'\n",buf);

　　close(sockfd);
　　return 0;
}