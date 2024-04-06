#include "Mail.h"



Mail::Mail()
{
    isSeen = false;
    folder = "Inbox";
    user = "";
    subject = "";
}

Mail::~Mail()
{}

void Mail::Convert(std::string buffer)
{
    // Get user and subject
    user = buffer.substr(buffer.find("From: ") + 6);
    user = user.substr(0, user.find("\r\n"));
    subject = buffer.substr(buffer.find("Subject: ") + 9);
    subject = subject.substr(0, subject.find("\r\n"));

    content = ExtractText(buffer);
    attachments = ExtractAttachments(buffer);
}

void Mail::SaveAttachment(size_t index, std::string path)
{
    fs::path filePath(path);
    if (fs::exists(path) && fs::is_directory(path))
    {
        filePath = (path + "/" + attachments[index].first);
    }

    std::ofstream out(filePath.string(), std::ios::binary | std::ios::out);
    std::vector<char> buffer;
    buffer = DecodeBase64(attachments[index].second);
    out.write(buffer.data(), buffer.size());
    out.close();
}

void Mail::SaveAttachments(std::string path)
{
    if (!fs::exists(path) && !fs::is_directory(path))
    {
        fs::create_directory(path);
    }
    for (int i = 0; i < attachments.size(); i++)
    {
        SaveAttachment(i, path);
    }
}

std::string Mail::EncodeBase64(const std::vector<char>& data)
{
    std::string encoded_string;

    size_t size = detail::base64::encoded_size(data.size());
    encoded_string.resize(size);
    boost::beast::detail::base64::encode(encoded_string.data(), data.data(), data.size());
    return encoded_string;
}

std::vector<char> Mail::DecodeBase64(const std::string& encoded_string)
{
    std::vector<char> decoded_data;
    std::stringstream ss(encoded_string);
    std::string line;
    while (std::getline(ss, line))
    {
        line = line.substr(0, line.find("\r"));
        if (line == "")break;
        size_t size = detail::base64::decoded_size(line.size());
        char* decoded = new char[size] { 0 };
        boost::beast::detail::base64::decode(decoded, line.c_str(), line.size());
        decoded_data.insert(decoded_data.end(), decoded, decoded + size);
        delete[] decoded;
    }
    if (decoded_data.back() == '\0')
    {
        decoded_data.pop_back();
    }
    return decoded_data;
}

void Mail::Save(std::string mainPath)
{
    std::ofstream out(mainPath + "/" + id + ".json");
    Json::Value root;
    root["Id"] = id;
    root["From"] = user;
    root["Subject"] = subject;
    root["Content"] = content;
    root["Attachments"] = Json::Value(Json::arrayValue);
    root["IsSeen"] = isSeen;
    root["Folder"] = folder;

    for (auto it = attachments.begin(); it != attachments.end(); it++)
    {
        Json::Value attachment;
        attachment["Name"] = it->first;
        attachment["Content"] = it->second;
        root["Attachments"].append(attachment);
    }
    out << root;
    out.close();
}

void Mail::Load(std::string path)
{
    std::ifstream in(path);
    Json::Value root;
    in >> root;
    in.close();
    id = root["Id"].asString();
    user = root["From"].asString();
    subject = root["Subject"].asString();
    content = root["Content"].asString();
    isSeen = root["IsSeen"].asBool();
    folder = root["Folder"].asString();
    for (size_t i = 0; i < root["Attachments"].size(); i++)
    {
        Json::Value attachment = root["Attachments"][(int)i];
        std::pair<std::string, std::string> pair(attachment["Name"].asString(), attachment["Content"].asString());
        attachments.push_back(pair);
    }
}

// Function to extract text part from MIME message
std::string Mail::ExtractText(const std::string& mimeMessage)
{
    // Simple implementation to extract text part between text/plain boundaries
    size_t textStart = mimeMessage.find("Content-Type: text/plain");
    if (textStart != std::string::npos)
    {
        size_t textEnd = mimeMessage.find("--", textStart + 1);
        if (textEnd != std::string::npos)
        {
            size_t contentStart = mimeMessage.find("\r\n\r\n", textStart) + 4; // Find the start of content after the headers
            return mimeMessage.substr(contentStart, textEnd - contentStart);
        }
        else
        {
            size_t contentStart = mimeMessage.find("\r\n\r\n", textStart) + 4; // Find the start of content after the headers
            size_t contentEnd = mimeMessage.find_last_of(".\r\n") - 2;
            return mimeMessage.substr(contentStart, contentEnd - contentStart);
        }
    }
    return "";
}

// Function to extract attachments with file names from MIME message
std::vector<std::pair<std::string, std::string>> Mail::ExtractAttachments(const std::string& mimeMessage)
{
    std::vector<std::pair<std::string, std::string>> attachments;
    size_t attachmentStart = mimeMessage.find("Content-Disposition: attachment");
    while (attachmentStart != std::string::npos)
    {
        size_t attachmentEnd = mimeMessage.find("--", attachmentStart + 1);
        if (attachmentEnd != std::string::npos)
        {
            size_t contentStart = mimeMessage.find("\r\n\r\n", attachmentStart) + 4; // Find the start of content after the headers
            size_t filenameStart = mimeMessage.find("filename=\"", attachmentStart) + 10;
            size_t filenameEnd = mimeMessage.find("\r\n", filenameStart) - 1;
            std::string filename = mimeMessage.substr(filenameStart, filenameEnd - filenameStart);
            attachments.push_back(std::make_pair(filename, mimeMessage.substr(contentStart, attachmentEnd - contentStart - 2)));
        }
        attachmentStart = mimeMessage.find("Content-Disposition: attachment", attachmentEnd + 1);
    }
    return attachments;
}


std::ostream& operator<<(std::ostream& os, const Mail& mail)
{
    os << "From: " << mail.user << std::endl;
    os << "Subject: " << mail.subject << std::endl;
    os << "Content: " << std::endl << mail.content << std::endl;
    return os;
}
