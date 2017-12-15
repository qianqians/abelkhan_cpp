/*this module file is codegen by juggle for c++*/
#ifndef _client_call_gate_fast_module_h
#define _client_call_gate_fast_module_h
#include "Imodule.h"
#include <memory>
#include <boost/signals2.hpp>
#include <string>

namespace module
{
class client_call_gate_fast : public juggle::Imodule {
public:
    client_call_gate_fast(){
        module_name = "client_call_gate_fast";
        protcolcall_set.insert(std::make_pair("refresh_udp_end_point", std::bind(&client_call_gate_fast::refresh_udp_end_point, this, std::placeholders::_1)));
        protcolcall_set.insert(std::make_pair("confirm_create_udp_link", std::bind(&client_call_gate_fast::confirm_create_udp_link, this, std::placeholders::_1)));
    }

    ~client_call_gate_fast(){
    }

    boost::signals2::signal<void() > sig_refresh_udp_end_point;
    void refresh_udp_end_point(std::shared_ptr<std::vector<boost::any> > _event){
        sig_refresh_udp_end_point();
    }

    boost::signals2::signal<void(std::string) > sig_confirm_create_udp_link;
    void confirm_create_udp_link(std::shared_ptr<std::vector<boost::any> > _event){
        sig_confirm_create_udp_link(
            boost::any_cast<std::string>((*_event)[0]));
    }

};

}

#endif
