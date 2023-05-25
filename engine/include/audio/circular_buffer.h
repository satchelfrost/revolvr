#pragma once

namespace rvr {
class CircularBuffer {
public:
    CircularBuffer(int maxSize);
    ~CircularBuffer();

    bool Enqueue(float item);
    bool Dequeue(float& item);
    float Front();
    bool isEmpty() const;
    bool isFull() const;
    int isSpaceAvailable(int amount) const;
    void Clear();

private:
    int head_ = 0;
    int tail_ = 0;
    int size_ = 0;
    int maxSize_;

    float* buffer_;
};
}