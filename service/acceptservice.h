
#ifndef _acceptservice_h
#define _acceptservice_h

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

#include "channel.h"
#include "process_.h"

namespace service
{

class acceptservice {
public:
	acceptservice(std::string ip, short port, std::shared_ptr<juggle::process> process) : _acceptor(_service){
		_process = process;

		_acceptor.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(ip), port));
		_acceptor.listen();

		auto s = std::make_shared<boost::asio::ip::tcp::socket>(_service);
		_acceptor.async_accept(*s, boost::bind(&acceptservice::onAccept, this, s, _1));
	}

	boost::signals2::signal<void(std::shared_ptr<juggle::Ichannel>)> sigchannelconnect;
	void onAccept(std::shared_ptr<boost::asio::ip::tcp::socket> s, boost::system::error_code ec) {
		if (ec) {
			s->close();
		}
		else {
			auto ch = std::make_shared<channel>(s);
			_process->reg_channel(ch);
			ch->sigondisconn.connect(boost::bind(&acceptservice::onChannelDisconn, this, _1));
			ch->sigdisconn.connect(boost::bind(&acceptservice::ChannelDisconn, this, _1));
			sigchannelconnect(ch);
		}

		s = std::make_shared<boost::asio::ip::tcp::socket>(_service);
		_acceptor.async_accept(*s, boost::bind(&acceptservice::onAccept, this, s, _1));
	}

	boost::signals2::signal<void(std::shared_ptr<juggle::Ichannel>)> sigchanneldisconnect;
	void onChannelDisconn(std::shared_ptr<channel> ch) {
		if (!sigchanneldisconnect.empty()) {
			sigchanneldisconnect(ch);
		}

		_process->unreg_channel(ch);
	}

	void ChannelDisconn(std::shared_ptr<channel> ch) {
		_process->unreg_channel(ch);
	}

	void poll(){
		_service.poll_one();
	}

private:
	boost::asio::io_service _service;
	boost::asio::ip::tcp::acceptor _acceptor;

	std::shared_ptr<juggle::process> _process;

};

}

#endif
