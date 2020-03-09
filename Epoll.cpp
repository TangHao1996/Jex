#include "Epoll.h"
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

namespace Jex {

const int Epoll::MAX_REQS = 4096;

Epoll::Epoll(int time):epollfd(epoll_create1(EPOLL_CLOEXEC)), 
	ready_events(MAX_REQS),
	all_req(MAX_REQS, NULL),
	wait_time(time) {
	if(epollfd <= 0){
		perror("epoll create error");
	}
}

Epoll::~Epoll(){
	close(epollfd);
	std::cout<<"epoll quit."<<std::endl;
}

void Epoll::epoll_add(Request::ptr req){
	if(all_req[req->getfd()] != NULL){
		printf("connected aleady.\n");
		return;
	}
	struct epoll_event event;
	event.data.fd = req->getfd();
	event.events = req->get_epoll_events();

	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, req->getfd(), &event) < 0){
		perror("epoll add error");
	}

	all_req[req->getfd()] = req;
}

void Epoll::epoll_modify(int fd, __uint32_t events){

	if(all_req[fd] == nullptr){
		std::cout<<"can't modify null epoll Request\n";
		return;
	}

	struct epoll_event event;
	event.data.fd = fd;
	event.events = events;
	if(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event) < 0){
		perror("epoll mod error");
	}
	//all_req[fd]->set_epoll_event(events);
}

void Epoll::epoll_delete(int fd){
	if(all_req[fd] == nullptr){
		std::cout<<"cannot delete null epoll Request\n";
		return;
	}
	struct epoll_event event;
	event.data.fd = fd;
	event.events = all_req[fd]->get_epoll_events();
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event) < 0){
		perror("epoll delete error");
	}
	all_req[fd] = nullptr;
	close(fd);
}

int Epoll::poll(){
	int req_count = 0;
	//while(true){
	req_count = epoll_wait(epollfd, &*ready_events.begin(), ready_events.size(), wait_time);
	if(req_count < 0){
		perror("epoll wait error");
		return -1;
	}
	//}
	//std::cout<<"number of ready request: "<<req_count<<std::endl;
	return req_count;
}

void Epoll::getReadyRequest(std::vector<Request::ptr> &ready_requests, int events_cnt){//IMPL

	for(int i = 0; i < events_cnt; i++){
		int fd = ready_events[i].data.fd;
		Request::ptr req(all_req[fd]);
		
		if(req){
			req->set_ready_event(ready_events[i].events);
			ready_requests.push_back(req);
		}
		else{
			perror("epoll returned a null Request");
		}
	}
}



}
