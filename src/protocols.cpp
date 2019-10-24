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
		//Informational 1xx  ��Ϣ
		'100' : 'Continue',  //����
		'101' : 'Switching Protocols',  //����Э��

		//Successful 2xx  �ɹ�
		'200' : 'OK',  //OK
		'201' : 'Created',  //����
		'202' : 'Accepted',  //�ѽ���
		'203' : 'Non-Authoritative Information',  //��Ȩ����Ϣ
		'204' : 'No Content',  //û������
		'205' : 'Reset Content',  //��������
		'206' : 'Partial Content',  //��������

		//Redirection 3xx  �ض���
		'300' : 'Multiple Choices',  //����ѡ��
		'301' : 'Moved Permanently',  //�����ƶ�
		'302' : 'Found',  //�ҵ�
		'303' : 'See Other',  //�μ�����
		'304' : 'Not Modified',  //δ�޸�
		'305' : 'Use Proxy',  //ʹ�ô���
		'306' : 'Unused',  //δʹ��
		'307' : 'Temporary Redirect',  //��ʱ�ض���

		//Client Error 4xx  �ͻ��˴���
		'400' : 'Bad Request',  //���������
		'401' : 'Unauthorized',  //δ����Ȩ
		'402' : 'Payment Required',  //��������
		'403' : 'Forbidden',  //��ֹ
		'404' : 'Not Found',  //û���ҵ�
		'405' : 'Method Not Allowed',  //����������
		'406' : 'Not Acceptable',  //���ɽ���
		'407' : 'Proxy Authentication Required',  //��Ҫ���������֤
		'408' : 'Request Timeout',  //����ʱ
		'409' : 'Conflict',  //ָ���ͻ
		'410' : 'Gone',  //�ĵ����õ��뿪��ָ����λ��
		'411' : 'Length Required',  //��ҪContent-Lengthͷ����
		'412' : 'Precondition Failed',  //ǰ������ʧ��
		'413' : 'Request Entity Too Large',  //����ʵ��̫��
		'414' : 'Request-URI Too Long',  //����URI̫��
		'415' : 'Unsupported Media Type',  //��֧�ֵ�ý������
		'416' : 'Requested Range Not Satisfiable',  //����ķ�Χ��������
		'417' : 'Expectation Failed',  //����ʧ��

		//Server Error 5xx  ����������
		'500' : 'Internal Server Error',  //�ڲ�����������
		'501' : 'Not Implemented',  //δʵ��
		'502' : 'Bad Gateway',  //���������
		'503' : 'Service Unavailable',  //���񲻿���
		'504' : 'Gateway Timeout',  //���س�ʱ
		'505' : 'HTTP Version Not Supported'  //HTTP�汾��֧��
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
		std::shared_ptr<gdl::Protocols::protocol*>��Ϊһ��Ԥ���ķ���ֵ�����ڿ��ܸ�������û��߲���.
	*/
	auto protobuf = analysis(channel);	
	if (nullptr == protobuf.get())
		return nullptr;

	//��������.
	auto buf = protobuf->response("");
	
	std::cout << "will write :" << buf->size()<<" bytes." << std::endl;
	//��remoteд������.
	write(channel, std::move(buf));

	return nullptr;
}

std::shared_ptr<gdl::Protocols::protocol> gdl::Protocols::PasrserContainer::analysis(Channel channel)
{
	Buffer buf = recvDataFromChannel(channel);
	//if nullptr == buf.
	if (nullptr == buf.get())
		return nullptr;//ת��nullptr.

	std::cout << "recv:\n" << buf.get()->data() << std::endl;
	for (auto& creactor : constructorVector) {

		auto ptr = creactor(std::move(buf));
		if (nullptr != ptr.get())//��ʽnull.
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
			//����buffer���������������buffer����.
			shaBuffer.reset();
		}
		});
	channel->enableWrite();
}

