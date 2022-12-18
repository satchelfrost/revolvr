#include <common.h>
#include "ecs/component/types/ritual.h"

namespace rvr {
Ritual::Ritual() : Component(ComponentType::Ritual), canUpdate(true), id(-1) {}

void Ritual::Begin() const {
    if (impl_ != nullptr)
        impl_->Begin();
    else
        THROW("[Ritual::Begin() failed] - Ritual has no implementation");
}

void Ritual::Update(float delta) const {
    if (impl_ != nullptr)
        impl_->Update(delta);
    else
        THROW("[Ritual::Update() failed] - Ritual has no implementation");
}

void Ritual::SetImplementation(RitualBehavior *impl) {
    if (impl_ == nullptr)
        impl_ = impl;
    else
        THROW("[Ritual::SetImplementation failed] - Ritual already has implementation");
}

Ritual::~Ritual() {
    delete impl_;
}

bool Ritual::HasImpl() {
    return impl_ != nullptr;
}
}