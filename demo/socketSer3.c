#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define WEBPORT 8111
#define RECV_BUFFER_SIZE 1024

int main(int argc, char** argv)
{
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	int sockfd, new_fd;
	int sin_size;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("call socket");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(WEBPORT);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("call bind");
		exit(1);
	}

	if (listen(sockfd, 10) == -1) {
		perror("call listen");
		exit(1);
	}

	for(; ;) {
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&cliaddr, &sin_size)) == -1) {
			perror("call accept");
			continue;
		}

		printf("connnect from ip: %s, port:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));


		char recv_buf[RECV_BUFFER_SIZE + 1];
		int read_bytes;
		read_bytes = recv(new_fd, recv_buf, RECV_BUFFER_SIZE, 0);
		if (read_bytes == -1) {
			perror("call recv");
			exit(1);
		}
		recv_buf[read_bytes] = 0;
		if (fputs(recv_buf, stdout) == EOF) {
			perror("call fputs");
			exit(1);
		}

		char *http_info = "HTTP/1.1 200 OK\r\nServer: cgroup/1.0\r\nConnection: keep-alive\r\nContent-Type: text/html\r\n\r\n<html><head><title>Hello World</title></head><body><h1>Hello World!</h1></body></html>";
		if (send(new_fd, http_info, strlen(http_info), 0) == -1) {
			perror("call send");
			exit(1);
		}

		close(new_fd);
	}
	exit(0);
}
