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
    name = root["General"]["Name"].asString();
    email = root["General"]["Email"].asString();
    password = root["General"]["Password"].asString();
    mailServer = root["General"]["MailServer"].asString();
    portSMTP = root["General"]["Port"]["SMTP"].asInt();
    portPOP3 = root["General"]["Port"]["POP3"].asInt();
    autoload = root["General"]["Autoload"].asInt();

    for (unsigned int i = 0; i < root["Filter"]["User"].size(); i++)
    {
        std::string user = root["Filter"]["User"][i]["To"].asString();
        std::vector<std::string> from;
        for (unsigned int j = 0; j < root["Filter"]["User"][i]["From"].size(); j++)
        {
            from.push_back(root["Filter"]["User"][i]["From"][j].asString());
        }
        userFilter[user] = from;
    }

    for (unsigned int i = 0; i < root["Filter"]["Subbject"].size(); i++)
    {
        std::string user = root["Filter"]["Subbject"][i]["To"].asString();
        std::vector<std::string> from;
        for (unsigned int j = 0; j < root["Filter"]["Subbject"][i]["From"].size(); j++)
        {
            from.push_back(root["Filter"]["Subbject"][i]["From"][j].asString());
        }
        subbjectFilter[user] = from;
    }

    for (unsigned int i = 0; i < root["Filter"]["Content"].size(); i++)
    {
        std::string user = root["Filter"]["Content"][i]["To"].asString();
        std::vector<std::string> from;
        for (unsigned int j = 0; j < root["Filter"]["Content"][i]["From"].size(); j++)
        {
            from.push_back(root["Filter"]["Content"][i]["From"][j].asString());
        }
        contentFilter[user] = from;
    }

    for (unsigned int i = 0; i < root["Filter"]["Spam"].size(); i++)
    {
        std::string user = root["Filter"]["Spam"][i]["To"].asString();
        std::vector<std::string> from;
        for (unsigned int j = 0; j < root["Filter"]["Spam"][i]["From"].size(); j++)
        {
            from.push_back(root["Filter"]["Spam"][i]["From"][j].asString());
        }
        spamFilter[user] = from;
    }

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

    if (userFilter.size() == 0)
    {
        root["Filter"]["User"] = Json::Value(Json::arrayValue);
    }
    for (std::map<std::string, std::vector<std::string>>::iterator it = userFilter.begin(); it != userFilter.end(); it++)
    {
        Json::Value user;
        user["To"].append(it->first);
        for (unsigned int i = 0; i < it->second.size(); i++)
        {
            user["From"].append(it->second[i]);
        }
        root["Filter"]["User"].append(user);
    }

    if (subbjectFilter.size() == 0)
    {
        root["Filter"]["Subbject"] = Json::Value(Json::arrayValue);
    }
    for (std::map<std::string, std::vector<std::string>>::iterator it = subbjectFilter.begin(); it != subbjectFilter.end(); it++)
    {
        Json::Value user;
        user["To"].append(it->first);
        for (unsigned int i = 0; i < it->second.size(); i++)
        {
            user["From"].append(it->second[i]);
        }
        root["Filter"]["Subbject"].append(user);
    }

    if (contentFilter.size() == 0)
    {
        root["Filter"]["Content"] = Json::Value(Json::arrayValue);
    }
    for (std::map<std::string, std::vector<std::string>>::iterator it = contentFilter.begin(); it != contentFilter.end(); it++)
    {
        Json::Value user;
        user["To"].append(it->first);
        for (unsigned int i = 0; i < it->second.size(); i++)
        {
            user["From"].append(it->second[i]);
        }
        root["Filter"]["Content"].append(user);
    }

    if (spamFilter.size() == 0)
    {
        root["Filter"]["Spam"] = Json::Value(Json::arrayValue);
    }
    for (std::map<std::string, std::vector<std::string>>::iterator it = spamFilter.begin(); it != spamFilter.end(); it++)
    {
        Json::Value user;
        user["To"].append(it->first);
        for (unsigned int i = 0; i < it->second.size(); i++)
        {
            user["From"].append(it->second[i]);
        }
        root["Filter"]["Spam"].append(user);
    }

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
