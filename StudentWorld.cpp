#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), bonusPoints(1000), crystalsRemaining(0), isLevelComplete(false)
{
    player = nullptr;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    crystalsRemaining = 0;
    int status = loadLevel();
    if (status == -1) {
        return GWSTATUS_PLAYER_WON;
    }
    else if (status == -2) {
        return GWSTATUS_LEVEL_ERROR;
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //update the text on the top of the screen to account for changes in the last tick
    updateDisplayText();

    //let every actor have a chance to do something
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if(currentActor->isAlive())
            currentActor->doSomething();
        //check if the player died each tick
        if (!player->isAlive())
            return GWSTATUS_PLAYER_DIED;
    }
    player->doSomething();

    //remove dead actors
    removeDeadActors();

    //If the player is on the exit and collected all the crystals, finish the level
    if (isLevelComplete) {
        isLevelComplete = false;
        return GWSTATUS_FINISHED_LEVEL;
    }

    if (bonusPoints > 0) {
        bonusPoints--;
    }

    if (!player->isAlive()) {
        return GWSTATUS_PLAYER_DIED;
    }

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if (player != nullptr) {
        delete player;
        player = nullptr;
    }
    if (actorList.size() != 0) {
        for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end();) {
            Actor* temp = *p;
            temp->setVisible(false);
            p = actorList.erase(p);
            delete temp;
        }
    }
}

int StudentWorld::loadLevel() {
    //Creates string that matches file name of current level
    string curLevel = "level";
    if (getLevel() < 10)
        curLevel += "0";
    curLevel += to_string(getLevel()) + ".txt";

    Level lev(assetPath());
    Level::LoadResult validity = lev.loadLevel(curLevel);
    if (validity == Level::load_fail_file_not_found || getLevel() == 100) {
        return -1; //player has won, no more levels
    }
    else if (validity == Level::load_fail_bad_format) {
        return -2; //error with map
    }
    else {
        //Loop through entire grid and add all actors
        for (int i = 0; i < VIEW_WIDTH; i++) {
            for (int j = 0; j < VIEW_HEIGHT; j++) {
                Level::MazeEntry spot = lev.getContentsOf(i, j);
                if (spot == Level::player) {
                    player = new Player(i, j, this);
                }
                if (spot == Level::wall) {
                    actorList.push_back(new Wall(i, j, this));
                }
                if (spot == Level::crystal) {
                    actorList.push_back(new Crystal(i, j, this));
                    crystalsRemaining++;
                }
                if (spot == Level::extra_life) {
                    actorList.push_back(new ExtraLifeGoodie(i, j, this));
                }
                if (spot == Level::restore_health) {
                    actorList.push_back(new RestoreHealthGoodie(i, j, this));
                }
                if (spot == Level::ammo) {
                    actorList.push_back(new AmmoGoodie(i, j, this));
                }
                if (spot == Level::exit) {
                    actorList.push_back(new Exit(i, j, this));
                }
                if (spot == Level::marble) {
                    actorList.push_back(new Marble(i, j, this));
                }
                if (spot == Level::pit) {
                    actorList.push_back(new Pit(i, j, this));
                }
                if (spot == Level::vert_ragebot) {
                    actorList.push_back(new RageBot(i, j, true, this));
                }
                if (spot == Level::horiz_ragebot) {
                    actorList.push_back(new RageBot(i, j, false, this));
                }   
                if (spot == Level::thiefbot_factory) {
                    actorList.push_back(new Factory(i, j, this, 0));
                }
                if (spot == Level::mean_thiefbot_factory) {
                    actorList.push_back(new Factory(i, j, this, 1));
                }
            }
        }
    }
    return 1;
}

void StudentWorld::removeDeadActors() {
    //check if each actor is dead, if so, delete it and remove it from the game/actor list
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end();) {
        Actor* currentActor = *p;
        if (!currentActor->isAlive()) {
            currentActor->setVisible(false);
            p = actorList.erase(p);
            delete currentActor;
        }
        else {
            p++;
        }
    }
}

void StudentWorld::finishLevel() {
    isLevelComplete = true;
    increaseScore(bonusPoints);
}

//checks whether player movement is valid
//1 indicates passable, 2 indicates not passabale, 3 indicates conditionally moveable object
int StudentWorld::isObstaclePresent(double x, double y) {
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->getPassable() == 2) {
            return 2;
        }
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->isPushable()) {
            return 3;
        }
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->isTraversable() == false) {
            return 2;
        }
    }
    return 1;
}

//uses previous function while also accounting for the player being present
int StudentWorld::isObstaclePresentIncludingPlayer(double x, double y) {
    if (player->getX() == x && player->getY() == y) {
        return 4;
    }
    return isObstaclePresent(x, y);
}

//checks whether a pea should collide or continue moving
int StudentWorld::isPeaObstaclePresent(double x, double y) {
    if (player->getX() == x && player->getY() == y) {
        return 3;
    }
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->getPassable() != 1)
            return 2;
    }
    return 1;
}

//notifies pit to delete itself if a marble is on it
bool StudentWorld::isHolePatched(double x, double y) {
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->isPushable())
            return true;
    }
    return false;
}

