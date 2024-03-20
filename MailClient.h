#pragma once
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Config.h"

class MailClient
{
public:
    MailClient();
    virtual ~MailClient();
    void Run();

private:
    void Init();
    void Loop();

private:
    bool m_isRunning;
    Config* config;

    int m_socket;
};