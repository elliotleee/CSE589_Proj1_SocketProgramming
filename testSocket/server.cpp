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
#include <vector> 
#include <algorithm>

#include "./include/global.h"
#include "./include/logger.h"

#define PORT "3490"

using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// use like split_msg(msg," ", result);
void split_msg(string& src, const string& separator, vector<string>& dest)
{
    string str = src;
    string substring;
    string::size_type start = 0, index;
    dest.clear();
    index = str.find_first_of(separator,start);
    do
    {
        if (index != string::npos)
        {    
            substring = str.substr(start,index-start );
            dest.push_back(substring);
            start =index+separator.size();
            index = str.find(separator,start);
            if (start == string::npos) break;
        }
    }while(index != string::npos);

    //the last part
    substring = str.substr(start);
    dest.push_back(substring);
}

void log_IP(string ip){
string command = "IP";
cse4589_print_and_log("[%s:SUCCESS]\n", command);
cse4589_print_and_log("IP:%s\n", ip);
cse4589_print_and_log("[%s:END]\n", command);
}

void log_AUTHOR() {
string command = "AUTHOR";
cse4589_print_and_log("[%s:SUCCESS]\n", command);
string ubit_name_1 = "lchen76";
string ubit_name_2 = "ziangli";
cse4589_print_and_log("I, %s, %s, have read and understood the course academic integrity policy.\n", ubit_name_1, ubit_name_2);
cse4589_print_and_log("[%s:END]\n", command);
}

void log_PORT(string port) {
string command = "PORT";
cse4589_print_and_log("[%s:SUCCESS]\n", command);
cse4589_print_and_log("PORT:%d\n", port);
cse4589_print_and_log("[%s:END]\n", command);
}

bool cmp(string p[], string q[]) {
  int p1, p2;
  if (p[2] == "") {
    p1 = 100000;
  }
  else {
    p1 = stoi(p[2]);
  }
  if (p[2] == "") {
    p2 = 100000;
  }
  else {
    p2 = stoi(q[2]);
  }
  
  return p1 < p2;
}

void log_LIST(string list[][10]) {
string command = "LIST";
cse4589_print_and_log("[%s:SUCCESS]\n", command);
string **res = new string*[4];
  for (int i = 0; i < 4; ++i) {
    res[i] = new string[3];
    res[i][0] = list[i][0];
    res[i][1] = list[i][1];
    res[i][2] = list[i][2];
  }


  sort(res, res + 4, cmp);
for(int i = 0; i < 4; ++i){
if(res[i][0] == "") break;
cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i, res[i][0], res[i][1], res[i][2]);
}
cse4589_print_and_log("[%s:END]\n", command);
}

void log_STATICS(string list[][10]) {
	string command = "STATICS";
    cse4589_print_and_log("[%s:SUCCESS]\n", command);
    string **res = new string*[4];
    for (int i = 0; i < 4; ++i) {
        res[i] = new string[3];
        res[i][0] = list[i][0];
        res[i][1] = list[i][1];
        res[i][2] = list[i][2];
    }
    sort(res, res + 4, cmp);
    for(int i = 0; i < 4; ++i) {
        if(res[i][0] == "") break;
        cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", i, res[i][0], res[i][3], res[i][4], res[i][5]);
    }
    cse4589_print_and_log("[%s:END]\n", command);
}

void log_BLOCKED(string list[][10], string cli_ip) {
    string command = "BLOCKED";
    cse4589_print_and_log("[%s:SUCCESS]\n", command);
    string blocked[3][3];
    int idx = 0;
    for(int i = 0; i < 4; ++i){
        if(list[i][1] == cli_ip){
            for(int j = 6; j < 9; ++j) {
                if(list[i][j] != ""){
                    blocked[idx][1] = list[i][j];
                    for(int z = 0; z < 4; ++z){
                        if(list[z][1] == blocked[idx][1]){
                            blocked[idx][0] = list[z][0];
                            blocked[idx][2] = list[z][2];
                        }
                    }
                }
                idx++;
            }
        }
    }
    string **res = new string*[3];
    for (int i = 0; i < 3; ++i) {
        res[i] = new string[3];
        res[i][0] = list[i][0];
        res[i][1] = list[i][1];
        res[i][2] = list[i][2];
    }
    sort(res, res + 3, cmp);
    for(int i = 0; i < 3; ++i){
        if(res[i][0] == "") break;
        cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i, res[i][0], res[i][1], res[i][2]);
    }
    cse4589_print_and_log("[%s:END]\n", command);
}

void log_EVENT(string from_ip, string msg, string to_ip) {
	string command = "EVENT";
	cse4589_print_and_log("[%s:SUCCESS]\n", command);
	cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", from_ip, to_ip, msg);
	cse4589_print_and_log("[%s:END]\n", command);
}

