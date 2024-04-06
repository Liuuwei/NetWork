#pragma once

#include "../include/Channel.h"

#include <functional>
#include <memory>
#include <vector>
#include <mutex>

class Poll;

class EventLoop {
public:
    EventLoop();
    void loop();
    void processFunctions();
    void runInLoop(std::function<void()> cb);
    void updateInLoop(std::shared_ptr<Channel> channel);
    bool inLoopThread();
private:
    void wake();
    void wakeCallback();

    int threadId_ = 0;
    std::shared_ptr<Poll> poll_;
    std::shared_ptr<Channel> wakeChannel_{};
    std::mutex functionMutex_;
    std::vector<std::function<void()>> functions_;
};