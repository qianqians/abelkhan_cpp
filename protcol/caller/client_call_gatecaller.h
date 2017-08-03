/*this caller file is codegen by juggle for c++*/
#ifndef _client_call_gate_caller_h
#define _client_call_gate_caller_h

#include "Icaller.h"
#include "Ichannel.h"

namespace caller
{
class client_call_gate : public juggle::Icaller {
public:
    client_call_gate(TSharedPtr<juggle::Ichannel> _ch) : Icaller(_ch) {
        module_name = "client_call_gate";
    }

    ~client_call_gate(){
    }

    void connect_server(FString argv0,int64 argv1){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("connect_server")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv0)));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueNumber(argv1)));
        ch->push(v);
    }

    void cancle_server(){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("cancle_server")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        ch->push(v);
    }

    void connect_hub(FString argv0,FString argv1){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("connect_hub")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv0)));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv1)));
        ch->push(v);
    }

    void disconnect_hub(FString argv0,FString argv1){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("disconnect_hub")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv0)));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv1)));
        ch->push(v);
    }

    void forward_client_call_hub(FString argv0,FString argv1,FString argv2,TArray< TSharedPtr<FJsonValue> >* argv3){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("forward_client_call_hub")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv0)));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv1)));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueString(argv2)));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueArray(argv3)));
        ch->push(v);
    }

    void heartbeats(int64 argv0){
        TArray< TSharedPtr<FJsonValue> > v;
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("client_call_gate")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueString("heartbeats")));
        v.Push(TSharedPtr<FJsonValue>(new FJsonValueArray(TArray< TSharedPtr<FJsonValue> >()));
        (v[2])->AsArray().Push(TSharedPtr<FJsonValue>(new FJsonValueNumber(argv0)));
        ch->push(v);
    }

};

}

#endif
