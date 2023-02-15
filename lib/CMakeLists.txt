add_library(server server.cpp)
add_library(chat chat.cpp)
add_library(user user.cpp)
add_library(message message.cpp)

target_include_directories(chat PUBLIC lib/inc)
target_include_directories(server PUBLIC lib/inc)
target_include_directories(user PUBLIC lib/inc)
target_include_directories(message PUBLIC lib/inc)