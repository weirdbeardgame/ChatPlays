#pragma once
#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/http_listener.h>
#include <cpprest/oauth2.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::oauth2::experimental;
using namespace web::http::experimental::listener;

static void open_browser(utility::string_t auth_uri)
{
#if defined(_WIN32) && !defined(__cplusplus_winrt)
    // NOTE: Windows desktop only.
    auto r = ShellExecuteA(NULL, "open", conversions::utf16_to_utf8(auth_uri).c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
    // NOTE: Linux/X11 only.
    string_t browser_cmd(U("xdg-open \"") + auth_uri + U("\""));
    (void)system(browser_cmd.c_str());
#endif
}

class Oauth2Client
{

private:
    // The assigned config used to generate the URI and open the authentication page.
    oauth2_config *oauth;
    // The catchers glove. Literally catches the resulting call to localhost
    http_listener *listen;

public:
    bool TwitchConnect();
    bool DiscordConnect();
    void OauthConnect(std::string clientID, std::string clientSecret, std::string url);
};