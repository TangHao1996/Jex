#ifndef LOG_H
#define LOG_H

#include<list>
#include<string>
#include<sstream>
#include<fstream>
#include<stdint.h>
#include<memory>//smart ptr

namespace Jex {


enum Level{
	DEBUG = 1,
	INFO = 2,
	WARN = 3,
	ERROR = 4,
	FATAL = 5
};
	
class LogEvent {
public:
	typedef std::shared_ptr<LogEvent> ptr;
	LogEvent();
private:
	const char* m_file = nullptr;//文件名
	int32_t m_line = 0;//行号
	uint32_t m_threadId = 0;//线程号
	uint32_t m_fiberId = 0;//协程号
	uint64_t m_time;//时间戳
	std::string m_content;
};
	

//格式器
class LogFormatter {
public:
	typedef std::shared_ptr<LogFormatter> ptr;

	std::string format(LogEvent::ptr event);//生成日志文本
private:	
};

//日志输出地
class LogAppender{
public:
	typedef std::shared_ptr<LogAppender> ptr;
	virtual ~LogAppender();
	void setFormatter(LogFormatter::ptr val){m_formatter = val;}

	virtual void log(Level level, LogEvent::ptr event) = 0;
protected:
	Level m_level;
	LogFormatter::ptr m_formatter;
};


class Logger{
public:
	typedef std::shared_ptr<Logger> ptr;

	Logger(const std::string& name = "root");
	void log(Level level, LogEvent::ptr event);
	void setLevel(Level level){m_level = level;}

	void debug(LogEvent::ptr event);
	void info(LogEvent::ptr event);
	void warn(LogEvent::ptr event);
	void error(LogEvent::ptr event);
	void fatal(LogEvent::ptr event);

	void addAppender(LogAppender::ptr appender);
	void delAppender(LogAppender::ptr appender);
private:
	std::string m_name;
	Level m_level;
	std::list< LogAppender::ptr> m_appenders;//appender 集合
};

//输出到控制台的appender
class StdoutLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<StdoutLogAppender> ptr;
	void log(Level level, LogEvent::ptr event) override;
};


//输出到文件的appender
class FileLogAppender : public LogAppender {
public:
	typedef std::shared_ptr<FileLogAppender> ptr;
	FileLogAppender(const std::string& val){filename = val;}
	void log(Level level, LogEvent::ptr event) override;
	
	void reopen();
private:
	std::string filename;
	std::ofstream file_stream;
};

}



#endif
