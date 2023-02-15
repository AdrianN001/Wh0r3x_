#include "server.h"
#include "thread"
#include <iostream>

int main()
{
    Server serv("192.168.1.22", 7000);
    std::cout << "Server Started" << std::endl;
    serv.Initalize_Channels();
    serv.print_all_channel();
    std::cout << "ENDED" << std::endl;

    serv.Start_Server();
    // serv.Start_Server();
}