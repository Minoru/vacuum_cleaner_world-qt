#include "world.h"

World::World(std::string filename) {
    /* we can be sure filename is valid - GUI checked it */

    /* world doesn't even exist yet, so agent couldn't possibly bump into
     * something */
    justBumped = false;

    /* read map file and initialise world */
}

void World::performAction(Agent::actions action) {
    switch(action) {
        default:
            /* do nothing */
            break;
    }
}

bool World::isCurrentPosDirty() {
    return (world[agentPosX][agentPosY] > 0);
}

int World::dirtAmount() {
    return world[agentPosX][agentPosY];
}

bool World::isJustBumped() {
    return justBumped;
}
