#ifndef RP_PHYSICS_RPH
#define RP_PHYSICS_RPH

#include "rp_grid_tools.h"

typedef struct
{
    // Basic physical properties for all standard (visible) game entities
    float externForce;
    float mass;
    float veloc;
    float accel;
    Vec2 pos;
}RigidBody;

typedef struct
{
    RigidBody basePhysics;
}PlayerPhysics;

#endif
