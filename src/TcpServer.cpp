#include "../include/TcpServer.h"
#include "../include/EventLoop.h"
#include "../include/TcpConnection.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <memory>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>
#include <iostream>

TcpServer::TcpServer(EventLoop* loop, std::string ip, int port) : loop_(loop) {
    signal(SIGPIPE, SIG_IGN);
    listenFd_ = initListen(ip, port);
    listenChannel_ = std::make_shared<Channel>(loop, listenFd_);
}

void TcpServer::start() {
    listenChannel_->setReadCallback(
        [this]() {
            this->listenChannelReadCallback();
        }
    );
    listenChannel_->enableListenEvent(EPOLLIN);
}

void TcpServer::setOnConnection(std::function<void (std::shared_ptr<TcpConnection>)> cb) {
    onConnection_ = std::move(cb);
}

void TcpServer::setOnMessage(std::function<void (std::shared_ptr<TcpConnection>, Buffer*)> cb) {
    onMessage_ = std::move(cb);
}

void TcpServer::setOnWrite(std::function<void (std::shared_ptr<TcpConnection>, Buffer *)> cb) {
    onWrite_ = std::move(cb);
}

int TcpServer::listenChannelReadCallback() {
    int fd = acceptNewConnection();
    if (fd != -1) {
        auto tcpConnection = std::make_shared<TcpConnection>(loop_, fd);
        tcpConnection->setReadCallback(onMessage_);
        tcpConnection->setWriteCallback(onWrite_);
        tcpConnection->init();
        
        tcpConnections_.insert({fd, tcpConnection});

        if (onConnection_) {
            onConnection_(tcpConnection);
        }

        return fd;
    }

    return -1;
}

int TcpServer::acceptNewConnection() {
    int fd = accept4(listenFd_, nullptr, 0, SOCK_NONBLOCK);

    return fd;
}

int TcpServer::initListen(const std::string& ip, int port) {
    sockaddr_in addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, O_NONBLOCK);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)(&opt), sizeof(opt));

    int ret = bind(fd, (sockaddr*)(&addr), sizeof(addr));
    assert(ret != -1);

    ret = listen(fd, 1024);
    assert(ret != -1);

    return fd;
}