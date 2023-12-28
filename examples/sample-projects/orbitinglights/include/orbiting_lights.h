#pragma once

#include <ecs/component/types/ritual.h>
#include <ecs/component/types/spatial.h>

class OrbitingLights : public rvr::Ritual {
    static const int NUM_LIGHTS = 10;
    rvr::Spatial* lightSpatials_[NUM_LIGHTS]{};

    float period_ = 8.0f;
    float currentTime_ = 0.0f;
    float apoapsis_ = 3.0f;  //https://en.wikipedia.org/wiki/Apsis
    float periapsis_ = 0.7f; // https://en.wikipedia.org/wiki/Apsis
    float semiMajorAxis_;
    float semiMinorAxis_;
    float eccentricity_;

    // Mean anomaly (M) and eccentric anomaly (E), and eccentricity (e)
    // see https://en.wikipedia.org/wiki/Kepler%27s_equation for details
    static float KeplerEquation(float M , float E, float e) ;
    static float KeplerDifferentiated(float E, float e) ;
    static float SolveKepler(float M, float e);
public:
    OrbitingLights(rvr::type::EntityId id);
    virtual void Update(float delta) override;
};
