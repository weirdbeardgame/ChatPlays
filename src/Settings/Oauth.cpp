#include "Oauth.h"

bool Oauth2Client::TwitchConnect()
{
    oauth = new oauth2_config("cyp873qkukunzp7ifvwu0ihdlrg0o0", "", "https://id.twitch.tv/oauth2/authorize", "", "http://localhost:3000");
    oauth->set_implicit_grant(true);
    return true;
}

bool Oauth2Client::DiscordConnect()
{
    std::cout << "Not Implemented" << std::endl;
    return false;
}