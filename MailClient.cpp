#include "MailClient.h"

MailClient::MailClient()
{
    m_isRunning = false;
    config = Config::GetInstance();
    mailmanager = MailManager::GetInstance();
    m_socket = -1;
    m_addrSTMP = NULL;
    m_addrPOP3 = NULL;
    Init();
}

MailClient::~MailClient()
{
    if (m_addrSTMP != NULL)
    {
        freeaddrinfo(m_addrSTMP);
    }
    if (m_addrPOP3 != NULL)
    {
        freeaddrinfo(m_addrPOP3);
    }
    if (m_socket != -1)
    {
        close(m_socket);
    }

    m_socket = -1;
    m_addrSTMP = NULL;
    m_addrPOP3 = NULL;
}

void MailClient::Run()
{
    Loop();
}

void MailClient::Init()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket == -1)
    {
        std::cerr << "socket error" << std::endl;
        return;
    }

    m_addrSTMP = Getaddrinfo(config->portSMTP);
    if (m_addrSTMP == NULL) return;
    m_addrPOP3 = Getaddrinfo(config->portPOP3);
    if (m_addrPOP3 == NULL) return;

    m_socket = socket(m_addrSTMP->ai_family, m_addrSTMP->ai_socktype, m_addrSTMP->ai_protocol);
    if (m_socket == -1)
    {
        std::cerr << "socket error" << std::endl;
        return;
    }

    m_isRunning = true;
}

void MailClient::Loop()
{
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

}

void MailClient::SendEmail()
{}

void MailClient::ViewEmail()
{
    ConnectPOP3();
    std::string msg = "USER " + config->email + "\r\n";
    send(m_socket, msg.c_str(), msg.size(), 0);
    msg = "PASS " + config->password + "\r\n";
    send(m_socket, msg.c_str(), msg.size(), 0);
    memset(m_buffer, 0, MAX_BUFFER_SIZE);
    recv(m_socket, m_buffer, MAX_BUFFER_SIZE, 0);
    std::cout << std::string(m_buffer) << std::endl;

    msg = "LIST\r\n";
    send(m_socket, msg.c_str(), msg.size(), 0);
    memset(m_buffer, 0, MAX_BUFFER_SIZE);
    recv(m_socket, m_buffer, MAX_BUFFER_SIZE, 0);
    std::cout << std::string(m_buffer) << std::endl;

    msg = "RETR 1\r\n";
    send(m_socket, msg.c_str(), msg.size(), 0);
    memset(m_buffer, 0, MAX_BUFFER_SIZE);
    recv(m_socket, m_buffer, MAX_BUFFER_SIZE, 0);
    std::cout << std::string(m_buffer) << std::endl;

    msg = "QUIT\r\n";
    send(m_socket, msg.c_str(), msg.size(), 0);
    memset(m_buffer, 0, MAX_BUFFER_SIZE);
    recv(m_socket, m_buffer, MAX_BUFFER_SIZE, 0);
    std::cout << std::string(m_buffer) << std::endl;
}

addrinfo* MailClient::Getaddrinfo(int port)
{
    struct addrinfo hints;
    struct addrinfo* result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    if (getaddrinfo(config->mailServer.c_str(), std::to_string(port).c_str(), &hints, &result) != 0)
    {
        std::cerr << "getaddrinfo error" << std::endl;
        return NULL;
    }

    return result;
}

void MailClient::ConnectSTMP()
{
    if (connect(m_socket, m_addrSTMP->ai_addr, m_addrSTMP->ai_addrlen) == -1)
    {
        std::cerr << "connect error" << std::endl;
        return;
    }

    memset(m_buffer, 0, MAX_BUFFER_SIZE);
    if (recv(m_socket, m_buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cerr << "recv error" << std::endl;
        return;
    }

    std::cout << std::string(m_buffer) << std::endl;
}

void MailClient::ConnectPOP3()
{
    if (connect(m_socket, m_addrPOP3->ai_addr, m_addrPOP3->ai_addrlen) == -1)
    {
        std::cerr << "connect error" << std::endl;
        return;
    }

    if (recv(m_socket, m_buffer, MAX_BUFFER_SIZE, 0) == -1)
    {
        std::cerr << "recv error" << std::endl;
        return;
    }
    std::cout << std::string(m_buffer) << std::endl;
}


