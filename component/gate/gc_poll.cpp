/*
 * qianqians
 * 2016-7-12
 * gc_poll.cpp
 */
#include "gc_poll.h"

#include <list>

namespace gate {

	static std::list<std::function<void()> > gc_fn_list;

	void gc_put(std::function<void()> gc_fn)
	{
		gc_fn_list.push_back(gc_fn);
	}

	void gc_poll()
	{
		for (auto gc_fn : gc_fn_list)
		{
			gc_fn();
		}
		gc_fn_list.clear();
	}

}