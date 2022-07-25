#pragma once

#include "pch.h"
#include "rvr_type.h"

class RVRObject {
public:
    RVRObject(int id, RVRType type);
    void AddChild(RVRObject* child);
    void SetParent(RVRObject* parent);
    RVRObject* GetParent() { return parent_; }
    std::list<RVRObject*>& GetChildren() { return children_; }


    const int id;
    const RVRType type;
    bool canUpdate;

    virtual void Begin() = 0;
    virtual void Update(float delta) = 0;

private:
    RVRObject* parent_;
    std::list<RVRObject*> children_;
};