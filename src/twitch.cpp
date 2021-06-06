#include "twitch.h"


bool Twitch::open()
{
    portno = atoi(address.c_str());

    info.ai_family = AF_INET;
    info.ai_flags = 0;
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP;

    int err = getaddrinfo(address.c_str(), "6697", &info, &infoP);
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

    if (!login())
    {
        std::cerr << "Login ERR" << std::endl;
        return false;
    }

    return true;
}

bool Twitch::login()
{
    if (!sendAll("PASS " + password + "\r\n"))
    {
        return false;
    }

    if (!sendAll("NICK weirdbeardgame\r\n"))
    {
        return false;
    }

    int i = 0;
    int buffSize = 0;
    // not equal to catch neg error!!!
    while ((i = recv(sock, &buffer[0], 512, 0)) != 0)
    {
        std::cout << "Recieving Data" << std::endl;
        buffSize += i;
        if (buffSize < 0)
        {
            std::cerr << "Recieve Err: " << gai_strerror(buffSize) << std::endl;
            return false;
        }
        std::string out (buffer.begin(), buffer.end());
        std::cout << out << std::endl;
    }

    return true;
}

bool Twitch::sendAll(std::string buf)
{
    size_t size = buf.size();
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

bool Twitch::update()
{
    // Recieve buffer. Recieve commands.
    while (sock)
    {
        // stay connected
        int i = 0;
        // not equal to catch neg error!!!
        for (int buffSize = 512; buffSize > 0; buffSize -= i)
        {
            i = recv(sock, &buffer[0], 512, 0);
            std::cout << "Recieving Data" << std::endl;
            buffSize += i;
            if (buffSize < 0)
            {
                std::cerr << "Recieve Err: " << gai_strerror(buffSize) << std::endl;
                return false;
            }

            std::string out (buffer.begin(), buffer.end());

            if (out.find("PING :tmi.twitch.tv"))
            {
                sendAll(pong);
            }

            if (out.find("up"))
            {
                controller.emit(UP);
            }

            if (out.find("down"))
            {
                controller.emit(DOWN);
            }

            if (out.find("left"))
            {
                controller.emit(LEFT);
            }

            if (out.find("right"))
            {
                controller.emit(RIGHT);
            }

            if (out.find("a"))
            {
                controller.emit(A);
            }

            if (out.find("b"))
            {
                controller.emit(B);
            }

            if (out.find("x"))
            {
                controller.emit(X);
            }

            if (out.find("y"))
            {
                controller.emit(Y);
            }

            if (out.find("start"))
            {
                controller.emit(START);
            }

            if (out.find("select"))
            {
                controller.emit(SELECT);
            }

            std::cout << out << std::endl;
            out.clear();
       }
    }
}

void Twitch::exit()
{
    memset(&info, 0, sizeof(info));
    delete infoP;
    close(sock);
}