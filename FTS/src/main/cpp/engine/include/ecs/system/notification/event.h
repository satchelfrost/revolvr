#pragma once

namespace rvr {
enum class EventType {
    Collision
};

class Event {
public:
    Event(EventType type);
    const EventType type;
};
}