#pragma once

#include <memory>

class Timer{
public:
	typedef std::shared_ptr<Timer> ptr;
	typedef std::function<void()> Callback;
	Timer(int timeout, Callback handler);
	~Timer();
private:
	int timeout;
	Callback timeout_handler;

	Timer *pre;
	Timer *next;

	void Start();
	void Stop();

	void Refresh();
};
