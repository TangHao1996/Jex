#pragma once

#include <memory>
#include <queue>
#include <string>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "Util.h"
#include <stdio.h>


namespace Jex{


enum BufState{
	FREE = 1,
	FLUSH
};


class LogBuffer{
public:
	typedef std::shared_ptr<LogBuffer> ptr;
	LogBuffer(size_t size):buf(new char[size]),m_size(size), cur(buf), m_state(FREE){};
	~LogBuffer(){delete[] buf;}
	BufState get_state(){return m_state;}
	size_t get_avai(){return cur - buf;}
	void append(const char *line, size_t len);
	void flush(FILE *file);
private:
	char *buf;
	size_t m_size;
	char *cur;
	BufState m_state;
};

class Logger{
public:
	typedef LogBuffer::ptr buffer_t;
	~Logger();
	void init();
	static std::shared_ptr<Logger> get_ins(const char *file, int line){
		if(!ins){
			ins.reset(new Logger());
		}
		ins->set_file_line(file, line);	

		return ins;
	}
	void append(const char *file, int line, const char *msg);
	static void *flush_buffer(void *ptr);
	Logger& operator <<(const char *msg){
		append(cur_file.c_str(), cur_line, msg);
		return *this;
	}
	void set_file_line(const char *file, int line){cur_file = file; cur_line = line;}

private:
	Logger();//隐藏构造函数
	friend class std::shared_ptr<Logger>;
	void flush_buffer_run();
	static std::shared_ptr<Logger> ins;
	FILE *fp;
	std::queue<buffer_t> free_queue;
	std::queue<buffer_t> flush_queue;
	std::condition_variable cond;
	std::mutex free_mtx;
	std::mutex flush_mtx;
	pthread_t flush_tid;
	bool quit;
	struct tm m_tm;
	std::string cur_file;
	int cur_line;
};
//std::shared_ptr<Logger> Logger::ins(nullptr);
#define INIT \
	do{ \
		Logger::get_ins()->init(); \
	}while(0)

#define LOG (*Logger::get_ins(__FILE__, __LINE__))

}
