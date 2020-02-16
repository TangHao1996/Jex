#pragma once
#include <string>

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

class HttpStream{
public:
	HttpStream();
	~HttpStream();

	void readHandler();
	void writeHandler();

private:
	std::string inBuffer;
	std::string outBuffer;

	HttpMethod m_method;
	std::string m_filename;
	HttpVersion m_version;

};


}
