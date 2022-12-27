#pragma once

#include <ecs/component/types/ritual.h>
#include <ecs/entity/entity.h>

namespace rvr {
class Subject {
public:
    Subject();
    void AddObserver(Ritual* ritual);
    void RemoveObserver(Ritual* ritual);
    void Notify(Event* event);

private:
    std::list<Ritual*> ritual_observers_;
    std::vector<Ritual*> observers_to_delete_;
    bool notifying;
};
}