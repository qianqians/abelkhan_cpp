
#ifndef _websocket_channel_h
#define _websocket_channel_h

#include <boost/any.hpp>
#include <boost/signals2.hpp>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/connection.hpp>
#include <websocketpp/server.hpp>

#include "msque.h"
#include "JsonParse.h"
#include "Ichannel.h"

namespace service
{

class webchannel : public juggle::Ichannel, public std::enable_shared_from_this<webchannel> {
public:
	webchannel(std::shared_ptr<websocketpp::server<websocketpp::config::asio> > _server, websocketpp::connection_hdl _hdl)
	{
		server = _server;
		hdl = _hdl;

		buff_size = 16 * 1024;
		buff_offset = 0;
		buff = new char[buff_size];
		memset(buff, 0, buff_size);

		is_close = false;
	}

	~webchannel() {
		delete[] buff;
	}

	boost::signals2::signal<void(std::shared_ptr<webchannel>)> sigdisconn;

	void recv(std::string resv_data)
	{
		if (is_close) {
			return;
		}

		try {
			while ((buff_offset + resv_data.size()) > buff_size)
			{
				buff_size *= 2;
				auto new_buff = new char[buff_size];
				memset(new_buff, 0, buff_size);
				memcpy(new_buff, buff, buff_offset);
				delete[] buff;
				buff = new_buff;
			}
			memcpy(buff + buff_offset, resv_data.c_str(), resv_data.size());
			buff_offset += resv_data.size();

			int32_t tmp_buff_len = buff_offset;
			int32_t tmp_buff_offset = 0;
			while (tmp_buff_len > (tmp_buff_offset + 4))
			{
				auto tmp_buff = (unsigned char *)buff + tmp_buff_offset;
				uint32_t len = (uint32_t)tmp_buff[0] | ((uint32_t)tmp_buff[1] << 8) | ((uint32_t)tmp_buff[2] << 16) | ((uint32_t)tmp_buff[3] << 24);

				if ((len + tmp_buff_offset + 4) <= tmp_buff_len)
				{
					tmp_buff_offset += len + 4;

					auto json_buff = &tmp_buff[4];
					std::string json_str((char*)(json_buff), len);
					try
					{
						Fossilizid::JsonParse::JsonObject obj;
						Fossilizid::JsonParse::unpacker(obj, json_str);
						que.push(std::any_cast<Fossilizid::JsonParse::JsonArray>(obj));
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
		}
		catch (std::exception e) {
			std::cout << "error:" << e.what() << std::endl;
			disconnect();
		}
	}

public:
	void disconnect() {
		is_close = true;
		sigdisconn(shared_from_this());

		try
		{
		}
		catch (std::exception e) {
			std::cout << "error:" << e.what() << std::endl;
		}
	}

	bool pop(Fossilizid::JsonParse::JsonArray  & out)
	{
		if (que.empty())
		{
			return false;
		}

		return que.pop(out);
	}

	void push(Fossilizid::JsonParse::JsonArray in)
	{
		if (is_close) {
			return;
		}

		try {
			std::string data;
			Fossilizid::JsonParse::pack(in, data);
			size_t len = data.size();
			unsigned char * _data = new unsigned char[len + 4];
			_data[0] = len & 0xff;
			_data[1] = len >> 8 & 0xff;
			_data[2] = len >> 16 & 0xff;
			_data[3] = len >> 24 & 0xff;
			memcpy(&_data[4], data.c_str(), data.size());
			size_t datasize = len + 4;

			server->send(hdl, _data, datasize, websocketpp::frame::opcode::binary);

			delete[] _data;
		}
		catch (std::exception e) {
			std::cout << "error:" << e.what() << std::endl;
			is_close = true;
		}
	}

private:
	Fossilizid::container::msque< Fossilizid::JsonParse::JsonArray > que;

	std::shared_ptr<websocketpp::server<websocketpp::config::asio> > server;
	websocketpp::connection_hdl hdl;

	char * buff;
	int32_t buff_size;
	int32_t buff_offset;

	bool is_close;

};

}

#endif
