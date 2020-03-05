#include "HttpSession.h"
#include "Util.h"
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>

enum Header_stage{
	H_KEY_START = 1,
	H_KEY_END,
	H_VALUE_START,
	H_VALUE_END,
	H_CR,
	H_RESTART,
	H_END
};

char favicon[555] = {
    '\x89', 'P',    'N',    'G',    '\xD',  '\xA',  '\x1A', '\xA',  '\x0',
    '\x0',  '\x0',  '\xD',  'I',    'H',    'D',    'R',    '\x0',  '\x0',
    '\x0',  '\x10', '\x0',  '\x0',  '\x0',  '\x10', '\x8',  '\x6',  '\x0',
    '\x0',  '\x0',  '\x1F', '\xF3', '\xFF', 'a',    '\x0',  '\x0',  '\x0',
    '\x19', 't',    'E',    'X',    't',    'S',    'o',    'f',    't',
    'w',    'a',    'r',    'e',    '\x0',  'A',    'd',    'o',    'b',
    'e',    '\x20', 'I',    'm',    'a',    'g',    'e',    'R',    'e',
    'a',    'd',    'y',    'q',    '\xC9', 'e',    '\x3C', '\x0',  '\x0',
    '\x1',  '\xCD', 'I',    'D',    'A',    'T',    'x',    '\xDA', '\x94',
    '\x93', '9',    'H',    '\x3',  'A',    '\x14', '\x86', '\xFF', '\x5D',
    'b',    '\xA7', '\x4',  'R',    '\xC4', 'm',    '\x22', '\x1E', '\xA0',
    'F',    '\x24', '\x8',  '\x16', '\x16', 'v',    '\xA',  '6',    '\xBA',
    'J',    '\x9A', '\x80', '\x8',  'A',    '\xB4', 'q',    '\x85', 'X',
    '\x89', 'G',    '\xB0', 'I',    '\xA9', 'Q',    '\x24', '\xCD', '\xA6',
    '\x8',  '\xA4', 'H',    'c',    '\x91', 'B',    '\xB',  '\xAF', 'V',
    '\xC1', 'F',    '\xB4', '\x15', '\xCF', '\x22', 'X',    '\x98', '\xB',
    'T',    'H',    '\x8A', 'd',    '\x93', '\x8D', '\xFB', 'F',    'g',
    '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f',    'v',    'f',    '\xDF',
    '\x7C', '\xEF', '\xE7', 'g',    'F',    '\xA8', '\xD5', 'j',    'H',
    '\x24', '\x12', '\x2A', '\x0',  '\x5',  '\xBF', 'G',    '\xD4', '\xEF',
    '\xF7', '\x2F', '6',    '\xEC', '\x12', '\x20', '\x1E', '\x8F', '\xD7',
    '\xAA', '\xD5', '\xEA', '\xAF', 'I',    '5',    'F',    '\xAA', 'T',
    '\x5F', '\x9F', '\x22', 'A',    '\x2A', '\x95', '\xA',  '\x83', '\xE5',
    'r',    '9',    'd',    '\xB3', 'Y',    '\x96', '\x99', 'L',    '\x6',
    '\xE9', 't',    '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',    '\xA7',
    '\xC4', 'b',    '1',    '\xB5', '\x5E', '\x0',  '\x3',  'h',    '\x9A',
    '\xC6', '\x16', '\x82', '\x20', 'X',    'R',    '\x14', 'E',    '6',
    'S',    '\x94', '\xCB', 'e',    'x',    '\xBD', '\x5E', '\xAA', 'U',
    'T',    '\x23', 'L',    '\xC0', '\xE0', '\xE2', '\xC1', '\x8F', '\x0',
    '\x9E', '\xBC', '\x9',  'A',    '\x7C', '\x3E', '\x1F', '\x83', 'D',
    '\x22', '\x11', '\xD5', 'T',    '\x40', '\x3F', '8',    '\x80', 'w',
    '\xE5', '3',    '\x7',  '\xB8', '\x5C', '\x2E', 'H',    '\x92', '\x4',
    '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g',    '\x98', '\xE9',
    '6',    '\x1A', '\xA6', 'g',    '\x15', '\x4',  '\xE3', '\xD7', '\xC8',
    '\xBD', '\x15', '\xE1', 'i',    '\xB7', 'C',    '\xAB', '\xEA', 'x',
    '\x2F', 'j',    'X',    '\x92', '\xBB', '\x18', '\x20', '\x9F', '\xCF',
    '3',    '\xC3', '\xB8', '\xE9', 'N',    '\xA7', '\xD3', 'l',    'J',
    '\x0',  'i',    '6',    '\x7C', '\x8E', '\xE1', '\xFE', 'V',    '\x84',
    '\xE7', '\x3C', '\x9F', 'r',    '\x2B', '\x3A', 'B',    '\x7B', '7',
    'f',    'w',    '\xAE', '\x8E', '\xE',  '\xF3', '\xBD', 'R',    '\xA9',
    'd',    '\x2',  'B',    '\xAF', '\x85', '2',    'f',    'F',    '\xBA',
    '\xC',  '\xD9', '\x9F', '\x1D', '\x9A', 'l',    '\x22', '\xE6', '\xC7',
    '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15', '\x90', '\x7',  '\x93',
    '\xA2', '\x28', '\xA0', 'S',    'j',    '\xB1', '\xB8', '\xDF', '\x29',
    '5',    'C',    '\xE',  '\x3F', 'X',    '\xFC', '\x98', '\xDA', 'y',
    'j',    'P',    '\x40', '\x0',  '\x87', '\xAE', '\x1B', '\x17', 'B',
    '\xB4', '\x3A', '\x3F', '\xBE', 'y',    '\xC7', '\xA',  '\x26', '\xB6',
    '\xEE', '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
    '\xA',  '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X',    '\x0',  '\x27',
    '\xEB', 'n',    'V',    'p',    '\xBC', '\xD6', '\xCB', '\xD6', 'G',
    '\xAB', '\x3D', 'l',    '\x7D', '\xB8', '\xD2', '\xDD', '\xA0', '\x60',
    '\x83', '\xBA', '\xEF', '\x5F', '\xA4', '\xEA', '\xCC', '\x2',  'N',
    '\xAE', '\x5E', 'p',    '\x1A', '\xEC', '\xB3', '\x40', '9',    '\xAC',
    '\xFE', '\xF2', '\x91', '\x89', 'g',    '\x91', '\x85', '\x21', '\xA8',
    '\x87', '\xB7', 'X',    '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N',
    'N',    'b',    't',    '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
    '\xEC', '\x86', '\x2',  'H',    '\x26', '\x93', '\xD0', 'u',    '\x1D',
    '\x7F', '\x9',  '2',    '\x95', '\xBF', '\x1F', '\xDB', '\xD7', 'c',
    '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF', '\x22', 'J',    '\xC3',
    '\x87', '\x0',  '\x3',  '\x0',  'K',    '\xBB', '\xF8', '\xD6', '\x2A',
    'v',    '\x98', 'I',    '\x0',  '\x0',  '\x0',  '\x0',  'I',    'E',
    'N',    'D',    '\xAE', 'B',    '\x60', '\x82',
};

