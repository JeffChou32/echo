
// Server program 
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <strings.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <time.h>
//#define PORT 55000 
#define MAXLINE 1024

void log(const char *message, const char *ip) {
	struct sockaddr_in logserver_addr;
	char log_msg[1024];
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	char timestamp[32];
	int log_sock;

	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
	snprintf(log_msg, sizeof(log_msg), "%s \"%s\" was received from %s", timestamp, message, ip);

	log_sock = socket(AF_INET, SOCK_DGRAM, 0);
	logserver_addr.sin_family = AF_INET;
	logserver_addr.sin_port = htons(9999);
	logserver_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	sendto(log_sock, log_msg, strlen(log_msg), 0, (struct sockaddr *)&logserver_addr, sizeof(logserver_addr));
	close(log_sock);
}


int max(int x, int y) 
{ 
    if (x > y) 
        return x; 
    else
        return y; 
}

int main(int argc, char *argv[]) 
{ 
	int port = atoi(argv[1]);
    int listenfd, connfd, udpfd, nready, maxfdp1; 
    char buffer[MAXLINE]; 
    pid_t childpid; 
    fd_set rset; 
    ssize_t n; 
    socklen_t len; 
    struct sockaddr_in cliaddr, servaddr; 
    char ip[INET_ADDRSTRLEN];

    /* create listening TCP socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
    	printf("socket creation failed...\n");
	exit(0);
}
	else printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
 
    // binding server addr structure to listenfd 
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    listen(listenfd, 10); 
 
    /* create UDP socket */
    udpfd = socket(AF_INET, SOCK_DGRAM, 0); 
    // binding server addr structure to udp sockfd 
    bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
 
    // clear the descriptor set 
    FD_ZERO(&rset); 
 
    // get maxfd 
    maxfdp1 = max(listenfd, udpfd) + 1; 
    for (;;) { 
 
        // set listenfd and udpfd in readset 
        FD_SET(listenfd, &rset); 
        FD_SET(udpfd, &rset); 
 
        // select the ready descriptor 
        nready = select(maxfdp1, &rset, NULL, NULL, NULL); 
 
        // if tcp socket is readable then handle 
        // it by accepting the connection 
        if (FD_ISSET(listenfd, &rset)) { 
            len = sizeof(cliaddr); 
            connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len); 
            if ((childpid = fork()) == 0) { 
                close(listenfd);
		for (;;) {
			inet_ntop(AF_INET, &servaddr.sin_addr, ip, INET_ADDRSTRLEN);
			bzero(buffer, sizeof(buffer));
			int n = recv(connfd, buffer, sizeof(buffer), 0);
			if (n<=0) break;
			printf("From TCP client: %s\t To client : %s", buffer, buffer);
			send(connfd, buffer, n, 0);
			log(buffer, ip);
		}
                close(connfd); 
                exit(0); 
            } 
            close(connfd); 
        } 
        // if udp socket is readable receive the message. 
        if (FD_ISSET(udpfd, &rset)) { 
	    inet_ntop(AF_INET, &servaddr.sin_addr, ip, INET_ADDRSTRLEN);
            len = sizeof(cliaddr); 
            bzero(buffer, sizeof(buffer)); 
            printf("\nMessage from UDP client: "); 
            n = recvfrom(udpfd, buffer, sizeof(buffer), 0, 
                        (struct sockaddr*)&cliaddr, &len); 
            puts(buffer); 
            sendto(udpfd, buffer, sizeof(buffer), 0, 
                (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		log(buffer, ip);
        } 
    } 
} 
