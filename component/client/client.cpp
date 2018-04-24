/*
 *  qianqians
 *  2014-10-5
 */

#include "client.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace client
{

client::client()
{
	boost::uuids::random_generator g;
	auto _uuid = g();
	uuid = boost::lexical_cast<std::string>(_uuid);

	_heartbeats = 0;

	_gate_call_client = std::make_shared<module::gate_call_client>();
	_gate_call_client->sig_connect_gate_sucess.connect(std::bind(&client::on_ack_connect_gate, this));
	_gate_call_client->sig_connect_hub_sucess.connect(std::bind(&client::on_ack_connect_hub, this, std::placeholders::_1));
	_gate_call_client->sig_call_client.connect(std::bind(&client::on_call_client, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_gate_call_client->sig_ack_heartbeats.connect(std::bind(&client::on_ack_heartbeats, this));
	auto tcp_process = std::make_shared<juggle::process>();
	tcp_process->reg_module(_gate_call_client);
	_tcp_conn = std::make_shared<service::connectservice>(tcp_process);

	_juggleservice.add_process(tcp_process);
}

bool client::connect_server(std::string tcp_ip, short tcp_port, int64_t tick)
{
	auto ch = _tcp_conn->connect(tcp_ip, tcp_port);
	_client_call_gate = std::make_shared<caller::client_call_gate>(ch);
	_client_call_gate->connect_server(uuid, tick);

	return true;
}

void client::connect_hub(std::string hub_name)
{
	_client_call_gate->connect_hub(uuid, hub_name);
}

void client::call_hub(std::string hub_name, std::string module_name, std::string func_name, std::shared_ptr<std::vector<boost::any> > _argvs)
{
	_client_call_gate->forward_client_call_hub(hub_name, module_name, func_name, _argvs);
}

int64_t client::poll()
{
	auto tick = timer.poll();

	_tcp_conn->poll();
	
	_juggleservice.poll();

	return tick;
}

void client::heartbeats(int64_t tick)
{
	if (_heartbeats < tick - 35 * 1000)
	{
		sigDisconnect();
	}
	else
	{
		_client_call_gate->heartbeats(tick);

		timer.addticktimer(tick + 30 * 1000, std::bind(&client::heartbeats, this, std::placeholders::_1));
	}
}

void client::on_ack_heartbeats()
{
	_heartbeats = timer.Tick;
}

void client::on_ack_connect_gate()
{
	_heartbeats = timer.Tick;
	_client_call_gate->heartbeats(timer.Tick);

	timer.addticktimer(timer.Tick + 30 * 1000, std::bind(&client::heartbeats, this, std::placeholders::_1));

	sigConnectGate();
}

void client::on_ack_connect_hub(std::string _hub_name)
{
	sigConnectHub(_hub_name);
}

void client::on_call_client(std::string module_name, std::string func_name, std::shared_ptr<std::vector<boost::any> > _argvs)
{
	modules.process_module_mothed(module_name, func_name, _argvs);
}

}