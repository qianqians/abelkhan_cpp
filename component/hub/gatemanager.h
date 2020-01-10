/*
 * gatemanager.h
 *
 *  Created on: 2016-7-12
 *      Author: qianqians
 */
#ifndef _gatemanager_h
#define _gatemanager_h

#include <map>
#include <set>
#include <memory>

#include <Ichannel.h>

#include <enetconnectservice.h>

#include <hub_call_gatecaller.h>

#include "hub.h"

namespace hub {

extern std::string current_client_uuid;

class gateproxy {
public:
	std::shared_ptr<caller::hub_call_gate> caller;
	std::shared_ptr<hub_service> _hub;

	gateproxy(std::shared_ptr<juggle::Ichannel> ch, std::shared_ptr<hub_service> hub) {
		caller = std::make_shared<caller::hub_call_gate>(ch);
		_hub = hub;
	}

	void reg_hub() {
		caller->reg_hub(_hub->uuid, _hub->name);
	}

	void connect_sucess(std::string client_uuid) {
		caller->connect_sucess(client_uuid);
	}

	void disconnect_client(std::string uuid) {
		caller->disconnect_client(uuid);
	}

	void forward_hub_call_client(std::string uuid, std::string _module, std::string func, Fossilizid::JsonParse::JsonArray argv) {
		caller->forward_hub_call_client(uuid, _module, func, argv);
	}

	void forward_hub_call_group_client(Fossilizid::JsonParse::JsonArray uuids, std::string _module, std::string func, Fossilizid::JsonParse::JsonArray argv) {
		caller->forward_hub_call_group_client(uuids, _module, func, argv);
	}

	void forward_hub_call_global_client(std::string _module, std::string func, Fossilizid::JsonParse::JsonArray argv) {
		caller->forward_hub_call_global_client(_module, func, argv);
	}
};

class gatemanager {
public:
	gatemanager(std::shared_ptr<service::enetconnectservice> _conn, std::shared_ptr<hub_service> _hub_) {
		conn = _conn;
		_hub = _hub_;

		current_client_uuid = "";
	}

	void connect_gate(std::string uuid, std::string ip, uint16_t port) {
		std::cout << "connect_gate ip:" << ip << " port:" << port << std::endl;
		conn->connect(ip, port, [this, uuid](std::shared_ptr<juggle::Ichannel> ch){
			gates[uuid] = std::make_shared<gateproxy>(ch, _hub);
			ch_gates[ch] = gates[uuid];
			gates[uuid]->reg_hub();
		});
	}

	void client_connect(std::string client_uuid, std::shared_ptr<juggle::Ichannel> gate_ch) {
		if (ch_gates.find(gate_ch) == ch_gates.end()) {
			return;
		}

		if (clients.find(client_uuid) == clients.end()) {
			return;
		}

		std::cout << "reg client:" << client_uuid << std::endl;

		clients[client_uuid] = ch_gates[gate_ch];
		clients[client_uuid]->connect_sucess(client_uuid);

		_hub->sig_client_connect(client_uuid);
	}

	void client_disconnect(std::string client_uuid) {
		if (clients.find(client_uuid) == clients.end()) {
			return;
		}

		clients.erase(client_uuid);
		_hub->sig_client_disconnect(client_uuid);
	}

	void client_exception(std::string client_uuid) {
		_hub->sig_client_exception(client_uuid);
	}

	void disconnect_client(std::string uuid) {
		if (clients.find(uuid) == clients.end()) {
			return;
		}
		
		clients[uuid]->disconnect_client(uuid);
		clients.erase(uuid);
	}

	void call_client(std::string uuid, std::string _module, std::string func, Fossilizid::JsonParse::JsonArray argvs) {
		if (clients.find(uuid) == clients.end()) {
			return;
		}

		clients[uuid]->forward_hub_call_client(uuid, _module, func, argvs);
	}

	void call_group_client(Fossilizid::JsonParse::JsonArray uuids, std::string _module, std::string func, Fossilizid::JsonParse::JsonArray argvs) {
		std::vector<std::shared_ptr<gateproxy> > tmp_gates;
		for (auto uuid : *uuids) {
			auto it_gate = clients.find(std::any_cast<std::string>(uuid));
			if (it_gate == clients.end()) {
				continue;
			}

			if (std::find(tmp_gates.begin(), tmp_gates.end(), it_gate->second) == tmp_gates.end()) {
				continue;
			}

			tmp_gates.push_back(it_gate->second);
		}

		for (auto gate_proxy : tmp_gates) {
			gate_proxy->forward_hub_call_group_client(uuids, _module, func, argvs);
		}
	}

	void call_global_client(std::string _module, std::string func, Fossilizid::JsonParse::JsonArray argvs) {
		for (auto gate : gates) {
			gate.second->forward_hub_call_global_client(_module, func, argvs);
		}
	}

private:
	std::shared_ptr<service::enetconnectservice> conn;
	std::shared_ptr<hub_service> _hub;
		
	std::unordered_map<std::string, std::shared_ptr<gateproxy> > clients;
	std::unordered_map<std::string, std::shared_ptr<gateproxy> > gates;
	std::unordered_map<std::shared_ptr<juggle::Ichannel>, std::shared_ptr<gateproxy> > ch_gates;

};

}

#endif //_gatemanager_h
