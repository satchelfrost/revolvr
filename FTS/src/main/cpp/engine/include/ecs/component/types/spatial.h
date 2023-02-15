#pragma once

#include <ecs/component/component.h>
#include <math/transform.h>

namespace rvr {
class Spatial : public Component {
public:
    Spatial(type::EntityId pId);

    // TODO: Make local and world transforms private
    // TODO: Move the UpdateWorld function out of SpatialSystem
    // TODO: Add convenience functions to Spatial
    math::Transform local;
    math::Transform world;
};
}