#include <Components/ComponentManager.h>
#include <fstream>
#include <vector>
#include <Components/ComponentIds.h>
#include <Components/GraphEdgeComponent.h>
#include <Components/GraphNodeComponent.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <Components/PathSegmentComponent.h>
#include <Components/TransformComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/SaveComponent.h>
#include <Components/TagComponent.h>
#include <Components/BuildingComponent.h>
#include <Components/BlueprintComponent.h>
#include <Components/MeshGroupComponent.h>
#include <Components/SharedComponent.h>
#include <sstream>
#include <Entities/EntityManager.h>
#include <nlohmann/json.hpp>

namespace Ice {

/*!
* \brief
* [ComponentManager::ComponentManager]
*
* \author Matthias.Gruen
* \date 2020/11/05
* [11/5/2020 Matthias.Gruen]
*/
ComponentManager::ComponentManager()
{

// 	TERRAIN_COMPONENT,
// 		MESH_COMPONENT,
// 		TEXTURE_COMPONENT,
// 		RENDER_MATERIALS_COMPONENT,
// 		TRANSFORM_COMPONENT,
// 		MODEL_REFERENCE_COMPONENT,
// 		PROJECTION_COMPONENT,
// 		CAMERA_COMPONENT,
// 		TAG_COMPONENT,
// 		INPUT_RECEIVER_COMPONENT,
// 		SKYBOX_COMPONENT,
// 		SKELETON_COMPONENT,
// 		ANIMATED_MESH_COMPONENT,
// 		MODEL_ANIMATION_COMPONENT,
// 		GRAPH_NODE_COMPONENT,
// 		GRAPH_EDGE_COMPONENT,
// 		PATH_COMPONENT,
// 		PATH_SEGMENT_COMPONENT,
// 		MAX_ID

	// Cannot call add component on a base class like "BaseComponent" because this is
	// a template method. Therefore this must be done separately for every type.
	m_mLoadComponentFuncs = {
		//{
		//	GRAPH_EDGE_COMPONENT,
		//	[](Entity e, std::istream& is) {
		//		return loadComponent<GraphEdgeComponent>(e, is);
		//	}
		//},
		{
			GRAPH_NODE_COMPONENT,
			[](Entity e, std::istream& is) {
				return loadComponent<GraphNodeComponent>(e, is);
			}
		},
        {
            MESH_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<MeshComponent>(e, is);
            }
        },
        {
            RENDER_MATERIALS_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<RenderMaterialsComponent>(e, is);
            }
        },
        {
            PATH_SEGMENT_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<PathSegmentComponent>(e, is);
            }
        },
		{
			TRANSFORM_COMPONENT,
			[](Entity e, std::istream& is) {
				return loadComponent<TransformComponent>(e, is);
			}
		},
		{
			MODEL_REFERENCE_COMPONENT,
			[](Entity e, std::istream& is) {
				return loadComponent<ModelReferenceComponent>(e, is);
			}
		},
        {
            TAG_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<TagComponent>(e, is);
            }
        },
		{
			BLUEPRINT_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<BlueprintComponent>(e, is);
            }
		},
        {
            MESH_GROUP_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<MeshGroupComponent>(e, is);
            }
        },
        {
            BUILDING_COMPONENT,
            [](Ice::Entity e, std::istream& is) {
                return loadComponent<BuildingComponent>(e, is);
            }
        },
        {
            SHARED_COMPONENT_BASE + TRANSFORM_COMPONENT,
            [](Entity e, std::istream& is) {
                return loadComponent<SharedComponent<TransformComponent>>(e, is);
            }
        }

	};

	m_mCreateCompFuncs = {
		//{
		//	GRAPH_EDGE_COMPONENT,
		//	[](Entity e, std::istream& is) {
		//		return loadComponent<GraphEdgeComponent>(e, is);
		//	}
		//},
		{
			GRAPH_NODE_COMPONENT,
			[](Entity e, const nlohmann::json& j) {
				addAndDeserialize<GraphNodeComponent>(e, j);
			}
		},
        {
            MESH_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<MeshComponent>(e, j);
            }
        },
        {
            RENDER_MATERIALS_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<RenderMaterialsComponent>(e, j);
            }
        },
        {
            PATH_SEGMENT_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<PathSegmentComponent>(e, j);
            }
        },
		{
			TRANSFORM_COMPONENT,
			[](Entity e, const nlohmann::json& j) {
				addAndDeserialize<TransformComponent>(e, j);
			}
		},
		{
			MODEL_REFERENCE_COMPONENT,
			[](Entity e, const nlohmann::json& j) {
				addAndDeserialize<ModelReferenceComponent>(e, j);
			}
		},
        {
            TAG_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<TagComponent>(e, j);
            }
        },
		{
			BLUEPRINT_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<BlueprintComponent>(e, j);
            }
		},
        {
            MESH_GROUP_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<MeshGroupComponent>(e, j);
            }
        },
        {
            BUILDING_COMPONENT,
            [](Ice::Entity e, const nlohmann::json& j) {
                addAndDeserialize<BuildingComponent>(e, j);
            }
        },
        {
            SHARED_COMPONENT_BASE + TRANSFORM_COMPONENT,
            [](Entity e, const nlohmann::json& j) {
                addAndDeserialize<SharedComponent<TransformComponent>>(e, j);
            }
        }
	};

}

