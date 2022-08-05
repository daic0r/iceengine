#ifndef MouseWheelEventArgs_h
#define MouseWheelEventArgs_h

#include <Events/Event.h>

namespace Ice
{

class MouseWheelEventArgs {
    int m_nX, m_nY;
    //float m_fX, float m_fY;

public:
    MouseWheelEventArgs(int x, int y)
        : m_nX{ x }, m_nY{ y } {}

    auto x() const noexcept { return m_nX; }
    auto y() const noexcept { return m_nY; }
};

} // namespace Ice


#endif