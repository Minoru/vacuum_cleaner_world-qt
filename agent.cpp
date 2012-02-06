#include "agent.h"
#include "world.h"

Agent::actions Agent::act(bool isBumped, int dirtAmount)
{
    /* Example agent moves randomly sucking as much dirt as possible */
    if(dirtAmount > 0)
    {
        return suck;
    }
    else
    {
        return (static_cast<actions> (rand() % 4));
    };
}
