#include "Mail.h"



Mail::Mail()
{
    isSeen = false;
    folder = "";
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

    // Check if it has attachments
    if (buffer.find("boundary") != std::string::npos)
    {
        // Get boundary
        std::string boundary = buffer.substr(buffer.find("boundary=\"") + 10, buffer.find("\"\r\n") - buffer.find("boundary=\"") - 10);
        boundary = boundary.substr(boundary.find_last_of("-") + 1);

        buffer = buffer.substr(buffer.find(boundary + "\r\n") + boundary.size() + 2);

        // Get mail content
        content = buffer.substr(buffer.find("7bit\r\n\r\n") + 8, buffer.find(boundary) - buffer.find("7bit\r\n\r\n") - 8);

        content = content.substr(0, content.find("\r\n"));
        // Get attachments
        do
        {
            buffer = buffer.substr(buffer.find(boundary + "\r\n") + boundary.size() + 2);
            if (buffer.find("name=") == std::string::npos)
            {
                break;
            }
            std::string fileName = buffer.substr(buffer.find("name=\"") + 6, buffer.find("\"\r\n") - buffer.find("name=\"") - 6);
            std::string attachment = buffer.substr(buffer.find("\r\n\r\n") + 4);
            attachment = attachment.substr(0, attachment.find("\r\n\r\n"));

            attachments[fileName] = attachment;
            // buffer = buffer.substr(buffer.find(boundary));
        } while (buffer.find(boundary) != std::string::npos && buffer.find("Content-Type:") != std::string::npos);
    }
    else
    {
        content = buffer.substr(buffer.find("\r\n\r\n") + 4);
        content = content.substr(0, content.find("\r\n\r\n"));
    }
}

void Mail::SaveAttachment(size_t index, std::string path)
{
    fs::path filePath(path);
    if (fs::exists(path) && fs::is_directory(path))
    {
        filePath = (path + "/" + attachments.begin()->first);
    }

    std::ofstream out(filePath.string(), std::ios::binary);
    std::vector<char> buffer;


    out << attachments.begin()->second;
    out.close();


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
    size_t size = detail::base64::decoded_size(encoded_string.size());
    char* decoded = new char[size];
    decoded_data.resize(size);

    boost::beast::detail::base64::decode(decoded, encoded_string.c_str(), encoded_string.size());

    decoded_data.insert(decoded_data.end(), decoded, decoded + size);
    delete[] decoded;

    return decoded_data;
}

void Mail::Save(std::string mainPath)
{
    if (folder != "")
    {
        mainPath = mainPath + "/" + folder;
    }
    std::ofstream out(mainPath + "/" + id + ".json");
    Json::Value root;
    root["From"] = user;
    root["Subject"] = subject;
    root["Content"] = content;
    root["Attachments"] = Json::Value(Json::arrayValue);
    for (std::map<std::string, std::string>::iterator it = attachments.begin(); it != attachments.end(); it++)
    {
        Json::Value attachment;
        attachment["Name"] = it->first;
        attachment["Content"] = it->second;
        root["Attachments"].append(attachment);
    }
    out << root;
    out.close();
}


std::ostream& operator<<(std::ostream& os, const Mail& mail)
{
    os << "From: " << mail.user << std::endl;
    os << "Subject: " << mail.subject << std::endl;
    os << "Content: " << mail.content << std::endl;
    return os;
}
