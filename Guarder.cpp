#include "Guarder.h"
#include "Util.h"
#include <sys/socket.h>
#include "Request.h"
#include <cstring>

namespace Jex{

Guarder::Guarder(int port)
	:listenfd(create_listenfd(port)),
	m_poll(new Epoll(EPOLL_TIME_INF)),
	listen_req(new Request(-1, 0, NULL)){
	
 	listen_req->setfd(listenfd);
	listen_req->set_epoll_req(EPOLLIN | EPOLLET);
	listen_req->set_handler(&connect_handler);//connect to new client
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

	if(accept_fd = accept(listenfd, (struct sockaddr *)&client_addr), &client_addr_len) == -1 ){
		perror("accept error");
		return;
	}
	
	
}

}
