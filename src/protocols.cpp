#include "protocols.hpp"
#include<string.h>


gdl::Protocols::HttpPaser::HttpPaser()
{
}

gdl::Protocols::HttpPaser::~HttpPaser()
{
}

std::shared_ptr<gdl::Protocols::protocol> gdl::Protocols::HttpPaser::analysis(Buffer _buf)
{
	std::cout << "++++++++++++++++++" << std::endl;
	return std::shared_ptr<gdl::Protocols::protocol>(new  HttpPaser(_buf));
}



gdl::Protocols::Buffer gdl::Protocols::HttpPaser::response(const std::string& body)
{
	return std::move(buf);
}



gdl::Protocols::PasrserContainer::~PasrserContainer()
{
}

void gdl::Protocols::PasrserContainer::registerConstructor(Constructor&& constructor)
{
	constructorVector.push_back(std::move(constructor));
}




std::shared_ptr<gdl::Protocols::protocol> gdl::Protocols::PasrserContainer::pasrser(Buffer&&)
{
	return nullptr;
}

std::shared_ptr<gdl::Protocols::protocol> gdl::Protocols::PasrserContainer::pasrser(Channel channel)
{
	/*
		std::shared_ptr<gdl::Protocols::protocol*>作为一个预留的返回值，后期可能根据设计用或者不用.
	*/
	std::cout << "-----------------paser start..." << std::endl;
	auto protobuf = analysis(channel);
	if (nullptr == protobuf.get())
		return nullptr;

	//解析请求.
	auto buf = protobuf->response("");
	//向remote写入数据.
	write(channel, buf);

	std::cout << "---------------------paser end." << std::endl;
	return nullptr;
}

std::shared_ptr<gdl::Protocols::protocol> gdl::Protocols::PasrserContainer::analysis(Channel channel)
{
	std::cout << "-----------------paser analysis start..." << std::endl;
	Buffer buf = recvDataFromChannel(channel);
	//if nullptr == buf.
	if (nullptr == buf.get())
		return nullptr;//转发nullptr.

	std::cout << "recv " << buf->size() << " bytes.\n" << buf->data() << std::endl;
	for (auto& creactor : constructorVector) {

		auto ptr = creactor(buf);
		if (nullptr != ptr.get())//显式null.
			return ptr;
	}
	return nullptr;
}

gdl::Protocols::Buffer gdl::Protocols::PasrserContainer::recvDataFromChannel(Channel channel)
{
	//recv data from channel.
	Buffer buf = bufferpool.Get();
	std::cout << "got buffer from buffer pool." << std::endl;
	while (true) {

		ssize_t n = gdl::socktool::Wrecv(channel->Fd(), buf->data(), buf->usableSize(), 0);
		if (0 < n)
			buf->loaded(n);
		else if (0 == n) {
			channel->close();
			return nullptr;
		}
		else if (-1 == n) {
			if (EAGAIN == errno || EWOULDBLOCK == errno)
				return std::move(buf);
			else {
				//error occured.
				Glog().mount(gdl::LogLevel::LWARN, gdl::strTool::format("recv error from channel %d, %d", channel->Id(), errno) + strerror(errno));
				channel->close();
				return nullptr;
			}
		}
		else
			return nullptr;//do nothing....
	}
}

void gdl::Protocols::PasrserContainer::write(Channel channel, Buffer buffer)
{
	std::cout << "protocol write xxxxxxxxxxxxxxxxxxxxxxxxxxx" << std::endl;

	channel->onWrite([=] {

		std::cout << "handleWrite....." << std::endl;
		char response[] = "HTTP/1.1 200 OK\r\ncontent-type:text/html; charset=utf-8\r\ncontent-length:11\r\n\r\njust a test.";

		const size_t len_r = strlen(response);
		gdl::socktool::Wsend(channel->Fd(), response, len_r, 0);
		channel->enableWrite(false);
		std::cout << "handleWrite.....end." << std::endl;
		});
	channel->enableWrite();
}

