#pragma once

#include "../include/Channel.h"

#include <memory>
#include <sys/epoll.h>
#include <unordered_map>
#include <vector>

class EventLoop;

class Poll {
public:
    Poll();

    void poll();
    void processEvents(int n);
    void updateChannle(std::shared_ptr<Channel> channel);
    void removeChannle(std::shared_ptr<Channel> channel);
    void update(int operation, std::shared_ptr<Channel> channel);
private:
    EventLoop* loop;
    int epollFd_;
    std::vector<epoll_event> epollEvents_;
    std::unordered_map<int, std::weak_ptr<Channel>> channels_;

};