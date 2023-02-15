#include "server.h"

Server::Server(std::string ip_address, int port)
{
    this->m_ip_address = ip_address;
    this->m_port = port;

    int sockfd;
    sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        std::cout << "Error" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        this->m_socket = sockfd;
    }

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    int res = bind(sockfd, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));

    if (res < 0)
    {
        std::cout << "Error" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
    close(this->m_socket);
}

void Server::Start_Server()
{
    std::cout << "STARTED THE SERVER" << std::endl;
    sockaddr_in cliaddr;

    socklen_t len;
    char BUFFER[MAXLINE];
    while (1)
    {

        len = sizeof(cliaddr); // len is value/result

        int n = recvfrom(this->m_socket, (char *)BUFFER, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *)&cliaddr,
                         &len);
        BUFFER[n] = '\0';

        std::thread thread(&Server::process, this, BUFFER, cliaddr);

        printf("[*] Message received: %s\n", BUFFER);
        /*
        sendto(this->m_socket, (const char *)hello, strlen(hello),
               MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
               len);
        */
        thread.join();
    }
}

void Server::process(std::string input, sockaddr_in address)
{
    if (input.substr(0, 10) == "$handshake")
    {
        std::string username = input.substr(11, input.length());
        std::cout << "[*] User joined: " << username << "\n";
        Add_User(username, address);
    }
    else if (input.substr(0, 8) == "$leaving")
    {
        std::string username = input.substr(9, input.length());
        std::cout << "[*] User left: " << username << '\n';
        Remove_User(address);
    }
    else if (input.substr(0, 8) == "$message")
    {

        std::string message = input.substr(9, input.length());
        User *author = get_user(address);

        std::cout << "[*] Message received from: " << author->user_name << std::endl;

        if (author->active_channel->channel_name != HOME_CHAT)
        {
            author->active_channel->add_message(*author, message);
        }
    }
    else if (input.substr(0, 5) == "$join")
    {

        User *author = get_user(address);
        std::string channel_name = input.substr(6, input.length());
        std::cout << channel_name << std::endl;
        Channel *new_channel = get_channel(channel_name);
        if (new_channel == nullptr)
        {
            send_message("ERROR", address);
            return;
        }
        else
        {
            author->active_channel = get_channel(channel_name);
        }
    }
    else if (input.substr(0, 10) == "$list_user")
    {

        for (int i = 0; i < m_Active_user.size(); i++)
        {
            if (m_Active_user[i].active_channel == nullptr) // home
            {
                std::cout << "Username: " << m_Active_user[i].user_name << "\tActive_Channel: HOME " << std::endl;
            }
            else
            {
                std::cout << "Username: " << m_Active_user[i].user_name << "\tActive_Channel: " << m_Active_user[i].active_channel->channel_name << std::endl;
            }
        }
    }
}

void Server::Add_User(std::string username, sockaddr_in address)
{
    m_Active_user.emplace_back(username, address);
}

void Server::Remove_User(sockaddr_in &address)
{
    std::string this_ip = inet_ntoa(address.sin_addr);

    m_Active_user.erase(std::remove_if(
                            m_Active_user.begin(), m_Active_user.end(),
                            [&this_ip](const User &user)
                            {
                                const std::string their_ip = inet_ntoa(user.address.sin_addr);
                                return their_ip == this_ip;
                            }),
                        m_Active_user.end());
}

void Server::send_message(std::string &message, sockaddr_in address)
{
    sendto(this->m_socket, message.c_str(), message.length(), MSG_CONFIRM, (const sockaddr *)&address, sizeof(address));
}
void Server::send_message(std::string message, sockaddr_in address)
{
    sendto(this->m_socket, message.c_str(), message.length(), MSG_CONFIRM, (const sockaddr *)&address, sizeof(address));
}
void Server::send_message(std::string &message, User user)
{
    sendto(this->m_socket, message.c_str(), message.length(), MSG_CONFIRM, (const sockaddr *)&user.address, sizeof(user.address));
}

//
// HOT FUNCTION: CALLED EVERYTIME SOMEONE WRITES INTO THE CHAT
//
void Server::broadcast_message(std::string &message)
{

    for (const User &user : m_Active_user)
    {
        sendto(this->m_socket, message.c_str(), message.length(), MSG_CONFIRM, (const sockaddr *)&user.address, sizeof(user.address));
    }
}
User *Server::get_user(std::string &username)
{
    for (User &user : m_Active_user)
    {
        if (user.user_name == username)
        {
            return &user;
        }
    }
    return nullptr;
}
User *Server::get_user(sockaddr_in addr)
{
    for (User &user : m_Active_user)
    {
        if (user.get_ip_address() == inet_ntoa(addr.sin_addr))
        {
            return &user;
        }
    }
    return nullptr;
}

Channel *Server::get_channel(std::string channel_name)
{
    for (Channel &channel : m_Channels)
    {
        if (channel.channel_name == "home")
        {
            continue;
        }
        if (channel_name == channel.channel_name)
        {
            return &channel;
        }
    }
    return nullptr;
}

void Server::Initalize_Channels()
{
    namespace fs = std::filesystem;

    std::string path = "../dep";
    size_t curr_index = 0;
    for (const auto &entry : fs::directory_iterator(path))
    {
        std::cout << "[*] PATH : " << entry.path() << " LOADED" << std::endl;
        std::string file_path = entry.path();
        std::string file_name = file_path.substr(path.length() + 1, file_path.length() - 11);

        m_Channels[curr_index] = Channel(file_name, file_path);
        m_Channels[curr_index].load_messages();
        curr_index++;
    }
}

void Server::print_all_channel()
{
    for (Channel &chan : m_Channels)
    {
        if (chan.channel_name == "home")
        {
            continue;
        }
        std::cout << "{*} PRINTING LOG OF: " << chan.channel_name << std::endl;
        chan.print_chat_log();
    }
}
