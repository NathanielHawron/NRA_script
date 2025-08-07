#pragma once

#include <string>

namespace nra_script{
    struct Version{
        const int MAJOR, MINOR, PATCH;
        const char TAG;
        std::string toString() const;
        inline operator std::string() const {return this->toString();};
        bool operator==(const Version &v) const;
        inline bool operator!=(const Version &v) const {return !((*this) == v);};
    };
    extern const Version LIB_VERSION;
    constexpr Version HEADER_VERSION{0,0,1,'a'};
}