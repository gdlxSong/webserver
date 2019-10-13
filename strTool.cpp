#include"util.hpp"



using string  = std::string;

string gdl::strTool::format(const char* fmt, ...) {
	char buffer[500];
	std::unique_ptr<char[]> release1;
	char* base;
	for (int iter = 0; iter < 2; iter++) {
		int bufsize;
		if (iter == 0) {
			bufsize = sizeof(buffer);
			base = buffer;
		}
		else {
			bufsize = 30000;
			base = new char[bufsize];
			release1.reset(base);
		}
		char* p = base;
		char* limit = base + bufsize;
		if (p < limit) {//true
			va_list ap;
			va_start(ap, fmt);
			p += vsnprintf(p, limit - p, fmt, ap);  //这里vsnprintf可能在缓冲区不够的情况下返回负值，改变p，导致iter =2
			va_end(ap);
		}
		// Truncate to available space if necessary
		if (p >= limit) {
			if (iter == 0) {
				continue;  // Try again with larger buffer
			}
			else {
				p = limit - 1;
				*p = '\0';
			}
		}
		break;
	}
	return base;
}