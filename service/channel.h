
#ifndef _channel_h
#define _channel_h

#include <queue>

#include <boost/asio.hpp>
#include <boost/any.hpp>

#include "JsonParse.h"
#include "Ichannel.h"

namespace service
{

class channel : public juggle::Ichannel {
public:
	channel(std::shared_ptr<boost::asio::ip::tcp::socket> _s)
	{
		s = _s;

		buff_size = 1024 * 1024;
		buff_offset = 0;
		buff = new char[buff_size];
		
		s->async_receive(boost::asio::buffer(read_buff, 1024 * 1024), std::bind(&channel::onRecv, this, std::placeholders::_1, std::placeholders::_2));
	}

private:
	void onRecv(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		recv(read_buff, bytes_transferred);
	}

	void recv(const char * data, int32_t size)
	{
		if ( (buff_offset + size) > buff_size)
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
			auto tmp_buff = buff + tmp_buff_offset;
			int len = (int)tmp_buff[0] | ((int)tmp_buff[1] << 8) | ((int)tmp_buff[2] << 16) | ((int)tmp_buff[3] << 24);

			if ( (len + tmp_buff_offset + 4) <= tmp_buff_len )
			{
				Fossilizid::JsonParse::JsonObject obj;
				Fossilizid::JsonParse::unpacker(obj, std::string(&tmp_buff[4], len));
				que.push(boost::any_cast<Fossilizid::JsonParse::JsonArray>(obj));

				tmp_buff_offset += len + 4;
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
	}

public:
	bool pop(std::shared_ptr<std::vector<boost::any> >  & out)
	{
		if (que.empty())
		{
			return false;
		}

		out = que.front();
		que.pop();

		return true;
	}

	void push(std::shared_ptr<std::vector<boost::any> > in)
	{
		auto data = Fossilizid::JsonParse::pack(in);

		s->async_send(boost::asio::buffer(data), [](const boost::system::error_code& error, std::size_t bytes_transferred){});
	}

private:
	std::queue< std::shared_ptr<std::vector<boost::any> > > que;

	std::shared_ptr<boost::asio::ip::tcp::socket> s;

	char read_buff[1024 * 1024];

	char * buff;
	int32_t buff_size;
	int32_t buff_offset;

};

}

#endif
