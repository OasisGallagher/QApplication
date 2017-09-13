#pragma once

#include <map>

#include "object.h"
#include "memory.h"
#include "tools/debug.h"

class Factory {
	typedef Object (*FatroyMethod)();
	typedef std::map<std::string, FatroyMethod> Container;
	Factory();

public:
	template <class Internal>
	static typename Internal::Interface Create() {
		return typename Internal::Interface(Memory::Create<Internal>());
	}

	static Object Create(const std::string& name) {
		Container::iterator pos = instance.factoryMethods_.find(name);
		AssertX(pos != instance.factoryMethods_.end(), "no factroy method exists for: " + name);
		return Object(pos->second());
	}

	static void AddFactoryMethod(const std::string& name, FatroyMethod method) {
		bool done = instance.factoryMethods_.insert(std::make_pair(name, method)).second;
		AssertX(done, "failed to add factroy method for: " + name);
	}

private:
	static Factory instance;
	Container factoryMethods_;
};
