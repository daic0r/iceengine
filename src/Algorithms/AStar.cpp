#include <Algorithms/AStar.h>
#include <queue>
#include <list>
#include <array>
#include <map>
#include <set>
#include <algorithm>
#include <ranges>

namespace Ice
{
    AStar::AStar(std::size_t nWidth, std::size_t nHeight)
        : m_nWidth{ nWidth }, m_nHeight{ nHeight }
    {
        m_vGrid.resize(nHeight);
        for (auto& vRow : m_vGrid) {
            vRow.resize(nWidth);
        }
    }

    std::vector<AStar::coord_t> AStar::findPath(const coord_t& p1, const coord_t& p2, const static_task<float(const coord_t&)>& hCost, const static_task<float(const coord_t&, const coord_t&)>& dist) const {
        struct sPathNode {
            float gCost{}; // cost from start to here
            float fCost{};
            coord_t predecessor;
            coord_t coord;

            bool operator<(const sPathNode& rhs) const {
                return fCost < rhs.fCost;
            }
        };

        std::deque<sPathNode> openSet{};
        std::vector<sPathNode> vNodes;
        std::vector<coord_t> vRet;

        vNodes.emplace_back(0.0f, hCost(p1), coord_t{-1,-1}, p1);
        openSet.push_back(vNodes.back());

        while (!openSet.empty()) {
            auto current = openSet.front();
            if (current.coord == p2) {
                vRet.push_back(current.coord);
                auto pCur = &current;
                while (pCur->predecessor != p1) {
                    const auto iter = std::find_if(vNodes.begin(), vNodes.end(), [pCur](const sPathNode& node) { return node.coord == pCur->predecessor; });
                    vRet.push_back(iter->coord);
                    pCur = &(*iter);
                }
                break;
            }
            openSet.pop_front();

            const auto nbs = neighbors(current.coord);
            for (const auto& n : nbs) {
                if (!n.has_value())
                    continue;
                const auto tentativeG = current.gCost + dist(current.coord, n.value());
                auto iter = std::find_if(vNodes.begin(), vNodes.end(), [&n](const sPathNode& node) { return node.coord == n.value(); });
;
                bool bBetterPath{};
                if (iter == vNodes.end()) {
                    const auto fCost = tentativeG + hCost(n.value());
                    iter = vNodes.insert(vNodes.end(), sPathNode{ tentativeG, fCost, current.coord, n.value() });
                    bBetterPath = true;
                } 
                else 
                if (tentativeG < iter->gCost) {
                    const auto fCost = tentativeG + hCost(n.value());
                    iter->gCost = tentativeG;
                    iter->fCost = fCost;
                    iter->predecessor = current.coord;
                    bBetterPath = true;
                }
                if (bBetterPath) {
                    if (std::none_of(openSet.cbegin(), openSet.cend(), [iter](const sPathNode& node) { return node.coord == iter->coord; }))
                        openSet.insert(std::upper_bound(openSet.begin(), openSet.end(), *iter),  *iter);
                }

            }
        }

        return vRet;
    }

    bool AStar::at(const coord_t& coord) const {
        return m_vGrid.at(coord.second).at(coord.first);
    }

    std::array<std::optional<AStar::coord_t>, 8> AStar::neighbors(const coord_t& coord) const {
        std::array<std::optional<coord_t>, 8> arRet;

        std::size_t nIdx{};
        const auto x = coord.first;
        const auto y = coord.second;
        for (std::size_t this_y = (y > 0 ? (y - 1) : 0); this_y <= (y < (m_nHeight - 1) ? (y + 1) : y); ++this_y) {
            for (std::size_t this_x = (x > 0 ? (x - 1) : 0); this_x <= (x < (m_nWidth - 1) ? (x + 1) : x); ++this_x) {
                if (this_x == x && this_y == y)
                    continue;
                arRet[nIdx++] = std::make_pair(this_x, this_y);
            }
        }

        return arRet;
    }
} // namespace Ice
