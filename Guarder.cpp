#include "Guarder.h"
#include "Util.h"
#include <sys/socket.h>
#include "Request.h"
#include "ThreadPool.h"
#include "HttpSession.h"
#include <cstring>
#include <netinet/ip.h>
#include <iostream>


namespace Jex{

Guarder::Guarder(int port)
	:listenfd(create_listenfd(port)),
	m_poll(new Epoll(EPOLL_TIME_INF)),
	listen_req(new Request()){
	
 	listen_req->setfd(listenfd);
	listen_req->set_epoll_event(EPOLLIN | EPOLLET);
	listen_req->set_read_handler(std::bind(&Guarder::connect_handler, this));//connect to new client
	m_poll->epoll_add(listen_req);//epoll
}
 
Guarder::~Guarder(){
	std::cout<<"server destroied."<<std::endl;
}

void Guarder::start(){
	quit = false;
}

void Guarder::stop(){
	quit = true;
}

void Guarder::loop(){
	
	std::vector<Request::ptr> ready_requests;
	while(!quit){
		int ready_cnt = m_poll->poll();
		m_poll->getReadyRequest(ready_requests, ready_cnt);

		for(auto& it : ready_requests){
			if(ThreadPool::append_task(it) == -1){
				std::cout<<"task append error"<<std::endl;
			}
		}
	}
}

void Guarder::connect_handler(){
	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	socklen_t client_addr_len = sizeof(client_addr);
	int accept_fd = 0;

	if(accept_fd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len) == -1 ){
		perror("accept error");
		return;
	}
	std::cout<<"new connection handled by thread "<<(int)pthread_self()<<std::endl;
	Request::ptr req(new Request());
	req->setfd(accept_fd);
	req->set_epoll_event(EPOLLIN | EPOLLET);
	m_poll->epoll_add(req);
	HttpSession::ptr sess(new HttpSession());
	req->bind_session(sess);
}

}
