
#ifndef _udpconnectservice_h
#define _udpconnectservice_h

#include <map>

#include <process_.h>

#include "udpchannel.h"

namespace service
{

class udpconnectservice {
public:
	udpconnectservice(std::shared_ptr<juggle::process> process)
	{
		_process = process;
	}

	std::shared_ptr<juggle::Ichannel> connect(std::string ip, short port)
	{
		auto s = std::make_shared<boost::asio::ip::udp::socket>(_service);

		auto ch = std::make_shared<udpchannel>(s, ip, port);
		auto data = std::make_shared<ch_data>();
		memset(data->buff, 0, 16*1024);
		data->ch = ch;
		ch_map.insert(std::make_pair(s, data));
		_process->reg_channel(ch);

		s->async_receive_from(boost::asio::buffer(data->buff, 16*1024), data->ep, boost::bind(&udpconnectservice::onRecv, this, s, _1, _2));

		return ch;
	}

	void poll()
	{
		_service.poll_one();
	}

private:
	void onRecv(std::shared_ptr<boost::asio::ip::udp::socket> s, const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		auto data = ch_map[s];
		auto ch = data->ch;
		if (boost::asio::ip::address::from_string(ch->ip) == data->ep.address() && ch->port == data->ep.port()){
			ch->recv(data->buff, bytes_transferred);
		}
		s->async_receive_from(boost::asio::buffer(data->buff, 16*1024), data->ep, boost::bind(&udpconnectservice::onRecv, this, s, _1, _2));
	}


private:
	struct ch_data
	{
		char buff[16*1024];
		boost::asio::ip::udp::endpoint ep;
		std::shared_ptr<udpchannel> ch;
	};
	std::map< std::shared_ptr<boost::asio::ip::udp::socket>, std::shared_ptr<ch_data> > ch_map;

	boost::asio::io_service _service;
	std::shared_ptr<juggle::process> _process;


};

}

#endif
