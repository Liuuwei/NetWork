#include "../include/TcpConnection.h"
#include "../include/Channel.h"
#include "../include/EventLoop.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <cerrno>
#include <memory>
#include <sys/epoll.h>
#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop, int fd) : channel_(std::make_shared<Channel>(loop, fd)) {

}

TcpConnection::~TcpConnection() {
    channel_->shutdown();
}

void TcpConnection::init() {
    channel_->setReadCallback(
        [this]() {
            this->read();
        }
    );

    channel_->setWriteCallback(
        [this]() {
            this->write();
        }
    );

    channel_->enableListenEvent(EPOLLIN);
}

void TcpConnection::setReadCallback(std::function<void (std::shared_ptr<TcpConnection>, Buffer *)> cb) {
    readCallback_ = std::move(cb);
}

void TcpConnection::setWriteCallback(std::function<void (std::shared_ptr<TcpConnection>, Buffer *)> cb) {
    writeCallback_ = std::move(cb);
}

void TcpConnection::read() {
    auto n = buffer_.read(channel_->fd());
    if (n == -1) {
        if (errno != EAGAIN || errno != EWOULDBLOCK) {
            closeChannel();
            return ;
        }
    } else if (n == 0) {
        channel_->shutdown();
    }

    if (readCallback_) {
        readCallback_(shared_from_this(), &buffer_);
    }
}

void TcpConnection::write() {
    if (buffer_.writeSize() == 0) {
        channel_->unableListenEvent(EPOLLOUT);
        return ;
    }
    
    auto n = buffer_.write(channel_->fd());
    if (n == -1) {
        if (errno != EACCES || errno != EWOULDBLOCK) {
            closeChannel();
            return ;
        }
    }

    if (writeCallback_) {
        writeCallback_(shared_from_this(), &buffer_);
    }
}

void TcpConnection::send(std::string msg) {
    buffer_.send(msg);
    channel_->enableListenEvent(EPOLLOUT);
}

void TcpConnection::closeChannel() {
    channel_->shutdown();
}