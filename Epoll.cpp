#include "Epoll.h"
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>

namespace Jex {

const int Epoll::MAX_REQS = 4096;

Epoll::Epoll(int time):epollfd(epoll_create1(EPOLL_CLOEXEC)), 
	ready_events(MAX_REQS),
	all_req(MAX_REQS),
	wait_time(time) {
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

	all_req[req->getfd()] = req;
}

void Epoll::epoll_modify(Request::ptr req){

	int fd = req->getfd();
	if(all_req[fd] == nullptr){
		perror("can't modify null epoll Request");
		return;
	}

	struct epoll_event event;
	event.data.fd = fd;
	event.events = req->get_epoll_events();
	if(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) < 0){
		perror("epoll mod error");
	}
	all_req[req->getfd()] = req;
}

void Epoll::epoll_delete(Request::ptr req, Request::epoll_req_t events){
	struct epoll_event event;
	event.data.fd = req->getfd();
	event.events = events;
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, req->getfd(), &event) < 0){
		perror("epoll delete error");
	}
	all_req[req->getfd()] = nullptr;
}

int Epoll::poll(){
	int req_count = 0;
	while(true){
		req_count = epoll_wait(epollfd, &*ready_events.begin(), ready_events.size(), wait_time);
		if(req_count < 0)
			perror("epoll wait error");
		return -1;
	}
	return req_count;
}

void Epoll::getReadyRequest(std::vector<Request::ptr> &ready_requests, int events_cnt){//IMPL

	for(int i = 0; i < events_cnt; i++){
		int fd = ready_events[i].data.fd;
		Request::ptr req(all_req[fd]);
		
		if(req){
			req->set_ready_event(ready_events[i].events);
			req->set_epoll_event(0);
			ready_requests.push_back(req);
		}
		else{
			perror("epoll returned a null Request");
		}
	}
}



}
