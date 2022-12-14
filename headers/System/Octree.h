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
#include <System/Types.h>
#include <System/static_task.h>

namespace Ice
{

#ifdef _DEBUG_OCTREE
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
#endif

    template<typename T>
    class Octree 
#ifdef _DEBUG_OCTREE
        : public IOctreeTraversal
#endif
    {
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

       struct sIntersectParams {
            glm::vec3 t0; // entry point along ray in each dimension
            glm::vec3 t1; // exit point along ray in each dimension
            glm::vec3 tm; // center point along ray in each dimension

        };
        struct leaf_node;
        struct branch_node;

        using node_t = std::variant<branch_node, leaf_node>;
        using onhitleafnodefunc_t = static_task<SubdivisionIntersectionBehavior(const std::vector<T>&)>;

        struct base_node {
#ifdef _DEBUG_OCTREE
            static inline std::size_t NEXT_ID = 1;
            std::size_t nID = NEXT_ID++;
#endif
            AABB box;
        };
        struct leaf_node : base_node {
            std::vector<T> vObjects;
        };
        struct branch_node : base_node {
            std::array<std::unique_ptr<node_t>, 8> arNodes;
        };

        void construct(const std::vector<std::pair<AABB, T>>&, std::unique_ptr<node_t>& node, const AABB& originalBox, int nLevel = 0);
        
        template<typename NodePositionMappingFunc>
        SubdivisionIntersectionBehavior intersects_impl(const Ray&, const node_t* node, const sIntersectParams& params, NodePositionMappingFunc&& mappingFunc, const onhitleafnodefunc_t& onHitLeafNode) const;

        node_t* find(const glm::vec3& point, node_t* pStart = nullptr) const noexcept;

        static NodePosition getFirstSubNodeIndex(const sIntersectParams& params) noexcept;
        static NodePosition getNextSubNode(NodePosition current, const sIntersectParams& params);
        auto transformRayAndGetNodeMappingFunction(Ray&) const noexcept;
        branch_node& root() noexcept { return std::get<branch_node>(*m_root); };
        const branch_node& root() const noexcept { return std::get<branch_node>(*m_root); };
        const AABB& boundingBox() const noexcept { return root().box; }
    public:
        Octree() = default;
        Octree(const std::vector<std::pair<AABB, T>>& vBoxes, const AABB& outerBox, std::size_t nMaxDepth = 6);
        bool intersects(Ray, const onhitleafnodefunc_t& onHitLeafNode) const;
        void emplace(const AABB& box,const T& val);


        /////////////////////////////////////////////////////////
        // Traversal
#ifdef _DEBUG_OCTREE
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
#endif
        /////////////////////////////////////////////////////////

    private:

        std::unique_ptr<node_t> m_root;
        std::size_t m_nMaxDepth{6};

#ifdef _DEBUG_OCTREE
        std::stack<node_t*> m_vTraverseNodes;
        mutable std::vector<std::size_t> m_vIntersectLeaves;
#endif   
    };
    
    template<typename T>
    Octree<T>::Octree(const std::vector<std::pair<AABB, T>>& vBoxes, const AABB& outerBox, std::size_t nMaxDepth) : m_nMaxDepth{nMaxDepth} {
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
        if (nLevel <= m_nMaxDepth && vPoints.size() > 2) {
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
    SubdivisionIntersectionBehavior Octree<T>::intersects_impl(const Ray& ray, const node_t* node, const sIntersectParams& params, NodePositionMappingFunc&& mappingFunc, const onhitleafnodefunc_t& onHitLeafNode) const {
        if (params.t1.x < 0.0f || params.t1.y < 0.0f || params.t1.z < 0.0f)
            return SubdivisionIntersectionBehavior::CONTINUE;
        const auto intersectBehavior = std::visit(visitor {
            [&,this,mappingFunc=std::forward<NodePositionMappingFunc>(mappingFunc)](const branch_node& curNode) { 
#ifdef _DEBUG_OCTREE
                this->m_vIntersectLeaves.push_back(curNode.nID);
#endif
                SubdivisionIntersectionBehavior ret;
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
                    subNodeParams.tm = (subNodeParams.t0 + subNodeParams.t1) * 0.5f;
                    // Process
                    const auto nTranslatedIndex = mappingFunc(nSubNode);
                    const auto pSubNode = curNode.arNodes[nTranslatedIndex].get(); 
                    ret = intersects_impl(ray, pSubNode, subNodeParams, mappingFunc, onHitLeafNode);
                    if (static_cast<int>(ret) & 2) // ABORT bit 2 set
                        return ret;
                    // Determine next node
                    nSubNode = getNextSubNode(nSubNode, params);
                }
                return ret;
            },
            [&onHitLeafNode,this](const leaf_node& leaf) {
#ifdef _DEBUG_OCTREE
                this->m_vIntersectLeaves.push_back(leaf.nID);
#endif
                if (!leaf.vObjects.empty())
                    return onHitLeafNode(leaf.vObjects);
                return SubdivisionIntersectionBehavior::CONTINUE;
            }
        }, *node);
        return intersectBehavior;
    }

    template<typename T>
    bool Octree<T>::intersects(Ray ray, const onhitleafnodefunc_t& onHitLeafNode) const {
#ifdef _DEBUG_OCTREE
        m_vIntersectLeaves.clear();
#endif
        auto mapNodeFunc = transformRayAndGetNodeMappingFunction(ray);
        sIntersectParams params;
        params.t0 = (boundingBox().minVertex() - ray.origin()) / ray.direction();
        params.t1 = (boundingBox().maxVertex() - ray.origin()) / ray.direction();
        params.tm = (params.t0 + params.t1) * 0.5f;
        if (std::max(params.t0[0], std::max(params.t0[1], params.t0[2])) >= std::min(params.t1[0], std::min(params.t1[1], params.t1[2])))
            return false;
        return intersects_impl(ray, m_root.get(), params, mapNodeFunc, onHitLeafNode) == SubdivisionIntersectionBehavior::ABORT_SUCCESS;
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