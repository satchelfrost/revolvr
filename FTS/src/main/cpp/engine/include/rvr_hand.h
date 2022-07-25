#pragma once

#include "rvr_spatial.h"

class RVRHand : public RVRSpatial {
public:
    RVRHand(int id, int side);

    int GetSide() {return side_;}
private:
    int side_; // 0 left, 1 right
};