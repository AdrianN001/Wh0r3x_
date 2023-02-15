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
    while (1)
    {
        sockaddr_in cliaddr;

        socklen_t len;
        char BUFFER[MAXLINE];
        int n;

        len = sizeof(cliaddr); // len is value/result

        n = recvfrom(this->m_socket, (char *)BUFFER, MAXLINE,
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
        std::cout << "Hello message sent." << std::endl;
    }
}

void Server::process(std::string input, sockaddr_in address)
{
    if (input.substr(0, 11) == "$handshake")
    {
        std::string username = input.substr(12, input.length());
        std::cout << "[*] User joined: " << username << "\n";
        Add_User(username, address);
    }
    else if (input.substr(0, 9) == "$leaving")
    {
        std::string username = input.substr(10, input.length());
        std::cout << "[*] User left: " << username << '\n';
        Remove_User(address);
    }
    else if (input.substr(0, 9) == "$message")
    {
        std::string message = input.substr(10, input.length());
        User *author = get_user(address);

        if (author->active_channel->channel_name != HOME_CHAT)
        {
            author->active_channel->add_message(*author, message);
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

void Server::Initalize_Channels()
{
    namespace fs = std::filesystem;

    std::string path = "../dep";
    size_t curr_index = 0;
    for (const auto &entry : fs::directory_iterator(path))
    {
        std::cout << "[*] PATH : " << entry.path() << " LOADED" << std::endl;
        std::string file_path = entry.path();
        std::string file_name = file_path.substr(path.length() + 1, file_path.length() - 7);

        m_Channels[curr_index] = Channel(file_name, file_path);
        curr_index++;
    }
}