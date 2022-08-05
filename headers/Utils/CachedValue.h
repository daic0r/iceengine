//
//  CachedValue.h
//  IceCraft
//
//  Created by Matthias Gruen on 20.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef CachedValue_h
#define CachedValue_h

#include <functional>

namespace Ice {

template<typename ValueType, typename DirtyFunc = std::function<bool()>>
class CachedValue {
public:
    using type = ValueType;
    
private:
    mutable ValueType m_cachedValue;
    mutable bool m_bInitialized{ false };
    std::function<ValueType()> m_valueGetter;
    DirtyFunc m_isDirtyFunc;
    
public:
    CachedValue() = default;
    CachedValue(const CachedValue& other) = default;
    CachedValue& operator=(const CachedValue& other) = default;
    CachedValue(const std::function<ValueType()>& func, const DirtyFunc& isDirtyFunc) : m_valueGetter{ func }, m_isDirtyFunc{ isDirtyFunc } {}
    CachedValue(const ValueType& val, const std::function<ValueType()>& func, const DirtyFunc& isDirtyFunc) : CachedValue{ func, isDirtyFunc }, m_cachedValue{ val }, m_bInitialized{ true } {}
    CachedValue(ValueType&& val, const std::function<ValueType()>& func, const std::function<bool()>& isDirtyFunc) : CachedValue{ func, isDirtyFunc }, m_cachedValue{ std::forward<ValueType>(val) }, m_bInitialized{ true } {}

    void setValueGetter(const std::function<ValueType()>& func) noexcept { m_valueGetter = func; }
    void setDirtyGetter(const DirtyFunc& isDirtyFunc) noexcept { m_isDirtyFunc = isDirtyFunc; }
    
    CachedValue& operator=(const ValueType& other) {
        m_cachedValue = other;
    }
    
    CachedValue& operator=(ValueType&& other) {
        m_cachedValue = std::forward<ValueType>(other);
    }
    
    const ValueType& cachedValue() const noexcept { return m_cachedValue; }
    const ValueType& refreshValue() const {
        m_cachedValue = m_valueGetter();
        m_bInitialized = true;
        return m_cachedValue;
    }
    const ValueType& value() const {
        if (m_isDirtyFunc() || !m_bInitialized) {
            return refreshValue();
        }
        return cachedValue();
    }
    
    operator const ValueType&() const {
        return value();
    }
};

}


#endif /* CachedValue_h */
