#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>

namespace nra_script{
    namespace parser{
        struct ParserInfo{
        public:
            // Log to vector
            std::vector<std::string> *info = nullptr;
            std::vector<std::string> *warnings = nullptr;
            std::vector<std::string> *errors = nullptr;
            std::vector<std::string> *commands = nullptr;

            // Log to file
            std::filesystem::path parentPath = std::filesystem::current_path();
            std::filesystem::path *infoPath = nullptr;
            std::filesystem::path *warningsPath = nullptr;
            std::filesystem::path *errorsPath = nullptr;
            std::filesystem::path *commandsPath = nullptr;

            // Standard types and keywords
            const static std::vector<std::string> TYPES;
            const static std::vector<std::string> KEYWORDS;

            // Extra types and keywords
            std::vector<std::string> ext_types;
            std::vector<std::string> ext_keywords;
        };
    }
}