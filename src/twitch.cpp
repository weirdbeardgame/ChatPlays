#include "twitch.h"


bool Twitch::open()
{
    portno = atoi(address.c_str());

    sockIn.sin_family = AF_INET;
    sockIn.sin_port = htons(portno);
    if ((host = gethostbyname(address.c_str())) != 0)
    {
        std::cerr << "HOSTNAME COULD NOT BE RESOLVED" << std::endl;
        return false;
    }

    info.ai_family = AF_INET;
    info.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP;

    int err = getaddrinfo(address.c_str(), "443", &info, &infoP);
    if (err != 0)
    {
        // Note: errno is not set by addr info
        std::cerr << "ADDERINFO UNINTALIZED " << gai_strerror(err) << std::endl;
        return false;
    }

    sock = socket(info.ai_family, info.ai_socktype, info.ai_protocol);

    if (sock < 0)
    {
        std::cerr << "SOCKET ERROR: " << strerror(errno) << std::endl;
        return false;
    }

    int conErr = connect(sock, info.ai_addr, info.ai_addrlen);
    if (conErr != 0)
    {
        std::cerr << "CONNECTION ERROR: " << strerror(errno) << std::endl;
        return false;
    }

}