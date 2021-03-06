#include "HttpSession.h"
#include "Util.h"
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

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
	m_cstate = CONNECTED;
}

HttpSession::~HttpSession(){

}


void HttpSession::readHandler(){

	int read_num = readn(m_epoll_fd, inBuffer);//原buffer里的内容已读空
	reset();
	if(read_num < 0){
		perror("http buffer read error");
		return;
	}
	else if(read_num == 0){
		errorHandler(DISCONNECT);
		return;
	}
	
	////////////////////////////////////////////////////
	//std::cout<<"*************recieve request*************"<<std::endl;
	//std::cout<<inBuffer;

	if(requestLine() < 0){//method uri http version
		std::cout<<"bad request line"<<std::endl;
		errorHandler(BAD_REQUEST);
		return;
	}
	if(parse_headers() < 0){
		std::cout<<"parse header err"<<m_headers.size()<<std::endl;
		errorHandler(BAD_REQUEST);
		return;
	}
	////////////////////////////////////////////////////
	//for(auto it : m_headers){
	//	std::cout<<it.first<<":** "<<it.second<<std::endl;
	//}
	if(prepare() < 0){
		errorHandler(NOT_FOUND);
		return;
	}
	inBuffer.clear();
	writeHandler();	

}

void HttpSession::writeHandler(){
	writen(m_epoll_fd, outBuffer);
	if(outBuffer.size() == 0)
		std::cout<<"handle write finish."<<std::endl;
}

void HttpSession::errorHandler(HttpError err){
	reset();
	inBuffer.clear();
	outBuffer.clear();
	
	if(err == DISCONNECT){
		std::cout<<"disconnected, fd: "<<m_epoll_fd<<std::endl;
		m_cstate = DISCONNECTED;
	}

	if(err == NOT_FOUND){
		std::string content_("<html><title>404 Not Found</title>");
		outBuffer += "HTTP/1.1 404 Not Found!\r\n";
		outBuffer += "Content-Type: text/html\r\n";
		outBuffer += "Content-Length: " + std::to_string(content_.size());
		outBuffer += "Server: Jex\r\n";
		outBuffer += "\r\n";
		outBuffer += content_;
	}
	writeHandler();
}

int HttpSession::requestLine(){

	read_cur_index = inBuffer.find('\n') + 1;
	std::string line_ = inBuffer.substr(0, read_cur_index + 1);
	if(read_cur_index < 10){
		return -1;
	}

	size_t method_end = 0;
	if(line_.find("GET") >= 0){
		method_end = line_.find("GET") + 3;
		m_method = GET;
	}
	else if(line_.find("POST") >= 0){
		method_end = line_.find("POST") + 4;
		m_method = POST;
	}
	else if(line_.find("HEAD") >= 0){
		method_end = line_.find("HEAD") + 4;
		m_method = HEAD;
	}
	else{
		return -1;
	}

	size_t uri_start = line_.find('/') + 1;
	if(inBuffer[uri_start] == ' '){
		m_filename = "index.html";//导航页
		m_version = HTTP11;
		return 1;
	}
	size_t uri_end = line_.find(' ', uri_start);
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
		std::cout<<"file: "<<m_filename<<std::endl;

		if(m_filename == "helloworld" || m_filename == "index.html"){
			outBuffer += "Content-type: text/plain\r\n";
			outBuffer += "Content-Length: 11\r\n";//不加length的后果是浏览器会一直读直到连接断开
			outBuffer += "\r\n";
			outBuffer += "Hello World";
			return 1;
		}

		if(m_headers["Connection"]== "Keep-Alive" || m_headers["Connection"] == "keep-alive"){
			outBuffer += "Connection: Keep-Alive\r\n";
			outBuffer += "Keep-Alive: timeout=3000\r\n";
		}

		struct stat file_;
		if (stat(m_filename.c_str(), &file_) < 0) {
			perror("stat error");//找不到文件应该给对方发送错误信息
   			outBuffer.clear();
      		return -1;
    	}

		std::string filetype = "text/plain";
		int dot_pos = m_filename.find('.');
		if(dot_pos >= 0){
			std::string type_ = m_filename.substr(dot_pos);
			if(type_ == ".html" || type_ == ".htm")
				filetype = "text/html";
			else if(type_ == ".avi")
				filetype = "video/x-msvideo";
			else if(type_ == ".bmp")
				filetype = "image/bmp";
			else if(type_ == ".ico")
				filetype = "image/x-icon";
			else if(type_ == ".jpg")
				filetype = "image/jpeg";
			else if(type_ == ".png")
				filetype = "image/png";
			else if(type_ == ".txt" || type_ == ".c")
				filetype = "text/plain";
			else if(type_ == ".mp3")
				filetype = "audio/mp3";
			else
				filetype = "text/html";
		}
		outBuffer += "Content-Type: " + filetype + "\r\n";
		outBuffer += "Content-Length: " + std::to_string(file_.st_size) + "\r\n";
		outBuffer += "Server: Jex\r\n";
		outBuffer += "\r\n";

		int src_fd = open(m_filename.c_str(), O_RDONLY, 0);
		if(src_fd < 0){
			perror("open file error");
			outBuffer.clear();
			return -1;
		}
		void *file_mem = mmap(NULL, file_.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
		close(src_fd);
		if(reinterpret_cast<long>(file_mem) < 0){
			perror("mmap error");
			munmap(file_mem, file_.st_size);
			outBuffer.clear();
			return -1;
		}
		char *file_ptr = static_cast<char *>(file_mem);
		outBuffer += std::string(file_ptr, file_ptr + file_.st_size);
		munmap(file_mem, file_.st_size);
		return 1;
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
