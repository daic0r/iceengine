#ifndef Octree_h
#define Octree_h

#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <System/Math.h>
#include <System/AABB.h>
#include <ranges>
#include <variant>
#include <System/Ray.h>
#include <Utils/TemplateHelpers.h>
#include <memory>
#include <stack>

namespace Ice
{
    struct sTraversalDebugInfo {
        std::size_t nID{};
        const AABB* box{};
        bool bIsLeaf{};
    };
    class IOctreeTraversal {
    public:
        virtual sTraversalDebugInfo startTraversal() noexcept = 0;
        virtual sTraversalDebugInfo next() noexcept = 0;
        virtual bool done() const noexcept = 0;
        virtual const std::vector<std::size_t>& intersected() const noexcept = 0;
    };

    template<typename T>
    class Octree : public IOctreeTraversal {
        friend class IOctreeTraversal;

        enum NodePosition {
            BOTTOM_LEFT_FRONT,
            BOTTOM_LEFT_BACK,
            TOP_LEFT_FRONT,
            TOP_LEFT_BACK,
            BOTTOM_RIGHT_FRONT,
            BOTTOM_RIGHT_BACK,
            TOP_RIGHT_FRONT,
            TOP_RIGHT_BACK,
            EXIT
        };

        enum class Plane {
            YZ,
            XZ,
            XY
        };

        struct sIntersectParams {
            glm::vec3 t0; // entry point along ray in each dimension
            glm::vec3 t1; // exit point along ray in each dimension
            glm::vec3 tm; // center point along ray in each dimension

        };
        struct leaf_node;
        struct branch_node;

        using node_t = std::variant<branch_node, leaf_node>;

        struct base_node {
            static inline std::size_t NEXT_ID = 1;
            AABB box;
            std::size_t nID = NEXT_ID++;
        };
        struct leaf_node : base_node {
            std::vector<T> vObjects;
        };
        struct branch_node : base_node {
            std::array<std::unique_ptr<node_t>, 8> arNodes;
        };

        void construct(const std::vector<std::pair<AABB, T>>&, std::unique_ptr<node_t>& node, const AABB& originalBox, int nLevel = 0);
        
        template<typename NodePositionMappingFunc>
        bool intersects_impl(const Ray&, const node_t* node, const sIntersectParams& params, NodePositionMappingFunc&& mappingFunc, std::vector<T>& vOut) const;

        node_t* find(const glm::vec3& point, node_t* pStart = nullptr) const noexcept;

        static NodePosition getFirstSubNodeIndex(const sIntersectParams& params) noexcept;
        static NodePosition getNextSubNode(NodePosition current, const sIntersectParams& params);
        static sIntersectParams calculateIntersectParams(const Ray&, const AABB& box) noexcept;
        auto transformRayAndGetNodeMappingFunction(Ray&) const noexcept;
        branch_node& root() noexcept { return std::get<branch_node>(*m_root); };
        const branch_node& root() const noexcept { return std::get<branch_node>(*m_root); };
        const AABB& boundingBox() const noexcept { return root().box; }
    public:
        Octree() = default;
        Octree(const std::vector<std::pair<AABB, T>>& vBoxes, const AABB& outerBox);
        bool intersects(Ray, std::vector<T>& vOut) const;
        void emplace(const AABB& box,const T& val);


        /////////////////////////////////////////////////////////
        // Traversal
        sTraversalDebugInfo startTraversal() noexcept override {
            m_vTraverseNodes.emplace(m_root.get());
            return { root().nID, &boundingBox(), false };
        }

       sTraversalDebugInfo next() noexcept override {
            if (m_vTraverseNodes.empty())
                return { 0, nullptr, false };
            auto pTop = m_vTraverseNodes.top();
            m_vTraverseNodes.pop();
            return std::visit(visitor{ 
                [&,this](const branch_node& branch) {
                    for (int i = 0; i < 8; ++i)
                        m_vTraverseNodes.emplace(branch.arNodes[7-i].get());
                    return sTraversalDebugInfo{ branch.nID, &branch.box, false };
                },
                [&](const leaf_node& leaf) {
                    m_vTraverseNodes.pop();
                    return sTraversalDebugInfo{ leaf.nID, &leaf.box, true };
                }
            }, *pTop);
        }
        bool done() const noexcept override { 
            return m_vTraverseNodes.empty();
        }
        const std::vector<std::size_t>& intersected() const noexcept { return m_vIntersectLeaves; }
        /////////////////////////////////////////////////////////

