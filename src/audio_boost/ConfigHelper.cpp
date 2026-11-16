
#include "ConfigHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include "log.h"

std::map<std::string, int> m_configMap;
std::map<std::string, float> m_configFloatMap;

ConfigHelper::ConfigHelper(const std::string& configFile)
    : m_configFile(configFile) {}

bool ConfigHelper::readConfig() {
    LOGDBG("readConfig...");   
    std::ifstream file(m_configFile);
    if (!file.is_open()) {
        LOGDBG("open Config file failure");   
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value; 
 //       LOGDBG("readConfig line:%s",line);       
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            if(key.substr(0, 9) == "f_channel")
            {
                m_configFloatMap[key] = std::stof(value);
            }else{
                m_configMap[key] = std::stoi(value);
            }            
        }
    }

    file.close();
    return true;
}

bool ConfigHelper::writeConfig() const {
    std::ofstream file(m_configFile);
    if (!file.is_open()) {
        std::cerr << "无法打开配置文件进行写入: " << m_configFile << std::endl;
        return false;
    }

    for (const auto& pair : m_configMap) {
        file << pair.first << "=" << pair.second << "\n";
    }

    for (const auto& float_pair : m_configFloatMap) {
        file << float_pair.first << "=" << std::setiosflags(std::ios::fixed) << std::setprecision(1) << float_pair.second << "\n";
    }

    file.close();
    return true;
}

void ConfigHelper::set(const std::string& key, const int& value) {
    m_configMap[key] = value;
}

int ConfigHelper::get(const std::string& key) const {
    auto it = m_configMap.find(key);
    if (it != m_configMap.end()) {
        return it->second;
    }
    return -1; 
}

void ConfigHelper::setFloat(const std::string& key, const float& value) {
    m_configFloatMap[key] = value;
}

float ConfigHelper::getFloat(const std::string& key) const {
    auto it = m_configFloatMap.find(key);
    if (it != m_configFloatMap.end()) {
        return it->second;
    }
    return -1.0f; 
}
