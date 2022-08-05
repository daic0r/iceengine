#include <Entities/EntityComponentBuffer.h>

#include <map>
#include <typeindex>
#include <typeinfo>

namespace Ice {

namespace detail {

	std::map<std::type_index, void*> mInstances;

	void* getInstance(const std::type_info& ti) {
		auto iter = mInstances.find(std::type_index{ ti });
		if (iter == mInstances.end())
			return nullptr;
		return iter->second;
	}

	void setInstance(const std::type_info& ti, void* pInst) {
		auto idx = std::type_index{ ti };
		if (pInst == nullptr) {
			auto iter = mInstances.find(idx);
			if (iter != mInstances.end())
				mInstances.erase(iter);
			return;
		}
		mInstances[idx] = pInst;
	}
}

}
