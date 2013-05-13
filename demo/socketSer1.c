#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DPORT 1818
int main(int argc, char** argv)
{
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	int sockfd, new_fd;
	char *msg = "Hello World\n";
	int sin_size;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("call socket");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(DPORT);

	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("call bind");
		exit(1);
	}

	if (listen(sockfd, 10) == -1) {
		perror("call listen");
		exit(1);
	}

	for(;;) {
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&cliaddr, &sin_size)) == -1) {
			perror("call accept");
			continue;
		}

		printf("connection from %s, port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

		if (send(new_fd, msg, strlen(msg), 0) == -1) {
			perror("call send");
			exit(1);
		}
	}
	close(new_fd);
}
