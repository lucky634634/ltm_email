#include "MailClient.h"

MailClient::MailClient()
{
    m_isRunning = false;
    config = Config::GetInstance();
    m_socket = -1;
    Init();
}

MailClient::~MailClient()
{
    if (m_socket != -1)
    {
        close(m_socket);
    }
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


    m_isRunning = false;
}

void MailClient::Loop()
{
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

            break;
        case 2:
            break;
        default:
            std::cout << "Vui chọn lại!" << std::endl;
            break;
        }
        std::cout << "\n\n\n";
    }
}
