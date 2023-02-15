#include "chat.h"

bool file_exists(std::string &fileName)
{

    std::ifstream file(fileName);

    if (!file)
    {
        file.close();
        return false;
    }
    file.close();
    return true;
}

Channel::Channel(std::string name, std::string fileName)
{
    if (!file_exists(fileName))
    {
        std::cout << "[ERROR] Tried to initalize channel with invalid file name" << std::endl;
        std::cout << "FILENAME: " << fileName;
        exit(1);
    }
    this->channel_name = name;
    this->m_storage_file = fileName;
}

Channel::Channel()
{
    this->channel_name = HOME_CHAT;
}

void Channel::write_chat(Message &new_message)
{

    std::ofstream outfile;

    outfile.open(m_storage_file, std::ios_base::app); // append instead of overwrite
    outfile << new_message.export_message_as_raw();

    outfile.close();
}

void Channel::add_message(User &author, std::string content)
{
    Message new_message(author, content);
    write_chat(new_message);

    messages.push_back(new_message);
}

std::vector<std::string> split(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

Message create_message_from_line(std::string &line)
{
    std::vector<std::string> items = split(line, "<<>>");
    std::string author = items[0].substr(1, items[0].length() - 1);
    std::string content = items[1].substr(1, items[1].length() - 1);

    User auth(author);
    return {auth, content};
}

void Channel::load_messages()
{
    std::string line;

    std::ifstream file(m_storage_file);
    while (getline(file, line))
    {
        messages.push_back(create_message_from_line(line));
    }
    file.close();
}
