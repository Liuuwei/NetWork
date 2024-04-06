#include "../include/EventLoop.h"
#include "../include/Poll.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>

EventLoop::EventLoop() : threadId_(gettid()) {
    poll_ = std::make_shared<Poll>();
    
    int wakeFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    wakeChannel_ = std::make_shared<Channel>(this, wakeFd);
    
    wakeChannel_->setReadCallback(
        [this]() {this->wake();
    });
    wakeChannel_->enableListenEvent(EPOLLIN);
}

void EventLoop::loop() {
    while (true) {
        poll_->poll();
        processFunctions();
    }
}

void EventLoop::processFunctions() {
    std::vector<std::function<void()>> functions;

    std::unique_lock<std::mutex> lock(functionMutex_);
    functions.swap(functions_);
    lock.unlock();

    for (auto& cb : functions) {
        cb();
    }
}

void EventLoop::runInLoop(std::function<void ()> cb) {
    std::unique_lock<std::mutex> lock(functionMutex_);
    functions_.push_back(std::move(cb));
    lock.unlock();

    if (!inLoopThread()) {
        wake();
    }
}

void EventLoop::updateInLoop(std::shared_ptr<Channel> channel) {
    if (inLoopThread()) {
        poll_->updateChannle(channel);
    } else {
        runInLoop(
            [this, channel]() {
                poll_->updateChannle(channel);
            }
        );
    }
}

bool EventLoop::inLoopThread() {
    return gettid() == threadId_;
}

void EventLoop::wake() {
    int64_t one = 1;
    auto n = write(wakeChannel_->fd(), &one, sizeof(one));
}

void EventLoop::wakeCallback() {
    int64_t one = 1;
    auto n = read(wakeChannel_->fd(), &one, sizeof(one));
}