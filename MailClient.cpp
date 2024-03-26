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

}