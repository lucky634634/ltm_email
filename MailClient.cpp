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
    std::thread fetchData = std::thread(&MailClient::FetchData, this);
    fetchData.detach();

    std::cout << "Xin chào, " << config->name << "!" << std::endl;
    while (m_isRunning)
    {
        std::cout << "Vui lòng chọn Menu" << std::endl;
        std::cout << "1. Gửi email" << std::endl;
        std::cout << "2. Xem danh sách các email đã nhận" << std::endl;
        std::cout << "0. Thoát" << std::endl;
        std::cout << "Vui chọn: ";

        int choice;
        std::cin >> choice;
        switch (choice)
        {
        case 0:
            m_isRunning = false;
            break;
        case 1:
            SendEmail();
            break;
        case 2:
            ViewEmails();
            break;
        default:
            std::cout << "Vui chọn lại!" << std::endl;
            break;
        }
    }
}

void MailClient::SendEmail()
{
    std::cout << "Đây thông tin soạn email" << std::endl;
    std::cout << "To: ";
    std::string to;
    std::cin.ignore();
    std::getline(std::cin, to);
    std::string cc;
    std::cout << "CC: ";
    std::getline(std::cin, cc);
    std::string bcc;
    std::cout << "BCC: ";
    std::getline(std::cin, bcc);
    std::string subject;
    std::cout << "Subject: ";
    std::getline(std::cin, subject);
    std::cout << "Content: (. để thoát)" << std::endl;
    std::string content;
    do
    {
        std::string temp;
        std::getline(std::cin, temp);
        if (temp == ".")
        {
            content = content.substr(0, content.size() - 2);
            break;
        }
        content += temp + "\r\n";
    } while (true);
    std::cout << "Có gửi kèm file (Y/N): ";
    std::string choice;
    std::cin >> choice;
    std::vector<std::string > attachments;
    if (choice == "Y" || choice == "y")
    {
        std::cout << "Số lượng file: ";
        int count;
        std::cin >> count;
        int i = 0;
        while (i < count)
        {
            std::cout << "File name: ";
            std::string fileName;
            std::cin.ignore();
            std::getline(std::cin, fileName);
            if (fs::exists(fileName) && fs::is_regular_file(fileName))
            {
                if (GetFileSize(fileName) > 3 * 1024 * 1024)
                {
                    std::cout << "File qua lớn" << std::endl;
                    continue;
                }
                attachments.push_back(fileName);
                i++;
            }
        }
    }

    SendMail(to, cc, bcc, subject, content, attachments);
}

void MailClient::ViewEmails()
{
    std::cout << "Đây là danh sách các folder trong mailbox của bạn: " << std::endl;
    std::vector<std::string> folders = GetFolders();
    for (size_t i = 0; i < folders.size(); i++)
    {
        std::cout << i + 1 << ". " << folders[i] << std::endl;
    }
    std::cout << "0. Thoat" << std::endl;
    std::cout << "Vui chọn: ";
    int choice;
    std::cin >> choice;
    if (choice == 0)
    {
        return;
    }
    std::cin.ignore();
    ViewMailFolder(folders[choice - 1]);
}

void MailClient::ReadMail(size_t index)
{
    std::cout << m_mailList[index] << std::endl;
    m_mailList[index].isSeen = true;
    if (m_mailList[index].attachments.size() > 0)
    {
        std::cout << "Danh sách attached files: " << std::endl;
        for (auto& attachment : m_mailList[index].attachments)
        {
            std::cout << attachment.first << std::endl;
        }
        std::cout << "Bạn có muốn lưu file(Y/N): " << std::endl;
        std::string choice;
        std::cin >> choice;
        if (choice == "có" || choice == "Có" || choice == "Co" || choice == "co" || choice == "1" || choice == "yes" || choice == "Yes" || choice == "YES" || choice == "y" || choice == "Y")
        {
            std::cin.ignore();
            std::string path;
            std::cout << "Chọn đường dẫn lưu file: ";
            std::getline(std::cin, path);
            m_mailList[index].SaveAttachments(path);
        }
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


    auto check = [](std::string str, std::vector<Mail> mailList)
        {
            for (const auto& mail : mailList)
            {
                if (mail.id == str)
                {
                    return true;
                }
            }
            return false;
        };

    for (size_t i = 0; i < count; i++)
    {
        if (check(ids[i], m_mailList))
        {
            continue;
        }

        if (!CreatePop3Socket())
        {
            return;
        }

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
            memset(buffer, 0, MAX_BUFFER_SIZE);
            int bufferSize = recv(m_pop3Socket, buffer, MAX_BUFFER_SIZE, 0);
            temp += std::string(buffer, bufferSize);
            currentSize += bufferSize;
        }
        Mail mail;
        mail.id = ids[i];
        mail.Convert(temp);
        FilterMail(mail);
        m_mailList.push_back(mail);
        ClosePop3Socket();
    }
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

    for (size_t i = 0; i < m_mailList.size(); i++)
    {
        m_mailList[i].Save(path.c_str());
    }
}

