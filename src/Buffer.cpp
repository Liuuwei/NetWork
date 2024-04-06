#include "../include/Buffer.h"

#include <assert.h>
#include <algorithm>
#include <unistd.h>
#include <iostream>

Buffer::Buffer() {

}

std::string Buffer::retrive() {
    auto str = std::string(readBuffer_.begin(), readBuffer_.end());
    readBuffer_.clear();

    return str;
}

std::string Buffer::retrive(int n) {
    assert(n <= readBuffer_.size());

    auto str = std::string(readBuffer_.begin(), readBuffer_.begin() + n);
    readBuffer_.erase(readBuffer_.begin(), readBuffer_.begin() + n);

    return str;
}

void Buffer::append(std::string msg) {
    writeBuffer_.insert(writeBuffer_.end(), msg.begin(), msg.end());
}

int Buffer::read(int fd) {
    std::vector<char> buffer(65536);
    auto n = ::read(fd, buffer.data(), buffer.size());

    readBuffer_.insert(readBuffer_.end(), buffer.begin(), buffer.begin() + n);

    return n;
}

int Buffer::write(int fd) {
    auto n = ::write(fd, writeBuffer_.data(), writeBuffer_.size());
    if (n == -1) {
        return -1;
    }

    writeBuffer_.erase(writeBuffer_.begin(), writeBuffer_.begin() + n);

    return n;
}

void Buffer::send(std::string msg) {
    append(msg);
}

int Buffer::readSize() const {
    return readBuffer_.size();
}

int Buffer::writeSize() const {
    return writeBuffer_.size();
}