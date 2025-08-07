#include "NRA_script/version.hpp"


constexpr nra_script::Version nra_script::LIB_VERSION{0,0,1,'a'};

std::string nra_script::Version::toString() const{
    return "v" + std::to_string(this->MAJOR) + "." + std::to_string(this->MINOR) + "." + std::to_string(this->PATCH) + "-" + this->TAG;
}
bool nra_script::Version::operator==(const nra_script::Version &v) const {
    return this->MAJOR == v.MAJOR && this->MINOR == v.MINOR && this->PATCH == v.PATCH && this->TAG == v.TAG;
}