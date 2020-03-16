#pragma once
#include <string>
#include <memory>
#include <map>
#include <unordered_map>

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

enum HttpError{
	DISCONNECT = 0,
	BAD_REQUEST = 400,
	NOT_FOUND= 404
};

enum ConnectionState{
	CONNECTED = 1,
	DISCONNECTED = -1
};

class HttpSession{
public:
	typedef std::shared_ptr<HttpSession>	ptr;
	HttpSession();

	~HttpSession();

	void readHandler();
	void writeHandler();
	void errorHandler(HttpError err);
	void setfd(int fd){m_epoll_fd = fd; }
	int get_inBuffer_size(){return inBuffer.size();}
	int get_outBuffer_size(){return outBuffer.size();}
	bool if_disconnect(){return (m_cstate == DISCONNECTED);}

	void reset();
	int requestLine();
	int parse_headers();
	int prepare();

private:
	int m_epoll_fd;
	std::string inBuffer;
	std::string outBuffer;
	//HTTP parse info
	size_t read_cur_index;
	HttpMethod m_method;
	std::string m_filename;
	HttpVersion m_version;
	ConnectionState m_cstate;
	std::map<std::string, std::string> m_headers;

};


}
