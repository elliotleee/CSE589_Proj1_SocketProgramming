/**
 * @ziangli_assignment1
 * @author  ziangli <ziangli@buffalo.edu>
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
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>

#include "../include/global.h"
#include "../include/logger.h"

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints); // 确保 struct 为空
　　hints.ai_family = AF_UNSPEC; // good for ipv4 and ipv6
　　hints.ai_socktype = SOCK_STREAM;

	if (argc != 2) {
		cse4589_print_and_log('Parameters error');
		exit(1);
　　}

	if(strcmp(argv[1], "s") == 0){
		create_sock_server(argv[2])
	}

	if(strcmp(argv[1], "c") == 0) {
		create_sock_client(argv[2])
	}
	
	
	return 0;
}

int create_sock_server(int port) {
	int sockfd, new_fd; // 在 sock_fd 进行 listen，new_fd 是新的连接
  	struct addrinfo hints, *servinfo, *p;
  	struct sockaddr_storage their_addr; // 连接者的地址资料
  	socklen_t sin_size;
  	struct sigaction sa;
  	int yes=1;
  	char s[INET6_ADDRSTRLEN];
  	int rv;

// hints 参数指向一个你已经填好相关资料的 struct addrinfo
  	memset(&hints, 0, sizeof hints);
  	hints.ai_family = AF_UNSPEC;
  	hints.ai_socktype = SOCK_STREAM;
 	hints.ai_flags = AI_PASSIVE; // 使用我的 IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// 以循环找出全部的结果，并绑定（bind）到第一个能用的结果
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) == -1) {
		perror("server: socket");
		continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	if(p->ai_family == AF_INET){
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
		addr = &(ipv4->sin_addr);
		ipver = "IPv4";
	} else {
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
		addr = &(ipv6->sin6_addr);
		ipver = "IPv6";
	}
	// convert the IP to a string and print it:
	　inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
	　printf(" %s: %s\n", ipver, ipstr);

	freeaddrinfo(servinfo); // 全部都用这个 structure; 将链表全部释放

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // 收拾全部死掉的 processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) { // 主要的 accept() 循环
	
	sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

		if (new_fd == -1) {
		perror("accept");
		continue;
		}

		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
		printf("server: got connection from %s\n", s);
	
		if (!fork()) { // 这个是 child process
		close(sockfd); // child 不需要 listener

		if (send(new_fd, "Hello, world!", 13, 0) == -1)
			perror("send");

		close(new_fd);

		exit(0);
		}
		close(new_fd); // parent 不需要这个
	}

	return 0;
}

int create_sock_client(int port) {

}