void MailClient::LoadLocalMailList()
{
    if (!fs::exists(config->email))
    {
        return;
    }
    fs::path path(config->email);
    for (auto& entry : fs::directory_iterator(path))
    {
        Mail mail;
        mail.Load(entry.path().string());
        m_mailList.push_back(mail);
    }
}

void MailClient::FilterMail(Mail& mail)
{
    for (auto& filter : config->userFilter)
    {
        for (auto& word : filter.second)
        {
            if (mail.user.find(word) != std::string::npos)
            {
                mail.folder = filter.first;
                break;
            }
        }
    }
    for (auto& filter : config->subbjectFilter)
    {
        for (auto& word : filter.second)
        {
            if (mail.subject.find(word) != std::string::npos)
            {
                mail.folder = filter.first;
                break;
            }
        }
    }
    for (auto& filter : config->contentFilter)
    {
        for (auto& word : filter.second)
        {
            if (mail.content.find(word) != std::string::npos)
            {
                mail.folder = filter.first;
                break;
            }
        }
    }
    for (auto& filter : config->spamFilter)
    {
        for (auto& word : filter.second)
        {
            if (mail.content.find(word) != std::string::npos)
            {
                mail.folder = filter.first;
                break;
            }
            if (mail.subject.find(word) != std::string::npos)
            {
                mail.folder = filter.first;
                break;
            }
            if (mail.user.find(word) != std::string::npos)
            {
                mail.folder = filter.first;
                break;
            }
        }
    }
}

std::vector<std::string> MailClient::GetFolders()
{
    std::set <std::string> folders;
    folders.insert("Inbox");
    for (auto& filter : config->userFilter)
    {
        folders.insert(filter.first);
    }
    for (auto& filter : config->subbjectFilter)
    {
        folders.insert(filter.first);
    }
    for (auto& filter : config->contentFilter)
    {
        folders.insert(filter.first);
    }
    for (auto& filter : config->spamFilter)
    {
        folders.insert(filter.first);
    }
    return std::vector<std::string>(folders.begin(), folders.end());
}

void MailClient::ViewMailFolder(std::string folder)
{
    std::cout << "Đây là danh sách email trong " << folder << ": " << std::endl;
    std::vector <int> ids;
    for (size_t i = 0; i < m_mailList.size(); i++)
    {
        if (m_mailList[i].folder == folder)
        {
            ids.push_back(i);
            std::cout << ids.size() << ": " << (m_mailList[i].isSeen ? "(Đã đọc) " : "(Chua đọc) ") << m_mailList[i].user << " - " << m_mailList[i].subject << std::endl;
        }
    }
    std::cout << "Bạn muốn đọc email thứ mấy: ";
    int choice;
    std::cin >> choice;
    if (choice > 0 && choice <= ids.size())
    {
        ReadMail(ids[choice - 1]);
    }
}

