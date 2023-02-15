#include "server.h"
#include <iostream>

int main()
{
    Server serv("192.168.1.15", 7000);
    std::cout << "Server Started" << std::endl;
    serv.Initalize_Channels();
    serv.Start_Server();
}