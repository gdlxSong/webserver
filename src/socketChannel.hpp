
#ifndef SOCKETCHANNEL_HPP
#define SOCKETCHANNEL_HPP


#include"SslChannel.hpp"
#include"poller.hpp"
#include<functional>



namespace gdl {

	class EPoller;

	using Callable = std::function<void()>;

	class SocketChannel {

	public:
		SocketChannel(SslChannel* _sslChannel, EPoller* _epoller, int _fd, uint32_t _events);
		SocketChannel(SocketChannel* psc) {
			*this = *psc;
		}
		virtual ~SocketChannel();

		void enableRead(bool flag = true);
		void enableWrite(bool flag = true);
		void enableWriteRead(bool flag = true);

		bool readable();
		bool writeable();

		void onRead(Callable&& cb) { readcb = cb; enableRead(); }
		void onWrite(Callable&& cb) { writecb = cb; }


		void handleRead();
		void handleWrite();

		int Fd() { return this->fd_; }
		uint64_t Id() { return channelId; }
		uint32_t Event() { return events; }


		void close();

	private:
		int fd_;
		uint32_t events;
		EPoller* epoller;
		SslChannel* sslChannel;
		uint64_t channelId;
		time_t lastrotate;
		Callable readcb, writecb, errorcb;
	};


}









#endif