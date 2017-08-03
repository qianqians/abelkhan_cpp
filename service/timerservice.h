
#ifndef _module_h
#define _module_h

#include "Json.h"
#include "Map.h"

namespace service
{

class timerservice {
public:
	void addticktimer(int64 _tick, callback cb, void * agrv)
	{
		while (cbs.Find(_tick) != nullptr)
		{
			_tick++;
		}

		struct func _func;
		_func.agrv = agrv;
		_func.cb = cb;
		cbs.Add(_tick, _func);
	}

	int64 poll()
	{
		Tick = time(nullptr);

		TArray<int64> remove;
		for (auto it = cbs.begin(); it != cbs.end(); it++)
		{
			if (it.Key() <= Tick)
			{
				it.Value().cb(agrv, Tick);

				remove.Add(it.Key());
			}
		}

		for (auto i = 0; i < remove.Num(); i++)
		{
			cbs.Remove(remove[i]);
		}

		return Tick;
	}

private:
	typedef void(*callback)(void * agrv, int64 tick);
	struct func
	{
		void * agrv;
		callback cb;
	};
	TMap<int64, func> cbs;

public:
	int64 Tick;

};

}

#endif
