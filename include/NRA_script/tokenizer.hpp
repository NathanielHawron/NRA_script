#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>

namespace nra_script{
    namespace tokenizer{
        struct TokenizationInfo{
        public:
            static bool readFile(std::filesystem::path filePath, std::string &res);
            
            // Log to vector
            std::vector<std::string> *info = nullptr;
            std::vector<std::string> *warnings = nullptr;
            std::vector<std::string> *errors = nullptr;
            std::vector<std::string> *tokens = nullptr;

            // Log to file
            std::filesystem::path parentPath = std::filesystem::current_path();
            std::filesystem::path *infoPath = nullptr;
            std::filesystem::path *warningsPath = nullptr;
            std::filesystem::path *errorsPath = nullptr;
            std::filesystem::path *tokensPath = nullptr;
            
            // Standard symbols and brackets
            static const std::unordered_map<char, const std::vector<std::string>> SYMBOLS;
            static const std::string LBRACKETS;
            static const std::string RBRACKETS;

            // Extra symbols
            std::unordered_map<char, const std::vector<std::string>> ext_symbols;
        };
        struct token{
        public:
            enum class TYPE : uint8_t {
                // Begins with a letter, ends before non-alphanumeric
                WORD = 0,
                // Starts and ends in "" or ''
                STRING = 1,
                // Starts with number, ends before non-number
                NUMBER = 2,
                // Any non-bracket (except angle), non-space, non-comma, and non-semicolon symbol (or combination of symbols) from the symbol table
                SYMBOL = 3,
                // Any left bracket ({[
                LBRACKET = 4,
                // Any right bracket )}]
                RBRACKET = 5,
                // Semicolon, comma
                BREAK = 6
            };
            static const std::array<std::string, 7> TOKEN_STRINGS;
            TYPE type;
            std::string value;
            std::size_t row;
        };
        bool tokenize(const std::string &file, const TokenizationInfo &tokenizationInfo, std::vector<token> &tokens);
    }
}