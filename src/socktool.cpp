







#include"socktool.hpp"

int gdl::socktool::setNonBlock(socket_t fd, bool value)
{
	int mode = fcntl(fd, F_GETFL);
	mode = value ? mode | O_NONBLOCK : mode & ~O_NONBLOCK;
	return fcntl(fd, F_SETFL, mode);
}

int gdl::socktool::setReuseAddr(socket_t fd, bool value)
{
	int flag = value ? 1 : 0;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof flag);
}

int gdl::socktool::setReusePort(socket_t fd, bool value)
{
	int flag = value ? 1 : 0;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof flag);
}

int gdl::socktool::setNoDelay(socket_t fd, bool value)
{
	int flag = value ? 1 : 0;
	return setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &flag, sizeof(flag));
}