    private:

        std::unique_ptr<node_t> m_root;

        std::stack<node_t*> m_vTraverseNodes;
        mutable std::vector<std::size_t> m_vIntersectLeaves;
   
    };
    
    template<typename T>
    Octree<T>::Octree(const std::vector<std::pair<AABB, T>>& vBoxes, const AABB& outerBox) {
        construct(vBoxes, m_root, outerBox);
    }

    template<typename T>
    void Octree<T>::construct(const std::vector<std::pair<AABB, T>>& vPoints, std::unique_ptr<node_t>& node, const AABB& originalBox, int nLevel) {
        const auto center = originalBox.center();

        struct sNodeInfo {
            std::vector<std::pair<AABB, T>> vNodeContents;
            AABB box;
        };
        std::array<sNodeInfo, 8> arSubNodes;
        for (int i = 0; i < 8; ++i) {
            AABB box = originalBox;
            switch (i) {
                case BOTTOM_LEFT_BACK:
                    box.maxVertex().x = center[0];
                    box.maxVertex().y = center[1];
                    box.minVertex().z = center[2];
                    break;
                 case BOTTOM_RIGHT_BACK:
                    box.minVertex().x = center[0];
                    box.maxVertex().y = center[1];
                    box.minVertex().z = center[2];
                    break;
                case BOTTOM_LEFT_FRONT:
                    box.maxVertex().x = center[0];
                    box.maxVertex().y = center[1];
                    box.maxVertex().z = center[2];
                    break;
                 case BOTTOM_RIGHT_FRONT:
                    box.minVertex().x = center[0];
                    box.maxVertex().y = center[1];
                    box.maxVertex().z = center[2];
                    break;
                case TOP_LEFT_BACK:
                    box.maxVertex().x = center[0];
                    box.minVertex().y = center[1];
                    box.minVertex().z = center[2];
                    break;
                 case TOP_RIGHT_BACK:
                    box.minVertex().x = center[0];
                    box.minVertex().y = center[1];
                    box.minVertex().z = center[2];
                    break;
                case TOP_LEFT_FRONT:
                    box.maxVertex().x = center[0];
                    box.minVertex().y = center[1];
                    box.maxVertex().z = center[2];
                    break;
                 case TOP_RIGHT_FRONT:
                    box.minVertex().x = center[0];
                    box.minVertex().y = center[1];
                    box.maxVertex().z = center[2];
                    break;
                default:
                    throw std::logic_error("Invalid index");
            }
            arSubNodes[i].box = box;
            std::ranges::copy_if(vPoints, std::back_inserter(arSubNodes[i].vNodeContents), [&box](const auto& elem) {
                return box.intersects(elem.first);
            });
       }

        node = std::make_unique<node_t>();
        if (nLevel < 6 && vPoints.size() > 2) {
            *node = branch_node{};
            auto& thisNode = std::get<branch_node>(*node);
            thisNode.box = originalBox;
            for (int i = 0; i < 8; ++i) {
                construct(arSubNodes[i].vNodeContents, thisNode.arNodes[i], arSubNodes[i].box, nLevel + 1);
            }
        }
        else {
            *node = leaf_node{};
            auto& thisNode = std::get<leaf_node>(*node);
            thisNode.box = originalBox;
            auto& v = thisNode.vObjects;
            std::ranges::transform(vPoints, std::back_inserter(v), [](const std::pair<AABB, T>& kvp) {
                const auto bIsFirst = kvp.second[0].x == 0 && kvp.second[1].x == 0 && kvp.second[2].x == 15 && kvp.second[0].z == 0 && kvp.second[1].z == 15 && kvp.second[2].z == 0;
                const auto bIsSecond = kvp.second[0].x == 15 && kvp.second[1].x == 15 && kvp.second[2].x == 30 && kvp.second[0].z == 0 && kvp.second[1].z == 15 && kvp.second[2].z == 15;
                return kvp.second;
            });
        }
    }

