#include "world.h"

World::World(string filename) {
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
            || line.find_first_not_of(' ') == string::npos) {
        /* First lines in a file may be:
         * *  C++-style comments;
         * *  just empty lines (may contain spaces).
         * We'll just ignore this stuff. */
        getline(mapFile, line);

        if(mapFile.eof()) {
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
    try {
        parameters >> agentPosX >> agentPosY >> dirtyProbability; 
    } catch (ios_base::failure f) {
        if(parameters.eof())
            errorMessage = QTranslator::tr("End of line reached while reading initial parameters");
        else
            errorMessage = f.what();

        return;
    };
    initialAgentPosX = agentPosX;
    initialAgentPosY = agentPosY;
    unsigned int mapSeed;
    try {
        parameters >> mapSeed;
    } catch (ios_base::failure f) {
        /* It's okay to catch eof here - moreover, it is *expected*, because
         * seed is the last number on the parameters line */
        if(! parameters.eof()) {
            errorMessage = QTranslator::tr(f.what());

            return;
        }
    };
    /* we use this.seed to keep world's random generator separate from
     * everything else */
    seed = mapSeed;

    /* still, we need to initialize random generator for others
     * Using double initialization to make world's and other's generators go
     * different ways */
    srand(seed);
    srand(rand());

    try {
        parameters >> seed;
    } catch (ios_base::failure f) {
        //TODO: decide keep this warning or not
        /*if(! parameters.eof())
            QMessageBox::warning(parent, QTranslator::tr("Malformed map"),
                QTranslator::tr("seed wasn't the last value on the parameters line"));*/
    }

    /* Finally, it's time to read the map itself */
    /* Initializing lineno to -1 because we didn't read first line (which must
     * have index zero) yet */
    int lineno = -1;
    while(! mapFile.eof()) {
        lineno++;
        getline(mapFile, line);
        istringstream map(line);
        /* Throw an exception when eof reached */
        map.exceptions(ifstream::eofbit);
        int columnno = 0;
        while(! map.eof()) {
            char c;
            int val;
            try {
                map >> c;
            } catch (ios_base::failure f) {
                if(map.eof())
                    break;
            };
            switch(c) {
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
            if(lineno == 0) {
                /* It's the first iteration, so we need to allocate space for
                 * columns */
                world.push_back(new vector<int>);
                world.back()->push_back(val);
            } else {
                /* columnno can't be equal to world_width because columnno
                 * counts from zero */
                if(columnno >= world_width) {
                    errorMessage = QTranslator::tr("Line %1 is wider than others (should be %2)").arg(lineno).arg(world_width);

                    return;
                } else {
                    world[columnno]->push_back(val);
                }
            }
            columnno++;
        }
        if(lineno == 0) {
            /* First iteration is over, now we know how wide map is */
            world_width = world.size();
        }
    }
    world_height = lineno;

    mapFile.close();

    lastAgentAction = Agent::idle;

    errorMessage = QString();
}

void World::performAction(Agent::actions action) {
    justBumped = false;

    switch(action) {
        case Agent::moveLeft:
            if(agentPosX == 0 || agentPosX == world_width-1
                    || world[agentPosX-1]->at(agentPosY) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosX--;
            };
            consumedEnergy++;
            break;

        case Agent::moveRight:
            if(agentPosX == 0 || agentPosX == world_width-1
                    || world[agentPosX+1]->at(agentPosY) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosX++;
            };
            consumedEnergy++;
            break;

        case Agent::moveUp:
            if(agentPosY == 0 || agentPosY == world_height-1
                    || world[agentPosX]->at(agentPosY-1) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosY--;
            };
            consumedEnergy++;
            break;

        case Agent::moveDown:
            if(agentPosY == 0 || agentPosY == world_height-1
                    || world[agentPosX]->at(agentPosY+1) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosY++;
            };
            consumedEnergy++;
            break;

        case Agent::suck:
            if(world[agentPosX]->at(agentPosY) > 0) {
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
    dirtyDegree = 0;
    for(int col = 0; col < world_width; col++)
        for(int row = 0; row < world_height; row++)
            if(world[col]->at(row) != OBSTACLE &&
                    static_cast<double>(rand_r(&seed))/RAND_MAX
                    < dirtyProbability) {
                world[col]->at(row)++;
                dirtyDegree += world[col]->at(row);
            }

    /* Yet another corn in the sandglass... */
    currentTime++;
}

void World::doOneStep() {
    performAction(agent.act(justBumped, world[agentPosX]->at(agentPosY)));
}

void World::resetMap() {
    /* clean all cells */
    for(int col = 0; col < world_width; col++)
        for(int row = 0; row < world_height; row++)
            if(world[col]->at(row) != OBSTACLE)
                world[col]->at(row) = 0;

    /* reset agent's position */
    agentPosX = initialAgentPosX;
    agentPosY = initialAgentPosY;

    lastAgentAction = Agent::idle;
}

