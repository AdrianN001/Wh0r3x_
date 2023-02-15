add_executable(a.out main.cpp)

target_link_libraries(a.out server)
target_link_libraries(a.out chat)
target_link_libraries(a.out message)
target_link_libraries(a.out user)
target_include_directories(a.out PUBLIC ../lib/inc)