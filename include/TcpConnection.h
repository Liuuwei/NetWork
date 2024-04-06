#pragma once

#include "../include/Buffer.h"

#include <functional>
#include <memory>

class EventLoop;
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop, int fd);
    ~TcpConnection();
    void init();
    void setReadCallback(std::function<void(std::shared_ptr<TcpConnection>, Buffer*)>);
    void setWriteCallback(std::function<void(std::shared_ptr<TcpConnection>, Buffer*)>);
    void send(std::string msg);
private:
    void read();
    void write();
    void closeChannel();

    int fd_;
    std::shared_ptr<Channel> channel_;
    Buffer buffer_;

    std::function<void(std::shared_ptr<TcpConnection>, Buffer*)> readCallback_;
    std::function<void(std::shared_ptr<TcpConnection>, Buffer*)> writeCallback_;
};