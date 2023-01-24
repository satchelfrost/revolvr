#include <ecs/system/notification/subject.h>
#include <common.h>

namespace rvr {
void Subject::AddObserver(Ritual* ritual) {
    ritual_observers_.push_back(ritual);
}

void Subject::RemoveObserver(Ritual* ritual) {
    ritual_observers_.remove(ritual);
}

void Subject::Notify(Event* event) {
    bool itrInvalidated = false;
    // Check for invalidation of iterator, since,
    // OnNotify() may result in a call to RemoveObserver()
    while (!itrInvalidated) {
        int originalSize = ritual_observers_.size();
        for (auto ritual : ritual_observers_) {
            if (originalSize != ritual_observers_.size()) {
                itrInvalidated = true;
                break;
            }
            ritual->OnNotify(event);
        }

        // If iterator was invalidated, loop again
        itrInvalidated = !itrInvalidated;
    }
}
}
