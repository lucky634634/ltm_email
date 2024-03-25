#ifndef MAIL_H
#define MAIL_H
#include <iostream>
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>

class Mail
{
public:
    Mail();
    ~Mail();
    void Save();
    void Load();
    void Convert(std::vector<char> buffer);

public:
    bool isSeen;
    std::string id;
    std::string folder;
    std::string user;
    std::string subject;
    std::vector<char> content;
};

#endif