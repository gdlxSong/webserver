#include"log.hpp"
#include<stdio.h>
#include<unistd.h>
#include"timer.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include"socktool.hpp"
#include<iostream>





void gdl::Glog::configLog(const std::string& logpath, time_t interval, size_t max_log_size)
{

	/*
		struct GlogData {
		std::pair<bool, int> hopen;
		time_t lastrotate;											//last rotate time.
		time_t interval;											//interval time.
		size_t max_size;											//max buffer size.
		std::mutex mut;
		std::string logpath;
		std::string buffer;
	};
	
	*/
	m_adjust_interval = interval;
	g_logdata.logpath = logpath;
	g_logdata.interval = interval;
	g_logdata.max_size = max_log_size;
	g_logdata.hopen = std::make_pair(false, -1);
	g_logdata.lastrotate = time(NULL);

}

void gdl::Glog::fflush()
{
	::fflush(fdopen(g_logdata.hopen.second, "a+"));
}

void gdl::Glog::init(const std::string& logpath, time_t interval, size_t max_log_size)
{
	configLog(logpath, interval, max_log_size);
}

void gdl::Glog::mount(LogLevel level, const std::string& msg)
{
	//generate record.
	std::lock_guard<std::mutex> lock(g_logdata.mut);
	std::string buf = g_levelStrs_[level] + gdl::Timestamp::now() + msg + "\n";
	//mount.
	if (g_logdata.buffer.size() > g_logdata.max_size)
		m_adjust_interval = SIZE_INTERVAL_MIN_ms + (g_logdata.interval - SIZE_INTERVAL_MIN_ms) / 2;
	g_logdata.buffer += buf;
	std::cout << buf << std::endl;
}

void gdl::Glog::handleWrite()
{


	std::lock_guard<std::mutex> lock(g_logdata.mut);
	//check file open.
	auto& fd = g_logdata.hopen.second;
	auto& flag = g_logdata.hopen.first;
	auto& buffer = g_logdata.buffer;

	if (buffer.empty()) {
		close();
		return;
	}
	if (!flag) {
		//file is closed. open file.
		fd = open(g_logdata.logpath.c_str(), O_APPEND | O_WRONLY);
		if (-1 != fd) {
			flag = true;
			gdl::socktool::setNonBlock(true);
		}else {
			//clear buffer. ignore record.
			buffer.clear();
			buffer.shrink_to_fit();
		}
	}
	
	//write buffer to logfile.
	if (flag) {
		//update lastrotate.
		g_logdata.lastrotate = time(NULL);
		if (!buffer.empty()) {
			write(fd, buffer.c_str(), buffer.size());
			//shrink_to_fit.
			buffer.clear();
			if (buffer.capacity() > SIZE_MAX_LOG) 
				buffer.shrink_to_fit();
			//update interval.
			g_logdata.interval = m_adjust_interval;
		}
	}

	fflush();
}

void gdl::Glog::close()
{
	if (g_logdata.hopen.first) {
		g_logdata.hopen.first = false;
		::close(g_logdata.hopen.second);
	}
	g_logdata.interval += SIZE_INTERVAL_MIN_ms;
}





const char* gdl::Glog::g_levelStrs_[LALL + 1] = {
	"[FATAL]  ", "[ERROR]  ", "[UERR]   ", "[WARN]   ", "[INFO]   ", "[DEBUG]  ", "[TRACE]  ", "[ALL]    ",
};



gdl::GlogData gdl::Glog::g_logdata;