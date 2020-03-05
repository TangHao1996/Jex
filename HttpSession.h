#pragma once
#include <string>
#include <memory>
#include <map>


namespace Jex{

enum HttpMethod{
	POST = 1,
	GET = 2,
	HEAD = 3
};

enum HttpVersion{
	HTTP10 = 1,
	HTTP11 = 2
};

enum ConnectionState{
	CONNECTED = 0,
	DISCONNECTED
};

class HttpSession{
public:
	typedef std::shared_ptr<HttpSession>	ptr;
	HttpSession();

	~HttpSession();

	void readHandler();
	void writeHandler();
	void errorHandler();
	void setfd(int fd){m_epoll_fd = fd; }

	void reset();
	int requestLine();
	int parse_headers();
	int prepare();

private:
	int m_epoll_fd;
	std::string inBuffer;
	std::string outBuffer;

	size_t read_cur_index;
	HttpMethod m_method;
	std::string m_filename;
	HttpVersion m_version;
	ConnectionState m_cstate;
	std::map<std::string, std::string> m_headers;

};


}