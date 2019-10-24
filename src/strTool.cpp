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
			p += vsnprintf(p, limit - p, fmt, ap);  //����vsnprintf�����ڻ���������������·��ظ�ֵ���ı�p������iter =2
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

bool gdl::strTool::startWith(const std::string& str, const std::string& partial) {
	return 0 == str.find(partial);
}

bool gdl::strTool::endWith(const std::string& str, const std::string& partial) {
	return (str.size() - partial.size()) == str.rfind(partial);
}


std::vector<std::string> gdl::strTool::split(const std::string& str, std::string demiliters) {

	//demiliters Ϊ�ָ�紮.
	//return �в�����demiliters.

	std::vector<std::string> vret;
	size_t pos_cur = 0, pos_pre = 0;
	while (true) {
		pos_cur = str.find(demiliters, pos_pre);
		if (std::string::npos == pos_cur) {
			vret.emplace_back(str.substr(pos_pre));
			return vret;
		}

		if (pos_pre != pos_cur) {
			//�������string�����.
			vret.emplace_back(str.substr(pos_pre, pos_cur - pos_pre));
		}

		if (pos_cur + demiliters.size() == str.size())
			return vret;
		//����
		pos_pre = pos_cur + demiliters.size();
	}
}


std::vector<std::string> gdl::strTool::rsplit(const std::string& str, std::string demiliters, int n) {

	std::vector<std::string> vret;

	//if n == 0
	if (0 >= n) {
		vret.push_back(str);
		return vret;
	}

	auto cret = gdl::strTool::getposs(str, demiliters);

	size_t pos_cur = 0, pos_pre = 0;
	for (int index = 0; index < cret.size(); index++) {

		//set pos_cur.
		pos_cur = cret.at(index);
		if (n-- > 0) {
			if (pos_cur != pos_pre)
				vret.emplace_back(str.substr(pos_pre, pos_cur - pos_pre));
			//if index == cret.size - 1.
			if (index + 1 == cret.size()) {
				//this is last index.
				if (pos_cur + demiliters.size() != str.size())
					vret.emplace_back(str.substr(pos_cur + demiliters.size()));
			}
		}
		else {
			//end .
			vret.emplace_back(str.substr(pos_pre));
			return vret;
		}
		pos_pre = pos_cur + demiliters.size();
	}
	return vret;
}


std::vector<std::size_t> gdl::strTool::getposs(const std::string& str, std::string subStr) {

	std::vector<std::size_t> vret;
	size_t pos_cur = 0, pos_pre = 0;
	while (true) {
		pos_cur = str.find(subStr, pos_pre);

		if (std::string::npos == pos_cur) {
			return vret;
		}

		vret.push_back(pos_cur);

		if (pos_cur + subStr.size() == str.size())
			return vret;
		//����
		pos_pre = pos_cur + subStr.size();
	}
}

int gdl::strTool::count(const std::string& str, std::string subStr) {

	auto vret = getposs(str, subStr);
	return vret.size();
}


bool gdl::strTool::isNums(std::string str) {

	for (auto it : str) {
		if (48 > it || 57 < it)
			return false;
	}
	return true;
}


std::string gdl::strTool::to_lower(std::string str) {


	size_t sz = str.size();
	for (size_t i = 0; i < sz; i++) {		//�õ�������дҲ������Ч����Σ��Ȼ��������.

		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 32;
	}
	return str;
}