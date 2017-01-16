#include "world.h"

#ifdef _MSC_VER
/* Windows doesn't have rand_r() so provide an implementation from FreeBSD libc
 * which is distributed under the following terms:
 *
 * Copyright (c) 1990, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Posix rand_r function added May 1999 by Wes Peters <wes@softweyr.com>.
 */

static int do_rand(unsigned long *ctx)
{
    /*
    * Compute x = (7^5 * x) mod (2^31 - 1)
    * without overflowing 31 bits:
    *      (2^31 - 1) = 127773 * (7^5) + 2836
    * From "Random number generators: good ones are hard to find",
    * Park and Miller, Communications of the ACM, vol. 31, no. 10,
    * October 1988, p. 1195.
    */
    long hi, lo, x;

    /* Can't be initialized with 0, so use another value. */
    if (*ctx == 0)
            *ctx = 123459876;
    hi = *ctx / 127773;
    lo = *ctx % 127773;
    x = 16807 * lo - 2836 * hi;
    if (x < 0)
            x += 0x7fffffff;
    return ((*ctx = x) % ((unsigned long)RAND_MAX + 1));
}

int rand_r(unsigned int *ctx)
{
    unsigned long val = (unsigned long) *ctx;
    int r = do_rand(&val);

    *ctx = (unsigned int) val;
    return (r);
}
#endif

World::World(string filename)
{
    /* we can be sure filename is valid - GUI checked it */

    /* world doesn't even exist yet, so agent couldn't possibly bump into
     * something */
    justBumped = false;
    /* ...and of course time doesn't tick yet */
    currentTime = 0;

    /* read map file and initialise world */
    string line;
    ifstream mapFile(filename.c_str());
    getline(mapFile, line);
    while (line.substr(0,2).compare("//") == 0 || line.empty()
            || line.find_first_not_of(' ') == string::npos)
    {
        /* First lines in a file may be:
         * *  C++-style comments;
         * *  just empty lines (may contain spaces).
         * We'll just ignore this stuff. */
        getline(mapFile, line);

        if(mapFile.eof())
        {
            // FIXME: display error message here
            break;
        }
    }
    /* Invariant: comment block is passed, 'line' should contain initial agent
     * position, dirty probability and random seed now. */

    istringstream parameters(line);
    /* Throw an exception if:
     * (eofbit) end of file reached
     * (badbit) some other error occured */
    parameters.exceptions(ifstream::eofbit | ifstream::badbit);
    try
    {
        parameters >> agentPosX >> agentPosY >> dirtyProbability; 
    }
    /* We're really only interested in catching ios_base::failure here, but GCC
     * 5 has a bug so we're working around it. See
     * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66145 for details.
     *
     * The same applies to all catch blocks in this constructor.
     */
    catch (std::exception f)
    {
        if(parameters.eof())
            errorMessage = QTranslator::tr("End of line reached while reading initial parameters");
        else
            errorMessage = f.what();

        return;
    };
    initialAgentPosX = agentPosX;
    initialAgentPosY = agentPosY;
    unsigned int mapSeed;
    try
    {
        parameters >> mapSeed;
    }
    catch (std::exception f)
    {
        /* It's okay to catch eof here - moreover, it is *expected*, because
         * seed is the last number on the parameters line */
        if(! parameters.eof())
        {
            errorMessage = QTranslator::tr(f.what());

            return;
        }
    };
    /* we use this.seed to keep world's random generator separate from
     * everything else */
    seed = mapSeed;

    /* still, we need to initialize random generator for others */
    srand(time(NULL));

    try
    {
        parameters >> seed;
    }
    catch (std::exception f)
    {
        //TODO: decide keep this warning or not
        /*if(! parameters.eof())
            QMessageBox::warning(parent, QTranslator::tr("Malformed map"),
                QTranslator::tr("seed wasn't the last value on the parameters line"));*/
    }

    /* Finally, it's time to read the map itself */
    /* Initializing lineno to -1 because we didn't read first line (which must
     * have index zero) yet */
    int lineno = -1;
    while(! mapFile.eof())
    {
        lineno++;
        getline(mapFile, line);
        istringstream map(line);
        /* Throw an exception when eof reached */
        map.exceptions(ifstream::eofbit);
        int columnno = 0;
        while(! map.eof())
        {
            char c;
            int val;
            try
            {
                map >> c;
            }
            catch (std::exception f)
            {
                if(map.eof())
                    break;
            };
            switch(c)
            {
                case MAP_OBSTACLE:
                    val = OBSTACLE;
                    break;

                case MAP_ROAD:
                    val = 0;
                    break;

                default:
                    errorMessage = QTranslator::tr("Unknown char in map at line %1 : %2").arg(lineno).arg(c);

                    return;
            }
            if(lineno == 0)
            {
                /* It's the first iteration, so we need to allocate space for
                 * columns */
                world.push_back(new vector<int>);
                world.back()->push_back(val);
            }
            else
            {
                /* columnno can't be equal to world_width because columnno
                 * counts from zero */
                if(columnno >= world_width)
                {
                    errorMessage = QTranslator::tr("Line %1 is wider than others (should be %2)").arg(lineno).arg(world_width);

                    return;
                }
                else
                {
                    world[columnno]->push_back(val);
                }
            }
            columnno++;
        }
        if(lineno == 0)
        {
            /* First iteration is over, now we know how wide map is */
            world_width = world.size();
        }
    }
    world_height = lineno;

    mapFile.close();

    lastAgentAction = Agent::idle;

    errorMessage = QString();
    consumedEnergy = 0;
    dirtyDegree = 0;
    currentTime = 0;

    agent = new Agent();
}

