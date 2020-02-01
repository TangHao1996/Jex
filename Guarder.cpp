#include "Guarder.h"
#include "Util.h"
#include <sys/socket.h>
#include "Request.h"

namespace Jex{

Guarder::Guarder(int port)
	:listenfd(create_listenfd(port)),
	m_poll(new Epoll(EPOLL_TIME_INF)),
	listen_req(new Request(-1, 0, NULL)){
	
 	listen_req->setfd(listenfd);
	listen_req->set_epoll_req(EPOLLIN | EPOLLET);
	listen_req->set_handler(NULL);//NEED TO IMPL
	m_poll->epoll_add(listen_req);//epoll
}
 
Guarder::~Guarder(){

}

void Guarder::loop(){
	
}


}
