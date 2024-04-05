#include "MailClient.h"

MailClient::MailClient()
{
    m_isRunning = false;
    config = Config::GetInstance();

    Init();
}

MailClient::~MailClient()
{

}

void MailClient::Run()
{
    Loop();
}

void MailClient::Init()
{
    m_isRunning = true;
}

void MailClient::Loop()
{
    std::cout << "Xin chào, " << config->name << "!" << std::endl;
    LoadMailList();
    while (m_isRunning)
    {
        std::cout << "Vui lòng chọn Menu" << std::endl;
        std::cout << "1. Gửi email" << std::endl;
        std::cout << "2. Xem danh sách các email đã nhận" << std::endl;
        std::cout << "0. Thoát" << std::endl;
        std::cout << "Vui chọn: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();
        switch (choice)
        {
        case 0:
            m_isRunning = false;
            break;
        case 1:
            SendEmail();
            break;
        case 2:
            ViewEmail();
            break;
        default:
            std::cout << "Vui chọn lại!" << std::endl;
            break;
        }
        std::cout << "\n\n\n";
    }
    SaveMailList();
}

void MailClient::SendEmail()
{}

void MailClient::ViewEmail()
{
    for (size_t i = 0; i < m_mailList.size(); i++)
    {
        std::cout << i << ". " << m_mailList[i].content << std::endl;
        std::cout << "id: " << m_mailList[i].id << std::endl;
        if (m_mailList[i].attachments.size() > 0)
        {
            std::cout << "Danh sách attached files: " << std::endl;
            for (auto& attachment : m_mailList[i].attachments)
            {
                std::cout << attachment.first << std::endl;
            }
        }
    }
}

void MailClient::ReadMail(size_t index)
{
    std::cout << m_mailList[index] << std::endl;
    if (m_mailList[index].attachments.size() > 0)
    {
        std::cout << "Danh sách attached files: " << std::endl;
        for (auto& attachment : m_mailList[index].attachments)
        {
            std::cout << attachment.first << std::endl;
        }
        std::cout << "Chọn đường dẫn lưu file: ";

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string choice;
        std::getline(std::cin, choice);
    }
}

bool MailClient::CreatePop3Socket()
{
    m_pop3Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_pop3Socket == -1)
    {
        std::cout << "Error creating socket" << std::endl;
        return false;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(config->portPOP3);
    serverAddress.sin_addr.s_addr = inet_addr(config->mailServer.c_str());

    if (connect(m_pop3Socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cout << "Error connecting to server" << std::endl;
        return false;
    }

    char buffer[MAX_BUFFER_SIZE];

    if (recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return false;
    }

    std::string msg = "USER " + config->email + "\r\n";
    if (send(m_pop3Socket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return false;
    }

    memset(buffer, 0, MAX_BUFFER_SIZE);
    if (recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return false;
    }

    msg = "PASS " + config->password + "\r\n";
    if (send(m_pop3Socket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return false;
    }
    if (recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return false;
    }

    return true;
}

void MailClient::ClosePop3Socket()
{
    std::string msg = "QUIT\r\n";
    if (send(m_pop3Socket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    close(m_pop3Socket);
}

void MailClient::LoadMailList()
{
    size_t count = GetMailCount();
    std::vector<std::string> ids = GetMailIDs();
    if (!CreatePop3Socket())
    {
        return;
    }
    for (size_t i = 0; i < count; i++)
    {

        std::string msg = "RETR " + std::to_string(i + 1) + "\r\n";

        if (send(m_pop3Socket, msg.c_str(), msg.size(), 0) == -1)
        {
            std::cout << "Error sending data" << std::endl;
            return;
        }
        char buffer[MAX_BUFFER_SIZE] = { 0 };
        long long currentSize = recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0);
        if (currentSize == -1)
        {
            std::cout << "Error receiving data" << std::endl;
            return;
        }
        std::string temp = buffer;
        size_t mailSize = std::stoi(temp.substr(temp.find("+OK ") + 4));
        while ((size_t)currentSize < mailSize)
        {
            currentSize += recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0);
            temp += buffer;
        }

        Mail mail;
        mail.id = ids[i];
        mail.Convert(temp);

        m_mailList.push_back(mail);
    }
    ClosePop3Socket();
}

size_t MailClient::GetMailCount()
{
    if (!CreatePop3Socket())
    {
        return 0;
    }

    std::string msg = "STAT\r\n";

    if (send(m_pop3Socket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return 0;
    }
    char buffer[MAX_BUFFER_SIZE] = { 0 };
    if (recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return 0;
    }
    std::string temp = buffer;
    temp = temp.substr(temp.find("+OK ") + 4);
    size_t count = std::stoi(temp.substr(0, temp.find(" ")));


    ClosePop3Socket();
    return count;
}

std::vector<std::string> MailClient::GetMailIDs()
{
    if (!CreatePop3Socket())
    {
        return std::vector<std::string>();
    }
    std::string msg = "UIDL\r\n";
    char buffer[MAX_BUFFER_SIZE] = { 0 };
    if (send(m_pop3Socket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return std::vector<std::string>();
    }
    if (recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return std::vector<std::string>();
    }
    std::string temp = buffer;
    temp = temp.substr(temp.find("\r\n") + 2);
    std::vector<std::string> ids;
    while (!temp.empty())
    {
        ids.push_back(temp.substr(temp.find(" ") + 1, temp.find("\r\n") - temp.find(" ") - 1));
        temp = temp.substr(temp.find("\r\n") + 2);
    }
    ClosePop3Socket();
    return ids;
}

void MailClient::SaveMailList()
{
    fs::path path(config->email);
    if (!fs::exists(path) && !fs::is_directory(path))
    {
        fs::create_directory(path);
    }

    if (!fs::exists(path / "inbox") && !fs::is_directory(path / "inbox"))
    {
        fs::create_directory(path / "inbox");
    }

    for (auto userFilter : config->userFilter)
    {
        fs::path userPath = path / userFilter.first;
        if (!fs::exists(userPath) && !fs::is_directory(userPath))
        {
            fs::create_directory(userPath);
        }
    }

    for (auto subbjectFilter : config->subbjectFilter)
    {
        fs::path userPath = path / subbjectFilter.first;
        if (!fs::exists(userPath) && !fs::is_directory(userPath))
        {
            fs::create_directory(userPath);
        }
    }
    for (auto contentFilter : config->contentFilter)
    {
        fs::path userPath = path / contentFilter.first;
        if (!fs::exists(userPath) && !fs::is_directory(userPath))
        {
            fs::create_directory(userPath);
        }
    }
    for (auto spamFilter : config->spamFilter)
    {
        fs::path userPath = path / spamFilter.first;
        if (!fs::exists(userPath) && !fs::is_directory(userPath))
        {
            fs::create_directory(userPath);
        }
    }

    for (size_t i = 0; i < m_mailList.size(); i++)
    {
        m_mailList[i].Save(path.c_str());
        std::cout << "Saved " << i << " mail" << std::endl;
    }
}
