#pragma once

#include <vector>
#include <string>

class Buffer {
public:
    Buffer();
    std::string retrive();
    std::string retrive(int n);
    void append(std::string str);
    int read(int fd);
    int write(int fd);
    int readSize() const;
    int writeSize() const;
    void send(std::string msg);
private:
    std::vector<char> readBuffer_;
    std::vector<char> writeBuffer_; 
};