    template<typename T>
    Octree<T>::node_t* Octree<T>::find(const glm::vec3& point, node_t* pStart) const noexcept {
        auto pCurNode = pStart ? pStart : m_root.get();
        if (!std::visit([&point](const auto& start) { return start.box.contains(point); }, *pCurNode))
            return nullptr;
        node_t* pRet{};
        while (pCurNode) {
            std::visit(visitor {
                [&point,&pCurNode](const branch_node& branch) {
                    for (const auto& pSubNode : branch.arNodes) {
                        const auto bContains = std::visit( 
                            [&point](const auto& subNode) {
                                return subNode.box.contains(point);
                            }
                        , *pSubNode);
                        if (bContains) {
                            pCurNode = pSubNode.get();
                            return;
                        }
                    }
                    throw std::logic_error("Point contained by surrounding box but not by any of the contained ones!");
                },
                [&pRet,&pCurNode](const leaf_node& leaf){
                    pRet = pCurNode;
                    pCurNode = nullptr;
                } 
            },  *pCurNode);
        }
        return pRet;
    }

    template<typename T>
    template<typename NodePositionMappingFunc>
    bool Octree<T>::intersects_impl(const Ray& ray, const node_t* node, const sIntersectParams& params, NodePositionMappingFunc&& mappingFunc, std::vector<T>& vOut) const {
        if (params.t1.x < 0.0f || params.t1.y < 0.0f || params.t1.z < 0.0f)
            return false;
        const auto bRet = std::visit(visitor {
            [&,this,mappingFunc=std::forward<NodePositionMappingFunc>(mappingFunc)](const branch_node& curNode) { 
                bool bRet{};

                auto nSubNode = getFirstSubNodeIndex(params);
                while (nSubNode != NodePosition::EXIT) {
                    sIntersectParams subNodeParams; 
                    // Get sub-node params
                    switch (nSubNode) {
                        case BOTTOM_LEFT_FRONT:
                            subNodeParams.t0 = glm::vec3{ params.t0.x, params.t0.y, params.t0.z };
                            subNodeParams.t1 = glm::vec3{ params.tm.x, params.tm.y, params.tm.z };
                            break;
                        case BOTTOM_LEFT_BACK:
                            subNodeParams.t0 = glm::vec3{ params.t0.x, params.t0.y, params.tm.z };
                            subNodeParams.t1 = glm::vec3{ params.tm.x, params.tm.y, params.t1.z };
                            break;
                        case TOP_LEFT_FRONT:
                            subNodeParams.t0 = glm::vec3{ params.t0.x, params.tm.y, params.t0.z };
                            subNodeParams.t1 = glm::vec3{ params.tm.x, params.t1.y, params.tm.z };
                            break;
                        case TOP_LEFT_BACK:
                            subNodeParams.t0 = glm::vec3{ params.t0.x, params.tm.y, params.tm.z };
                            subNodeParams.t1 = glm::vec3{ params.tm.x, params.t1.y, params.t1.z };
                            break;
                        case BOTTOM_RIGHT_FRONT:
                            subNodeParams.t0 = glm::vec3{ params.tm.x, params.t0.y, params.t0.z };
                            subNodeParams.t1 = glm::vec3{ params.t1.x, params.tm.y, params.tm.z };
                            break;
                        case BOTTOM_RIGHT_BACK:
                            subNodeParams.t0 = glm::vec3{ params.tm.x, params.t0.y, params.tm.z };
                            subNodeParams.t1 = glm::vec3{ params.t1.x, params.tm.y, params.t1.z };
                            break;
                        case TOP_RIGHT_FRONT:
                            subNodeParams.t0 = glm::vec3{ params.tm.x, params.tm.y, params.t0.z };
                            subNodeParams.t1 = glm::vec3{ params.t1.x, params.t1.y, params.tm.z };
                            break;
                        case TOP_RIGHT_BACK:
                            subNodeParams.t0 = glm::vec3{ params.tm.x, params.tm.y, params.tm.z };
                            subNodeParams.t1 = glm::vec3{ params.t1.x, params.t1.y, params.t1.z };
                            break;
                    }
                    subNodeParams.tm = (params.t0 + params.t1) * 0.5f;
                    // Process
                    const auto nTranslatedIndex = mappingFunc(nSubNode);
                    const auto pSubNode = curNode.arNodes[nTranslatedIndex].get(); 
                    bRet |= intersects_impl(ray, pSubNode, subNodeParams, mappingFunc, vOut);
                    // Determine next node
                    nSubNode = getNextSubNode(nSubNode, params);
                }
                return bRet;
            },
            [&vOut,this](const leaf_node& leaf) {
                if (leaf.vObjects.size() > 0)
                    this->m_vIntersectLeaves.push_back(leaf.nID);
                vOut.insert(vOut.end(), leaf.vObjects.begin(), leaf.vObjects.end());
                return !vOut.empty();
            }
        }, *node);
        return bRet;
    }

