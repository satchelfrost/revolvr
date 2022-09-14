#include "objects/rvr_hand.h"

RVRHand::RVRHand(int id, int side) : RVRSpatial(id, RVRType::Hand), side_(side) {}