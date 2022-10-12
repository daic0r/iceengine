//
//  StreamTools.h
//  IceCraft
//
//  Created by Matthias Gruen on 08.11.20.
//  Copyright � 2020 Matthias Gruen. All rights reserved.
//

#ifndef StreamTools_h
#define StreamTools_h

#include <IceEngine.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <functional>
#include <iterator>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <System/Types.h>

//std::ostream& operator<<(std::ostream&, const glm::vec2&);
//std::istream& operator>>(std::istream&, glm::vec2&);
//
//std::ostream& operator<<(std::ostream&, const glm::vec3&);
//std::istream& operator>>(std::istream&, glm::vec3&);

namespace Ice {

static constexpr char COMPONENT_FIELD_SEPARATOR = '|';

class StreamTools {
public:
    template<typename Key, typename Value> static void readMapFromStream(std::istream& is, std::map<Key, Value>& m, const std::function<void(std::stringstream&, Value&)>& readMapValueFunc) {
        m.clear();

        std::string strField;
        std::getline(is, strField, COMPONENT_FIELD_SEPARATOR);
        size_t pos;
        while ((pos = strField.find(";")) != std::string::npos) {
            std::string strData;
            Key k;
            strData = strField.substr(0, pos);
            std::stringstream ss{ strData };
            if constexpr(std::is_same<Key, std::string>::value)
                readStringFromStream(ss, k);
            else
                ss >> k;
            readMapValueFunc(ss, m[k]);

            strField = strField.substr(pos + 1);
        }
    }

    template<typename Key, typename Value> static void readMapFromStream(std::istream& is, std::unordered_map<Key, Value>& m, const std::function<void(std::stringstream&, Value&)>& readMapValueFunc) {
        m.clear();

        std::string strField;
        std::getline(is, strField, COMPONENT_FIELD_SEPARATOR);
        size_t pos;
        while ((pos = strField.find(";")) != std::string::npos) {
            std::string strData;
            Key k;
            strData = strField.substr(0, pos);
            std::stringstream ss{ strData };
            if constexpr(std::is_same<Key, std::string>::value)
                readStringFromStream(ss, k);
            else
                ss >> k;
            readMapValueFunc(ss, m[k]);

            strField = strField.substr(pos + 1);
        }
    }
    
    static void writeStringToStream(std::ostream&, const std::string&);
    static bool readStringFromStream(std::istream&, std::string&);

};

// In order to read back the vector using >> we must manually outout COMPONENT_FIELD_SEPARATOR to the stream
// This not done here in case this is used in conjunction with other data structures
template<typename T> static std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    for (auto iter = v.cbegin(); iter != v.cend(); ++iter) {
		if constexpr (std::is_same_v<T, std::string>) {
			StreamTools::writeStringToStream(os, *iter);
		}
		else {
			os << *iter;
		}
        if (iter != v.end() - 1)
            os << " ";
    }
    //} else {
    //    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>{ os, " " });
    //}
    return os;
}

template<typename K, typename V> static std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
    for (const auto& [key, vec] : m) {
        if constexpr(std::is_same<K, std::string>::value)
            StreamTools::writeStringToStream(os, key);
        else
            os << key;
        os << " ";
        os << vec;
        os << ";";
    }
    return os;
}

template<typename K, typename V> static std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& m) {
    for (const auto& [key, vec] : m) {
        if constexpr(std::is_same<K, std::string>::value)
            StreamTools::writeStringToStream(os, key);
        else
            os << key;
        os << " ";
        os << vec;
        os << ";";
    }
    return os;
}

template<typename T> static std::istream& operator>>(std::istream& is, std::vector<T>& v) {
    v.clear();

    std::string strField;
    std::getline(is, strField, Ice::COMPONENT_FIELD_SEPARATOR);
    std::stringstream ss{ strField };
    
    if constexpr(std::is_same_v<T, std::string>) {
        std::string strIn;
        while (StreamTools::readStringFromStream(ss, strIn))
            v.push_back(strIn);
    } else {
        std::copy(std::istream_iterator<T>{ ss }, std::istream_iterator<T>{}, std::back_inserter(v));
    }
    return is;
} 

template<typename Key, typename Value> static std::istream& operator>>(std::istream& is, std::map<Key, Value>& m) {
    StreamTools::readMapFromStream<Key, Value>(is, m, [](auto& ss, Value& val) {
        ss >> val;
    });
    return is;
}

template<typename Key, typename Value> static std::istream& operator>>(std::istream& is, std::unordered_map<Key, Value>& m) {
    StreamTools::readMapFromStream<Key, Value>(is, m, [](auto& ss, Value& val) {
        ss >> val;
    });
    return is;
}

template<typename Key, typename Value> static std::istream& operator>>(std::istream& is, std::map<Key, std::vector<Value>>& m) {
    StreamTools::readMapFromStream<Key, std::vector<Value>>(is, m, [](auto& ss, std::vector<Value>& val) {
        std::copy(std::istream_iterator<Value>{ ss }, std::istream_iterator<Value>{}, std::back_inserter(val));
    });
    return is;
}

std::ostream& operator<<(std::ostream&, OrthogonalDirection);
std::istream& operator>>(std::istream&, OrthogonalDirection&);
std::ostream& operator<<(std::ostream&, EdgeOrientation);
std::istream& operator>>(std::istream&, EdgeOrientation&);

}

namespace glm {
    std::ostream& operator<<(std::ostream&, const glm::vec2&);
    std::istream& operator>>(std::istream&, glm::vec2&);

    std::ostream& operator<<(std::ostream&, const glm::vec3&);
    std::istream& operator>>(std::istream&, glm::vec3&);
}

#endif /* StreamTools_h */
