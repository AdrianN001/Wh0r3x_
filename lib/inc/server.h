#pragma once
#include "user.h"
#include "message.h"
#include "chat.h"
#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <algorithm>
#include <array>

#define MAXLINE 1024
#define CHAT_ROOMS_NUMBER 10u

class Server
{

private:
    int m_port;
    std::string m_ip_address;
    int m_socket;

private:
    std::vector<User> m_Active_user;
    std::array<Channel, CHAT_ROOMS_NUMBER> m_Channels; // vector isn't safe because User holds a pointer to the channel and vector's realloc would break it

private:
    void Add_User(std::string username, sockaddr_in address);
    void Remove_User(std::string &username);
    void Remove_User(sockaddr_in &address);

private:
    User *get_user(std::string &username);
    User *get_user(sockaddr_in addr);
    Channel *get_channel(std::string channel_name);

public:
    Server(std::string ip_address, int port);
    void Start_Server();
    void Initalize_Channels();
    void print_all_channel();

    ~Server();

private:
    void process(std::string input, sockaddr_in address);
    void send_message(std::string &message, sockaddr_in address);
    void send_message(std::string message, sockaddr_in address);

    void send_message(std::string &message, User user);
    void broadcast_message(std::string &message);
};