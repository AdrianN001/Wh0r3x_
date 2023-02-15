#pragma once

#include "user.h"
#include "message.h"

#include <string>
#include <arpa/inet.h>
#include <vector>
#include <iostream>

#include <fstream>

#define HOME_CHAT "home"

//
// [username]<<>>[content]
//
class User;
struct Message;

class Channel
{
public:
    std::string channel_name;
    std::vector<Message> messages;

private:
    std::string m_storage_file;
    std::vector<User> m_Active_users;

public:
    Channel(std::string name, std::string fileName);
    Channel(); // uninitalized
public:
    void load_messages();

public:
    void add_message(User &author, std::string content);

private:
    void write_chat(Message &new_message);
    void limit_chat();
};

#include "message.h"
#include "user.h"