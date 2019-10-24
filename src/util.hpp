



#ifndef HELPER_GDL_HPP
#define HELPER_GDL_HPP

#include<string>
#include<memory>
#include<stdio.h>
#include<unistd.h>
#include <stdarg.h>
#include<vector>
#include<string.h>


namespace gdl {




	struct noncopyable {
		noncopyable() = default;
		virtual ~noncopyable() = default;
		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable&) = delete;
	};


	using string = std::string;

	namespace strTool {

		std::string format(const char* fmt, ...);
		bool startWith(const std::string& str, const std::string& partial);
		bool endWith(const std::string& str, const std::string& partial);
		std::vector<std::string> split(const std::string& str, std::string demiliters);
		std::vector<std::string> rsplit(const std::string& str, std::string demiliters, int n);
		std::vector<std::size_t> getposs(const std::string& str, std::string subStr);
		int count(const std::string& str, std::string subStr);
		bool isNums(std::string str);
		std::string to_lower(std::string str);
	}






}






#endif