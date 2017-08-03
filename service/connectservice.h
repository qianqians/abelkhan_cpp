
#ifndef _modulemng_h
#define _modulemng_h

#include "module.h"

namespace common
{

class modulemanager {
public:
	void add_module(FString module_name, TSharedPtr<imodule> module)
	{
		modules.Add(module_name, module);
	}

	public void process_module_mothed(FString module_name, FString cb_name, const TArray< TSharedPtr<FJsonValue> > & InArray)
	{
		auto module = modules.Find(module_name);
		if (module != nullptr)
		{
			(*module)->invoke(cb_name, InArray);
		}
	}

private:
	TMap<FString, TSharedPtr<imodule> > modules;

};

}

#endif
