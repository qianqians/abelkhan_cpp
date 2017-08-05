/*this module file is codegen by juggle for c++*/
#ifndef _gate_call_client_fast_module_h
#define _gate_call_client_fast_module_h
#include "Imodule.h"
#include <memory>
#include <boost/signals2.hpp>
#include <string>

namespace module
{
class gate_call_client_fast : public juggle::Imodule {
public:
    gate_call_client_fast(){
        module_name = "gate_call_client_fast";
        protcolcall_set.insert(std::make_pair("confirm_refresh_udp_end_point", std::bind(&gate_call_client_fast::confirm_refresh_udp_end_point, this, std::placeholders::_1)));
        protcolcall_set.insert(std::make_pair("call_client", std::bind(&gate_call_client_fast::call_client, this, std::placeholders::_1)));
    }

    ~gate_call_client_fast(){
    }

    boost::signals2::signal<void() > sig_confirm_refresh_udp_end_point;
    void confirm_refresh_udp_end_point(std::shared_ptr<std::vector<boost::any> > _event){
        sig_confirm_refresh_udp_end_point();
    }

    boost::signals2::signal<void(std::string, std::string, std::shared_ptr<std::vector<boost::any> >) > sig_call_client;
    void call_client(std::shared_ptr<std::vector<boost::any> > _event){
        sig_call_client(
            boost::any_cast<std::string>((*_event)[0]), 
            boost::any_cast<std::string>((*_event)[1]), 
            boost::any_cast<std::shared_ptr<std::vector<boost::any> >>((*_event)[2]));
    }

};

}

#endif
