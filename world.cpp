#include "world.h"

World::World(string filename, QWidget * parent) {
    /* we can be sure filename is valid - GUI checked it */

    /* world doesn't even exist yet, so agent couldn't possibly bump into
     * something */
    justBumped = false;

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
        QString error_msg;
        if(parameters.eof())
            error_msg = QTranslator::tr("end of line reached while reading initial parameters");
        else
            error_msg = f.what();

        QMessageBox::critical(parent, QTranslator::tr("Error parsing map"),
                QTranslator::tr("Error: ") + error_msg);
        // FIXME: exit more gracefully
        created = false;
        return;
    };
    unsigned int seed;
    try {
        parameters >> seed;
    } catch (ios_base::failure f) {
        /* It's okay to catch eof here - moreover, it is *expected*, because
         * seed is the last number on the parameters line */
        if(! parameters.eof()) {
            QString error_msg;
            QMessageBox::critical(parent, QTranslator::tr("Error parsing map"),
                QTranslator::tr("Error: ") + f.what());
            // FIXME: exit more gracefully
            created = false;
            return;
        }
    };
    srand(seed);

    try {
        parameters >> seed;
    } catch (ios_base::failure f) {
        if(! parameters.eof())
            QMessageBox::warning(parent, QTranslator::tr("Malformed map"),
                QTranslator::tr("seed wasn't the last value on the parameters line"));
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
                    QMessageBox::critical(parent, 
                        QTranslator::tr("Error parsing map"),
                        /* "Error: " is not concatanated with the error message
                         * to make translations easier */
                        QTranslator::tr("Error: ") +
                        QTranslator::tr("unknown char in map at line ")
                        + QString::number(lineno) + ": " + c);
                    created = false;
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
                    QMessageBox::critical(parent,
                        QTranslator::tr("Error parsing map"),
                        // FIXME: rewrite that message with placeholders
                        QTranslator::tr("Error: ") +
                        QTranslator::tr("line ") + QString::number(lineno)
                        + QTranslator::tr(" is wider than others (should be ")
                        + QString::number(world_width) + ")");
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

    created = true;
}

void World::performAction(Agent::actions action) {
    switch(action) {
        case Agent::moveLeft:
            if(agentPosX == 0 || agentPosX == world_width-1
                    || world[agentPosX-1]->at(agentPosY) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosX--;
            };
            break;

        case Agent::moveRight:
            if(agentPosX == 0 || agentPosX == world_width-1
                    || world[agentPosX+1]->at(agentPosY) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosX++;
            };
            break;

        case Agent::moveUp:
            if(agentPosY == 0 || agentPosY == world_height-1
                    || world[agentPosX]->at(agentPosY-1) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosY--;
            };
            break;

        case Agent::moveDown:
            if(agentPosY == 0 || agentPosY == world_height-1
                    || world[agentPosX]->at(agentPosY+1) == OBSTACLE) {
                justBumped = true;
            } else {
                agentPosY++;
            };
            break;

        case Agent::suck:
            if(world[agentPosX]->at(agentPosY) > 0) {
                world[agentPosX]->at(agentPosY)--;
            };
            break;

        case Agent::doNothing:
            /* that one is easy */
            break;

        default:
            /* we can't possibly reach that state because of static compiler
             * checks, so it's safe to do nothing */
            break;
    }
}

bool World::isCurrentPosDirty() {
    return world[agentPosX]->at(agentPosY) > 0;
}

int World::dirtAmount() {
    return world[agentPosX]->at(agentPosY);
}

bool World::isJustBumped() {
    return justBumped;
}
