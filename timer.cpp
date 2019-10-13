#include "timer.hpp"
#include<stdio.h>








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
