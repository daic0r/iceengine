#ifndef AStar_h
#define AStar_h

#include <vector>
#include <tuple>
#include <array>
#include <System/static_task.h>
#include <optional>

namespace Ice
{
    class AStar {
        using coord_t = std::pair<int,int>;

        bool at(const coord_t& coord) const;
        std::array<std::optional<coord_t>, 8> neighbors(const coord_t& coord) const;

    public:
        AStar(std::size_t nWidth, std::size_t nHeight);
        std::vector<coord_t> findPath(const coord_t& p1, const coord_t& p2, const static_task<float(const coord_t&)>& hCost, const static_task<float(const coord_t&, const coord_t&)>& dist) const;
        auto& grid() noexcept { return m_vGrid; }
        const auto& grid() const noexcept { return m_vGrid; }


    private:
        std::size_t m_nWidth{}, m_nHeight{};
        std::vector<std::vector<bool>> m_vGrid;
    };
} // namespace Ice


#endif