#include "message.h"

std::string Message::export_message_as_raw()
{
    std::string res;
    res += "[";
    res += author.user_name;
    res += "]<<>>[";
    res += content;
    res += "]";

    return res;
}
