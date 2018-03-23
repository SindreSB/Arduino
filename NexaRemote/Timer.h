#pragma once
class Timer
{
public:
	Timer(int interval, void (*)());
	void run();
private:
	unsigned long lastrun;
	int interval;
	void (*callback)();
};

