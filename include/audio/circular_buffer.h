/***************************************************************************/
/* Copyright (c) 2022-present RevolVR Engine contributors (see authors.md) */
/* This code is licensed under the MIT license (MIT)                       */
/* (http://opensource.org/licenses/MIT)                                    */
/***************************************************************************/

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