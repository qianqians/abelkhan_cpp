/*
 * heartbeat_handle.h
 *
 *  Created on: 2016-7-14
 *      Author: qianqians
 */
#ifndef _heartbeat_handle_h
#define _heartbeat_handle_h

#include <boost/bind.hpp>
#include <timerservice.h>

#include "clientmanager.h"
#include "udpchannelmanager.h"

void heartbeat_handle(std::shared_ptr<gate::clientmanager> _clientmanager, std::shared_ptr<service::timerservice> _timerservice, int64_t tick) {
	_clientmanager->heartbeat_client(tick);
	_timerservice->addticktimer(5 * 1000, std::bind(&heartbeat_handle, _clientmanager, _timerservice, std::placeholders::_1));
}

void tick_udpchannel_handle(std::shared_ptr<gate::udpchannelmanager> _udpchannelmanager, std::shared_ptr<service::timerservice> _timerservice, int64_t tick) {
	_udpchannelmanager->tick_udpchannel(tick);
	_timerservice->addticktimer(10 * 1000, std::bind(&tick_udpchannel_handle, _udpchannelmanager, _timerservice, std::placeholders::_1));
}

#endif //_heartbeat_handle_h
