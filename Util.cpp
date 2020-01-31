#include "Util.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

namespace Jex{


int create_listenfd(int port){
	if(port < 0 || port > 65535)
		return -1;
	
	int listenfd = 0;
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){//ipv4,TCP
		perror("listenfd socket create error");
		return -1;
	}
	
	struct sockaddr_in server_addr;
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//使用自动分配可用IP
	server_addr.sin_port = htons((unsigned short)port);
	if(bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("listenfd bind error");
		close(listenfd);
		return -1;
	}

	//设置最长建立连接队列
	if(listen(listenfd, 2048) == -1){
		perror("set listen limit error");
		close(listenfd);
		return -1;
	}


	return listenfd;
}

}