namespace Jex{
HttpSession::HttpSession(){

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
	inBuffer.clear();
	writeHandler();	

}

void HttpSession::writeHandler(){
	writen(m_epoll_fd, outBuffer);
	if(outBuffer.size() == 0)
		std::cout<<"handle write finish."<<std::endl;
}

void HttpSession::errorHandler(){
	inBuffer.clear();
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
		//icon
		if(m_filename == "favicon.ico") {
			outBuffer += "Content-Type: image/png\r\n";
			outBuffer += "Content-Length: " + std::to_string(sizeof favicon) + "\r\n";
			outBuffer += "Server: jextest\n";

			outBuffer += "\r\n";
			outBuffer += std::string(favicon, favicon + sizeof favicon);
			return 1;
		}

		if(m_headers["Connection"]== "Keep-Alive" || m_headers["Connection"] == "keep-alive"){
			outBuffer += "Connection: Keep-Alive\r\n";
			outBuffer += "Keep-Alive: timeout=3000\r\n";
		}
		if(m_filename == "helloworld" || m_filename == "index.html"){
			outBuffer += "Content-type: text/plain\r\n";
			outBuffer += "Content-Length: 11\r\n";//不加length的后果是浏览器会一直读直到连接断开
			outBuffer += "\r\n";
			outBuffer += "Hello World";
			return 1;
		}

		std::string filetype;
		struct stat file_;
		if (stat(m_filename.c_str(), &file_) < 0) {
   			outBuffer.clear();
      		return -1;
    	}
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
