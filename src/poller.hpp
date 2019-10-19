#ifndef POLLER_HPP
#define POLLER_HPP


#include<memory>
#include<set>
#include"socktool.hpp"
#include"socketChannel.hpp"
#include"log.hpp"


namespace gdl {

	class SocketChannel;
	class Glog;


#define SIZE_ACTIVE 2000
	class EPoller {
		
	public:
		EPoller();
		EPoller(EPoller* ptr) {
			*this = *ptr;
		}
		virtual ~EPoller();
		void addChannel(gdl::SocketChannel* channel);
		void updateChannel(gdl::SocketChannel* channel);
		void removeChannel(gdl::SocketChannel* channel);


		void loop_once(int waitMs);
		void handleEpoller();

		static std::shared_ptr<EPoller> createEPoller();




	private:
		int epollfd;
		epoll_event activeEvents[SIZE_ACTIVE];
		std::set<gdl::SocketChannel*> channelSets;
		gdl::Glog logger;
	};


}





#endif