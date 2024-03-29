#include "Windows/WinConnect.h"

bool Socket::Open(const char *hostName, const char *port)
{
    WSADATA winSockData;
    if (WSAStartup(MAKEWORD(2, 2), &winSockData) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    memset(&info, 0, sizeof(info));
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP;

    int err = getaddrinfo(hostName, port, &info, &infoP);
    if (err != 0)
    {
        // Note: errno is not set by addr info
        std::cerr << "ADDERINFO UNINTALIZED " << WSAGetLastError() << std::endl;
        return false;
    }

    for (addrinfo *connectP = infoP; connectP != nullptr; connectP = connectP->ai_next)
    {
        sock = socket(connectP->ai_family, connectP->ai_socktype, connectP->ai_protocol);

        if (sock < 0)
        {
            std::cerr << "SOCKET ERROR: " << WSAGetLastError() << std::endl;
            return false;
        }

        // ioctlsocket(sock, FIONBIO, (unsigned long*)1);

        int conErr = connect(sock, connectP->ai_addr, connectP->ai_addrlen);

        if (conErr != 0)
        {
            std::cerr << "CONNECTION ERROR: " << WSAGetLastError() << std::endl;
            return false;
        }
    }

    return true;
}

char *Socket::Recieve()
{
    int i = 0;
    int buffSize = 480, buffRecieved = 0;
    char *buff = new char[512];
    if (i = recv(sock, buff, buffSize, 0) > 0)
    {
        // not equal to catch neg error!!!

        std::cout << "Buff: " << buff << std::endl;

        if (i == 0)
        {
            std::cerr << "Connection severed by server" << std::endl;
            return nullptr;
        }

        else if (i < 0)
        {
            std::cerr << "Recieve Err: " << WSAGetLastError() << std::endl;
            return nullptr;
        }
        if (buffSize < i)
        {
            buffSize = i;
            buff = new char[buffSize];
        }
    }
    return buff;
}

int Socket::Send(std::string buf, int siz)
{
    if (sock <= 0)
    {
        std::cerr << "Connection terminated" << std::endl;
        return -1;
    }

    std::cout << "Buffer: " << val << std::endl;

    int size = send(sock, val, siz, 0);
    if (size <= 0)
    {
        std::cerr << "Send Err: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Size: " << size << std::endl;
    return size;
}

bool Socket::openSockFile(fs::path socket, char slot)
{
    // Open(socket.string().c_str(), slot);
    return false;
}

bool Socket::isConnected()
{
    return sock;
}

std::string Socket::ParseCommand(std::string command)
{
    std::string buffer = command.substr(command.find_last_of(":") + 1);
    buffer = buffer.erase(buffer.find("\r\n"));
    return buffer;
}

bool Socket::Send(std::string buf, int siz)
{
    if (sock <= 0)
    {
        std::cerr << "Connection terminated" << std::endl;
        return -1;
    }

    std::cout << "Buffer: " << buf << std::endl;

    int size = send(sock, buf.c_str(), siz, 0);
    if (size <= 0)
    {
        std::cerr << "Send Err: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Size: " << size << std::endl;
    return size;
}

void Socket::Disconnect()
{
    memset(&info, 0, sizeof(info));
    delete infoP;
}