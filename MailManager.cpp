#include "MailManager.h"

MailManager* MailManager::instance = nullptr;

MailManager* MailManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new MailManager();
    }
    return instance;
}

void MailManager::CreateSaveDirection()
{
    std::string mainPath = config->email;
    if (!DirectoryExists(mainPath))
    {
        fs::create_directory(mainPath);
    }

    for (unsigned int i = 0; i < config->userFilter.size(); i++)
    {
        if (!DirectoryExists(mainPath + "/" + config->userFilter.begin()->first))
        {
            fs::create_directory(mainPath + "/" + config->userFilter.begin()->first);
        }
    }

    for (unsigned int i = 0; i < config->subbjectFilter.size(); i++)
    {
        if (!DirectoryExists(mainPath + "/" + config->subbjectFilter.begin()->first))
        {
            fs::create_directory(mainPath + "/" + config->subbjectFilter.begin()->first);
        }
    }

    for (unsigned int i = 0; i < config->contentFilter.size(); i++)
    {
        if (!DirectoryExists(mainPath + "/" + config->contentFilter.begin()->first))
        {
            fs::create_directory(mainPath + "/" + config->contentFilter.begin()->first);
        }
    }

    for (unsigned int i = 0; i < config->spamFilter.size(); i++)
    {
        if (!DirectoryExists(mainPath + "/" + config->spamFilter.begin()->first))
        {
            fs::create_directory(mainPath + "/" + config->spamFilter.begin()->first);
        }
    }

}

MailManager::MailManager()
{
    config = Config::GetInstance();
    CreateSaveDirection();
}

MailManager::~MailManager()
{}

bool MailManager::DirectoryExists(const fs::path& path)
{
    return fs::exists(path) && fs::is_directory(path);
}
