#ifndef AGENT_H
#define AGENT_H

class World;

class Agent
{
public:
    /* actions an agent can perform */
    enum actions
    {
        moveUp,
        moveDown,
        moveLeft,
        moveRight,
        suck,
        idle
    };

    /* this function is get called when it's time for agent to look at the
     * world and make some difference */
    actions act(bool, int);
};
#endif
