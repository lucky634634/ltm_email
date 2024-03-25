#ifndef MAILMANAGER_H
#define MAILMANAGER_H

#include <iostream>
#include <vector>
#include <Mail.h>
#include <boost/filesystem.hpp>
#include "Config.h"

namespace fs = boost::filesystem;

class MailManager
{
public:
    static MailManager* GetInstance();
    void CreateSaveDirection();

private:
    MailManager();
    ~MailManager();
    static bool DirectoryExists(const fs::path& path);

private:
    static MailManager* instance;
    Config* config;
    std::vector<Mail> mails;
};

#endif