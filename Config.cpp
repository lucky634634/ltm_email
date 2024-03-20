#include <Config.h>

Config* Config::instance = NULL;

void Config::Load()
{
    std::ifstream config("config.json");
    if (!config.is_open())
    {
        Save();
        return;
    }
    Json::Value root;
    config >> root;
    name = root["Username"].asString();
    email = root["Email"].asString();
    password = root["Password"].asString();
    mailServer = root["MailServer"].asString();
    portSMTP = root["Port"]["SMTP"].asInt();
    portPOP3 = root["Port"]["POP3"].asInt();
    autoload = root["Autoload"].asInt();
    std::cout << name << " " << email << " " << password << " " << mailServer << " " << portSMTP << " " << portPOP3 << " " << autoload << std::endl;
    config.close();
}

void Config::Save()
{
    std::ofstream config("config.json");
    if (!config.is_open())
    {
        return;
    }

    Json::Value root;
    root["General"]["Name"] = name;
    root["General"]["Email"] = email;
    root["General"]["Password"] = password;
    root["General"]["MailServer"] = mailServer;
    root["General"]["Port"]["SMTP"] = portSMTP;
    root["General"]["Port"]["POP3"] = portPOP3;
    root["General"]["Autoload"] = autoload;
    config << root;
    config.close();
}

Config* Config::GetInstance()
{
    if (instance == NULL)
    {
        instance = new Config();
    }
    return instance;
}

Config::Config()
{
    name = "";
    email = "";
    password = "";
    mailServer = "";
    portSMTP = 0;
    portPOP3 = 0;
    autoload = 0;
    Load();
}
