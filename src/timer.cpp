#include "timer.hpp"
#include<stdio.h>
#include<math.h>







std::string gdl::Timestamp::now()
{
	return getTimestamp(time(NULL));
}

std::string gdl::Timestamp::getTimestamp(const time_t t)
{
	char buf[1024] = { 0 };
	struct tm tt;
	localtime_r(&t, &tt);
	snprintf(buf, 1023, "%04d/%02d/%02d-%02d:%02d:%02d ", tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec);
	return std::string(buf);
}

void gdl::TimerSeq::TimerSqueue::set(Handler&& handler, time_t interval)
{
	//auto push.
	handlerList.emplace_back(std::make_pair(interval, std::move(handler)));
	auto iter = handlerList.end() -- ;
	handlerMap.emplace(time(NULL) + iter->first, std::move(iter));
}

void gdl::TimerSeq::TimerSqueue::handleTimeOut()
{
	time_t _cur = time(NULL);
	for (auto it = handlerMap.begin(); it != handlerMap.end(); it++) {
		if (_cur > it->first || (it->first > _cur && it->first - _cur < 5))
			return;
		//re do
	}
}

void gdl::TimerSeq::TimerSqueue::interval()
{
}
