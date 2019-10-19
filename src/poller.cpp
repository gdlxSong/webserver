#include "poller.hpp"
#include"log.hpp"
#include"myexcept.hpp"
#include<string.h>
#include"util.hpp"
#include<errno.h>
#include<iostream>
#include"socktool.hpp"
#include"timer.hpp"

/*

	使用EPoller集中管理socket
	这里有一点：当socket.close关闭后，socket会自动从epoll中删除.



*/



gdl::EPoller::EPoller()
{
	epollfd = epoll_create1(EPOLL_CLOEXEC);
	exitif(-1 == epollfd, [&] {
			logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("create epoll error %d, %s", errno, strerror(errno)));
			logger.handleWrite();

		});
	bzero(activeEvents, sizeof(activeEvents));
}

gdl::EPoller::~EPoller()
{
	while (channelSets.size())
		(*channelSets.begin())->close();
	::close(epollfd);
	logger.mount(gdl::LogLevel::LINFO, "Epoller exited.");
}

void gdl::EPoller::addChannel(gdl::SocketChannel* channel)
{
	epoll_event ev;
	ev.events = channel->Event();
	ev.data.ptr = channel;
	ev.data.fd = channel->Fd();

	int iret = epoll_ctl(epollfd, EPOLL_CTL_ADD, channel->Fd(), &ev);
	doif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LERROR, gdl::strTool::format("epoll_ctl ADD socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});
	channelSets.insert(channel);
}

void gdl::EPoller::updateChannel(gdl::SocketChannel* channel)
{
	epoll_event ev;
	ev.events = channel->Event();
	ev.data.ptr = channel;
	int iret = epoll_ctl(epollfd, EPOLL_CTL_MOD, channel->Fd(), &ev);
	doif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LERROR, gdl::strTool::format("epoll_ctl MOD socket error %d %s", errno, strerror(errno)));
		channel->close();
		});
}

void gdl::EPoller::removeChannel(gdl::SocketChannel* channel)
{
	//remove from channelSets;
	channelSets.erase(channel);
	//remove from activeEvents
	for(int i = 0; i< SIZE_ACTIVE; i++)
		if (channel == activeEvents[i].data.ptr) {
			activeEvents[i].data.ptr = NULL;
			break;
		}
}

void gdl::EPoller::loop_once(int waitMs)
{
	Timer timer;
	//epoll_wait.
	std::cout << "epoll wait... has "<<channelSets.size()<<" connector." << std::endl;
	int n = gdl::socktool::Wepoll_wait(epollfd, activeEvents, SIZE_ACTIVE, -1);

	exitif(-1 == n, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("epoll_wait error %d %s.", errno, strerror(errno)));
		logger.handleWrite();
		});

	if (0 == n)
		logger.mount(gdl::LogLevel::LINFO, gdl::strTool::format("epoll_wait timeout. waitMs = %d ms.", waitMs));

	if (n > 0) {
		for (int i = 0; i < n; i++) {
			auto ch = static_cast<gdl::SocketChannel*>(activeEvents[i].data.ptr);
			uint32_t events = activeEvents[i].events;
			
			if (events & POLLOUT) {
				logger.mount(gdl::LogLevel::LINFO, gdl::strTool::format("event of POLLOUT. channelId = %d.", ch->Id()));
				ch->handleWrite();			
			}
			else if (events & (POLLIN | POLLERR)) {
				logger.mount(gdl::LogLevel::LINFO, gdl::strTool::format("event of POLLIN | POLLERR. channelId = %d.", ch->Id()));
				ch->handleRead();
			}
			else
				logger.mount(gdl::LogLevel::LINFO, gdl::strTool::format("event not in {POLLIN | POLLOUT | POLLERR}. error %d %s", errno, strerror(errno)));
		}
		
	}
	logger.handleWrite();
	std::cout << "looped. elapsed " << timer.elapsed() << " ms." << std::endl;
}

void gdl::EPoller::handleEpoller()
{
	//accept.

}

std::shared_ptr<gdl::EPoller> gdl::EPoller::createEPoller()
{
	return std::make_shared<gdl::EPoller>(new EPoller);
}
