
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
		memset(buff, 0, buff_size);
		
		memset(read_buff, 0, 16 * 1024);
		s->async_read_some(boost::asio::buffer(read_buff, 16 * 1024), boost::bind(&channel::onRecv, this, _1, _2));
	}

	~channel(){
		delete[] buff;
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

		if ((buff_offset + bytes_transferred) > buff_size)
		{
			buff_size *= 2;
			auto new_buff = new char[buff_size];
			memset(new_buff, 0, buff_size);
			memcpy(new_buff, buff, buff_offset);
			delete[] buff;
			buff = new_buff;
		}
		memcpy(buff + buff_offset, read_buff, bytes_transferred);
		buff_offset += bytes_transferred;

		recv();
	}

	void recv()
	{
		try{
			int32_t tmp_buff_len = buff_offset;
			int32_t tmp_buff_offset = 0;
			while (tmp_buff_len > (tmp_buff_offset + 4))
			{
				auto tmp_buff = (unsigned char *)buff + tmp_buff_offset;
				uint32_t len = (uint32_t)tmp_buff[0] | ((uint32_t)tmp_buff[1] << 8) | ((uint32_t)tmp_buff[2] << 16) | ((uint32_t)tmp_buff[3] << 24);

				if ((len + tmp_buff_offset + 4) <= tmp_buff_len)
				{
					std::string json_str((char*)(&tmp_buff[4]), len);
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
						disconnect();

						return;
					}
				}
				else
				{
					break;
				}
			}

			buff_offset = tmp_buff_len - tmp_buff_offset;
			if (tmp_buff_len > tmp_buff_offset)
			{
				auto new_buff = new char[buff_size];
				memset(new_buff, 0, buff_size);
				memcpy(new_buff, &buff[tmp_buff_offset], buff_offset);
				delete[] buff;
				buff = new_buff;
			}

			memset(read_buff, 0, 16 * 1024);
			s->async_read_some(boost::asio::buffer(read_buff, 16 * 1024), boost::bind(&channel::onRecv, this, _1, _2));
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

			size_t len = data.size();
			unsigned char * _data = new unsigned char[len + 4];
			_data[0] = len & 0xff;
			_data[1] = len >> 8 & 0xff;
			_data[2] = len >> 16 & 0xff;
			_data[3] = len >> 24 & 0xff;
			memcpy_s(&_data[4], len, data.c_str(), data.size());
			size_t datasize = len + 4;

			size_t offset = 0;
			while (offset < datasize) {
				auto tmp_len = s->send(boost::asio::buffer(&_data[offset], datasize - offset));
				offset += tmp_len;
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