//pea damages all objects present when it despawns
void StudentWorld::damageAllObstaclesPresent(double x, double y) {
    if (player->getX() == x && player->getY() == y) {
        player->takeDamage();
        if (player->getHitPoints() <= 0){
            this->playSound(SOUND_PLAYER_DIE);
            decLives();
            player->kill();
        }
        else {
            this->playSound(SOUND_PLAYER_IMPACT);
        }
    }
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->getPassable() != 1) {
            currentActor->takeDamage();
            if (currentActor->getHitPoints() <= 0) {
                currentActor->kill();
            }
        }
    }
}

//return true if the marble can move and false if not
bool StudentWorld::attemptToPush(double x, double y) {
    //check for any items in the square the marble would move to
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && !currentActor->canSwallowMarble()) {
            return false;
        }
    }
    return true;
}

void StudentWorld::pushMarble(double x, double y,int direction) {
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y) {
            currentActor->push(x, y, direction);
        }
    }
}

void StudentWorld::removeMarble(double x, double y) {
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isAlive() && currentActor->getX() == x && currentActor->getY() == y && currentActor->isPushable()) {
            currentActor->setVisible(false);
            p = actorList.erase(p);
            delete currentActor;
        }
    }
}

void StudentWorld::updateDisplayText() {
    string s = formatScoreText(getLevel(), bonusPoints, getScore(), getLives(), player->getHealthPercentage(), player->getAmmo());
    setGameStatText(s);
}

string StudentWorld::formatScoreText(int level, int bonus, int score, int lives, int health, int ammo) {
    ostringstream oss;
    oss << "Score: " << setfill('0') << setw(7) << score
        << "  Level: " << setw(2) << level
        << "  Lives: " << setw(2) << setfill(' ') << lives
        << "  Health: " << setw(3) << health << "%"
        << "  Ammo: " << setw(3) << ammo
        << "  Bonus: " << setw(4) << bonus;
    string s = oss.str();
    return s;
}

void StudentWorld::createPea(double x, double y, double offset, int direction) {
    actorList.push_front(new Pea(x, y, direction, offset, this));
}

void StudentWorld::obtainCrystal() {
    crystalsRemaining--;
}

int StudentWorld::getCrystalsRemaining() const{
    return crystalsRemaining;
}

Player* StudentWorld::getPlayer() {
    return player;
}

int StudentWorld::getTickMax() {
    int ticks = (28 - getLevel()) / 4; // levelNumber is the current level number (0, 1, 2, etc.)
    if (ticks < 3)
        ticks = 3; // no RageBot moves more frequently than this
    return ticks;
}

bool StudentWorld::playerIsAbove(double x, double y) {
    for (double i = y + 1; i < VIEW_HEIGHT; i++) {
        if (isObstaclePresentIncludingPlayer(x, i) == 4) {
            return true;
        }
        if (isPeaObstaclePresent(x, i) != 1) {
            return false;
        }
    }
    return false;
}

//the four "playerIs" + direction functions check for an unobstructed path from the robot to the player
bool StudentWorld::playerIsBelow(double x, double y) {
    for (double i = y - 1; i > 0; i--) {
        if (isObstaclePresentIncludingPlayer(x, i) == 4) {
            return true;
        }
        if (isPeaObstaclePresent(x, i) != 1) {
            return false;
        }
    }
    return false;
}
bool StudentWorld::playerIsLeft(double x, double y) {
    for (double i = x - 1; i > 0; i--) {
        if (isObstaclePresentIncludingPlayer(i, y) == 4) {
            return true;
        }
        if (isPeaObstaclePresent(i, y) != 1) {
            return false;
        }
    }
    return false;
}
bool StudentWorld::playerIsRight(double x, double y) {
    for (double i = x + 1; i < VIEW_WIDTH; i++) {
        if (isObstaclePresentIncludingPlayer(i, y) == 4) {
            return true;
        }
        if (isPeaObstaclePresent(i, y) != 1) {
            return false;  
        }
    }
    return false;  
}

Actor* StudentWorld::getGoodieAt(double x, double y) {
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->isVisible() && currentActor->getX() == x && currentActor->getY() == y && currentActor->isGoodie()) {
            return currentActor;
        }
    }
    //no goodie at location
    return nullptr;
}

//returns the number of thief bot unless there is a thief bot on the factory, in which case return 999
int StudentWorld::getNumThiefBots(double x, double y) {
    int numThieves = 0;
    for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
        Actor* currentActor = *p;
        if (currentActor->canSteal() && currentActor->isVisible() && currentActor->getX() <= x+1 && currentActor->getX() >= x-1 && currentActor->getY() <= y+1 && currentActor->getY() >= y-1) {
            numThieves++;
            //check for case where a thief bot is already present on the factory
            if (currentActor->getX() == x && currentActor->getY() == y)
                return 999;
        }
    }
    return numThieves;
}

void StudentWorld::spawnThiefBot(double x, double y, int spawnType) {
    if (spawnType == 0) {
        actorList.push_front(new ThiefBot(IID_THIEFBOT, 5, x, y, this));
    }
    else {
        actorList.push_front(new MeanThiefBot(x, y, this));
    }
}