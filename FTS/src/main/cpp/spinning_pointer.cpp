#include "include/spinning_pointer.h"

SpinningPointer::SpinningPointer(int id) : RVRMesh(id) {
    scale = {0.01, 0.01, 0.5};
    turnAmt_ = 0;
    turnSpeed_ = 3;
    transDirection_ = 1;

    // Disable the update in order to not be annoying.
    // Comment out if you want to see this object in action.
    canUpdate = false;
}

void SpinningPointer::Begin() {

}

void SpinningPointer::Update(float delta) {
    // Rotate by the turn speed every frame
    turnAmt_ = (turnAmt_ + turnSpeed_) % 360;
    float angle = 3.14f * (float)turnAmt_ / 180.0f;
    pose.orientation = XrQuaternionf_CreateFromVectorAngle({0, 1, 0}, angle);

    // Frame-rate independent translation back and forth
    float transAmt = pose.position.z + 0.1f * delta * (float)transDirection_;
    pose.position.z = transAmt;
    if (abs(transAmt) > 0.25)
        transDirection_ *= -1;
}
