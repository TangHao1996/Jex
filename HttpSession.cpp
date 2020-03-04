#include "HttpSession.h"
#include "Util.h"
#include <stdio.h>
#include <iostream>

enum Header_stage{
	H_KEY_START = 1,
	H_KEY_END,
	H_VALUE_START,
	H_VALUE_END,
	H_CR,
	H_RESTART,
	H_END
};


namespace Jex{
HttpSession::HttpSession(){

}

HttpSession::~HttpSession(){

}


void HttpSession::readHandler(){

	int read_num = readn(m_epoll_fd, inBuffer);//原buffer里的内容已读空
	if(read_num < 0){
		perror("http buffer read error");
		return;
	}
	else if(read_num == 0){
		m_cstate = DISCONNECTED;
		reset();
		errorHandler();
		std::cout<<"disconnected"<<std::endl;
		return;
	}
	
	////////////////////////////////////////////////////
	//std::cout<<"*************recieve request*************"<<std::endl;
	//std::cout<<inBuffer;

	if(requestLine() < 0){//method uri http version
		reset();
		std::cout<<"bad request line"<<std::endl;
		errorHandler();
		return;
	}
	if(parse_headers() < 0){
		std::cout<<"parse header err"<<m_headers.size()<<std::endl;
		reset();
		errorHandler();
		return;
	}
	////////////////////////////////////////////////////
	//for(auto it : m_headers){
	//	std::cout<<it.first<<":** "<<it.second<<std::endl;
	//}
	if(prepare() < 0){
		reset();
		errorHandler();
		return;
	}
	writeHandler();	

}

void HttpSession::writeHandler(){
	writen(m_epoll_fd, outBuffer);
}

void HttpSession::errorHandler(){

}

int HttpSession::requestLine(){

	read_cur_index = inBuffer.find('\n') + 1;
	std::string line_ = inBuffer.substr(0, read_cur_index + 1);
	if(read_cur_index < 10){
		return -1;
	}

	size_t method_end = 0;
	if(line_.find("GET") >= 0){
		method_end = line_.find("GET") + 5;
		m_method = GET;
	}
	else if(line_.find("POST") >= 0){
		method_end = line_.find("POST") + 6;
		m_method = POST;
	}
	else if(line_.find("HEAD") >= 0){
		method_end = line_.find("HEAD") + 6;
		m_method = HEAD;
	}
	else{
		return -1;
	}

	size_t uri_start = line_.find('/');
	if(inBuffer[method_end + 1] != '/'){
		m_filename = "index.html";//导航页
		m_version = HTTP11;
		return 1;
	}
	size_t uri_end = line_.find(' ', method_end + 1);
	if(uri_end < 0){
		return -1;
	}

	m_filename = line_.substr(uri_start, uri_end - uri_start);
	
	size_t version_start = line_.find('/', uri_end);
	std::string ver_ = line_.substr(version_start + 1, 3);
	if(version_start < 0){
		return -1;
	}
	if(ver_ == "1.0")
		m_version = HTTP10;
	else if(ver_ == "1.1")
		m_version = HTTP11;
	else
		return -1;

	return 1;
}

int HttpSession::parse_headers(){
	size_t i = read_cur_index;
	std::string &str = inBuffer;

	Header_stage stage = H_KEY_START;
	size_t key_start = 0;
	size_t key_end = 0;
	size_t value_start = 0;
	size_t value_end = 0;
	bool finish = false;

	for(; i < inBuffer.size() && !finish; i++){
		switch(stage){
			case H_KEY_START:{
				if(str[i] == '\r' || str[i] == '\n'){
					break;
				}
				key_start = i;
				stage = H_KEY_END;
				break;
			}
			case H_KEY_END:{
				if(str[i] == ':'){
					key_end = i;
					if(key_end - key_start < 0)
						return -1;
				}
				if(str[i] == ' '){
					stage = H_VALUE_START;
				}
				break;
			}
			case H_VALUE_START:{
				value_start = i;
				stage = H_VALUE_END;
				break;
			}
			case H_VALUE_END:{
				if(str[i] == '\r'){
					value_end = i;
					if(value_end - value_start < 0)
						return -1;
					stage = H_CR;
				}
				break;
			}
			case H_CR:{
				if(str[i] == '\n'){
					std::string key_ = str.substr(key_start, key_end - key_start);
					std::string value_ = str.substr(value_start, value_end - value_start);
					m_headers[key_] = value_;
					stage = H_RESTART;
				}
				else{
					return -1;
				}
				break;
			}
			case H_RESTART:{
				if(str[i] == '\r'){
					stage = H_END;
				}
				else{
					key_start = i;
					stage = H_KEY_END;
				}
				break;
			}
			case H_END:{
				if(str[i] == '\n'){
					finish = true;
				}
				else{
					return -1;
					std::cout<<str[i]<<str[i+1]<<std::endl;
				}
				break;
			}
		}
	}
	
	if(stage == H_END){
		read_cur_index = i + 1;
		return 1;
	}
	else
		return -1;
}

int HttpSession::prepare(){
	if(m_method == GET || m_method == HEAD){
		outBuffer.clear();
		outBuffer += "HTTP/1.1 200 OK\r\n";
		outBuffer += "Content-type: text/plain\r\n";
		outBuffer += "\r\n";
		outBuffer += "Hello World!";
	}

	return 1;
}

void HttpSession::reset(){
	//inBuffer.clear();
	read_cur_index = 0;
	m_filename.clear();
	m_headers.clear();
}



}
