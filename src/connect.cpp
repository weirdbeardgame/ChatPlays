#include "connect.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

bool Connect::open(const char* hostname, char* port)
{
    info.ai_family = AF_INET;
    info.ai_flags = AI_PASSIVE;
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP;

    int err = getaddrinfo(hostname, port, &info, &infoP);
    if (err != 0)
    {
        // Note: errno is not set by addr info
        std::cerr << "ADDERINFO UNINTALIZED " << gai_strerror(err) << std::endl;
        return false;
    }

    for (addrinfo *connectP = infoP; connectP != nullptr; connectP = connectP->ai_next)
    {
        sock = socket(connectP->ai_family, connectP->ai_socktype, connectP->ai_protocol);

        if (sock < 0)
        {
            std::cerr << "SOCKET ERROR: " << strerror(errno) << std::endl;
            return false;
        }

        int conErr = connect(sock, connectP->ai_addr, connectP->ai_addrlen);

        if (conErr != 0)
        {
            std::cerr << "CONNECTION ERROR: " << strerror(errno) << std::endl;
            return false;
        }
    }

    return true;
}

bool Connect::recieve(std::string &buffS)
{
    int i = 0;
    int buffSize = 512, buffRecieved = 0;
    char* buff = new char[512];
    while (buffRecieved < buffSize)
    {
        // not equal to catch neg error!!!
        i = recv(sock, buff + buffRecieved, buffSize, 0); 
        std::cout << buff << std::endl;

        if (i == 0)
        {
            std::cerr << "Connection severed by server" << std::endl;
            return false;
        }

        else if (i < 0)
        {
            std::cerr << "Recieve Err: " << gai_strerror(i) << std::endl;
            return false;
        }
        if (buffSize < i)
        {
            buffSize = i;
            buff = new char[buffSize];
        }
        buffRecieved += i;
        buffSize -= i;
    }
        buffS = buff;
        std::cout << buffS << std::endl;
        delete[] buff;
        return true;
}

bool Connect::isConnected()
{
    return sock;
}

std::string Connect::parseCommand(std::string command)
{
    return strtok(command.data(), "!");
}

bool Connect::sendAll(std::string buf)
{
    size_t size = buf.size();
    if (sock <= 0)
    {
        std::cerr << "Connection terminated" << std::endl;
        return false;
    }
    int i = send(sock, buf.c_str(), size, 0);
    if (i < 0)
    {
        std::cerr << "Send Err: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Buff: " << buf << " Buff Size: " << buf.size() << std::endl;
    std::cout << "Sent: " << i << std::endl;
    return true;

}

void Connect::disconnect()
{
    memset(&info, 0, sizeof(info));
    delete infoP;
    close(sock);
}