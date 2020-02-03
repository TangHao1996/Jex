#include "Epoll.h"
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>

namespace Jex {

const int Epoll::MAX_REQS = 4096;

Epoll::Epoll(int time):epollfd(epoll_create1(EPOLL_CLOEXEC)), ready_events(MAX_REQS), wait_time(time) {
	if(epollfd <= 0){
		perror("epoll create error");
	}
}

Epoll::~Epoll(){

}

void Epoll::epoll_add(Request::ptr req){
	struct epoll_event event;
	event.data.fd = req->getfd();
	event.events = req->get_epoll_events();
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, req->getfd(), &event) < 0){
		perror("epoll add error");
	}
}

void Epoll::epoll_modify(Request::ptr req){
	struct epoll_event event;
	event.data.fd = req->getfd();
	event.events = req->get_epoll_events();
	if(epoll_ctl(epollfd, EPOLL_CTL_MOD, req->getfd(), &event) < 0){
		perror("epoll mod error");
	}
}

void Epoll::epoll_delete(Request::ptr req, Request::epoll_req_t events){
	struct epoll_event event;
	event.data.fd = req->getfd();
	event.events = events;
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, req->getfd(), &event) < 0){
		perror("epoll delete error");
	}
}

void Epoll::poll(){
	while(true){
		int req_count = epoll_wait(epollfd, &*ready_events.begin(), ready_events.size(), wait_time);
		if(req_count < 0)
			perror("epoll wait error");
		return;
	}
}

}
