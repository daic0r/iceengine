//
//  EntityManager.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 02.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Entities/EntityManager.h>

namespace Ice {

bool EntityManager::m_bInstanced{ false };

EntityManager::EntityManager() noexcept {
    assert(!m_bInstanced);
    m_bInstanced = true;
}

}
