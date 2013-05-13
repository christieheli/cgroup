#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DPORT 1818
//#define HOST 127.0.0.1
#define MAXDATASIZE 100
char *host_name ="127.0.0.1";//需要搜寻服务端IP地址

int main(int argc, char** argv)
{
	struct sockaddr_in servaddr;
	int sockfd, n;
	char buf[MAXDATASIZE];

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("call sock");
		exit(1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(DPORT);
	if (inet_pton(AF_INET, host_name, &servaddr.sin_addr) == -1) {
		perror("call inet_pton");
		exit(1);
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		perror("call connect");
		exit(1);
	}

	while((n = recv(sockfd, buf, MAXDATASIZE, 0)) > 0) {
		buf[n] = 0;
		if (fputs(buf, stdout) == EOF) {
			perror("call fputs");
			exit(1);
		}
	}

	if (n == -1)
		perror("call recv");

	exit(0);
}

