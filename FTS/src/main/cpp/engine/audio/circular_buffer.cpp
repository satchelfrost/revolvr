#include <audio/circular_buffer.h>
#include <common.h>

namespace rvr {
CircularBuffer::CircularBuffer(int maxSize) : maxSize_(maxSize) {
    buffer_ = new float[maxSize];
}

CircularBuffer::~CircularBuffer() {
    delete[] buffer_;
}

bool CircularBuffer::Enqueue(float item) {
    if (isFull())
        return false;

    buffer_[tail_] = item;
    tail_ = (tail_ + 1) % maxSize_;
    size_++;
    return true;
}

bool CircularBuffer::Dequeue(float& item) {
    if (isEmpty())
        return false;

    item = buffer_[head_];
    head_= (head_ + 1) % maxSize_;
    size_--;
    return true;
}

float CircularBuffer::Front() {
    return buffer_[head_];
}

bool CircularBuffer::isEmpty() const {
    return size_ == 0;
}

bool CircularBuffer::isFull() const {
    return size_ == maxSize_;
}

int CircularBuffer::isSpaceAvailable(int amount) const {
    return amount < (maxSize_ - size_);
}

void CircularBuffer::Clear() {
    head_ = 0;
    tail_ = 0;
    size_ = 0;
}
}