void World::performAction(Agent::actions action)
{
    justBumped = false;

    switch(action)
    {
        case Agent::moveLeft:
            if(agentPosX == 0 || agentPosX == world_width-1
                    || world[agentPosX-1]->at(agentPosY) == OBSTACLE)
            {
                justBumped = true;
            }
            else
            {
                agentPosX--;
            };
            consumedEnergy++;
            break;

        case Agent::moveRight:
            if(agentPosX == 0 || agentPosX == world_width-1
                    || world[agentPosX+1]->at(agentPosY) == OBSTACLE)
            {
                justBumped = true;
            }
            else
            {
                agentPosX++;
            };
            consumedEnergy++;
            break;

        case Agent::moveUp:
            if(agentPosY == 0 || agentPosY == world_height-1
                    || world[agentPosX]->at(agentPosY-1) == OBSTACLE)
            {
                justBumped = true;
            }
            else
            {
                agentPosY--;
            };
            consumedEnergy++;
            break;

        case Agent::moveDown:
            if(agentPosY == 0 || agentPosY == world_height-1
                    || world[agentPosX]->at(agentPosY+1) == OBSTACLE)
            {
                justBumped = true;
            }
            else
            {
                agentPosY++;
            };
            consumedEnergy++;
            break;

        case Agent::suck:
            if(world[agentPosX]->at(agentPosY) > 0)
            {
                world[agentPosX]->at(agentPosY)--;
            };
            consumedEnergy += 2;
            break;

        case Agent::idle:
            /* that one is easy */
            break;

        default:
            /* we can't possibly reach that state because of static compiler
             * checks, so it's safe to do nothing */
            break;
    }

    lastAgentAction = action;

    /* Run through the map and add some dirt randomly */
    for(int col = 0; col < world_width; col++)
        for(int row = 0; row < world_height; row++)
        {
            if(world[col]->at(row) != OBSTACLE)
            {
                //TODO: replace rand_r() with platform-independent function

                if(static_cast<double>(rand_r(&seed))/RAND_MAX
                        < dirtyProbability)
                    world[col]->at(row)++;

                int dirt = world[col]->at(row);
                dirtyDegree += dirt * dirt;
            }
        }

    /* Yet another corn in the sandglass... */
    currentTime++;
}

void World::doOneStep()
{
    performAction(agent->act(justBumped, world[agentPosX]->at(agentPosY), lastAgentAction));
}

void World::resetMap()
{
    /* clean all cells */
    for(int col = 0; col < world_width; col++)
        for(int row = 0; row < world_height; row++)
            if(world[col]->at(row) != OBSTACLE)
                world[col]->at(row) = 0;

    /* reset agent's position */
    agentPosX = initialAgentPosX;
    agentPosY = initialAgentPosY;

    lastAgentAction = Agent::idle;

    currentTime = 0;
    justBumped = 0;
    dirtyDegree = 0;
    consumedEnergy = 0;

    delete agent;
    agent = new Agent();
}

