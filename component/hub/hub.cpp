/*
 * qianqians
 * 2020-1-10
 * hub.cpp
 */

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "hub_call_hubmodule.h"
#include "center_call_hubmodule.h"
#include "center_call_servermodule.h"
#include "gate_call_hubmodule.h"
#include "dbproxy_call_hubmodule.h"

#include "hub.h"
#include "centerproxy.h"
#include "center_msg_handle.h"
#include "dbproxyproxy.h"
#include "dbproxy_msg_handle.h"
#include "hubsvrmanager.h"
#include "hub_svr_msg_handle.h"
#include "gatemanager.h"
#include "gate_msg_handle.h"
#include "gc_poll.h"

namespace hub{

hub_service::hub_service(std::string config_file_path, std::string config_name) {
	uuid = boost::lexical_cast<std::string>(boost::uuids::random_generator()());

	_config = std::make_shared<config::config>(config_file_path);
	_center_config = _config->get_value_dict("center");
	_root_config = _config;
	_config = _config->get_value_dict(config_name);

	name = _config->get_value_string("hub_name");
}

void hub_service::init() {
	enet_initialize();

	_timerservice = std::make_shared<service::timerservice>();
	close_handle = std::make_shared<closehandle>();
	hubs = std::make_shared<hubsvrmanager>(shared_from_this());

	auto ip = _config->get_value_string("ip");
	auto port = _config->get_value_int("port");
	auto hub_call_hub = std::make_shared<module::hub_call_hub>();
	hub_call_hub->sig_reg_hub.connect(std::bind(hub_msg::reg_hub, hubs, std::placeholders::_1));
	hub_call_hub->sig_reg_hub_sucess.connect(std::bind(hub_msg::reg_hub_sucess));
	hub_call_hub->sig_hub_call_hub_mothed.connect(std::bind(hub_msg::hub_call_hub_mothed, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	auto hub_process = std::make_shared<juggle::process>();
	hub_process->reg_module(hub_call_hub);
	_hub_service = std::make_shared<service::enetacceptservice>(ip, port, hub_process);

	_center_process = std::make_shared<juggle::process>();
	_center_service = std::make_shared<service::connectservice>(_center_process);

	auto gate_process = std::make_shared<juggle::process>();
	_gate_service = std::make_shared<service::enetconnectservice>(gate_process);
	gates = std::make_shared<gatemanager>(_gate_service, shared_from_this());
	auto gate_call_hub = std::make_shared<module::gate_call_hub>();
	gate_call_hub->sig_reg_hub_sucess.connect(std::bind(gate_msg::reg_hub_sucess));
	gate_call_hub->sig_client_connect.connect(std::bind(gate_msg::client_connect, gates, std::placeholders::_1));
	gate_call_hub->sig_client_disconnect.connect(std::bind(gate_msg::client_disconnect, gates, std::placeholders::_1));
	gate_call_hub->sig_client_exception.connect(std::bind(gate_msg::client_exception, gates, std::placeholders::_1));
	gate_call_hub->sig_client_call_hub.connect(std::bind(gate_msg::client_call_hub, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	gate_process->reg_module(gate_call_hub);

	_juggleservice = std::make_shared<service::juggleservice>();
	_juggleservice->add_process(hub_process);
	_juggleservice->add_process(_center_process);
	_juggleservice->add_process(gate_process);
}

void hub_service::connect_center() {
	std::cout << "begin on connect center" << std::endl;

	auto ip = _center_config->get_value_string("ip");
	auto port = _center_config->get_value_int("port");
	auto center_ch = _center_service->connect(ip, port);

	_centerproxy = std::make_shared<centerproxy>(center_ch);

	auto center_call_hub = std::make_shared<module::center_call_hub>();
	auto center_call_server = std::make_shared<module::center_call_server>();
	center_call_server->sig_reg_server_sucess.connect(std::bind(center_msg::reg_server_sucess, _centerproxy));
	center_call_server->sig_close_server.connect(std::bind(center_msg::close_server, close_handle));
	center_call_hub->sig_distribute_server_address.connect(std::bind(center_msg::distribute_server_address, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	center_call_hub->sig_reload.connect(std::bind(center_msg::reload, shared_from_this(), std::placeholders::_1));
	_center_process->reg_module(center_call_hub);
	_center_process->reg_module(center_call_server);

	_centerproxy->reg_server(_config->get_value_string("ip"), _config->get_value_int("port"), uuid);

	std::cout << "end on connect center" << std::endl;
}

void hub_service::connect_gate(std::string uuid, std::string ip, uint16_t port) {
	gates->connect_gate(uuid, ip, port);
}

void hub_service::reg_hub(std::string hub_ip, uint16_t hub_port) {
	_hub_service->connect(hub_ip, hub_port, [this](std::shared_ptr<juggle::Ichannel> ch){
		auto caller = std::make_shared< caller::hub_call_hub>(ch);
		caller->reg_hub(name);
	});
}

void hub_service::try_connect_db(std::string dbproxy_ip, uint16_t dbproxy_port) {
	if (!_config->has_key("dbproxy")) {
		return;
	}

	auto dbproxy_cfg = _root_config->get_value_dict(_config->get_value_string("dbproxy"));
	auto _db_ip = dbproxy_cfg->get_value_string("ip");
	auto _db_port = dbproxy_cfg->get_value_int("port");
	if (dbproxy_ip != _db_ip || dbproxy_port != _db_port) {
		return;
	}

	auto dbproxy_process = std::make_shared<juggle::process>();
	_dbproxy_service = std::make_shared<service::connectservice>(dbproxy_process);
	auto ch = _dbproxy_service->connect(_db_ip, _db_port);
	_dbproxyproxy = std::make_shared<dbproxyproxy>(ch);

	auto dbproxy_call_hub = std::make_shared<module::dbproxy_call_hub>();
	dbproxy_call_hub->sig_reg_hub_sucess.connect(std::bind(db_msg::reg_hub_sucess));
	dbproxy_call_hub->sig_ack_create_persisted_object.connect(std::bind(db_msg::ack_create_persisted_object, _dbproxyproxy, std::placeholders::_1, std::placeholders::_2));
	dbproxy_call_hub->sig_ack_updata_persisted_object.connect(std::bind(db_msg::ack_updata_persisted_objec, _dbproxyproxy, std::placeholders::_1));
	dbproxy_call_hub->sig_ack_get_object_count.connect(std::bind(db_msg::ack_get_object_count, _dbproxyproxy, std::placeholders::_1, std::placeholders::_2));
	dbproxy_call_hub->sig_ack_get_object_info.connect(std::bind(db_msg::ack_get_object_info, _dbproxyproxy, std::placeholders::_1, std::placeholders::_2));
	dbproxy_call_hub->sig_ack_get_object_info_end.connect(std::bind(db_msg::ack_get_object_info_end, _dbproxyproxy, std::placeholders::_1));
	dbproxy_call_hub->sig_ack_remove_object.connect(std::bind(db_msg::ack_remove_object, _dbproxyproxy, std::placeholders::_1));

	dbproxy_process->reg_module(dbproxy_call_hub);
	_juggleservice->add_process(dbproxy_process);
}

void hub_service::poll() {
	auto time_now = msec_time();
	while (1) {
		try {
			_hub_service->poll();
			_center_service->poll();
			_gate_service->poll();
			_dbproxy_service->poll();

			_timerservice->poll();

			_juggleservice->poll();
		}
		catch (std::exception err) {
			std::cout << "error:" << err.what() << std::endl;
		}

		if (close_handle->is_closed) {
			_timerservice->addticktimer(2000, [](uint64_t timetmp) {
				exit(0);
			});
		}
	}
}

}
