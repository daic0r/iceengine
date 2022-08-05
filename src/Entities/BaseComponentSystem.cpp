#include <Entities/BaseComponentSystem.h>

namespace Ice {

/*!
* \brief
* [BaseComponentSystem::addEntity]
*
* \author Matthias.Gruen
* \date 2020/10/01
* [10/1/2020 Matthias.Gruen]
*/
void BaseComponentSystem::addEntity(Entity entId, SceneId scId) noexcept
{
    std::scoped_lock<std::recursive_mutex> lck{ m_entityMtx };
    auto [iter, bInsertion] = m_sEntities[scId].emplace(entId);
    if (bInsertion)
        onEntityAdded(entId);
}

/*!
* \brief
* [BaseComponentSystem::removeEntity]
*
* \author Matthias.Gruen
* \date 2020/10/01
* [10/1/2020 Matthias.Gruen]
*/
void BaseComponentSystem::removeEntity(Entity entId, SceneId scId) noexcept
{
    std::scoped_lock<std::recursive_mutex> lck{ m_entityMtx };
    m_sEntities[scId].erase(entId);
}

}
