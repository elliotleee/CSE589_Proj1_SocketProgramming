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

#define PORT "3490"

using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main() {
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
	int i,j, jc,rv;


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
        printf("INITICALISE FINISH\n");
		break;
	}

	// 若我们进入这个判断式，则表示我们 bind() 失败
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(ai); // all done with this
    printf("start listen\n");
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
					printf("%s\n",buf);
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
