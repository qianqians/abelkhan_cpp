
#ifndef _websocket_acceptservice_h
#define _websocket_acceptservice_h

#include <thread>
#include <functional>

#include <boost/signals2.hpp>

#include "msque.h"
#include "websocketchannel.h"
#include "process_.h"

namespace service
{

class webacceptservice {
public:
	webacceptservice(std::string ip, short port, std::shared_ptr<juggle::process> process) 
		: th([this, port]() {
			server = std::make_shared<websocketpp::server<websocketpp::config::asio> >();
			server->init_asio();

			server->set_open_handler(websocketpp::lib::bind(&webacceptservice::onAccept, this, websocketpp::lib::placeholders::_1));
			server->set_close_handler(websocketpp::lib::bind(&webacceptservice::onChannelDisconn, this, websocketpp::lib::placeholders::_1));
			server->set_message_handler(websocketpp::lib::bind(&webacceptservice::onMsg, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));

			server->listen(port);
			server->start_accept();

			server->run();
		}) 
	{
		_process = process;
	}

	boost::signals2::signal<void(std::shared_ptr<juggle::Ichannel>)> sigchannelconnect;
	void onAccept(websocketpp::connection_hdl hdl) {
		auto ch = std::make_shared<webchannel>(server, hdl);
		ch->sigdisconn.connect(boost::bind(&webacceptservice::ChannelDisconn, this, _1));
		
		_chs_mu.lock();
		_chs.insert(std::make_pair(hdl.lock().get(), ch));
		_chs_mu.unlock();

		_process->reg_channel(ch);

		sigchannelconnect(ch);
	}

	boost::signals2::signal<void(std::shared_ptr<juggle::Ichannel>)> sigchanneldisconnect;
	void onChannelDisconn(websocketpp::connection_hdl hdl) {
		_chs_mu.lock();
		auto ch = _chs[hdl.lock().get()];
		_chs.erase(hdl.lock().get());
		_chs_mu.unlock();

		if (!sigchanneldisconnect.empty()) {
			sigchanneldisconnect(ch);
		}

		gc_fn_que.push([this, ch]() {
			_process->unreg_channel(ch);
		});
	}

	void onMsg(websocketpp::connection_hdl hdl, websocketpp::server<websocketpp::config::asio>::message_ptr msg)
	{
		std::string str_msg = msg->get_payload();

		_chs_mu.lock();
		auto ch = _chs[hdl.lock().get()];
		_chs_mu.unlock();

		ch->recv(str_msg);
	}

	void ChannelDisconn(std::shared_ptr<webchannel> ch) {
		gc_fn_que.push([this, ch]() {
			_process->unreg_channel(ch);
		});
	}

	void poll(){
	}

	void gc_poll() {
		std::function<void()> gc_fn;
		while (gc_fn_que.pop(gc_fn))
		{
			gc_fn();
		}
	}

private:
	std::shared_ptr<websocketpp::server<websocketpp::config::asio> > server;

	std::shared_ptr<juggle::process> _process;

	std::mutex _chs_mu;
	std::map<void*, std::shared_ptr<webchannel> > _chs;

	std::thread th;

	Fossilizid::container::msque<std::function<void()> > gc_fn_que;

};

}

#endif
