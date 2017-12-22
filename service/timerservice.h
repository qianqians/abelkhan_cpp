
#ifndef _timerservice_h
#define _timerservice_h

#include <time.h>

#include <functional>
#include <map>
#include <vector>

namespace service
{

class timerservice {
private:
	std::map<int64_t, std::function< void(int64_t) > > cbs;

public:
	timerservice() 
	{
		Tick = time(nullptr) * 1000;
	}

	void addticktimer(int64_t _tick, std::function< void(int64_t) > cb)
	{
		_tick += Tick;
		while (cbs.find(_tick) != cbs.end())
		{
			_tick++;
		}

		cbs.insert(std::make_pair(_tick, cb));
	}

	int64_t poll()
	{
		Tick = time(nullptr) * 1000;

		std::vector<int64_t> remove;
		for (auto it = cbs.begin(); it != cbs.end(); it++)
		{
			if (it->first <= Tick)
			{
				it->second(it->first);

				remove.push_back(it->first);
			}
		}

		for (auto key : remove)
		{
			cbs.erase(key);
		}

		return Tick;
	}

public:
	time_t Tick;

};

}

#endif
