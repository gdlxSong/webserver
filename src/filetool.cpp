#include "filetool.hpp"
#include<iostream>


int gdl::fileTool::getFileSize(const char* path)
{
	std::cout << "Path = " << path << std::endl;
	struct stat stb;
	bzero(&stb, sizeof(struct stat));

	if (-1 == stat(path, &stb)) {
		return -1;
	}
	if (!S_ISREG(stb.st_mode))
		return -1;
	if (S_ISDIR(stb.st_mode))
		return -1;
	return stb.st_size;
}
