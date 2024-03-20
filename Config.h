#pragma once
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
private:
    std::string name;
    std::string email;
    std::string password;
    std::string mailServer;
    int portSMTP;
    int portPOP3;
    int autoload;
};