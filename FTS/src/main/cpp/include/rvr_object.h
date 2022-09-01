#pragma once

#include "pch.h"
#include "rvr_type.h"

class RVRObject {
public:
    RVRObject(int id, RVRType type);
    virtual ~RVRObject(){}
    void AddChild(RVRObject* child);
    void SetParent(RVRObject* parent);
    RVRObject* GetParent() { return parent_; }
    std::list<RVRObject*>& GetChildren() { return children_; }
    void Destroy();

    void SetName(std::string name);
    std::string GetName();

    const int id;
    int weakParentId;
    const RVRType type;
    bool canUpdate;

    virtual void Begin() = 0;
    virtual void Update(float delta) = 0;

private:
    void RemoveFromParent();
    void GenerateAndSetName();
    RVRObject* parent_;
    std::list<RVRObject*> children_;
    std::string name_;
};