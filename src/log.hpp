


/*


	@我对日志的设计：
	首先日志模块只是负责将日志条目加入待写入日志文件的缓冲队列里面,其日志的写操作有fd触发线程池去工作。
	此外日志文件也必须对其定时检查是否需要关闭, 定时重复执行序列。
	日志的格式：
		[Level] At timestamp, msg.

		Level: enum LogLevel { LFATAL = 0, LERROR, LUERR, LWARN, LINFO, LDEBUG, LTRACE, LALL };



*/








#ifndef __LOG___HPP
#define __LOG___HPP






















#include<thread>
#include<condition_variable>
#include<fstream>
#include<queue>
#include<string>
#include<ctime>
#include<sys/stat.h>
#include<map>
#include<utility>

#define TIME_RELEASE_FILE_MS 								30
#define MAX_FILESIZE_MB										10

namespace gdl {

//1024*1024 -> 1M
#define SIZE_MAX_LOG										1048576
#define SIZE_INTERVAL_MIN_ms								50


	enum LogLevel { LFATAL = 0, LERROR, LUERR, LWARN, LINFO, LDEBUG, LTRACE, LALL };

	struct GlogData {
		std::pair<bool, int> hopen;
		time_t lastrotate;											//last rotate time.
		time_t interval;											//interval time.
		size_t max_size;											//max buffer size.
		std::mutex mut;
		std::string logpath;
		std::string buffer;
	};

	class Glog {

	public:
		Glog() = default;
		void init(const std::string& logpath, time_t interval, size_t max_log_size = SIZE_MAX_LOG);
		std::string getPath() { return g_logdata.logpath; }
		long getInterval() { return g_logdata.interval; }
		void mount(LogLevel level, const std::string& msg);	//write msg to buffer.
		void handleWrite();// write buffer to file.
	private:
		void close();
		void configLog(const std::string& logpath, time_t interval, size_t max_log_size = SIZE_MAX_LOG);

	private:
		void fflush();

		time_t m_adjust_interval;
		static GlogData g_logdata;
		static const char* g_levelStrs_[LALL + 1];
	};

}

#endif