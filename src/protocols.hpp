


#ifndef PROTOCOL_GDL_HPP
#define PROTOCOL_GDL_HPP

#include<string>
#include"socketChannel.hpp"

#include<vector>
#include<iostream>
#include"log.hpp"
#include"util.hpp"
#include <sys/types.h>
#include <sys/socket.h>

#include"buffer.hpp"




namespace gdl {

	namespace Protocols {
		using Buffer = std::shared_ptr<gdl::BufferTool::Buffer>;
		using Channel = std::shared_ptr<gdl::SocketChannel>;

		class protocol {

		public:
			protocol() = default;
			virtual~protocol() = default;

			virtual std::shared_ptr<protocol> analysis(Buffer) = 0;//预解析器.
			virtual Buffer response(const std::string& body) = 0;
		};




		class HttpPaser : public protocol {

		public:
			HttpPaser();
			virtual~HttpPaser();
			std::shared_ptr<protocol> analysis(Buffer) override;//预解析器.
			Buffer response(const std::string& body);

		private:
			HttpPaser(Buffer buffer) : buf(buffer) {}
			Buffer buf;

		};





		class PasrserContainer {




			//观察者模式设计.
			using Constructor = std::function<std::shared_ptr<protocol>(Buffer)>;
		public:
			PasrserContainer(gdl::ObjectPool<gdl::BufferTool::Buffer>& bufpool) : bufferpool(bufpool) {

			}
			virtual~PasrserContainer();

			void registerConstructor(Constructor&&);

			std::shared_ptr<protocol> pasrser(Buffer&&);
			std::shared_ptr<protocol> pasrser(Channel);

		private:
			std::shared_ptr<protocol> analysis(Channel);
			Buffer recvDataFromChannel(Channel);
			void write(Channel channel, Buffer buffer);

		private:
			gdl::ObjectPool<gdl::BufferTool::Buffer>& bufferpool;
			std::vector<Constructor> constructorVector;

		};



	}




}










#endif