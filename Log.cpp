#include "Log.h"
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#define LOGLINE_MAX (1024)
const char LOG_FILE_PATH[] = "Jex.log";
const size_t BUFFER_SIZE = 8 * 1024 * 1024;

namespace Jex{

void LogBuffer::append(const char *line, size_t len){
	if(cur + len > buf + m_size){
		std::cout<<"buffer has no enough space."<<std::endl;
		return;
	}
	memcpy(cur, line, len);
	cur = cur + len;
} 

void LogBuffer::flush(FILE *file){
	size_t ret_len = fwrite(buf, sizeof(char), (cur - buf), file);
	if(ret_len != static_cast<size_t>(cur - buf)){
		std::cout<<"fwrite error,"<<std::endl;
	}
	cur = buf;
	fflush(file);
}

std::shared_ptr<Logger> Logger::ins;
Logger::Logger(): fp(nullptr), quit(true){

}

Logger::~Logger(){

	buffer_t cur;
	std::lock_guard<std::mutex> lock(free_mtx);
	while(!free_queue.empty()){
		cur = free_queue.front();
		free_queue.pop();
		if(cur->get_avai() != BUFFER_SIZE){
			cur->flush(fp);
			cond.notify_one();
		}
	}

	quit = true;
	if(pthread_join(flush_tid, NULL) != 0){
		perror("flush thread join error");
	}
	while(!free_queue.empty()){
		free_queue.pop();
	}
	while(!flush_queue.empty()){
		flush_queue.pop();
	}
	
	if(fp){
		fclose(fp);
	}
}

void Logger::init(){
	fp = fopen(LOG_FILE_PATH, "w+");
	if(fp == nullptr){
		std::cout<<"open log file error."<<std::endl;
	}
	quit = false;
	if(pthread_create(&flush_tid, NULL, flush_buffer, (void*)this) < 0){
		perror("flush thread create error");
		return;
	}
}

void Logger::append(const char *file, int line, const char *msg){
	if(quit)
		return;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t timep = tv.tv_sec;
	localtime_r(&timep, &m_tm);
	char logline[LOGLINE_MAX];
	size_t head_len = snprintf(logline, 
							LOGLINE_MAX,
							"%d-%02d-%02d %02d:%02d:%02d ",
							m_tm.tm_year+1900,
							m_tm.tm_mon,
							m_tm.tm_mday,
							m_tm.tm_hour,
							m_tm.tm_min,
							m_tm.tm_sec);
	size_t body_len = snprintf(logline + head_len,
							LOGLINE_MAX,
							"%s:%d\n%s\n",
							file,
							line,
							msg);

	std::cout<<"insert log line:"<<logline<<std::endl;

	std::lock_guard<std::mutex> lock(free_mtx);//函数返回时自动解锁
	while(true){
		if(free_queue.empty()){
			LogBuffer::ptr cur(new LogBuffer(BUFFER_SIZE));
			cur->append(logline, head_len + body_len);
			free_queue.push(cur);
			break;
		}
		else{
			LogBuffer::ptr cur = free_queue.front();
			if(cur->get_avai() > head_len + body_len){
				cur->append(logline, head_len + body_len);
				break;
			}
			else{
				std::lock_guard<std::mutex> lock_flush(flush_mtx);
				flush_queue.push(cur);
				free_queue.pop();
				cond.notify_one();
			}
		}
	}

}


void *Logger::flush_buffer(void *ptr){
	Logger *logger_ = (Logger*)ptr;
	logger_->flush_buffer_run();
	return (void *)0;
}

void Logger::flush_buffer_run(){
	LogBuffer::ptr cur;
	while(true){
		std::unique_lock<std::mutex> lock(flush_mtx);
		while(flush_queue.empty() && !quit){
			cond.wait(lock);
		}

		if(flush_queue.empty() && quit){//只有队列为空且Logger退出时才结束此线程
			std::cout<<"flush thread quit."<<std::endl;
			return;
		}
		cur = flush_queue.front();
		flush_queue.pop();
		cur->flush(fp);
		{
		std::lock_guard<std::mutex> lock(free_mtx);
		free_queue.push(cur);
		}
	}

}





}
