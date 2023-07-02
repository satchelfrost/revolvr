/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/

#pragma once

#include <pch.h>
#include <queue>

namespace rvr {
struct keyInfo {
    int32_t action;
    int64_t eventTime;
    int64_t downTime;
    int64_t repeatTimer;
};

class KeyboardHandler{
public:
    KeyboardHandler();
    void HandleKeyEvent(AInputEvent* event);
    std::vector<char> GetProcessedKeyBuffer();

private:
    static bool IsLetter(int32_t keyCode);
    static bool IsNumber(int32_t keyCode);
    static bool IsShift(int32_t keyCode);
    static char LetterToChar(int32_t keyCode, bool shift);
    static char NumberToChar(int32_t keyCode, bool shift);
    void ClearQueue();
    int32_t Peek();
    int32_t Pop();

    std::map<int32_t, keyInfo> keyInfoMap_;
    std::queue<int32_t> keyCodeQueue_;
    const int64_t REPEAT_THRESHOLD;
    const int64_t REPEAT_INTERVAL;
    const int32_t MAX_KEY_CODE_QUEUE;
};
}