#pragma once

#include <memory>
#include <queue>
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
	void append(const char *file, int line, const char *logline, size_t len);
	static void *flush_buffer(void *ptr);
	
private:
	Logger();//隐藏构造函数
	void flush_buffer_run();
	static std::shared_ptr<Logger> logger;
	FILE *fp;
	std::queue<buffer_t> free_queue;
	std::queue<buffer_t> flush_queue;
	std::condition_variable cond;
	std::mutex free_mtx;
	std::mutex flush_mtx;
	pthread_t flush_tid;
	bool quit;
};


}
