#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>

class Config
{
public:
    void Load();
    void Save();
    static Config* GetInstance();
private:
    Config();
    static Config* instance;
public:
    // General
    std::string name;
    std::string email;
    std::string password;
    std::string mailServer;
    int portSMTP;
    int portPOP3;
    int autoload;

    // Filter
    std::map <std::string, std::vector<std::string>> userFilter;
    std::map <std::string, std::vector<std::string>> subbjectFilter;
    std::map <std::string, std::vector<std::string>> contentFilter;
    std::map <std::string, std::vector<std::string>> spamFilter;
};

#endif