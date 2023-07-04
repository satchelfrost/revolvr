/********************************************************************/
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*  This code is licensed under the MIT license (MIT)               */
/*  (http://opensource.org/licenses/MIT)                            */
/********************************************************************/

#include <platform/keyboard_handler.h>
#include "check.h"

#define MILLISECOND 1000000

namespace rvr {
KeyboardHandler::KeyboardHandler() : REPEAT_THRESHOLD(1000 * MILLISECOND), REPEAT_INTERVAL(300 * MILLISECOND),
MAX_KEY_CODE_QUEUE(100) {}

void KeyboardHandler::HandleKeyEvent(AInputEvent *event) {
    int32_t keyCode = AKeyEvent_getKeyCode(event);
    int32_t action = AKeyEvent_getAction(event);
    int64_t time = AKeyEvent_getEventTime(event);

    // Avoid accumulating too many characters
    if (keyCodeQueue_.size() > MAX_KEY_CODE_QUEUE)
        ClearQueue();

    // Never before seen keyCodes
    if (keyInfoMap_.find(keyCode) == keyInfoMap_.end()) {
        keyCodeQueue_.push(keyCode);
        keyInfoMap_[keyCode] = {action, time, 0, 0};
        return;
    }

    // Conditionally store keycodes
    keyInfo& keyInfo = keyInfoMap_[keyCode];
    if (action == AKEY_EVENT_ACTION_DOWN) {
        if (keyInfo.action == action) {
            int64_t diff = time - keyInfo.eventTime;
            keyInfo.downTime += diff;
            if (keyInfo.downTime >= REPEAT_THRESHOLD) {
                if (keyInfo.repeatTimer >= REPEAT_INTERVAL) {
                    keyInfo.repeatTimer = 0;
                    keyCodeQueue_.push(keyCode);
                }
                else {
                    keyInfo.repeatTimer += diff;
                }
            }
            return;
        }
        keyCodeQueue_.push(keyCode);
    }
    keyInfoMap_[keyCode] = {action, time, 0, 0};
}

bool KeyboardHandler::IsLetter(int32_t keyCode) {
    return (keyCode >= AKEYCODE_A) && (keyCode <= AKEYCODE_Z);
}

bool KeyboardHandler::IsNumber(int32_t keyCode) {
    return (keyCode >= AKEYCODE_0) && (keyCode <= AKEYCODE_9);
}

std::vector<char> KeyboardHandler::GetProcessedKeyBuffer() {
    std::vector<char> buffer;
    while (!keyCodeQueue_.empty()) {
        int32_t keyCode = Pop();
        if (IsShift(keyCode)) {
            if (IsLetter(Peek())) {
                int32_t letter = Pop();
                buffer.push_back(LetterToChar(letter, true));
            }
            else if (IsNumber(Peek())) {
                int32_t number = Pop();
                buffer.push_back(NumberToChar(number, true));
            }
            else {
                continue;
            }
        }
        else if (IsLetter(keyCode)) {
            buffer.push_back(LetterToChar(keyCode, false));
        }
        else if (IsNumber(keyCode)) {
            buffer.push_back(NumberToChar(keyCode, false));
        }
        else {
            continue;
        }
    }
    return buffer;
}

char KeyboardHandler::LetterToChar(int32_t keyCode, bool shift) {
    if (!IsLetter(keyCode)) {
        Log::Write(Log::Level::Error, Fmt("Received non-letter keycode %d in LetterToChar", keyCode));
        return '!';
    }

    int32_t offset = (shift) ? ('A' - AKEYCODE_A) : ('a' - AKEYCODE_A);
    return (char)(keyCode + offset);
}

bool KeyboardHandler::IsShift(int32_t keyCode) {
    return (keyCode == AKEYCODE_SHIFT_LEFT) || (keyCode == AKEYCODE_SHIFT_RIGHT);
}

int32_t KeyboardHandler::Pop() {
    int32_t keyCode = keyCodeQueue_.front();
    keyCodeQueue_.pop();
    return keyCode;
}

int32_t KeyboardHandler::Peek() {
    int32_t keyCode = keyCodeQueue_.front();
    return keyCode;
}

char KeyboardHandler::NumberToChar(int32_t keyCode, bool shift) {
    if (!IsNumber(keyCode)) {
        Log::Write(Log::Level::Error, Fmt("Received non-number keycode %d in NumberToChar", keyCode));
        return '!';
    }

    switch (keyCode) {
        case AKEYCODE_0: return (shift) ? ')' : '0';
        case AKEYCODE_1: return (shift) ? '!' : '1';
        case AKEYCODE_2: return (shift) ? '@' : '2';
        case AKEYCODE_3: return (shift) ? '#' : '3';
        case AKEYCODE_4: return (shift) ? '$' : '4';
        case AKEYCODE_5: return (shift) ? '%' : '5';
        case AKEYCODE_6: return (shift) ? '^' : '6';
        case AKEYCODE_7: return (shift) ? '&' : '7';
        case AKEYCODE_8: return (shift) ? '*' : '8';
        case AKEYCODE_9: return (shift) ? ')' : '9';
        default:
            THROW("Keycode was not a number");
    }
}

void KeyboardHandler::ClearQueue() {
    while (!keyCodeQueue_.empty())
        keyCodeQueue_.pop();
}
}