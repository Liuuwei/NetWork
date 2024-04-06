#pragma once

#include <functional>
#include <memory>

class EventLoop;

class Channel : public std::enable_shared_from_this<Channel> {
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void update();
    void executeIO();
    void enableListenEvent(int events);
    void unableListenEvent(int events);
    void enableTriggerEvent(int events);
    void unableTriggerEvent(int events);
    void setReadCallback(std::function<void()> cb);
    void setWriteCallback(std::function<void()> cb);
    int fd() const;
    int events() const;
    void shutdown();
private:
    EventLoop* loop_ = nullptr;
    int fd_ = 0;
    int listenEvents_ = 0;
    int triggerEvents_ = 0;

    std::function<void()> readCallback_{};
    std::function<void()> writeCallbacl_{};
};