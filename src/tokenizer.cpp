#include "NRA_script/tokenizer.hpp"

#include "NRA_script/logs.hpp"

using namespace nra_script;
using namespace tokenizer;

const std::array<std::string, 7> token::TOKEN_STRINGS = {
    "WORD", "STRING", "NUMBER", "SYMBOL", "LBRACKET", "RBRACKET", "BREAK"
};
const std::unordered_map<char, const std::vector<std::string>> TokenizationInfo::SYMBOLS = {
    // + (add), ++ (pre/post increment), += (add and assign)
    {'+',{"+", "="}},
    // - (sub), -- (pre/post decrement), -= (sub and assign), -> (NYI right arrow)
    {'-',{"-", "=", ">"}},
    // * (mult), ** (exponent), *= (mult and assign), **= (exponent and assign)
    {'*',{"*=", "*", "="}},
    // / (div), /= (div and assign)
    {'/',{"="}},
    // % (mod), %= (mod and assign)
    {'%',{"="}},
    // & (bitwise and, reference), && (boolean and), &= (bitwise and and assign)
    {'&',{"&", "="}},
    // | (bitwise or), || (boolean or, left if truthy else right), |= (bitwise or and assign), |. (conditional truthy access)
    {'|',{"|", "=", "."}},
    // ! (boolean not), != (value non-equality), !== (reference non-equality), !~= (fuzzy non-equality)
    {'!',{"~=","==","="}},
    // ~ (bitwise not), ~= (bitwise not and assign), ~== (fuzzy equality)
    {'~',{"==","="}},
    // ^ (bitwise xor), ^= (bitwise xor and assign)
    {'^',{"="}},
    // = (assign), == (value equality), === (reference equality), =? (assign if RHS value)
    {'=',{"==", "=", "?"}},
    // < (less than), <= (less than or equal), << (bitshift left), <- (NYI left arrow)
    {'<',{"=", "<", "-"}},
    // > (greater than), >= (greater than or equal), >> (bitshift right)
    {'>',{"=", ">"}},
    // ? (NYI ternary part 1), ?? (left it value else right), ?= (assign if LHS empty), ?. (conditional value access)
    {'?',{"?", "=", "."}},
    // : (NYI ternary part 2), (NYI linear range)
    {':',{}},
    // . (access), (NYI range)
    {'.',{}},
    // $ (import/export)
    {'$',{}},
    // @ (metadata, extension functions)
    {'@',{}},
    // NYI preprocessor?
    {'#',{}}
};
const std::string TokenizationInfo::LBRACKETS = "({[";
const std::string TokenizationInfo::RBRACKETS = ")}]";

bool TokenizationInfo::readFile(std::filesystem::path filePath, std::string &res){
    std::ifstream fstream{filePath};
    if(!fstream.is_open()){
        return false;
    }
    std::ostringstream buff;
    buff << fstream.rdbuf();
    res = buff.str();
    return true;
}

