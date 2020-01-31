#include "Guarder.h"
#include "Util.h"
#include <sys/socket.h>
#include "Request.h"

namespace Jex{

Guarder::Guarder(int port)
	:listenfd(create_listenfd(port)),
	m_poll(new Epoll(-1)){
	
	epoll_event event;
	event.events = (EPOLLIN | EPOLLET);//检测输入请求，边沿检测
	event.data.fd = listenfd;			//回调函数入口放在Request类中
	m_poll->epoll_add(listenfd, &event);//epoll
}

Guarder::~Guarder(){

}

void Guarder::loop(){
	
}


}
