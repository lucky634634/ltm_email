#include <iostream>
#include <sys/socket.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <fstream>
// sudo apt-get install libjsoncpp-dev
#include <jsoncpp/json/json.h>

#include "Config.h"
#include "MailClient.h"

int main(int argc, char* argv[])
{
    MailClient client;
    client.Run();
    return 0;
}