#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Timer.h"

Timer::Timer(int interval, void (*callback)()) : interval(interval), callback(callback)
{
	lastrun = millis();
}

void Timer::run()
{
	if ((millis() - lastrun) > interval)
	{
		lastrun = millis();
		callback();
	}
}