bool tokenizer::tokenize(const std::string &file, const TokenizationInfo &tokenizationInfo, std::vector<token> &tokens){
    // Create logs if required and not provided
    Logs infoLogs{tokenizationInfo.info, tokenizationInfo.infoPath != nullptr, tokenizationInfo.infoPath == nullptr ? std::filesystem::path{} : tokenizationInfo.parentPath / (*tokenizationInfo.infoPath)};
    Logs warningLogs{tokenizationInfo.warnings, tokenizationInfo.warningsPath != nullptr, tokenizationInfo.warningsPath == nullptr ? std::filesystem::path{} : tokenizationInfo.parentPath / (*tokenizationInfo.warningsPath)};
    Logs errorLogs{tokenizationInfo.errors, tokenizationInfo.errorsPath != nullptr, tokenizationInfo.errorsPath == nullptr ? std::filesystem::path{} : tokenizationInfo.parentPath / (*tokenizationInfo.errorsPath)};
    Logs tokenLogs{tokenizationInfo.tokens, tokenizationInfo.tokensPath != nullptr, tokenizationInfo.tokensPath == nullptr ? std::filesystem::path{} : tokenizationInfo.parentPath / (*tokenizationInfo.tokensPath)};

    // Tokenize
    std::size_t index = 0;
    std::size_t row = 0;
    std::vector<std::pair<uint32_t, char>> brackets;
    while(index < file.size()){
        char c = file.at(index);
        if(c == '/'){
            // Check for comments or other meanings of /
            if(index+1 == file.size()){
                // / cannot be the last character in the file
                errorLogs.push("Invalid use of \"/\"");
                return false;
            }else if(file.at(index+1) == '/'){
                // // means comment, ignore rest of line
                index += 1;
                while(++index < file.size() && file.at(index) != '\n'){}
                ++row;
            }else if(file.at(index+1) == '*'){
                // /* means multiline comment, ignore until */
                index += 1;
                while(++index+1 < file.size() && file.at(index) != '*' && file.at(index+1) != '/'){
                    if(file.at(index) == 'n'){
                        ++row;
                    }
                }
                ++index;
                if(!(file.at(index-1) == '*' && file.at(index) == '/')){
                    errorLogs.push("Unclosed multiline comment");
                    return false;
                }
                ++index;
            }else if(file.at(index+1) == '='){
                // divided by equals symbol
                tokens.push_back({token::TYPE::SYMBOL,"/=",row});
                index += 2;
            }else{
                // divided by symbol
                tokens.push_back({token::TYPE::SYMBOL,"/",row});
                index += 1;
            }
        }else if(c == '.' || (c >= '0' && c <= '9')){
            // Check if number, or find correct operator (.)
            char c2 = file.at(index+1);
            if(c == '0' && c2 == 'b'){
                // 0b means binary (0 and 1)
                std::string bin{'b'};
                ++index;
                char cn = file.at(++index);
                while(cn == '0' || cn == '1'){
                    bin += {cn};
                    cn = file.at(++index);
                }
                tokens.push_back({token::TYPE::NUMBER,bin,row});
            }else if(c == '0' && c2 == 'x'){
                // 0x means hexadecimal (0-9, A-F), preferring capital
                std::string hex{'x'};
                ++index;
                char cn = file.at(++index);
                while(('0' <= cn && cn <= '9') || ('a' <= cn && cn <= 'f') || ('A' <= cn && cn <= 'F')){
                    hex += {('a' <= cn && cn <= 'f') ? (char)(cn - 'a' + 'A') : cn};
                    cn = file.at(++index);
                }
                tokens.push_back({token::TYPE::NUMBER,hex,row});
            }if(c == '.' && c2 == '.'){
                // Consecutive dots may be implemented in the future
                std::size_t i=2;
                ++index;
                while(file.at(++index) == '.'){
                    ++i;
                }
                warningLogs.push("Multiple consecutive . not yet supported (found " + std::to_string(i) + ")");
            }else if((c2 >= '0' && c2 <= '9') || (c2 == '.' && file.at(index+2) >= '0' && file.at(index+2) <= '9')){
                bool dotFound = c == '.' || c2 == '.';
                // If its a number, start collecting the number
                std::string num = {c};
                num += {c2};
                index += 2;
                char cn = file.at(index);
                while((cn >= '0' && cn <= '9') || (!dotFound && cn == '.')){
                    num += {cn};
                    cn = file.at(++index);
                }
                tokens.push_back({token::TYPE::NUMBER,num,row});
            }else{//if(c >= '0' && c <= '9'){
                // Handle single digit numbers
                tokens.push_back({token::TYPE::NUMBER,{c},row});
                index += 1;
            }
        }else{
            // Check if next character is symbol
            auto symbolTable = tokenizationInfo.SYMBOLS.find(c);
            if(symbolTable == tokenizationInfo.SYMBOLS.end()){
                symbolTable = tokenizationInfo.ext_symbols.find(c);
            }
            if(symbolTable != tokenizationInfo.SYMBOLS.end() && symbolTable != tokenizationInfo.ext_symbols.end()){
                // If symbol found, check if compound symbol
                bool foundMatch = false;
                for(const auto &symbol : symbolTable->second){
                    bool match = true;
                    for(uint32_t i=0;i<symbol.size();++i){
                        if(symbol.at(i) != file.at(index+i+1)){
                            match = false;
                            break;
                        }
                    }
                    if(match){
                        tokens.push_back({token::TYPE::SYMBOL, std::string{c} + symbol, row});
                        index += 1 + symbol.size();
                        foundMatch = true;
                        break;
                    }
                }
                if(!foundMatch){
                    tokens.push_back({token::TYPE::SYMBOL,{c},row});
                    ++index;
                }
            }else if(TokenizationInfo::LBRACKETS.find(c) != std::string::npos){
                // Left bracket
                tokens.push_back({token::TYPE::LBRACKET,{c},row});
                brackets.push_back({row, c});
                ++index;
            }else if(TokenizationInfo::RBRACKETS.find(c) != std::string::npos){
                // Right bracket
                tokens.push_back({token::TYPE::RBRACKET,{c},row});
                if(brackets.empty()){
                    errorLogs.push("Unpaired closing bracket " + std::string{c} + " on line " + std::to_string(row));
                    return false;
                }
                std::pair<uint32_t, char> &prevBracket = brackets.back();
                char pbs = prevBracket.second;
                if((pbs=='('&&c==')') || (pbs=='['&&c==']') || (pbs=='{'&&c=='}')){
                    brackets.pop_back();
                }else{
                    errorLogs.push("Mismatched brackets " + std::string{prevBracket.second} + " on line " + std::to_string(prevBracket.first) + " and " + std::string{c} + " on line " + std::to_string(row));
                    return false;
                }
                ++index;
            }else if(c == '"' || c == '\''){
                // Collect string, and translate special characters
                std::string str;
                bool escape = false;
                char cn = file.at(++index);
                while(cn != c || escape){
                    if(escape){
                        switch(cn){
                            case '\\': str += "\\";break;
                            case 't': str += "\t";break;
                            case 'n': str += "\n";break;
                            case '0': str += "\0";break;
                            case '"': str += "\"";break;
                            case '\'': str += "'";break;
                            case '?': str += "\?";break;
                            case 'a': str += "\a";break;
                            case 'b': str += "\b";break;
                            case 'f': str += "\f";break;
                            case 'r': str += "\r";break;
                            case 'v': str += "\v";break;
                            default:{
                                errorLogs.push("Unknown escaped character: "+std::string{cn});
                                return false;
                            }
                        }
                        escape = false;
                    }else{
                        if(cn == '\\'){
                            escape = true;
                        }else{
                            str += {cn};
                        }
                    }
                    cn = file.at(++index);
                }
                tokens.push_back({token::TYPE::STRING,str,row});
                ++index;
            }else if(c == ';' || c == ','){
                // break
                ++index;
                tokens.push_back({token::TYPE::BREAK,{c},row});
            }else if(c == ' ' || c == '\t' || c == '\n'){
                // Whitespace can be ignored
                char cn = file.at(index);
                while(cn == ' ' || cn == '\t' || cn == '\n'){
                    if(cn == '\n'){
                        ++row;
                    }
                    cn = file.at(++index);
                }
            }else{
                // word
                std::string word{c};
                char cn = file.at(++index);
                while(('a' <= cn && cn <= 'z') || ('A' <= cn && cn <= 'Z') || ('0' <= cn && cn <= '9') || cn == '_'){
                    word += {cn};
                    cn = file.at(++index);
                }
                tokens.push_back({token::TYPE::WORD,word,row});
            }
        }
    }
    // Check unclosed brackets
    if(!brackets.empty()){
        std::pair<uint32_t, char> &lastBracket = brackets.back();
        errorLogs.push("Unpaired opening brackets (" + std::to_string(brackets.size()) + "), last one is " + std::string{lastBracket.second} + " on line " + std::to_string(lastBracket.first));
        return false;
    }
    // Log all tokens if possible
    if(tokenLogs.logs != nullptr){
        for(const auto t : tokens){
            std::string tStr = token::TOKEN_STRINGS[(std::size_t)t.type] + " (" + std::to_string(t.row) + "): " + t.value;
            tokenLogs.logs->push_back(tStr);
        }
    }
    return true;
}