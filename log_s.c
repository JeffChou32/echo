#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
	int log_sock;
	struct sockaddr_in logserver_addr, client_addr;
	socklen_t addr_len;
	char buffer[1024];
	FILE *log_file;

	log_sock = socket(AF_INET, SOCK_DGRAM, 0);
	logserver_addr.sin_family = AF_INET;
	logserver_addr.sin_addr.s_addr = INADDR_ANY;
	logserver_addr.sin_port = htons(9999);
	bind(log_sock, (struct sockaddr *)&logserver_addr, sizeof(logserver_addr));

	log_file = fopen("echo.log", "a");

	for(;;) {
		addr_len = sizeof(client_addr);
		bzero(buffer, sizeof(buffer));
		recvfrom(log_sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);
		fprintf(log_file, "%s\n", buffer);
		fflush(log_file);	
	}
	fclose(log_file);
	close(log_sock);
	return 0;

}
