#ifndef IJSONSaver_h
#define IJSONSaver_h

#include <Entities/Entity.h>
#include <nlohmann/json.hpp>

namespace Ice {

class IJSONSaver {
public:
    virtual ~IJSONSaver() = default;
    virtual void save(nlohmann::json&, Entity) = 0;
};

}

#endif