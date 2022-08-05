#ifndef KeyDownEventArgs_h
#define KeyDownEventArgs_h

#include <Events/Event.h>

namespace Ice
{

class KeyDownEventArgs {
    char m_ch;

public:
    KeyDownEventArgs(char ch) : m_ch{ ch } {}

    auto key() const noexcept { return m_ch; }
};
    
} // namespace Ice



#endif