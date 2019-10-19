

#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include"util.hpp"
#include"address.hpp"
#include"poller.hpp"
#include<utility>
#include<tuple>
#include<list>
#include<memory>
#include"myexcept.hpp"
#include"socktool.hpp"
#include<atomic>
#include<mutex>
#include"util.hpp"
#include"protocols.hpp"
#include"objectpool.hpp"
#include"buffer.hpp"
namespace gdl {




	class Connector : public gdl::noncopyable {

	public:
		Connector() = default;
		virtual ~Connector() {}

		template<class Callable>
		void handleSigle(int signal, Callable&& cb);

	private:

	};



	class ServerConn : Connector {



		using PusherAble = std::function<void(std::function<void()>)>;//线程任务投递器.
		struct ConnInfo {
			gdl::IpAddr::IpAddrv4 localAddr;
			gdl::IpAddr::IpAddrv4 remoteAddr;
			std::shared_ptr<gdl::SocketChannel> channel;
		};
	public:
		ServerConn(const std::string& host, unsigned short port, PusherAble&& psr);
		virtual ~ServerConn();

		void serverInit();

		std::shared_ptr<gdl::EPoller> getEpoller() { return epoller; }

		void handleAccept();

		void loop(int waitMs);

		void start(int waitMs);

		std::string serverDescriptions();

	private:
		std::atomic<bool> exited;
		std::shared_ptr<gdl::EPoller> epoller;
		std::shared_ptr <gdl::IpAddr::IpAddrv4> serAddr;
		std::shared_ptr<gdl::SocketChannel> serChannel;
		std::list<ConnInfo> connInfo;
		gdl::Glog logger;
		std::timed_mutex tmut;
		
		std::shared_ptr<gdl::Protocols::PasrserContainer> paserContainer;
		gdl::ObjectPool<gdl::BufferTool::Buffer> bufferpool;
		//callback.
		PusherAble pusher;
	};







	template<class Callable>
	inline void Connector::handleSigle(int signal, Callable&& cb)
	{
		cb(signal);
	}

}





#endif