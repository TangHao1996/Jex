#include "Request.h"

namespace Jex{

Request::Request(int fd, epoll_req_t events, Callback_ptr_t func)
	: m_fd(fd), m_req(events), m_handler(func){

}

}
