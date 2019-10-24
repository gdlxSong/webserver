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
	return std::shared_ptr<gdl::Protocols::protocol>(new  HttpPaser(std::move(_buf)));
}



gdl::Protocols::Buffer gdl::Protocols::HttpPaser::response(const std::string& body)
{
	
	//Method url HTTP/1.1\r\nFields.
	//get url.
	auto vret = gdl::strTool::rsplit(buf->data(), " ", 3);
	if (vret.size() <= 2) {
		buf->eat(buf->size());
		return std::move(buf);
	}

	std::string path = vret[1];
	
	if ("/" == path)path = "/index.html";
	path = "/home/gdl/www/gdl" + path;
	std::cout << "path: " << path << std::endl;
	auto vtype = gdl::strTool::split(path, ".");
	auto suffix = "." + *(vtype.end() - 1);
	std::string type = getContentType(suffix);
	std::cout << "type: " << type << std::endl;
	std::string response = "HTTP/1.1 200 OK\r\ncontent-type:";
	response += type;
	response += "\r\ncontent-length:";
	
	buf->clear();
	//----------get buffer size.
	int filesize = gdl::fileTool::getFileSize(path.c_str());
	std::cout << "fileSize=" << filesize << std::endl;
	if (-1 != filesize) {
	
		response += std::to_string(filesize);
		response += "\r\n\r\n";

		strncpy(buf->data(), response.c_str(), response.size());
		buf->loaded(response.size());
		std::fstream file(path, std::ios::in|std::ios::binary);
		if (file.is_open()) {
			while (!file.eof()) {
				file.read(buf->end(), buf->usableSize());
				std::streamsize n = file.gcount();
				std::cout << "read " << n << " bytes from " << path << std::endl;
				if (n > 0)
					buf->loaded(n);
			}
		}
	
	}
	else {
		std::string respon = "HTTP/1.1 403 Not Found\r\ncontent-length:0\r\n\r\n";
		strncpy(buf->data(), respon.c_str(), respon.size());
		buf->loaded(respon.size());
	}
	
	//std::cout << buf->data() << std::endl;
	
	return std::move(buf);
}



/*

					HTTP Status

HttpStatus = {
		//Informational 1xx  信息
		'100' : 'Continue',  //继续
		'101' : 'Switching Protocols',  //交换协议

		//Successful 2xx  成功
		'200' : 'OK',  //OK
		'201' : 'Created',  //创建
		'202' : 'Accepted',  //已接受
		'203' : 'Non-Authoritative Information',  //非权威信息
		'204' : 'No Content',  //没有内容
		'205' : 'Reset Content',  //重置内容
		'206' : 'Partial Content',  //部分内容

		//Redirection 3xx  重定向
		'300' : 'Multiple Choices',  //多种选择
		'301' : 'Moved Permanently',  //永久移动
		'302' : 'Found',  //找到
		'303' : 'See Other',  //参见其他
		'304' : 'Not Modified',  //未修改
		'305' : 'Use Proxy',  //使用代理
		'306' : 'Unused',  //未使用
		'307' : 'Temporary Redirect',  //暂时重定向

		//Client Error 4xx  客户端错误
		'400' : 'Bad Request',  //错误的请求
		'401' : 'Unauthorized',  //未经授权
		'402' : 'Payment Required',  //付费请求
		'403' : 'Forbidden',  //禁止
		'404' : 'Not Found',  //没有找到
		'405' : 'Method Not Allowed',  //方法不允许
		'406' : 'Not Acceptable',  //不可接受
		'407' : 'Proxy Authentication Required',  //需要代理身份验证
		'408' : 'Request Timeout',  //请求超时
		'409' : 'Conflict',  //指令冲突
		'410' : 'Gone',  //文档永久地离开了指定的位置
		'411' : 'Length Required',  //需要Content-Length头请求
		'412' : 'Precondition Failed',  //前提条件失败
		'413' : 'Request Entity Too Large',  //请求实体太大
		'414' : 'Request-URI Too Long',  //请求URI太长
		'415' : 'Unsupported Media Type',  //不支持的媒体类型
		'416' : 'Requested Range Not Satisfiable',  //请求的范围不可满足
		'417' : 'Expectation Failed',  //期望失败

		//Server Error 5xx  服务器错误
		'500' : 'Internal Server Error',  //内部服务器错误
		'501' : 'Not Implemented',  //未实现
		'502' : 'Bad Gateway',  //错误的网关
		'503' : 'Service Unavailable',  //服务不可用
		'504' : 'Gateway Timeout',  //网关超时
		'505' : 'HTTP Version Not Supported'  //HTTP版本不支持
};



*/

