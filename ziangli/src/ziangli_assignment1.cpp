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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>

#include "../include/global.h"
#include "../include/logger.h"

using namespace std;

string getIPAddress(){
    string ipAddress="Unable to get IP Address";
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    // retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    if (success == 0) {
        // Loop through linked list of interfaces
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                ipAddress=inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}

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
	char* p_end;
	long int port = strtol(argv[2], &p_end, 10);
	
	string command_str;

	if (argc != 2) {
		cse4589_print_and_log("Parameters error");
		exit(1);
　　}

	if(strcmp(argv[1], "s") == 0){
		create_sock_server(argv[2])
	}

	if(strcmp(argv[1], "c") == 0) {
		create_sock_client(argv[2])
	}
	
	while(1) {
		getline(cin, command_str);

		if(strcmp(command, "AUTHOR") == 0) {
			cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
			string ubit_name = 'lchen76';
			cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", ubit_name);
			cse4589_print_and_log("[%s:ERROR]\n", command_str);
		}
		else if(strcmp(command, "IP") == 0) {
			cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
			string ip_address = getIPAddress();
			cse4589_print_and_log("IP:%s\n", ip_address);
			cse4589_print_and_log("[%s:ERROR]\n", command_str);
		}
		else if(strcmp(command, "IP") == 0) {
			cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
			cse4589_print_and_log("PORT:%d\n", port);
			cse4589_print_and_log("[%s:ERROR]\n", command_str);
		}
		else if(strcmp(command, "LIST") == 0) {
			 
		}

	}
	
	return 0;
}
void Trans_Fd(int sockfd){
	struct sockaddr_in sa;
      int len = sizeof(sa);
      getpeername(m_sockfd, (struct sockaddr *)&sa, &len);
      cout << ("ip:%d\n",inet_ntoa(sa.addr) ntohs(sa.sin_port));
}

int create_sock_server(int port) {
	fd_set master; // master file descriptor 表
	fd_set read_fds; // 给 select() 用的暂时 file descriptor 表
	int fdmax; // 最大的 file descriptor 数目

	int listener; // listening socket descriptor
	int newfd; // 新接受的 accept() socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[256]; // 储存 client 数据的缓冲区
	int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

	int yes=1; // 供底下的 setsockopt() 设置 SO_REUSEADDR
	int i, j, rv;

	struct addrinfo hints, *ai, *p;

	FD_ZERO(&master); // 清除 master 与 temp sets
	FD_ZERO(&read_fds);

	// 给我们一个 socket，并且 bind 它
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	for(p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
		continue;
		}

		// 避开这个错误信息："address already in use"
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
		close(listener);
		continue;
		}

		break;
	}

	// 若我们进入这个判断式，则表示我们 bind() 失败
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	// 将 listener 新增到 master set
	FD_SET(listener, &master);

	// 持续追踪最大的 file descriptor
	fdmax = listener; // 到此为止，就是它了

	// 主要循环
	for( ; ; ) {
		read_fds = master; // 复制 master

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(4);
		}

		// 在现存的连接中寻找需要读取的数据
		for(i = 0; i <= fdmax; i++) {
		if (FD_ISSET(i, &read_fds)) { // 我们找到一个！！
			if (i == listener) {
			// handle new connections
			addrlen = sizeof remoteaddr;
			newfd = accept(listener,
				(struct sockaddr *)&remoteaddr,
				&addrlen);

			if (newfd == -1) {
				perror("accept");
			} else {
				FD_SET(newfd, &master); // 新增到 master set
				if (newfd > fdmax) { // 持续追踪最大的 fd
				fdmax = newfd;
				}
				printf("selectserver: new connection from %s on "
				"socket %d\n",
				inet_ntop(remoteaddr.ss_family,
					get_in_addr((struct sockaddr*)&remoteaddr),
					remoteIP, INET6_ADDRSTRLEN),
				newfd);
			}

			} else {
			// 处理来自 client 的数据
			if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
				// got error or connection closed by client
				if (nbytes == 0) {
				// 关闭连接
				printf("selectserver: socket %d hung up\n", i);
				} else {
				perror("recv");
				}
				close(i); // bye!
				FD_CLR(i, &master); // 从 master set 中移除

			} else {
				// 我们从 client 收到一些数据
				for(j = 0; j <= fdmax; j++) {
				// 送给大家！
				if (FD_ISSET(j, &master)) {
					// 不用送给 listener 跟我们自己
					if (j != listener && j != i) {
					if (send(j, buf, nbytes, 0) == -1) {
						perror("send");
					}
					}
				}
				}
			}
			} // END handle data from client
		} // END got new incoming connection
		} // END looping through file descriptors
	} // END for( ; ; )--and you thought it would never end!

	return 0;
}

int create_sock_client(int port) {
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
