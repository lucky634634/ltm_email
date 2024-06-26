#ifndef MAIL_H
#define MAIL_H
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <jsoncpp/json/json.h>
#include <boost/filesystem.hpp>
#include <boost/beast/core/detail/base64.hpp>

namespace fs = boost::filesystem;
namespace detail = boost::beast::detail;

class Mail
{
public:
    Mail();
    ~Mail();
    void Convert(std::string buffer);
    void SaveAttachment(size_t index, std::string path);
    void SaveAttachments(std::string path);

    friend std::ostream& operator<<(std::ostream& os, const Mail& mail);

    static std::string EncodeBase64(const std::vector<char>& data);
    static std::vector<char> DecodeBase64(const std::string& encoded_string);
    void Save(std::string mainPath);
    void Load(std::string path);

private:
    std::string ExtractText(const std::string& mimeMessage);
    std::vector<std::pair<std::string, std::string>> ExtractAttachments(const std::string& mimeMessage);

public:
    bool isSeen;
    std::string id;
    std::string folder;
    std::string user;
    std::string subject;
    std::string content;
    std::vector<std::pair<std::string, std::string>> attachments;
};

#endif