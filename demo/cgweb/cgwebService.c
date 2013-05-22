#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define WEBPORT 8111
#define RECV_BUFFER_SIZE 1024
#define BUFSIZE 8096

//web站点目录 修改成自己放web文件的目录
const char *web_path = "/Users/christieheli/work/cgroup/demo/cgweb";

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

	for(;;) {
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&cliaddr, &sin_size)) == -1) {
			perror("call accept");
			exit(1);
		}
		
		printf("connect from ip: %s, port: %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

		///////////////////////////////////////////////////////////////////////
		char recv_buff[RECV_BUFFER_SIZE+1];
		int read_bytes;
		read_bytes = read(new_fd, recv_buff, RECV_BUFFER_SIZE);
		if (read_bytes == 0 || read_bytes == -1) {
			perror("call read");
		}
		if (read_bytes > 0 && read_bytes < RECV_BUFFER_SIZE) {
			recv_buff[read_bytes] = 0;
		} else {
			recv_buff[0] = 0;
		}

		char *http_info;
		char request_file_info[256];
		//分割http head信息
		http_info = strtok(recv_buff , " ");
		int i = 1;
        while(http_info) {
			if (i == 2) {
				//获取请求的文件
				stpcpy(request_file_info, http_info);
			}	
			http_info = strtok(NULL, " ");
			i++;
        }
		if (strcmp(request_file_info, "/") == 0) {
			//访问域名,默认显示index文件
			stpcpy(recv_buff, "/index.html");
		} else {
			stpcpy(recv_buff, request_file_info);
		}

		char file_name[256];
		//初始化file_name
		stpcpy(file_name, web_path);
		//合并字符串
		strcat(file_name, recv_buff);

		//打印请求的完整路径文件名
		printf("file_name: %s\n", file_name);
		
		char buffer[BUFSIZE+1];
		int file_fd, ret;
		//只读方式打开文件
		file_fd = open(file_name, O_RDONLY);
		if (file_fd == -1) {
			perror("open");
			continue;
		}

		char *http_head_info = "HTTP/1.1 200 OK\r\nServer: cgroup/1.0\r\nConnection: keep-alive\r\nContent-Type: text/html\r\n\r\n";
		//发送响应头部信息
		if (send(new_fd, http_head_info, strlen(http_head_info), 0) == -1) {
			perror("call send");
			exit(1);
		}
		//读取文件内容
		while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
			//发送文件内容 即响应的http body信息
			write(new_fd,buffer,ret);
		}

		close(new_fd);
		///////////////////////////////////////////////////////////////////////
	}
	exit(0);
}
