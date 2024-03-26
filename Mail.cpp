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
    user = buffer.substr(buffer.find("To: ") + 4, buffer.find("\r\n") - buffer.find("To: ") - 4);
    subject = buffer.substr(buffer.find("Subject: ") + 9, buffer.find("\r\n") - buffer.find("Subject: ") - 9);

    if (buffer.find("boundary") != std::string::npos)
    {
        std::string boundary = buffer.substr(buffer.find("boundary=\"") + 10, buffer.find("\"\r\n") - buffer.find("boundary=\"") - 10);

        buffer = buffer.substr(buffer.find(boundary + "\r\n\r\n") + boundary.size() + 4);



        content = buffer.substr(buffer.find("7bit\r\n\r\n") + 8, buffer.find(boundary) - buffer.find("7bit\r\n\r\n") - 8);

        buffer = buffer.substr(buffer.find(content + "\r\n\r\n") + content.size() + 2);
        while (content.find("--") != std::string::npos)
        {
            std::string fileName = buffer.substr(buffer.find("filename=\"") + 10, buffer.find("\"\r\n") - buffer.find("filename=\"") - 10);
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

std::ostream& operator<<(std::ostream& os, const Mail& mail)
{
    os << "From: " << mail.user << std::endl;
    os << "Subject: " << mail.subject << std::endl;
    os << "Content: " << mail.content << std::endl;
    return os;
}
