
#ifndef _udpacceptservice_h
#define _udpacceptservice_h

#include <vector>

#include <process_.h>

#include "udpchannel.h"

namespace service
{

class udpacceptservice {
public:
	udpacceptservice(std::string ip, short port, std::shared_ptr<juggle::process> process) {
		_process = process;

		listen = std::make_shared<boost::asio::ip::udp::socket>(_service);
		boost::asio::ip::udp::endpoint ep(boost::asio::ip::address::from_string(ip), port);
		listen->open(ep.protocol());
		listen->bind(ep);
		memset(buff, 0, 16*1024);
		listen->async_receive_from(boost::asio::buffer(buff, 16*1024), sender, boost::bind(&udpacceptservice::onRecv, this, _1, _2));
	}

	void poll(){
		_service.poll();
	}

	boost::signals2::signal<void(std::shared_ptr<juggle::Ichannel>)> sigchannelconnect;
	void onRecv(const boost::system::error_code& error, std::size_t bytes_transferred){
		if (error) {
			return;
		}
		if (bytes_transferred == 0) {
			return;
		}

		std::shared_ptr<udpchannel> ch = nullptr;
		for (auto item : udps) {
			if (sender.address() == item.first.address() && sender.port() == item.first.port()) {
				ch = item.second;
				break;
			}
		}

		if (ch == nullptr) {
			ch = std::make_shared<udpchannel>(listen, sender.address().to_string(), sender.port());
			udps.push_back(std::make_pair(sender, ch));
			ch->sigondisconn.connect(boost::bind(&udpacceptservice::onChannelDisconn, this, _1));
			ch->sigdisconn.connect(boost::bind(&udpacceptservice::onChannelDisconn, this, _1));

			sigchannelconnect(ch);
			_process->reg_channel(ch);
		}

		ch->recv(buff, bytes_transferred);

		memset(buff, 0, 16 * 1024);
		listen->async_receive_from(boost::asio::buffer(buff, 16 * 1024), sender, boost::bind(&udpacceptservice::onRecv, this, _1, _2));
	}

	boost::signals2::signal<void(std::shared_ptr<juggle::Ichannel>)> sigchanneldisconnect;
	void onChannelDisconn(std::shared_ptr<udpchannel> ch) {
		if (!sigchanneldisconnect.empty()) {
			sigchanneldisconnect(ch);
		}

		for (auto it = udps.begin(); it != udps.end(); it++){
			if (it->second == ch) {
				udps.erase(it);
			}
		}

		_process->unreg_channel(ch);
	}

private:
	boost::asio::io_service _service;
	std::shared_ptr<juggle::process> _process;

	std::shared_ptr<boost::asio::ip::udp::socket> listen;
	char buff[16*1024];
	boost::asio::ip::udp::endpoint sender;

	std::vector<std::pair<boost::asio::ip::udp::endpoint, std::shared_ptr<udpchannel> > > udps;

};

}

#endif
