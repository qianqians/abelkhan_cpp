
#ifndef _client_h
#define _client_h

#include <map>

#include <boost/signals2.hpp>
#include <boost/any.hpp>

#include <hub_call_clientmodule.h>
#include <gate_call_clientmodule.h>

#include <client_call_hubcaller.h>
#include <client_call_gatecaller.h>

#include <modulemanager.h>
#include <timerservice.h>
#include <connectservice.h>
#include <juggleservice.h>

namespace client
{

class client {
public:
	boost::signals2::signal<void()> sigDisconnect;
	boost::signals2::signal<void()> sigConnectServer;

public:
	client(uint64_t xor_key);

	bool direct_connect_server(std::string hub_name, std::string tcp_ip, short tcp_port);
	bool connect_server(std::string tcp_ip, short tcp_port, int64_t tick);
	void call_hub(std::string hub_name, std::string module_name, std::string func_name, std::shared_ptr<std::vector<boost::any> > _argvs);
	int64_t poll();

private:
	void heartbeats(int64_t tick);

	void on_ack_heartbeats();
	void on_ack_connect_server();
	void on_call_client(std::string module_name, std::string func_name, std::shared_ptr<std::vector<boost::any> > _argvs);

public:
	std::string uuid;
	service::timerservice timer;
	common::modulemanager modules;

private:
	unsigned char xor_key;
	int64_t _heartbeats;
	
	std::shared_ptr<service::connectservice> _conn;
	std::shared_ptr<module::gate_call_client> _gate_call_client;
	std::shared_ptr<caller::client_call_gate> _client_call_gate;

	std::shared_ptr<module::hub_call_client> _hub_call_client;
	std::map<std::string, std::shared_ptr<caller::client_call_hub> > direct_caller_hub;

	service::juggleservice _juggleservice;

};

}

#endif
