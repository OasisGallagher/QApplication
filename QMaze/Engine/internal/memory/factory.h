#include <map>

#include "object.h"
#include "memory.h"
#include "tools/debug.h"

class Factory {
	typedef Object* (*FatroyMethod)();
	typedef std::map<std::string, FatroyMethod> Container;
	Factory();

public:
	template <class Ty>
	static Ty* Create() {
		return Memory::Create<Ty>();
	}

	static Object* Create(const std::string& name) {
		Container::iterator pos = instance.factoryMethods_.find(name);
		AssertX(pos != instance.factoryMethods_.end(), "no factroy method exists for: " + name);
		return instance.factoryMethods_[name]();
	}

	static void Release(Object* pointer) {
		Memory::Release(pointer);
	}

	static void AddFactoryMethod(const std::string& name, FatroyMethod method) {
		bool done = instance.factoryMethods_.insert(std::make_pair(name, method)).second;
		AssertX(done, "failed to add factroy method for: " + name);
	}

private:
	static Factory instance;
	Container factoryMethods_;
};
