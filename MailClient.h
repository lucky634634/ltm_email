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
#include "MailManager.h"

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
    struct addrinfo* Getaddrinfo(int port);

    void ConnectSTMP();
    void ConnectPOP3();

private:
    bool m_isRunning;
    Config* config;
    MailManager* mailmanager;

    int m_socket;

    struct addrinfo* m_addrSTMP;
    struct addrinfo* m_addrPOP3;

    char m_buffer[MAX_BUFFER_SIZE];
    std::thread m_autoload;
};

#endif