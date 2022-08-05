//
//  SaveSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 09.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/SaveSystem.h>
#include <Entities/EntityManager.h>
#include <Utils/TemplateHelpers.h>
#include <Interfaces/IStreamSaver.h>
#include <Interfaces/IJSONSaver.h>
//#include <nlohmann/json.hpp>

namespace Ice {

template<typename Output>
void SaveSystem::save(Output& os) {
    
    bool bIsFirst{ true };
    for (auto e : entities(entityManager.currentScene())) {
        if constexpr(std::is_same_v<Output, std::ostream>) {
            if (!bIsFirst) {
                os << "\n";
            }
        }
        save(os, e);
        if constexpr(std::is_same_v<Output, std::ostream>)
            bIsFirst = false;
    }
}

template<typename Output>
void SaveSystem::save(Output& os, Entity e) {
    if constexpr(std::is_same_v<Output, std::ostream>) {
        for (auto& pSaveable : entityManager.savers()) {
            std::visit(visitor {
                [&os,e](IStreamSaver* p) {
                    p->save(os, e);
                },
                [](IJSONSaver* p) {
                }
            }, pSaveable);
        }
    }
    else if constexpr(std::is_same_v<Output, nlohmann::json>) {
        os.push_back(nlohmann::json{});
        auto& objEntry = os.back();
        for (auto& pSaveable : entityManager.savers()) {
            std::visit(visitor {
                [](IStreamSaver* p) {
                },
                [&objEntry,e](IJSONSaver* p) {
                    p->save(objEntry, e);
                }
            }, pSaveable);
        }
    }
}

template void SaveSystem::save<std::ostream>(std::ostream&);
template void SaveSystem::save<nlohmann::json>(nlohmann::json&);
template void SaveSystem::save<std::ostream>(std::ostream&, Entity);
template void SaveSystem::save<nlohmann::json>(nlohmann::json&, Entity);

}