    template<typename T>
    bool Octree<T>::intersects(Ray r, std::vector<T>& vOut) const {
        m_vIntersectLeaves.clear();
        auto mapNodeFunc = transformRayAndGetNodeMappingFunction(r);
        auto params = calculateIntersectParams(r, boundingBox());
        float t;
        if ((t = std::max(params.t0[0], std::max(params.t0[1], params.t0[2]))) >= std::min(params.t1[0], std::min(params.t1[1], params.t1[2])))
            return false;
        /*
        if (t < 0.0f) {
            r.setOrigin(r.origin() + t * r.direction());
            params = calculateIntersectParams(r, boundingBox());
        }
        */
        return intersects_impl(r, m_root.get(), params, mapNodeFunc, vOut);
    }

    template<typename T>
    Octree<T>::sIntersectParams Octree<T>::calculateIntersectParams(const Ray& ray, const AABB& box) noexcept {
        sIntersectParams params;
        params.t0 = (box.minVertex() - ray.origin()) / ray.direction();
        params.t1 = (box.maxVertex() - ray.origin()) / ray.direction();
        params.tm = (params.t0 + params.t1) * 0.5f;
        return params;
    }
     
    template<typename T>
    Octree<T>::NodePosition Octree<T>::getFirstSubNodeIndex(const sIntersectParams& params) noexcept {
        int nRetOctant{};
        const std::array<float, 3> arT0 = { params.t0.x, params.t0.y, params.t0.z };
        const auto maxt0dim = static_cast<Plane>(std::distance(arT0.begin(), std::ranges::max_element(arT0)));
        switch (maxt0dim) {
            case Plane::XY:
                nRetOctant |= 4 * static_cast<int>(params.tm[0] < params.t0[2]); 
                nRetOctant |= 2 * static_cast<int>(params.tm[1] < params.t0[2]); 
                break;
            case Plane::XZ:
                nRetOctant |= 4 * static_cast<int>(params.tm[0] < params.t0[1]); 
                nRetOctant |= 1 * static_cast<int>(params.tm[2] < params.t0[1]); 
                break; 
            case Plane::YZ:
                nRetOctant |= 2 * static_cast<int>(params.tm[1] < params.t0[0]); 
                nRetOctant |= 1 * static_cast<int>(params.tm[2] < params.t0[0]); 
                break;
        }
        return static_cast<NodePosition>(nRetOctant);
    }

