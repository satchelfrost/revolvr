#pragma once

#include "rvr_object.h"

class MyRVRObject : public RVRObject {
public:
    MyRVRObject(int id);
    virtual void Begin() override;
    virtual void Update(float delta) override;
};