bool valid_ip(string ip_test) {
	int dot_num = 0;
	for(int i = 0; i < ip_test.length(); ++i){
		if(ip_test[i] == '.') {
			dot_num++;
		}
	}
	if(dot_num != 3) return false;
	vector<string> ip_parts;
	split_msg(ip_test,".", ip_parts);
	for(int i = 0; i < 4; ++i){
		for(int j = 0; j < ip_parts[i].length(); ++j){
			if(ip_parts[i][j] > '9' || ip_parts[i][j] < '0') return false;
		}
	}
	for(int i = 0; i < 4; ++i){
		if(stoi(ip_parts[i]) > 255) return false;
	}
	return true;
}

int server_process(string myPORT) {
	fd_set master; // master file descriptor 表
	fd_set read_fds; // 给 select() 用的暂时 file descriptor 表
	int fdmax; // 最大的 file descriptor 数目
	string blank = " ";

	int serversocketfd;

	int listener; // listening socket descriptor
	int newfd; // 新接受的 accept() socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[256]; // 储存 client 数据的缓冲区
	int nbytes;

    string ClientList[4][10] = {""};
    string msg;
   char charmsg[50000];

	char remoteIP[INET6_ADDRSTRLEN];
    char s[INET6_ADDRSTRLEN]={""};
	int yes=1; // 供底下的 setsockopt() 设置 SO_REUSEADDR
	int i,j, jc,rv;

	vector<vector<string>> buffer;
    vector<string> temp_buffer(3);
    vector<string> msg_p;

	struct addrinfo hints, *ai, *p;

	FD_ZERO(&master); // 清除 master 与 temp sets
	FD_ZERO(&read_fds);

	// 给我们一个 socket，并且 bind 它
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	string myIP;
	


	if ((rv = getaddrinfo(NULL, (const char*)myPORT.c_str(), &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	for(p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
		continue;
		}
	serversocketfd = listener;
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    myIP = s;



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
        FD_SET(fileno(stdin), &read_fds);

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
		perror("select");
		exit(4);
		}
   		
   		int mark;
	      if(msg_p[0] == "LIST"){
	          mark =1;
	      }
	      if(msg_p[0] == "STATICS"){
	          mark =2;
	      }
	      if(msg_p[0] == "IP"){
	          mark =3;
	      }
	      if(msg_p[0] == "AUTHOR"){
	          mark =4;
	      }
	      if(msg_p[0] == "PORT"){
	          mark =5;
	      }
	      if(msg_p[0] == "BLOCKED"){
	      	mark = 6;
	      }

		// 在现存的连接中寻找需要读取的数据
		if (FD_ISSET(fileno(stdin), &read_fds)) {
   		    read(fileno(stdin), charmsg, sizeof charmsg);
   		    msg = charmsg;
  		    fflush(stdin);
  		    split_msg(msg," ", msg_p);
 	     	switch (stoi(msg_p[0])){
    		  	case 3:{
      			    log_IP(myIP);
      			    break;
				}
     		  	case 4: {
     		  		log_AUTHOR();
     		  		break;
				   }
				case 5:{
					log_PORT(myPORT);
					break;
				}
				case 1:{
					log_LIST(ClientList);
					break;
				}
				case 2:{
					log_STATICS(ClientList);
					break;     
				} 
				case 6:{

					log_BLOCKED(ClientList,msg_p[1]);
					break;
				}
    		}
      //handleCommands(msg, 1);
    	}else{
			for(i = 0; i < fdmax; i++) {
				if (FD_ISSET(i, &read_fds)) { // 我们找到一个！！
				if (i == listener) {
				// handle new connections
				addrlen = sizeof remoteaddr;
				newfd = accept(listener,
				(struct sockaddr *)&remoteaddr,
				&addrlen);

				if (newfd == -1) {
					perror("accept");
				}else{
					FD_SET(newfd, &master); // 新增到 master set
					if (newfd > fdmax) { // 持续追踪最大的 fd
						fdmax = newfd;
					}
				}


			}else {
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

			}else if (FD_ISSET(serversocketfd, &read_fds)) {
     			 if(recv(serversocketfd, charmsg, sizeof charmsg, 0) == 0){
     				break;   
        			   
    			  }else{
    			  	  msg = charmsg;
     			       split_msg(msg," ", msg_p);
    				  switch (stoi(msg_p[0])){
				        //message
				        case 0:{

							for (int i = 0; i <=3 ;i ++){
								if (ClientList[i][2] == msg_p[1]){
									if(ClientList[i][5] == "1"){
										int tempsockfd = stoi(ClientList[i][9]);
										send(tempsockfd, (const char*)msg.c_str(), msg.length(), 0);
										msg = msg_p[3];
										for(int m = 4; m < msg_p.size(); m++){
											msg = msg +" "+ msg_p[m];
										}
										log_EVENT(msg_p[1], msg, msg_p[2]);
									}
									else{
										temp_buffer[0] = msg_p[1];
										temp_buffer[1] = msg_p[2];
										msg = msg_p[3];
										for(int n = 4; n < msg_p.size(); n++){
											msg = msg +" "+ msg_p[n];
										}
										temp_buffer[2] = msg;
										buffer.push_back(temp_buffer);
									}

								}else{
									cse4589_print_and_log("[%s:SUCCESS]\n", msg_p[0]);
									cse4589_print_and_log("[%s:ERROR]\n", msg_p[0]);
								}
							}
							break;
						}
				        // if(send(sockfd, msg, strlen(msg), 0) == strlen(msg))
            // 				printf("Done!\n");    
				        //     break;
				        // }

				        //hostname
				        case 1:{
							string host = msg_p[1];
							string host_ip = msg_p[2];
							string port = msg_p[3];
							for(int i = 0; i < 4; ++i) {
								if(ClientList[i][1] == host_ip) {
									ClientList[i][5] = 1;
									for(int j = 0; j < buffer.size(); ++j) {
										if(buffer[j][1] == host_ip){
											send(stoi(ClientList[i][9]), (const char*)buffer[j][2].c_str(), buffer[j][2].length(), 0);
											buffer.erase(buffer.begin()+ j-1);
										}
									}
									break;
								}
							}
							for(int i = 0; i < 4; ++i){
								if(ClientList[i][0] == ""){
									ClientList[i][0] = host;
									ClientList[i][1] = host_ip;
									ClientList[i][2] = port;
									ClientList[i][5] = "1";
									break;
								}
							}
				            break;
				        }        

				        //block ip
				        case 2:{
							string from_ip = msg_p[1];
							string to_ip = msg_p[2];
							for(int i = 0; i < 4; ++i) {
								if(ClientList[i][1] == from_ip){
									for(int j = 6; j < 9; ++j) {
										if(ClientList[i][j] == ""){
											ClientList[i][j] = to_ip;
											break;
										}
									}
								}
							}
				            break;
				        }

				        //unblock ip
				        case 3:{
							string from_ip = msg_p[1];
							string to_ip = msg_p[2];
							for(int i = 0; i < 4; ++i) {
								if(ClientList[i][1] == from_ip){
									for(int j = 6; j < 9; ++j) {
										if(ClientList[i][j] == to_ip){
											ClientList[i][j] = "";
											break;
										}
									}
								}
							}
				            break;
				        }

				        //log out
				        case 4:{
							string ip_addr = msg_p[1];
							for(int i = 0; i < 4; ++i) {
								if(ClientList[i][1] == ip_addr){
									ClientList[i][5] = "0";
									break;
								}
							}
				            break;
				        }

				        //exit
				        case 5:{
							string ip_addr = msg_p[1];
							for(int i = 0; i < 4; ++i){
								if(ClientList[i][1] == ip_addr) {
									for(int j = 0; j < 10; ++j) {
										ClientList[i][j] = "";
									}
									break;
								}
							}
				            break;
				        }      
				        //broadcast
				        case 6:{
				        	for(int i = 0 ; i<4 ;i++){
				        		if(ClientList[i][1] != ""){
				        			if(ClientList[i][5] == "1"){
				        				int tempsockfd = stoi(ClientList[i][9]);
				        				send(tempsockfd, (const char*)msg.c_str(), msg.length(), 0);
				        				msg = msg_p[2];
										for(int n = 3; n < msg_p.size(); n++){
											msg = msg +" "+ msg_p[n];
										}
				        				log_EVENT(msg_p[1],msg,ClientList[i][1]);
				        			}else{
				        				temp_buffer[0] = msg_p[1];
										temp_buffer[1] = ClientList[i][1];
										msg = msg_p[2];
										for(int n = 3; n < msg_p.size(); n++){
											msg = msg +" "+ msg_p[n];
										}
										temp_buffer[2] = msg;
										buffer.push_back(temp_buffer);
				        			}
                           		}
				        	}
				            break;
				        } 

				        //refresh
				        case 7:{
				            msg="1";
				        	int tempsockfd;
				        	for(int i = 0 ; i<4 ;i++){
				        		if(ClientList[i][1] != ""){
				        			if(msg_p[1] == ClientList[i][1]){
				        				tempsockfd = stoi(ClientList[i][9]);
				        			}
				        			for (int j =0; j < 3 ;j++){
                                  		  msg= msg + blank + ClientList[i][j];

                            		}
                           		}
				        	}
				        	send(tempsockfd, (const char*)msg.c_str(), msg.length(), 0);
				            break;
				        }


				      }
				      }
				        //handleClientEvents(msg);
				      }
				    }
			} // END handle data from client
		} // END got new incoming connection
		} // END looping through file descriptors
	}
	} // END for( ; ; )--and you thought it would never end!
    
int main(int argc,char** argv){
  string MYPORT = argv[1];
  server_process(MYPORT);
}