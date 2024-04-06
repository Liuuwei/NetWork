#pragma once

#include "Buffer.h"
#include "TcpConnection.h"
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

class EventLoop;
class Channel;

class TcpServer {
public:
    TcpServer(EventLoop* loop, std::string ip, int port);
    void start();
    void setOnConnection(std::function<void(std::shared_ptr<TcpConnection>)> cb);
    void setOnMessage(std::function<void(std::shared_ptr<TcpConnection>, Buffer*)> cb);
    void setOnWrite(std::function<void(std::shared_ptr<TcpConnection>, Buffer*)> cb);
private:
    int listenChannelReadCallback();
    int acceptNewConnection();
    int initListen(const std::string& ip, int port);

    EventLoop* loop_;
    int listenFd_;
    std::shared_ptr<Channel> listenChannel_;
    std::unordered_map<int, std::shared_ptr<TcpConnection>> tcpConnections_;

    std::function<void(std::shared_ptr<TcpConnection>)> onConnection_;
    std::function<void(std::shared_ptr<TcpConnection>, Buffer*)> onMessage_;
    std::function<void(std::shared_ptr<TcpConnection>, Buffer*)> onWrite_; 
};