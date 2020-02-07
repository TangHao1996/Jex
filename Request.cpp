#include "Request.h"

namespace Jex{

Request::Request() : m_fd(-1), m_event(0),m_ready_event(0), m_handler(NULL){
	
}

Request::Request(int fd, epoll_req_t events, Callback_ptr_t func)
	: m_fd(fd), m_event(events), m_handler(func){

}

Request::~Request(){

}

}
