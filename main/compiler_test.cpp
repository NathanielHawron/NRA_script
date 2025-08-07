#include <iostream>

#include "NRA_script/version.hpp"
#include "NRA_script/tokenizer.hpp"
#include "NRA_script/parser.hpp"

const nra_script::Version testVersion{0,0,0,'a'};

void testTokenizer(std::filesystem::path filePath, std::string fileName){
    std::string file;

    std::vector<std::string> infoLogs;
    std::vector<std::string> warningsLogs;
    std::vector<std::string> errorsLogs;
    std::vector<std::string> tokensLogs;

    std::filesystem::path infoPath = "info_"+fileName+".log";
    std::filesystem::path warningPath = "warning_"+fileName+".log";
    std::filesystem::path errorPath = "error_"+fileName+".log";
    std::filesystem::path tokenPath = "token_"+fileName+".log";

    nra_script::tokenizer::TokenizationInfo tokenizationInfo;
    nra_script::tokenizer::TokenizationInfo::readFile(filePath, file);
    tokenizationInfo.info = &infoLogs;
    tokenizationInfo.warnings = &warningsLogs;
    tokenizationInfo.errors = &errorsLogs;
    tokenizationInfo.tokens = &tokensLogs;
    tokenizationInfo.parentPath = std::filesystem::current_path() / "saves" / "compiler_test" / "logs";
    tokenizationInfo.infoPath = &infoPath;
    tokenizationInfo.warningsPath = &warningPath;
    tokenizationInfo.errorsPath = &errorPath;
    tokenizationInfo.tokensPath = &tokenPath;

    std::vector<nra_script::tokenizer::token> tokens;

    std::cout << (nra_script::tokenizer::tokenize(file, tokenizationInfo, tokens) ? "Compilation Successful" : "Compilation Failed") << std::endl;
    std::cout << "Info: " << infoLogs.size() << std::endl;
    for(auto i : infoLogs){
        std::cout << i << std::endl;
    }
    std::cout << "Warnings:" << warningsLogs.size() << std::endl;
    for(auto w : warningsLogs){
        std::cout << w << std::endl;
    }
    std::cout << "Errors:" << errorsLogs.size() << std::endl;
    for(auto e : errorsLogs){
        std::cout << e << std::endl;
    }
}

int main(){
    std::cout << "compiler test version: " << testVersion.toString() << std::endl;
    std::cout << nra_script::LIB_VERSION.toString() << " (lib) : " << nra_script::HEADER_VERSION.toString() << " (header) / v0.0.1-a (tested)" << std::endl;

    testTokenizer(std::filesystem::current_path() / "saves" / "compiler_test" / "compiler_test.nra", "compilerTest");
}