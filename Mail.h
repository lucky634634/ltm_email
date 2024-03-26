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
    void Convert(std::string buffer);

    friend std::ostream& operator<<(std::ostream& os, const Mail& mail);

public:
    bool isSeen;
    std::string id;
    std::string folder;
    std::string user;
    std::string subject;
    std::string content;
    std::map<std::string, std::string> attachments;
};

#endif