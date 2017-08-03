/*this module file is codegen by juggle for c++*/
#ifndef _gate_call_client_module_h
#define _gate_call_client_module_h

#include "Imodule.h"

namespace module
{
class gate_call_client : public juggle::Imodule {
public:
    gate_call_client(){
        module_name = "gate_call_client";
        protcolcall_set.Add("connect_gate_sucess_handle", &gate_call_client::connect_gate_sucess_handle);
        protcolcall_set.Add("connect_hub_sucess_handle", &gate_call_client::connect_hub_sucess_handle);
        protcolcall_set.Add("call_client_handle", &gate_call_client::call_client_handle);
    }

    ~gate_call_client(){
    }

    virtual void connect_gate_sucess();
    void connect_gate_sucess_handle(const TArray< TSharedPtr<FJsonValue> >& _event){
        connect_gate_sucess();
    }

    virtual void connect_hub_sucess(FString argv0);
    void connect_hub_sucess_handle(const TArray< TSharedPtr<FJsonValue> >& _event){
		FString argv0 = nullptr;
		if ( !(_event[0])->TryGetString(argv0) ){
			return;
		}
        connect_hub_sucess(argv0);
    }

    virtual void call_client(FString argv0, FString argv1, TArray< TSharedPtr<FJsonValue> >* argv2);
    void call_client_handle(const TArray< TSharedPtr<FJsonValue> >& _event){
		FString argv0 = nullptr;
		if ( !(_event[0])->TryGetString(argv0) ){
			return;
		}
		FString argv1 = nullptr;
		if ( !(_event[1])->TryGetString(argv1) ){
			return;
		}
		TArray< TSharedPtr<FJsonValue> >* argv2 = nullptr;
		if ( !(_event[2])->TryGetArray(argv2) ){
			return;
		}
        call_client(argv0, argv1, argv2);
    }

};

}

#endif
