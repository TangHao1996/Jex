#include "Util.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <fcntl.h>

namespace Jex{

const int MAX_READ_BUFF = 4096;	

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

void setNonBlocking(int fd){
	int old_flag = fcntl(fd, F_GETFL, 0);
	if(old_flag == -1){
		printf("get fd flag failed.\n");
		return;
	}
	int flag = old_flag | O_NONBLOCK;
	if(fcntl(fd, F_SETFL, flag) == -1){
		printf("set fd flag failed.\n");
		return;
	}

}

ssize_t readn(int fd, std::string &inBuffer){
	ssize_t nread = 0;
	ssize_t readSum = 0;

	while(true){
		char buff_[MAX_READ_BUFF];
		if((nread = read(fd, buff_, MAX_READ_BUFF)) < 0){
			if(errno == EINTR){//若只是被中断打断则继续
				//perror("");
				continue;
			}
			else if(errno == EAGAIN){
				return readSum;
			}
			else{
				perror("readn error");
				return -1;
			}
		}else if(nread == 0){
			break;
		}

		readSum += nread;
		inBuffer += std::string(buff_, buff_ + nread);
	}


	return readSum;
}

ssize_t writen(int fd, std::string &outBuffer){
  size_t nleft = outBuffer.size();
  ssize_t nwritten = 0;
  ssize_t writeSum = 0;
  const char *ptr = outBuffer.c_str();
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN)
          break;
        else{
		perror("writen error");
          return -1;
		}
      }
    }
    writeSum += nwritten;
    nleft -= nwritten;
    ptr += nwritten;
  }
  if (writeSum == static_cast<int>(outBuffer.size()))
    outBuffer.clear();
  else
    outBuffer = outBuffer.substr(writeSum);
  return writeSum;
}







}