std::string gdl::Protocols::HttpPaser::getContentType(std::string suffix)
{
	//analyze suffix.

	if (".html" == suffix)
		return "text/html; charset=utf-8";
	else if (".unkown" == suffix)
		return "application/unkown";
	else if (".js" == suffix)
		return "application/x-javascript";
	else if (".htm" == suffix)
		return "text/html; charset=utf-8";
	else if (".css" == suffix)
		return "text/css; charset=utf-8";
	else if (".txt" == suffix)
		return "text/plain; charset=utf-8";
	else if (".jpg" == suffix)
		return "application/x-jpg";
	else if (".png" == suffix)
		return "application/x-png";
	else if (".gif" == suffix)
		return "image/gif";
	else if (".swf" == suffix)
		return "application/x-shockwave-flash";
	else if (".ico" == suffix)
		return "image/x-icon";
	else if (".wav" == suffix)
		return "audio/wav";
	else if (".mp3" == suffix)
		return "audio/mp3";
	else if (".jpe" == suffix)
		return "image/jpeg";
	else if (".jpeg" == suffix)
		return "image/jpeg";
	else if (".pdf" == suffix)
		return "application/pdf";
	else if (".ppt" == suffix)
		return "application/vnd.ms-powerpoint";
	else if (".exe" == suffix)
		return "application/x-msdownload";
	else if (".dll" == suffix)
		return "application/x-msdownload";
	else if (".apk" == suffix)
		return "application/vnd.android.package-archive";
	else if (".xhtml" == suffix)
		return "text/html; charset=utf-8";
	else if (".xml" == suffix)
		return "text/xml;charset=utf-8";
	else if (".xls" == suffix)
		return "application/x-xls";
	else if (".img" == suffix)
		return "application/x-img";
	else if (".htx" == suffix)
		return "text/html; charset=utf-8";
	else if (".java" == suffix)
		return "java/*";
	else if ("." == suffix)
		return "application/x-";
	else if (".dot" == suffix)
		return "application/msword";
	else if (".001" == suffix)
		return "application/x-001";
	else if (".class" == suffix)
		return "java/*";
	else if (".avi" == suffix)
		return "video/avi";
	else if (".crt" == suffix)
		return "application/x-x509-ca-cert";
	else if (".bmp" == suffix)
		return "application/x-bmp";
	else if (".323" == suffix)
		return "text/h323; charset=utf-8";
	else if (".acp" == suffix)
		return "audio/x-mei-aac";
	else if (".asp" == suffix)
		return "text/asp; charset=utf-8";
	else if (".awf" == suffix)
		return "application/vnd.adobe.workflow";
	else if (".au" == suffix)
		return "audio/basic";
	else if (".ent" == suffix)
		return "text/xml; charset=utf-8";
	else if (".fax" == suffix)
		return "image/fax";
	else if (".jsp" == suffix)
		return "text/html; charset=utf-8";
	else if (".man" == suffix)
		return "application/x-troff-man";
	else if (".mp4" == suffix)
		return "video/mpeg4";
	else if (".mpeg" == suffix)
		return "video/mpg";
	else if (".mtx" == suffix)
		return "text/xml; charset=utf-8";
	else if (".net" == suffix)
		return "image/pnetvue";
	else if (".pcl" == suffix)
		return "application/x-pcl";
	else if (".plg" == suffix)
		return "text/html; charset=utf-8";
	else if (".ps" == suffix)
		return "application/postscript";
	else if (".pwz" == suffix)
		return "application/vnd.ms-powerpoint";
	else if (".rtf" == suffix)
		return "application/msword";
	else if (".xwd" == suffix)
		return "application/x-xwd";
	else if (".pci" == suffix)
		return "application/x-pci";
	else if (".movie" == suffix)
		return "video/x-sgi-movie";
	else if (".math" == suffix)
		return "text/xml; charset=utf-8";
	else if (".mac" == suffix)
		return "application/x-mac";
	else
		return "application/unkown";
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
	auto protobuf = analysis(channel);	
	if (nullptr == protobuf.get())
		return nullptr;

	//解析请求.
	auto buf = protobuf->response("");
	
	std::cout << "will write :" << buf->size()<<" bytes." << std::endl;
	//向remote写入数据.
	write(channel, std::move(buf));

	return nullptr;
}

std::shared_ptr<gdl::Protocols::protocol> gdl::Protocols::PasrserContainer::analysis(Channel channel)
{
	Buffer buf = recvDataFromChannel(channel);
	//if nullptr == buf.
	if (nullptr == buf.get())
		return nullptr;//转发nullptr.

	std::cout << "recv:\n" << buf.get()->data() << std::endl;
	for (auto& creactor : constructorVector) {

		auto ptr = creactor(std::move(buf));
		if (nullptr != ptr.get())//显式null.
			return ptr;
	}
	return nullptr;
}

gdl::Protocols::Buffer gdl::Protocols::PasrserContainer::recvDataFromChannel(Channel channel)
{
	
	//recv data from channel.
	std::cout << "get buf from pool.start" << std::endl;
	auto buf = bufferpool.Get();
	//std::unique_ptr <T, std::function<void(T*)> >
	//gdl::Protocols::Buffer buf(new gdl::BufferTool::Buffer);


	std::cout << "get buf from pool.end" << std::endl;
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
				return buf;
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
	std::shared_ptr<Buffer> shaBuffer(new Buffer(std::move(buffer)));
	channel->onWrite([=] ()mutable {

		//char response[] = "HTTP/1.1 200 OK\r\ncontent-type:text/html; charset=utf-8\r\ncontent-length:11\r\n\r\njust a test.";

		//const size_t len_r = strlen(response);
		//gdl::socktool::Wsend(channel->Fd(), response, len_r, 0);
		
		ssize_t n = gdl::socktool::Wsend(channel->Fd(), shaBuffer->get()->data(), shaBuffer->get()->size(), 0);
		std::cout << "send......size=" << n << " all=" << shaBuffer->get()->size() << std::endl;
		if (n > 0)
			shaBuffer->get()->eat(n);
		else {
			if (!(EWOULDBLOCK == errno || EAGAIN == errno)) {
				channel->close();
				Glog().mount(gdl::LogLevel::LWARN, gdl::strTool::format("send error from channel %d, %d", channel->Id(), errno) + strerror(errno));
			}
		}

		if (shaBuffer->get()->empty()) {

			channel->enableWrite(false);
			//回收buffer，匿名函数对象对buffer松手.
			shaBuffer.reset();
		}
		});
	channel->enableWrite();
}

