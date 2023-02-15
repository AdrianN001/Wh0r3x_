#pragma once
#include <string>
#include "user.h"

struct Message
{
    User author;
    std::string content;
    Message(User _author, std::string _content) : author(_author), content(_content){};

    std::string export_message_as_raw();
};
#include "user.h"