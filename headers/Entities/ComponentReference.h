#ifndef ComponentReference_h
#define ComponentReference_h

namespace Ice
{

    template<typename Component>
    struct ComponentReference {
        Entity ent;
        Component* pComp{};
    };

} // namespace Ice
#endif