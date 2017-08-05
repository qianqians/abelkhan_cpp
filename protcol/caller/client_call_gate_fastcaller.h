/*this caller file is codegen by juggle for c++*/
#ifndef _client_call_gate_fast_caller_h
#define _client_call_gate_fast_caller_h
#include <sstream>
#include <tuple>
#include <string>
#include "Icaller.h"
#include "Ichannel.h"
#include <boost/any.hpp>
#include <memory>

namespace caller
{
class client_call_gate_fast : public juggle::Icaller {
public:
    client_call_gate_fast(std::shared_ptr<juggle::Ichannel> _ch) : Icaller(_ch) {
        module_name = "client_call_gate_fast";
    }

    ~client_call_gate_fast(){
    }

    void refresh_udp_end_point(){
        auto v = std::make_shared<std::vector<boost::any> >();
        v->push_back("client_call_gate_fast");
        v->push_back("refresh_udp_end_point");
        v->push_back(std::make_shared<std::vector<boost::any> >());
        ch->push(v);
    }

    void confirm_create_udp_link(std::string argv0){
        auto v = std::make_shared<std::vector<boost::any> >();
        v->push_back("client_call_gate_fast");
        v->push_back("confirm_create_udp_link");
        v->push_back(std::make_shared<std::vector<boost::any> >());
        (boost::any_cast<std::shared_ptr<std::vector<boost::any> > >((*v)[2]))->push_back(argv0);
        ch->push(v);
    }

};

}

#endif
