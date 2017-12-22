
#ifndef _channel_h
#define _channel_h

#include <list>

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "JsonParse.h"
#include "Ichannel.h"

namespace service
{

class channel : public juggle::Ichannel, public std::enable_shared_from_this<channel> {
public:
	channel(std::shared_ptr<boost::asio::ip::tcp::socket> _s)
	{
		s = _s;

		buff_size = 16 * 1024;
		buff_offset = 0;
		buff = new char[buff_size];
		
		s->async_receive(boost::asio::buffer(read_buff, 16 * 1024), boost::bind(&channel::onRecv, this, _1, _2));
	}

	boost::signals2::signal<void(std::shared_ptr<channel>)> sigondisconn;
	boost::signals2::signal<void(std::shared_ptr<channel>)> sigdisconn;

private:
	void onRecv(const boost::system::error_code& error, std::size_t bytes_transferred){
		if (error){
			sigondisconn(shared_from_this());
			return;
		}

		if (bytes_transferred == 0){
			sigondisconn(shared_from_this());
			return;
		}

		recv(read_buff, bytes_transferred);
	}

	void recv(const char * data, int32_t size)
	{
		try{
			if ((buff_offset + size) > buff_size)
			{
				buff_size *= 2;
				auto new_buff = new char[buff_size];
				memcpy(new_buff, buff, buff_offset);
				delete buff;
				buff = new_buff;
			}
			memcpy(buff + buff_offset, data, size);

			int32_t tmp_buff_len = buff_offset + size;
			int32_t tmp_buff_offset = 0;
			while (tmp_buff_len > (tmp_buff_offset + 4))
			{
				auto tmp_buff = (unsigned char *)buff + tmp_buff_offset;
				uint32_t len = (uint32_t)tmp_buff[0] | ((uint32_t)tmp_buff[1] << 8) | ((uint32_t)tmp_buff[2] << 16) | ((uint32_t)tmp_buff[3] << 24);

				if ((len + tmp_buff_offset + 4) <= tmp_buff_len)
				{
					std::string json_str((char*)(&tmp_buff[4]));
					std::cout << "read:" << json_str << std::endl;
					try
					{
						Fossilizid::JsonParse::JsonObject obj;
						Fossilizid::JsonParse::unpacker(obj, json_str);
						que.push_back(boost::any_cast<Fossilizid::JsonParse::JsonArray>(obj));

						tmp_buff_offset += len + 4;
					}
					catch (Fossilizid::JsonParse::jsonformatexception e)
					{
						std::cout << "error:" << json_str << std::endl;
					}
				}
				else
				{
					memcpy(buff, &buff[tmp_buff_offset], tmp_buff_len - tmp_buff_offset);
					buff_offset = tmp_buff_len - tmp_buff_offset;
					tmp_buff_offset = tmp_buff_len;
				}
			}

			if (tmp_buff_len > tmp_buff_offset)
			{
				memcpy(buff, &buff[tmp_buff_offset], tmp_buff_len - tmp_buff_offset);
				buff_offset = tmp_buff_len - tmp_buff_offset;
				tmp_buff_offset = tmp_buff_len;
			}

			memset(read_buff, 0, 16 * 1024);
			s->async_receive(boost::asio::buffer(read_buff, 16 * 1024), boost::bind(&channel::onRecv, this, _1, _2));
		}
		catch (std::exception e) {
			std::cout << "error:" << e.what() << std::endl;
			disconnect();
		}
	}

	void onClose() {
		s->cancel();
		s->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		s->close();
	}

public:
	void disconnect() {
		s->get_io_service().post(std::bind(&channel::onClose, this));
	}

	bool pop(std::shared_ptr<std::vector<boost::any> >  & out)
	{
		if (que.empty())
		{
			return false;
		}

		out = que.front();
		que.pop_front();

		return true;
	}

	void push(std::shared_ptr<std::vector<boost::any> > in)
	{
		if (!s->is_open()){
			return;
		}

		try {
			auto data = Fossilizid::JsonParse::pack(in);

			//std::cout << "send:" << data << std::endl;

			size_t len = data.size();
			char * _data = new char[len + 4];
			_data[0] = len & 0xff;
			_data[1] = len >> 8 & 0xff;
			_data[2] = len >> 16 & 0xff;
			_data[3] = len >> 24 & 0xff;
			memcpy_s(&_data[4], len, data.c_str(), data.size());
			size_t datasize = len + 4;

			auto offset = s->send(boost::asio::buffer(_data, datasize));
			while (offset < datasize) {
				offset += s->send(boost::asio::buffer(&_data[len], datasize - offset));
			}

			delete[] _data;
		}
		catch (std::exception e) {
			std::cout << "error:" << e.what() << std::endl;
			sigondisconn(shared_from_this());
		}
	}

private:
	std::list< std::shared_ptr<std::vector<boost::any> > > que;

	std::shared_ptr<boost::asio::ip::tcp::socket> s;

	char read_buff[16 * 1024];

	char * buff;
	int32_t buff_size;
	int32_t buff_offset;

};

}

#endif
