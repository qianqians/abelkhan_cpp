/*
 * udpchannelmanager.h
 *
 *  Created on: 2016-7-11
 *      Author: qianqians
 */
#ifndef _udpchannelmanager_h
#define _udpchannelmanager_h

#include <string>
#include <map>

#include <Ichannel.h>

#include "timerservice.h"

namespace gate {

class udpchannelmanager {
public:
	udpchannelmanager(std::shared_ptr<service::timerservice> _timerservice_) {
		_timerservice = _timerservice_;
	}

	~udpchannelmanager(){
	}

	void add_udpchannel(std::shared_ptr<juggle::Ichannel> ch) {
		ch_refresh_table.insert(std::make_pair(ch, _timerservice->Tick));
	}

	void refresh_udpchannel(std::shared_ptr<juggle::Ichannel> ch){
		if (ch_refresh_table.find(ch) != ch_refresh_table.end()){
			ch_refresh_table[ch] = _timerservice->Tick;
		}
	}

	void tick_udpchannel(int64_t tick){
		std::vector<std::shared_ptr<juggle::Ichannel> > remove;
		for(auto item : ch_refresh_table){
			if ((tick - item.second) > 30 * 1000){
				remove.push_back(item.first);
			}
		}
		for (auto ch : remove){
			ch_refresh_table.erase(ch);
			ch->disconnect();
		}
	}

private:
	std::map<std::shared_ptr<juggle::Ichannel>, int64_t > ch_refresh_table;

	std::shared_ptr<service::timerservice> _timerservice;

};

}

#endif //_udpchannelmanager_h
