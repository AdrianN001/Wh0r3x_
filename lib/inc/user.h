#pragma once
#include <string>
#include <arpa/inet.h>

class Channel;
class User
{
public:
    std::string user_name;
    sockaddr_in address;
    Channel *active_channel;

public:
    User(std::string u_n) : user_name(u_n) {}                                                         // loading from messages
    User(std::string uN, sockaddr_in addr) : user_name(uN), address(addr), active_channel(nullptr) {} // at the point of creating the user isnt connected to any channel
    User(const User &other) = default;                                                                // copy constructor
public:
    std::string get_ip_address();

public:
    void change_channel(Channel *new_active_chat);
};

#include "chat.h"