void MailClient::SendMail(std::string to, std::string cc, std::string bcc, std::string subject, std::string message, std::vector<std::string> attachments)
{
    if (!CreateSmtpSocket())
    {
        return;
    }
    std::string msg = "MAIL FROM: <" + config->email + ">\r\n";
    if (send(m_smtpSocket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    char buffer[MAX_BUFFER_SIZE] = { 0 };
    if (recv(m_smtpSocket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return;
    }
    if (std::string(buffer).find("250") == std::string::npos)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    msg = "RCPT TO: <" + to + ">\r\n";
    if (send(m_smtpSocket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }

    memset(buffer, 0, MAX_BUFFER_SIZE);
    if (recv(m_smtpSocket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return;
    }
    if (std::string(buffer).find("250") == std::string::npos)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }

    msg = "DATA\r\n";
    if (send(m_smtpSocket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
    }
    memset(buffer, 0, MAX_BUFFER_SIZE);
    if (recv(m_smtpSocket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return;
    }
    if (std::string(buffer).find("354") == std::string::npos)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }

    msg = "From: " + config->email + "\r\n";
    if (!to.empty())
    {
        msg += "To: " + to + "\r\n";
    }
    if (!cc.empty())
    {
        msg += "Cc: " + cc + "\r\n";
    }
    if (!bcc.empty())
    {
        msg += "Bcc: " + bcc + "\r\n";
    }
    msg += "Subject: " + subject + "\r\n";
    msg += "MIME-Version: 1.0\r\n";
    if (attachments.size() > 0)
    {
        msg += "Content-Type: multipart/mixed; boundary=\"MyBoundary\"\r\n\r\n";
        msg += "--MyBoundary\r\n";
    }
    msg += "Content-Type: text/plain\r\n";
    msg += "Content-Transfer-Encoding: 7bit\r\n\r\n";
    msg += message + "\r\n";
    if (attachments.size() > 0)
    {
        msg += "--MyBoundary\r\n";
        for (size_t i = 0; i < attachments.size(); i++)
        {
            msg += "Content-Type: application/octet-stream\r\n";
            msg += "Content-Transfer-Encoding: base64\r\n";
            size_t pos = attachments[i].find_last_of("/");
            if (pos != std::string::npos)
            {
                msg += "Content-Disposition: attachment; filename=\"" + attachments[i].substr(pos + 1) + "\"\r\n\r\n";
            }
            else
            {
                msg += "Content-Disposition: attachment; filename=\"" + attachments[i] + "\"\r\n\r\n";
            }
            std::ifstream file(attachments[i].c_str(), std::ios::binary);
            if (!file)
            {
                std::cout << "Error opening file" << std::endl;
                return;
            }
            file.seekg(0, std::ios::end);
            size_t length = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(length);
            file.read(buffer.data(), length);
            file.close();
            std::string encoded = Mail::EncodeBase64(buffer);
            msg += encoded + "\r\n\r\n";
            msg += "--MyBoundary\r\n";
        }
    }
    msg += "\r\n.\r\n";
    if (send(m_smtpSocket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    memset(buffer, 0, MAX_BUFFER_SIZE);
    if (recv(m_smtpSocket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return;
    }
    if (std::string(buffer).find("250") == std::string::npos)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    msg = "QUIT\r\n";
    if (send(m_smtpSocket, msg.c_str(), msg.size(), 0) == -1)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    memset(buffer, 0, MAX_BUFFER_SIZE);
    if (recv(m_smtpSocket, buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return;
    }
    if (std::string(buffer).find("221") == std::string::npos)
    {
        std::cout << "Error sending data" << std::endl;
        return;
    }
    std::cout << "Gửi email thành công" << std::endl;
    close(m_smtpSocket);
}

bool MailClient::CreateSmtpSocket()
{
    m_smtpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_smtpSocket == -1)
    {
        std::cout << "Error creating socket" << std::endl;
        return false;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->portSMTP);
    server_addr.sin_addr.s_addr = inet_addr(config->mailServer.c_str());
    if (connect(m_smtpSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "Error connecting to server" << std::endl;
        return false;
    }
    char buffer[MAX_BUFFER_SIZE] = { 0 };
    int size = recv(m_smtpSocket, buffer, MAX_BUFFER_SIZE, 0);
    if (size == -1)
    {
        std::cout << "Error receiving data" << std::endl;
        return false;
    }
    if (std::string(buffer).find("220") == std::string::npos)
    {
        std::cout << "Error connecting to server" << std::endl;
        return false;
    }
    return true;
}

void MailClient::CloseSmtpSocket()
{
    close(m_smtpSocket);
}

long long MailClient::GetFileSize(std::string filePath)
{
    if (fs::exists(filePath) && fs::is_regular_file(filePath))
    {
        return fs::file_size(filePath);
    }
    return 0;
}

void MailClient::FetchData()
{
    LoadLocalMailList();
    while (m_isRunning)
    {
        LoadMailList();
        SaveMailList();
        sleep(config->autoload);
    }
}