    template<typename T>
    Octree<T>::NodePosition Octree<T>::getNextSubNode(NodePosition current, const sIntersectParams& params) {
        static const auto minIndex = [](float a, float b, float c) {
            const std::array<float, 3> ar{a,b,c};
            return static_cast<Plane>(std::distance(ar.begin(), std::ranges::min_element(ar)));
        };
        Plane exitPlane;
        switch (current) {
            case NodePosition::BOTTOM_LEFT_FRONT:
                {
                    exitPlane = minIndex(params.tm.x, params.tm.y, params.tm.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::BOTTOM_RIGHT_FRONT;
                        case Plane::XZ:
                            return NodePosition::TOP_LEFT_FRONT;
                        case Plane::XY:
                            return NodePosition::BOTTOM_LEFT_BACK;
                    }
                }
                break;
            case NodePosition::BOTTOM_LEFT_BACK:
                {
                    exitPlane = minIndex(params.tm.x, params.tm.y, params.t1.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::BOTTOM_RIGHT_BACK;
                        case Plane::XZ:
                            return NodePosition::TOP_LEFT_BACK;
                        case Plane::XY:
                            return NodePosition::EXIT;
                    }
                }
                break;
            case NodePosition::TOP_LEFT_FRONT:
                {
                    exitPlane = minIndex(params.tm.x, params.t1.y, params.tm.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::TOP_RIGHT_FRONT;
                        case Plane::XZ:
                            return NodePosition::EXIT;
                        case Plane::XY:
                            return NodePosition::TOP_LEFT_BACK;
                    }
                }
                break;
            case NodePosition::TOP_LEFT_BACK:
                {
                    exitPlane = minIndex(params.tm.x, params.t1.y, params.t1.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::TOP_RIGHT_BACK;
                        case Plane::XZ:
                            return NodePosition::EXIT;
                        case Plane::XY:
                            return NodePosition::EXIT;
                    }
                }
                break;
            case NodePosition::BOTTOM_RIGHT_FRONT:
                {
                    exitPlane = minIndex(params.t1.x, params.tm.y, params.tm.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::EXIT;
                        case Plane::XZ:
                            return NodePosition::TOP_RIGHT_FRONT;
                        case Plane::XY:
                            return NodePosition::BOTTOM_RIGHT_BACK;
                    }
                }
                break;
            case NodePosition::BOTTOM_RIGHT_BACK:
                {
                    exitPlane = minIndex(params.t1.x, params.tm.y, params.t1.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::EXIT;
                        case Plane::XZ:
                            return NodePosition::TOP_RIGHT_BACK;
                        case Plane::XY:
                            return NodePosition::EXIT;
                    }
                }
                break;
            case NodePosition::TOP_RIGHT_FRONT:
                {
                    exitPlane = minIndex(params.t1.x, params.t1.y, params.tm.z);
                    switch (exitPlane) {
                        case Plane::YZ:
                            return NodePosition::EXIT;
                        case Plane::XZ:
                            return NodePosition::EXIT;
                        case Plane::XY:
                            return NodePosition::TOP_RIGHT_BACK;
                    }
                }
                break;
            case NodePosition::TOP_RIGHT_BACK:
            case NodePosition::EXIT:
                return NodePosition::EXIT;
        }
        throw std::logic_error("Invalid node position");
    }

    template<typename T>
    auto Octree<T>::transformRayAndGetNodeMappingFunction(Ray& ray) const noexcept {
        auto dir = ray.direction();
        auto origin = ray.origin();
        int nXORMask{};
        for (glm::vec3::length_type i = 0; i < 3; ++i) {
            if (dir[i] < 0.0f) {
                dir[i] *= -1.0f;
                origin[i] = boundingBox().size(i) - origin[i];
                nXORMask |= (1 << (3-i-1));
            }
        }
        ray.setOrigin(origin);
        ray.setDirection(dir);
        return [nXORMask](NodePosition nNodePosition) {
            return static_cast<NodePosition>(static_cast<int>(nNodePosition) ^ nXORMask);
        };
    }

    template<typename T>
    void Octree<T>::emplace(const AABB& box, const T& value) {
        auto curNode = m_root.get();
        bool bDone{};
        while (!bDone) {
           std::visit(visitor {
                [&box,&value,&curNode](const branch_node& node) { 
                    bool bFound{};
                    for (const auto& pSubNode : node.arNodes) {
                        const auto bIntersect = std::visit(
                            [&box](const auto& n) {
                                return n.box.contains(box);
                            }
                        , *pSubNode);
                        if (bIntersect) {
                            curNode = pSubNode.get();
                            bFound = true;
                        }
                    }
                    if (!bFound)
                        throw std::logic_error("No fitting box found!");
                },
                [&value,&bDone](leaf_node& leaf) {
                    leaf.vObjects.push_back(value); 
                    bDone = true; 
                }
            }, *curNode);
        }
    }
} // namespace Ice


#endif