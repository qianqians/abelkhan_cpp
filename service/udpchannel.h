
#ifndef _udpchannel_h
#define _udpchannel_h

#include <list>

#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "JsonParse.h"
#include "Ichannel.h"

namespace service
{

class udpchannel : public juggle::Ichannel, public std::enable_shared_from_this<udpchannel> {
public:
	udpchannel(std::shared_ptr<boost::asio::ip::udp::socket> _s, std::string _ip, short _port){
		s = _s;
		ip = _ip;
		port = _port;

		buff_size = 16 * 1024;
		buff_offset = 0;
		buff = new char[buff_size];
	}

	~udpchannel(){
		delete[] buff;
	}
	
	boost::signals2::signal<void(std::shared_ptr<udpchannel>)> sigondisconn;
	boost::signals2::signal<void(std::shared_ptr<udpchannel>)> sigdisconn;

	void recv(const char * data, int32_t size){
		try {
			if ((buff_offset + size) > buff_size) {
				buff_size *= 2;
				auto new_buff = new char[buff_size];
				memcpy(new_buff, buff, buff_offset);
				delete buff;
				buff = new_buff;
			}
			memcpy(buff + buff_offset, data, size);

			int32_t tmp_buff_len = buff_offset + size;
			int32_t tmp_buff_offset = 0;
			while (tmp_buff_len > (tmp_buff_offset + 4)) {
				auto tmp_buff = (unsigned char *)buff + tmp_buff_offset;
				uint32_t len = (uint32_t)tmp_buff[0] | ((uint32_t)tmp_buff[1] << 8) | ((uint32_t)tmp_buff[2] << 16) | ((uint32_t)tmp_buff[3] << 24);

				if ((len + tmp_buff_offset + 4) <= tmp_buff_len) {
					std::string json_str((char*)(&tmp_buff[4]), len);

					Fossilizid::JsonParse::JsonObject obj;
					Fossilizid::JsonParse::unpacker(obj, json_str);
					que.push_back(boost::any_cast<Fossilizid::JsonParse::JsonArray>(obj));

					tmp_buff_offset += len + 4;
				}
				else {
					break;
				}
			}

			buff_offset = tmp_buff_len - tmp_buff_offset;
			if (tmp_buff_len > tmp_buff_offset) {
				auto new_buff = new char[buff_size];
				memset(new_buff, 0, buff_size);
				memcpy(new_buff, &buff[tmp_buff_offset], buff_offset);
				delete[] buff;
				buff = new_buff;
			}
		}
		catch (...) {
			sigondisconn(shared_from_this());
		}
	}

	void disconnect() {
		sigdisconn(shared_from_this());
	}

	bool pop(std::shared_ptr<std::vector<boost::any> >  & out){
		if (que.empty()){
			return false;
		}

		out = que.front();
		que.pop_front();

		return true;
	}

	void push(std::shared_ptr<std::vector<boost::any> > in){
		try {
			auto data = Fossilizid::JsonParse::pack(in);
			size_t len = data.size();
			char * _data = new char[len + 4];
			_data[0] = len & 0xff;
			_data[1] = len >> 8 & 0xff;
			_data[2] = len >> 16 & 0xff;
			_data[3] = len >> 24 & 0xff;
			memcpy_s(&_data[4], len, data.c_str(), data.size());
			size_t datasize = len + 4;

			boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
			auto offset = s->send_to(boost::asio::buffer(_data, datasize), ep);
			while (offset < datasize) {
				offset += s->send_to(boost::asio::buffer(&_data[len], datasize - offset), ep);
			}

			delete[] _data;
		}
		catch (...) {
			sigondisconn(shared_from_this());
		}
	}

public:
	std::string ip;
	short port;

private:
	std::list< std::shared_ptr<std::vector<boost::any> > > que;

	std::shared_ptr<boost::asio::ip::udp::socket> s;

	char * buff;
	int32_t buff_size;
	int32_t buff_offset;

};

}

#endif
