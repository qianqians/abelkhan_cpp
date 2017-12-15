
#ifndef _client_h
#define _client_h

#include <boost/signals2.hpp>
#include <boost/any.hpp>

#include <gate_call_clientmodule.h>
#include <client_call_gatecaller.h>
#include <gate_call_client_fastmodule.h>
#include <client_call_gate_fastcaller.h>

#include <modulemanager.h>
#include <timerservice.h>
#include <connectservice.h>
#include <udpconnectservice.h>
#include <juggleservice.h>

namespace client
{

class client {
public:
	boost::signals2::signal<void()> sigDisconnect;
	boost::signals2::signal<void()> sigConnectGate;
	boost::signals2::signal<void(std::string hub_name)> sigConnectHub;

public:
	client();

	bool connect_server(std::string tcp_ip, short tcp_port, std::string udp_ip, short udp_port, int64_t tick);
	void connect_hub(std::string hub_name);
	void call_hub(std::string hub_name, std::string module_name, std::string func_name, std::shared_ptr<std::vector<boost::any> > _argvs);
	int64_t poll();

private:
	void heartbeats(int64_t tick);
	void refresh_udp_link(int64_t tick);

	void on_ack_heartbeats();
	void on_ack_connect_gate();
	void on_ack_connect_hub(std::string _hub_name);
	void on_call_client(std::string module_name, std::string func_name, std::shared_ptr<std::vector<boost::any> > _argvs);
	
	void on_confirm_refresh_udp_end_point();

public:
	std::string uuid;
	service::timerservice timer;
	common::modulemanager modules;

private:
	int64_t _heartbeats;
	
	std::shared_ptr<service::connectservice> _tcp_conn;
	std::shared_ptr<module::gate_call_client> _gate_call_client;
	std::shared_ptr<caller::client_call_gate> _client_call_gate;

	std::string _udp_ip;
	short _udp_port;
	std::shared_ptr<service::udpconnectservice> _udp_conn;
	std::shared_ptr<module::gate_call_client_fast> _gate_call_client_fast;
	std::shared_ptr<caller::client_call_gate_fast> _client_call_gate_fast;

	service::juggleservice _juggleservice;

};

}

#endif
