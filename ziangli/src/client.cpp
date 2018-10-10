#include<stdio.h>
#include <cstdlib>
#include <sys/socket.h> // 提供socket函数及数据结构
#include <string>
// #include <string.h>
#include <arpa/inet.h> // 提供IP地址转换函数

#include "../include/global.h"
#include "../include/logger.h"

#define PORT 4430 // client port

#define MAXSIZE 100 // max size of bytes transfer
#include<iostream>

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
                // Check if interface is en0 which is the wifi connection on the iPhone
                ipAddress=inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
                printf(ipAddress)
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    // Free memory
    freeifaddrs(interfaces);
    return ipAddress;
}

int main(){
    getIPAddress()
}