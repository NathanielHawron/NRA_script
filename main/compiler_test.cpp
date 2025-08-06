#include <iostream>

#include "NRA_script/version.hpp"

const nra_script::Version testVersion{0,0,0,'a'};

int main(){
    std::cout << "compiler test version: " << testVersion.toString() << std::endl;
    std::cout << nra_script::LIB_VERSION.toString() << " (lib) : " << nra_script::HEADER_VERSION.toString() << " (header) / v0.0.0-a (tested)" << std::endl;
}