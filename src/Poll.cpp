#include "../include/Poll.h"
#include <sys/epoll.h>
#include <iostream>
#include "../include/Channel.h"

Poll::Poll() : epollFd_(epoll_create(1)), epollEvents_(65536) {

}

void Poll::poll() {
    int n = epoll_wait(epollFd_, epollEvents_.data(), epollEvents_.size(), -1);
    processEvents(n);
}

void Poll::processEvents(int n) {
    for (int i = 0; i < n; i++) {
        int fd = epollEvents_[i].data.fd;
        auto channle = channels_[fd].lock();
        if (channle) {
            channle->enableTriggerEvent(epollEvents_[i].events);
            channle->executeIO();
            channle->unableTriggerEvent(epollEvents_[i].events);
        }
    }
}

void Poll::updateChannle(std::shared_ptr<Channel> channel) {
    int operation = EPOLL_CTL_MOD;

    if (!channels_.count(channel->fd())) {
        channels_.insert({channel->fd(), channel});
        operation = EPOLL_CTL_ADD;
    }
    
    update(operation, channel);
}

void Poll::removeChannle(std::shared_ptr<Channel> channel) {
    if (channels_.find(channel->fd()) != channels_.end()) {
        channels_.erase(channel->fd());
    }

    update(EPOLL_CTL_DEL, channel);
}

void Poll::update(int operation, std::shared_ptr<Channel> channel) {
    epoll_event event{};
    event.data.fd = channel->fd();
    event.events= channel->events();

    epoll_ctl(epollFd_, operation, channel->fd(), &event);
}