
#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <string>
#include <map>

class ConfigHelper {
public:    
    ConfigHelper(const std::string& configFile);
    static ConfigHelper *getInstance()
    {
        static ConfigHelper *pConfigHelper = new ConfigHelper("/media/flash/userdata/audio_boost.conf");
        return pConfigHelper;
    }   
    bool readConfig();    
    bool writeConfig() const;    
    void set(const std::string& key, const int& value);
    int get(const std::string& key) const;
    void setFloat(const std::string& key, const float& value);
    float getFloat(const std::string& key) const;
private:
    std::string m_configFile;    
};

#endif 
