
#ifndef _module_h
#define _module_h

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

#include <boost/any.hpp>

namespace common
{

class imodule {
protected:
	std::map<std::string, std::function< void( std::shared_ptr<std::vector<boost::any> > ) > > cbs;

	void reg_cb(std::string cb_name, std::function< void( std::shared_ptr<std::vector<boost::any> > ) > cb)
	{
		cbs.insert(std::make_pair(cb_name, cb));
	}

public:
	void invoke(std::string cb_name, std::shared_ptr<std::vector<boost::any> > InArray)
	{
		auto cb = cbs.find(cb_name);
		if (cb != cbs.end())
		{
			(cb->second)(InArray);
		}
	}

};

}

#endif
