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
#include "Config.h"
#include "Mail.h"

#define MAX_BUFFER_SIZE 1024

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
    void ViewEmail();

private:
    bool m_isRunning;
    Config* config;

    std::vector<Mail> mailList;
};

#endif