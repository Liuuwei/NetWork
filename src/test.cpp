#include <iostream>
#include <memory>

#include "../include/TcpServer.h"
#include "../include/EventLoop.h"

void onConnection(std::shared_ptr<TcpConnection> conn) {
    std::cout << "On Connection" << std::endl;
}

void onMessage(std::shared_ptr<TcpConnection> conn, Buffer* buffer) {
    auto msg = buffer->retrive();
    std::cout << "msg: " << msg << std::endl;
    conn->send(msg);
}

int main() {
    EventLoop loop;
    TcpServer server(&loop, "127.0.0.1", 9999);
    server.setOnConnection(onConnection);
    server.setOnMessage(onMessage);
    server.start();
    loop.loop();
}