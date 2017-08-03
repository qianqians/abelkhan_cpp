/*this module file is codegen by juggle for c++*/
#ifndef _gate_call_client_fast_module_h
#define _gate_call_client_fast_module_h

#include "Imodule.h"

namespace module
{
class gate_call_client_fast : public juggle::Imodule {
public:
    gate_call_client_fast(){
        module_name = "gate_call_client_fast";
        protcolcall_set.Add("confirm_refresh_udp_end_point_handle", &gate_call_client_fast::confirm_refresh_udp_end_point_handle);
        protcolcall_set.Add("call_client_handle", &gate_call_client_fast::call_client_handle);
    }

    ~gate_call_client_fast(){
    }

    virtual void confirm_refresh_udp_end_point();
    void confirm_refresh_udp_end_point_handle(const TArray< TSharedPtr<FJsonValue> >& _event){
        confirm_refresh_udp_end_point();
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
