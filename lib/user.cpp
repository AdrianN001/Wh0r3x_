#include "user.h"

void User::change_channel(Channel *new_active_chat)
{
    active_channel = new_active_chat;
}

std::string User::get_ip_address()
{
    return inet_ntoa(address.sin_addr);
}

User::~User()
{
    delete active_channel;
}