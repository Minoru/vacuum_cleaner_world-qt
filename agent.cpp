#include "agent.h"
#include "world.h"

void Agent::act(World *world) {
    /* Example agent moves randomly sucking as much dirt as possible */
    if(world->isCurrentPosDirty()) {
        world->performAction(suck);
    } else {
        world->performAction(static_cast<actions> (rand() % 4));
    };
}