/*!
* \brief
* [ComponentManager::loadEntityFile]
*
* \author Matthias.Gruen
* \date 2020/11/05
* [11/5/2020 Matthias.Gruen]
*/
std::vector<Entity> ComponentManager::loadEntityFile(const std::string& strFile) const
{
	std::vector<Entity> vRet;
	std::ifstream f{ strFile };
	if (!f)
		return vRet;

	std::vector<std::string> vLines;
	std::string strLine;
	while (std::getline(f, strLine)) {
		vLines.emplace_back(strLine);
	}
	f.close();

	for (const auto& strLine : vLines) {
		std::stringstream ss{ strLine };

		Entity e = entityManager.createEntity();

		EntityComponentId entId{ 0 };
		while (ss >> entId) {

			auto loadFuncIter = m_mLoadComponentFuncs.find(entId);
			if (loadFuncIter == m_mLoadComponentFuncs.end()) {
				if (!subLoadComponent(e, entId, ss)) {
					std::stringstream ss;
					ss << "Unknown component id encountered when reading component file " << strFile << "; ID was " << entId;
					throw std::runtime_error(ss.str());
				}
			}
			else
				loadFuncIter->second(e, ss);
		}
		vRet.push_back(e);
	}			

	return vRet;
}
	
std::vector<Entity> ComponentManager::loadEntityFileJson(const std::string& strFile) const {
	std::vector<Entity> vRet;
	std::ifstream f{ strFile };
	if (!f)
		return vRet;
	const auto strContents = std::string{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
	auto j = nlohmann::json::parse(strContents);

	for (auto objIter = j.cbegin(); objIter != j.cend(); ++objIter) {
		auto e = entityManager.createEntity();

		for (auto compIter = objIter->cbegin(); compIter != objIter->cend(); ++compIter) {
			EntityComponentId entId = compIter->at("componentId");
			auto createFuncIter = m_mCreateCompFuncs.find(entId);
			auto& data = compIter->at("data");
			if (createFuncIter == m_mCreateCompFuncs.end()) {
				if (!subLoadComponent(e, entId, data)) {
					std::stringstream ss;
					ss << "Unknown component id encountered when reading component file " << strFile << "; ID was " << entId;
					throw std::runtime_error(ss.str());
				}

			} else {
				createFuncIter->second(e, data);
			}
		}
		vRet.push_back(e);
	} 

	return vRet;
}



}
