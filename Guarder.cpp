#include "Guarder.h"
#include "Util.h"
#include <sys/socket.h>
#include "Request.h"
#include <cstring>
#include <netinet/ip.h>

namespace Jex{

Guarder::Guarder(int port)
	:listenfd(create_listenfd(port)),
	m_poll(new Epoll(EPOLL_TIME_INF)),
	listen_req(new Request(-1, 0, NULL)),
	all_req(Epoll::MAX_REQS){
	
 	listen_req->setfd(listenfd);
	listen_req->set_epoll_req(EPOLLIN | EPOLLET);
	listen_req->set_handler(std::bind(&Guarder::connect_handler, this));//connect to new client
	m_poll->epoll_add(listen_req);//epoll
}
 
Guarder::~Guarder(){

}

void Guarder::loop(){
	
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
	
	
}

}
