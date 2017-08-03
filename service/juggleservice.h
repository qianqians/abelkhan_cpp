
#ifndef _module_h
#define _module_h

#include "Json.h"
#include "Map.h"

namespace service
{

class juggleservice {
public:
	void reg_cb(FString cb_name, callback cb)
	{
		cbs.Add(cb_name, cb);
	}

	void invoke(FString cb_name, const TArray< TSharedPtr<FJsonValue> > & InArray)
	{
		auto cb = cbs.Find(cb_name);
		if (cb != nullptr)
		{
			cb(InArray);
		}
	}

private:
	typedef void(*callback)(const TArray< TSharedPtr<FJsonValue> > & InArray);
	TMap<FString, callback> cbs;

};

}

#endif
