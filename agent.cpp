#include "agent.h"
#include "world.h"

void Agent::act(World *world) {
    /* Example agent does nothing */
    world->performAction(doNothing);
}
