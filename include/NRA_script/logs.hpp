#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

namespace nra_script{
    class Logs{
    public:
        std::vector<std::string> *logs;
        const bool logsOwned;
        const std::filesystem::path saveOnExit;
        Logs(std::vector<std::string> *logs, bool force = true, std::filesystem::path saveOnExit = std::filesystem::path()):logs{logs},logsOwned{logs == nullptr && force},saveOnExit{saveOnExit}{
            if(this->logsOwned){
                this->logs = new std::vector<std::string>();
            }
        }
        ~Logs(){
            if((!this->saveOnExit.empty()) && this->saveOnExit.has_filename()){
                this->saveToFile(this->saveOnExit);
            }
            if(this->logsOwned){
                delete this->logs;
            }
        }
        void push(std::string log){
            if(this->logs != nullptr){
                this->logs->push_back(log);
            }
        }
        bool saveToFile(std::filesystem::path path){
            if(this->logs == nullptr){
                return false;
            }
            std::ofstream fStream{path};
            if(!fStream.is_open()){
                return false;
            }
            for(const std::string s : *this->logs){
                fStream << s << '\n';
            }
            return true;
        }
    };
}