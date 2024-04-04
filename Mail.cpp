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
    user = buffer.substr(buffer.find("To: ") + 4, buffer.find("\r\n") - buffer.find("To: ") - 4);
    subject = buffer.substr(buffer.find("Subject: ") + 9, buffer.find("\r\n") - buffer.find("Subject: ") - 9);

    // Check if it has attachments
    if (buffer.find("boundary") != std::string::npos)
    {
        // Get boundary
        std::string boundary = buffer.substr(buffer.find("boundary=\"") + 10, buffer.find("\"\r\n") - buffer.find("boundary=\"") - 10);

        buffer = buffer.substr(buffer.find(boundary + "\r\n\r\n") + boundary.size() + 4);

        // Get mail content
        content = buffer.substr(buffer.find("7bit\r\n\r\n") + 8, buffer.find(boundary) - buffer.find("7bit\r\n\r\n") - 8);

        // Get attachments
        buffer = buffer.substr(buffer.find(content + "\r\n\r\n") + content.size() + 2);
        while (content.find("--") != std::string::npos)
        {
            if (buffer.find("name=") == std::string::npos)
            {
                break;
            }
            std::string fileName = buffer.substr(buffer.find("name=\"") + 10, buffer.find("\"\r\n") - buffer.find("name=\"") - 10);
            std::string attachment = buffer.substr(buffer.find("\r\n\r\n") + 4, buffer.find("\r\n\r\n" + boundary) - buffer.find("\r\n\r\n") - 4);

            attachments[fileName] = attachment;
            buffer = buffer.substr(buffer.find("\r\n\r\n" + boundary) + boundary.size() + 4);

        }
    }
    else
    {
        content = buffer.substr(buffer.find("\r\n\r\n") + 4);
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

std::ostream& operator<<(std::ostream& os, const Mail& mail)
{
    os << "From: " << mail.user << std::endl;
    os << "Subject: " << mail.subject << std::endl;
    os << "Content: " << mail.content << std::endl;
    return os;
}
