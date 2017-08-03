/*this caller file is codegen by juggle for c++*/
#ifndef _client_call_gate_fast_caller_h
#define _client_call_gate_fast_caller_h

#include "Icaller.h"
#include "Ichannel.h"

namespace caller
{
class client_call_gate_fast : public juggle::Icaller {
public:
    client_call_gate_fast(TSharedPtr<juggle::Ichannel> _ch) : Icaller(_ch) {
        module_name = "client_call_gate_fast";
    }

    ~client_call_gate_fast(){
    }

    void refresh_udp_end_point(){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate_fast")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("refresh_udp_end_point")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        ch->push(v);
    }

    void confirm_create_udp_link(FString argv0){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate_fast")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("confirm_create_udp_link")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv0)));
        ch->push(v);
    }

};

}

#endif
