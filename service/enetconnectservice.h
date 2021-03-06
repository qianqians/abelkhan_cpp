
#ifndef _enetconnectservice_h
#define _enetconnectservice_h

#include <unordered_map>

#include "enetchannel.h"
#include "process_.h"

namespace service
{

class enetconnectservice {
public:
	enetconnectservice(std::shared_ptr<juggle::process> process)
	{
		_process = process;

		_host = enet_host_create(nullptr, 2048, 1, 0, 0);
	}

	void connect(std::string ip, short port, std::function<void(std::shared_ptr<juggle::Ichannel>)> cb)
	{
		ENetAddress address;
		if (enet_address_set_host_ip(&address, ip.c_str()) != 0) {
			throw std::exception("enet_address_set_host_ip faild");
		}
		address.port = port;

		ENetPeer* peer = enet_host_connect(_host, &address, 1, 0);
		if (peer == nullptr) {
			throw std::exception("enet_host_connect faild");
		}

		std::string cb_handle = ip + ":" + std::to_string(port);
		cbs.insert(std::make_pair(cb_handle, cb));
	}

	void poll()
	{
		ENetEvent _event;
		if (enet_host_service(_host, &_event, 0) > 0) {
			switch (_event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				{
					char ip[256];
					enet_address_get_host_ip(&_event.peer->address, ip, 256);
					std::string cb_handle = std::string(ip) + ":" + std::to_string(_event.peer->address.port);
					auto cb = cbs[cb_handle];

					uint64_t peerHandle = (uint64_t)_event.peer->address.host << 32 | _event.peer->address.port;
					auto ch = std::make_shared<enetchannel>(_host, _event.peer);
					chs.insert(std::make_pair(peerHandle, ch));
					_process->reg_channel(ch);

					cb(ch);
					cbs.erase(cb_handle);
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				{
					uint64_t peerHandle = (uint64_t)_event.peer->address.host << 32 | _event.peer->address.port;
					chs[peerHandle]->recv((char*)_event.packet->data, _event.packet->dataLength);

					enet_packet_destroy(_event.packet);
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				{
					uint64_t peerHandle = (uint64_t)_event.peer->address.host << 32 | _event.peer->address.port;
					chs.erase(peerHandle);
				}
				break;
			default:
				break;
			}
		}
	}

private:
	ENetHost * _host;
	std::shared_ptr<juggle::process> _process;

	std::unordered_map<std::string, std::function<void(std::shared_ptr<juggle::Ichannel>)> > cbs;
	std::unordered_map<uint64_t, std::shared_ptr<enetchannel> > chs;
};

}

#endif
