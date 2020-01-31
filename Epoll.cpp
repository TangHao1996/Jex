#include "Epoll.h"
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>

namespace Jex {

const int MAX_REQS = 4096;

Epoll::Epoll(int time = -1):epollfd(epoll_create1(EPOLL_CLOEXEC)), req_events(MAX_REQS), wait_time(time) {
	if(epollfd <= 0){
		perror("epoll create error");
	}
}

Epoll::~Epoll(){

}

void Epoll::epoll_add(int fd, epoll_event* event){
	(*event).data.fd = fd;
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, event) < 0){
		perror("epoll add error");
	}
}

void Epoll::epoll_modify(int fd, epoll_event* event){
	(*event).data.fd = fd;
	if(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, event) < 0){
		perror("epoll mod error");
	}
}

void Epoll::epoll_delete(int fd, epoll_event* event){
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, event) < 0){
		perror("epoll delete error");
	}
}

void Epoll::poll(){
	while(true){
		int req_count = epoll_wait(epollfd, &*req_events.begin(), req_events.size(), wait_time);
		if(req_count < 0)
			perror("epoll wait error");
		return;
	}
}


}
