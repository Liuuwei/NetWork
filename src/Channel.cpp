#include "../include/Channel.h"
#include "../include/EventLoop.h"

#include <sys/epoll.h>
#include <iostream>

Channel::Channel(EventLoop* loop, int fd) : loop_(loop), fd_(fd) {

}

Channel::~Channel() {
    
}

void Channel::update() {
    loop_->updateInLoop(shared_from_this());
}

void Channel::enableListenEvent(int events) {
    listenEvents_ |= events;
    update();
}

void Channel::unableListenEvent(int events) {
    listenEvents_ &= ~events;
    update();
}

void Channel::enableTriggerEvent(int events) {
    triggerEvents_ |= events;
    update();
}

void Channel::unableTriggerEvent(int events) {
    triggerEvents_ &= ~events;
    update();
}

void Channel::setReadCallback(std::function<void ()> cb) {
    readCallback_ = std::move(cb);
}

void Channel::setWriteCallback(std::function<void ()> cb) {
    writeCallbacl_ = std::move(cb);
}

int Channel::fd() const {
    return fd_;
}

int Channel::events() const {
    return listenEvents_;
}

void Channel::executeIO() {
    if ((triggerEvents_ & EPOLLIN) == EPOLLIN) {
        if (readCallback_) {
            readCallback_();
        }
    }
    if ((triggerEvents_ & EPOLLOUT) == EPOLLOUT) {
        if (writeCallbacl_) {
            writeCallbacl_();
        }
    }
}

void Channel::shutdown() {
    listenEvents_ = 0;
    update();
}