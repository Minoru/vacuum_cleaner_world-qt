#ifndef WORLD_H
#define WORLD_H

#include <string>
#include "agent.h"

class World
{
public:
    /* World should be created from the map file */
    World(std::string);
    
    /* this function is called by agent at each step. It applies agent's action
     * to world and updates it (e.g. randomly adds dirt here and there) */
    void performAction(Agent::actions);
    /* returns true if there's dirt at agent's current position */
    bool isCurrentPosDirty();
    /* returns amount of dirt at agent's current position */
    int dirtAmount();
    /* returns true if agent just bumped into the wall */
    bool isJustBumped();

private:
    /* World is an array of world_width pointers to arrays of world_height ints
     * each (an array of columns). That enables us to write more intuitive code
     * (e.g. world[x][y]). Each int represents an amount of dirt in a given
     * cell. Initially it is all zeros. Amount of dirt is incremented randomly
     * at each step */
    int** world;
    int world_width, world_height;
    /* agent's current position */
    int agentPosX, agentPosY;
    /* true if agent just bumped into the wall */
    bool justBumped;

    /* how obstacles are indicated at map */
    static const int OBSTACLE = -1;
};
#endif
