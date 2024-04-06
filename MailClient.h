#ifndef MAILCLIENT_H
#define MAILCLIENT_H
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <boost/filesystem.hpp>
#include "Config.h"
#include "Mail.h"
#include <set>

#define MAX_BUFFER_SIZE 4096
#define fs boost::filesystem

class MailClient
{
public:
    MailClient();
    virtual ~MailClient();
    void Run();

private:
    void Init();
    void Loop();
    void SendEmail();
    void ViewEmails();
    void ReadMail(size_t index);
    bool CreatePop3Socket();
    void ClosePop3Socket();
    void LoadMailList();
    size_t GetMailCount();
    std::vector<std::string> GetMailIDs();
    void SaveMailList();
    void LoadLocalMailList();
    void FilterMail(Mail& mail);
    std::vector<std::string> GetFolders();
    void ViewMailFolder(std::string folder);
    void SendMail(std::string to, std::string subject, std::string message, std::vector<std::string> attachments = std::vector<std::string>());
    bool CreateSmtpSocket();
    void CloseSmtpSocket();
    long long GetFileSize(std::string filePath);
    void FetchData();

private:
    bool m_isRunning;
    Config* config;

    std::vector<Mail> m_mailList;

    int m_pop3Socket;
    int m_smtpSocket;
};

#endif