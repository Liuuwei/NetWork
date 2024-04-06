add_library(NetWork
Buffer.cpp
Channel.cpp
EventLoop.cpp
Poll.cpp
TcpConnection.cpp
TcpServer.cpp
)

add_executable(Test test.cpp)
target_link_libraries(Test NetWork)