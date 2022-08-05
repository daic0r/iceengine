//
//  PathAnimationSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/PathAnimationSystem.h>

#include <Entities/Entity.h>
#include <Entities/EntityManager.h>
#include <Components/TransformComponent.h>
#include <Entities/EntityComponentBuffer.h>
#include <Components/PathComponent.h>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <Components/LocationComponent.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
namespace Ice {

bool PathAnimationSystem::update(float fDeltaTime) {
    for (auto e : entities(entityManager.currentScene())) {
        
        auto& aniComp = entityManager.getComponent<PathAnimationComponent>(e);
        
        auto pathEnt = aniComp.m_vPaths.at(aniComp.m_nCurrentPathIdx).first;
        bool bInverted = aniComp.m_vPaths.at(aniComp.m_nCurrentPathIdx).second;
        
        ///////////////////////////////////////////////////////////////
        // Lambda to get the point, the tangent angle and the path-specific station
        auto getPointAngleStation = [&bInverted, &pathEnt, &aniComp](const PathComponent& pathComp) {
            glm::vec3 tangent;
            float fStation = aniComp.m_fCurrentPosition;
            fStation = bInverted ? pathComp.m_path.length() - fStation : fStation;
            [[maybe_unused]] auto point = pathComp.m_path.getPointAt(fStation, &tangent);
            
            const float fAngleDirectionSign = glm::cross(glm::vec3{ .0f, 0.0f, 1.f }, tangent).y;
            float fAngle = acosf(glm::dot(tangent, glm::vec3{ .0f, 0.0f, 1.f }));
            fAngle = fAngleDirectionSign >= 0.0f ? fAngle : 2*M_PI - fAngle;
            fAngle = !bInverted ? fAngle : fAngle + M_PI;
            
            return std::make_tuple(point, fAngle, fStation);
        };
        ///////////////////////////////////////////////////////////////

        const auto& pathComp = entityManager.getComponent<PathComponent>(pathEnt);
        auto [oldPoint, fOldAngle, fStationOld] = getPointAngleStation(pathComp);

        // Advance position
        aniComp.m_fCurrentPosition += fDeltaTime * aniComp.m_fSpeed;

        // If we're done with this path, move on to the next one
        if (aniComp.m_fCurrentPosition > pathComp.m_path.length() || (aniComp.m_fTargetPosition >= 0.0f && aniComp.m_nCurrentPathIdx == aniComp.m_vPaths.size() - 1 && aniComp.m_fCurrentPosition >= aniComp.m_fTargetPosition)) {
            aniComp.m_fCurrentPosition -= pathComp.m_path.length();
            ++aniComp.m_nCurrentPathIdx;
            if (aniComp.m_nCurrentPathIdx == aniComp.m_vPaths.size()) {
                m_vDoneEntities.push_back(e);
                continue;
            }
            pathEnt = aniComp.m_vPaths.at(aniComp.m_nCurrentPathIdx).first;
            bInverted = aniComp.m_vPaths.at(aniComp.m_nCurrentPathIdx).second;
        }
        
        const auto& pathCompNow = entityManager.getComponent<PathComponent>(pathEnt);
        auto [p, fAngle, fStation] = getPointAngleStation(pathCompNow);

        // Update transform component
        // First the old rotation is undone, then the new one is applied
        auto invOldRot = !aniComp.m_bFirstFrame ? glm::inverse(glm::rotate(glm::mat4{ 1.0f }, fOldAngle, glm::vec3{ 0.0f, 1.0f, 0.0f })) : glm::mat4{ 1.0f };//
        auto rotMat = glm::rotate(glm::mat4{ 1.0f }, fAngle, glm::vec3{ 0.0f, 1.0f, 0.0f });
        entityManager.getComponent<TransformComponent>(e, [&p=p, &rotMat, &invOldRot](TransformComponent& transComp) {
            transComp.m_transform = transComp.m_transform * rotMat * invOldRot;
            transComp.m_transform[3] = glm::vec4{ p, 1.0f };
        });
        
        // Update location
		if (entityManager.hasComponent<LocationComponent>(e)) {
			auto& locationComp = entityManager.getComponent<LocationComponent>(e);
			auto locIter = std::find_if(locationComp.m_locations.begin(), locationComp.m_locations.end(), [&aniComp](const auto& kvp) {
				return kvp.first == aniComp.m_nodeType;
				});
			if (locIter == locationComp.m_locations.end()) {
				locationComp.m_locations.emplace_back(aniComp.m_nodeType, LocationComponent::Location{});
				locIter = locationComp.m_locations.end() - 1;
			}
			locIter->second.m_path = pathEnt;
			locIter->second.m_fStation = fStation;
		}
        
        aniComp.m_bFirstFrame = false;
    }
    
    if (m_vDoneEntities.size() > 0) {
        for (auto e : m_vDoneEntities) {
            entityManager.removeComponent<PathAnimationComponent>(e);
        }
        m_vDoneEntities.clear();
    }

    return true;
}

}
