/*
 * hub.h
 *
 *  Created on: 2020-1-10
 *      Author: qianqians
 */
#ifndef _hub_h
#define _hub_h

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

#include <acceptservice.h>
#include <process_.h>
#include <Ichannel.h>
#include <Imodule.h>
#include <channel.h>
#include <juggleservice.h>
#include <timerservice.h>

#include <config.h>
#include <module.h>
#include <modulemanager.h>

#include <hub_call_gatemodule.h>
#include <client_call_gatemodule.h>
#include <center_call_servermodule.h>

namespace hub{
	
class hub_service {
public:
	hub_service();

	void poll();

public:
	std::string uuid;
	std::string name;

	boost::signals2::signal<void(std::string) > sig_client_connect;
	boost::signals2::signal<void(std::string) > sig_client_disconnect;
	boost::signals2::signal<void(std::string) > sig_client_exception;
	boost::signals2::signal<void(std::string) > sig_hub_connect;

	common::modulemanager modules;

};
}

#endif //_hub_h
