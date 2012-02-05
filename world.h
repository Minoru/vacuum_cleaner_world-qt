#ifndef WORLD_H
#define WORLD_H

#include <QTranslator>
#include <string>
using std::string;
#include <sstream>
using std::istringstream;
#include <fstream>
using std::ifstream;
using std::ios_base;
#include <vector>
using std::vector;
#include <cstdlib>
#include "agent.h"

class World
{
public:
    /* World should be created from the map file.
     * Pointer to parent widget is required so constructor can display some
     * warning and error dialogs */
    World(string);
    
    /* this function is called by agent at each step. It applies agent's action
     * to world and updates it (e.g. randomly adds dirt here and there) */
    void performAction(Agent::actions);
    /* returns true if there's dirt at agent's current position */
    bool isCurrentPosDirty();
    /* returns amount of dirt at agent's current position */
    int dirtAmount();
    /* returns true if agent just bumped into the wall */
    bool isJustBumped();

    QString getErrorMessage()
    {
        return errorMessage;
    }

    vector< vector<int> * > getWorld()
    {
        return world;
    }

    int getWorldWidth()
    {
        return world_width;
    }

    int getWorldHeight()
    {
        return world_height;
    }

private:
    /* World is a set of columns (vector of pointers to vectors, in fact). That
     * allows us to write quite intuitive code (e.g. world[x]->at(y)). Note,
     * though, that beginning of coordinates is in the upper left corner. Each
     * cell (which is int by the way) represents an amount of dirt in a given
     * cell. Initially it is all zeros. Amount of dirt is incremented randomly
     * at each step */
    vector< vector<int> * > world;
    int world_width, world_height;
    /* agent's current position */
    int agentPosX, agentPosY;
    /* true if agent just bumped into the wall */
    bool justBumped;
    /* probability of cell's dirty level increasing by one */
    float dirtyProbability;

    /* how obstacles are indicated at map */
    static const int OBSTACLE = -1;

    static const char MAP_OBSTACLE = 'O',
                      MAP_ROAD = '-';

    QString errorMessage;
};
#endif
