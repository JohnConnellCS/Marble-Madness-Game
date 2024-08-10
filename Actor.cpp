#include "Actor.h"
#include "StudentWorld.h"

//*******ACTOR*******//
Actor::Actor(int id, double x, double y, int direction, StudentWorld* myWorld)
	:GraphObject(id, x, y, direction), playersWorld(myWorld)
{
	//Sets all actors to alive, and all actors to visible other than the exit
	this->determineVisibility();
	lifeStatus = true;
}

bool Actor::isAlive() const {
	return lifeStatus;
}

void Actor::kill() {
	lifeStatus = false;
	this->setVisible(false);
}

void Actor::takeDamage() {
	return;
}

int Actor::getHitPoints() const {
	return 999; //value that will never be attained
}

//Passability is a parameter used to tell if 
//1. The object is always passable
//2. The object is never passable
//3. The object is passable given certain circumstances
bool Actor::isPushable() const {
	if (getPassable() == 3)
		return true;
	return false;
}

bool Actor::isTraversable() const {
	return true;
}

bool Actor::canSwallowMarble() const {
	return false;
}

void Actor::push(double x, double y, int direction) { return; }

StudentWorld* Actor::getWorld() const {
	return playersWorld;
}

void Actor::determineVisibility() {
	this->setVisible(true);
}




//*******ACTOR WITH HEALTH*******//
ActorWithHealth::ActorWithHealth(int id, double x, double y, int direction, int hp, StudentWorld* myWorld)
	:Actor(id, x, y, direction, myWorld)
{
	hitPoints = hp;
}

void ActorWithHealth::takeDamage() {
	hitPoints -= 2;
	if (hitPoints <= 0) {
		this->kill();
	}
}

void ActorWithHealth::restoreHealth() {
	hitPoints = 20;
}

int ActorWithHealth::getHitPoints() const{
	return hitPoints;
}

void ActorWithHealth::shoot(bool isAlly) const{
	//Initializes a pea's direction and location depending on the position of the shooter
	if (getDirection() == up) {
		getWorld()->createPea(this->getX(), this->getY() + 1, 0, up);
	}
	else if (getDirection() == left) {
		getWorld()->createPea(this->getX() - 1, this->getY(), 0, left);
	}
	else if (getDirection() == right) {
		getWorld()->createPea(this->getX() + 1, this->getY(), 0, right);
	}
	else if (getDirection() == down) {
		getWorld()->createPea(this->getX(), this->getY() - 1, 0, down);
	}
	if (isAlly) {
		getWorld()->playSound(SOUND_PLAYER_FIRE);
	}
	else {
		getWorld()->playSound(SOUND_ENEMY_FIRE);
	}
}


//*******PLAYER*******//
Player::Player(double x, double y, StudentWorld* sw)
	:ActorWithHealth(IID_PLAYER, x, y, right, 20, sw)
{
	ammo = 20;
}

void Player::increaseAmmo(int amt) {
	ammo += amt;
}

int Player::getAmmo() const{
	return ammo;
}

void Player::addAmmo() {
	ammo += 20;
}

int Player::getHealthPercentage() const {
	return  100 * getHitPoints() / 20;
}

void Player::doSomething() {
	if (!isAlive()) {
		return;
	}
	int ch;
	//Obtain keyboard input and either 1.reset the level 2.move in a direction or 3.shoot a pea
	if (getWorld()->getKey(ch)) {
		switch (ch)
		{
		case KEY_PRESS_ESCAPE:
			this->kill();
			break;
		case KEY_PRESS_LEFT:
			if (getWorld()->isObstaclePresent(this->getX() - 1, this->getY()) == 1){
				this->moveTo(this->getX() - 1, this->getY());
				this->setDirection(left);
				break;
			}
			if (getWorld()->isObstaclePresent(this->getX() - 1, this->getY()) == 3) {
				if (getWorld()->attemptToPush(this->getX() - 2, this->getY())) {
					getWorld()->pushMarble(this->getX() - 1, this->getY(), left);
					this->moveTo(this->getX() - 1, this->getY());
					this->setDirection(left);
				}
			}
			this->setDirection(left);
			break;
		case KEY_PRESS_RIGHT:
			if (getWorld()->isObstaclePresent(this->getX() + 1, this->getY()) == 1) {
				this->moveTo(this->getX() + 1, this->getY());
				this->setDirection(right);
				break;
			}
			if (getWorld()->isObstaclePresent(this->getX() + 1, this->getY()) == 3) {
				if (getWorld()->attemptToPush(this->getX() + 2, this->getY())) {
					getWorld()->pushMarble(this->getX() + 1, this->getY(), right);
					this->moveTo(this->getX() + 1, this->getY());
					this->setDirection(right);
				}
			}
			this->setDirection(right);
			break;
		case KEY_PRESS_DOWN:
			if (getWorld()->isObstaclePresent(this->getX(), this->getY() - 1) == 1) {
				this->moveTo(this->getX(), this->getY() - 1);
				this->setDirection(down);
				break;
			}
			if (getWorld()->isObstaclePresent(this->getX(), this->getY() - 1) == 3) {
				if (getWorld()->attemptToPush(this->getX(), this->getY() - 2)) {
					getWorld()->pushMarble(this->getX(), this->getY() - 1, down);
					this->moveTo(this->getX(), this->getY() - 1);
					this->setDirection(down);
				}
			}
			this->setDirection(down);
			break;
		case KEY_PRESS_UP:
			if (getWorld()->isObstaclePresent(this->getX(), this->getY() + 1) == 1) {
				this->moveTo(this->getX(), this->getY() + 1);
				this->setDirection(up);
				break;
			}
			if (getWorld()->isObstaclePresent(this->getX(), this->getY() + 1) == 3) {
				if (getWorld()->attemptToPush(this->getX(), this->getY() + 2)) {
					getWorld()->pushMarble(this->getX(), this->getY() + 1, up);
					this->moveTo(this->getX(), this->getY() + 1);
					this->setDirection(up);
				}
			}
			this->setDirection(up);
			break;
		case KEY_PRESS_SPACE:
			if (ammo > 0) {
				ammo--;
				shoot(true);
			}
			break;
		default:
			break;
		}
	}
}

//*******ROBOT*******//
//Robot is the base class for all rage bots, thief bots, and mean thief bots
Robot::Robot(int id, double x, double y, int direction, int hp, StudentWorld* sw) 
	:ActorWithHealth(id, x, y, direction, hp, sw), tickCounter(0)
{}

//Reset the tick counter and return true if the robot should move
//Otherwise increment the tick and return false
bool Robot::checkTick() {
	if (getTickCounter() != getWorld()->getTickMax()) {
		addTick();
		return false;
	}
	resetTick();
	return true;
}

//Uses StudentWorld Helper Functions to determine if the Robot is facing the player
//And that there is no obstacle in between the player and the robot
bool Robot::attemptToShoot() {
	if (!hasGun()) {
		return false;
	}
	if (getDirection() == up && getWorld()->playerIsAbove(getX(), getY())) {
		return true;
	}
	else if (getDirection() == down && getWorld()->playerIsBelow(getX(), getY())) {
		return true;
	}
	else if (getDirection() == left && getWorld()->playerIsLeft(getX(), getY())) {
		return true;
	}
	else if (getDirection() == right && getWorld()->playerIsRight(getX(), getY())) {
		return true;
	}
	return false;
}

//Returns true 
//1.if robot moved or direction was updated
//2.if rage bot was successfully direction flipped
//Returns false if thief bot cannot move and needs a new direction
bool Robot::moveRobot(bool hasLinearMovement) {
	if (getDirection() == up) {
		if (getWorld()->isObstaclePresentIncludingPlayer(getX(), getY() + 1) == 1) {
			moveTo(getX(), getY() + 1);
			return true;
		}
		else if (!hasLinearMovement) {
			return false;
		}
		else {
			setDirection(down);
		}
	}
	else if (getDirection() == down) {
		if (getWorld()->isObstaclePresentIncludingPlayer(getX(), getY() - 1) == 1) {
			moveTo(getX(), getY() - 1);
			return true;
		}
		else if (!hasLinearMovement) {
			return false;
		}
		else {
			setDirection(up);
		}
	}
	else if (getDirection() == left) {
		if (getWorld()->isObstaclePresentIncludingPlayer(getX() - 1, getY()) == 1) {
			moveTo(getX() - 1, getY());
			return true;
		}
		else if (!hasLinearMovement) {
			return false;
		}
		else {
			setDirection(right);
		}
	}
	else if (getDirection() == right) {
		if (getWorld()->isObstaclePresentIncludingPlayer(getX() + 1, getY()) == 1) {
			moveTo(getX() + 1, getY());
			return true;
		}
		else if (!hasLinearMovement) {
			return false;
		}
		else{
			setDirection(left);
		}
	}
	return true;
}

void Robot::takeDamage() {
	this->damage();
	if (getHitPoints() <= 0) {
		getWorld()->playSound(SOUND_ROBOT_DIE);
		this->kill();
	}
	else {
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	}
}


//*******RAGE BOT*******//
RageBot::RageBot(double x, double y, bool isVert, StudentWorld* sw)
	:Robot(IID_RAGEBOT, x, y, (isVert ? down : right), 10, sw)
{}
void RageBot::doSomething() {
	if (!isAlive()) {
		return;
	}
	if (!checkTick()) {
		return;
	}
	//Shoot at the player if they are in range, otherwise update movement
	if (!attemptToShoot()) {
		moveRobot(true);
	}
	else {
		shoot(false);
	}
}
void RageBot::takeDamage() {
	Robot::takeDamage();
	if(getHitPoints() <= 0)
		getWorld()->increaseScore(100);
}

//*******THIEF BOT*******//
ThiefBot::ThiefBot(int id, int hp, double x, double y, StudentWorld* sw) 
	:Robot(id, x, y, right, hp, sw)
{
	//initialize a seperate counter for when the thief bot should change location
	distBeforeTurn = randInt(1, 6);
	distCounter = 0;
	//thief bots use pointers to keep track of goodies they steal
	stolenGoodie = nullptr;
}
void ThiefBot::doSomething() {
	if (!isAlive()) {
		return;
	}
	if (!checkTick()) {
		return;
	}
	if (!hasGun() || !attemptToShoot()) {
		Actor* goodie = getWorld()->getGoodieAt(getX(), getY());
		if (goodie != nullptr) {
			//if nothing is stolen, the thief bot attempts to move
			int stealChance = randInt(1, 10);
			if (stealChance == 1) {
				this->stolenGoodie = goodie;
				if (stolenGoodie != nullptr) {
				}
				stolenGoodie->disable();
				getWorld()->playSound(SOUND_ROBOT_MUNCH);
				return;
			}
		}
		//if it is time to change direction or an obstacle is present
		//change direction and attempt to move in every direction
		if (distCounter == distBeforeTurn || moveRobot(false) == false) {
			distCounter = 0;
			distBeforeTurn = randInt(1, 6);
			int newDir = randInt(1, 4);
			int counter = 0;
			if (newDir == 1) {
				this->setDirection(up);
			}
			else if (newDir == 2) {
				this->setDirection(down);
			}
			else if (newDir == 3) {
				this->setDirection(left);
			}
			else if (newDir == 4) {
				this->setDirection(right);
			}
			//Try to move in all four directions, do nothing else if unsuccessful
			while (moveRobot(false) == false && counter < 4) {
				//Checks each direction for possible movement
				//Returns to original direction if unsuccesful
				if (newDir == 4) {
					newDir = 1;
				}
				else {
					newDir++;
				}
				if (newDir == 1) {
					this->setDirection(up);
				}
				else if (newDir == 2) {
					this->setDirection(down);
				}
				else if (newDir == 3) {
					this->setDirection(left);
				}
				else if (newDir == 4) {
					this->setDirection(right);
				}
				counter++;
			}
		}
		else {
			distCounter++;
		}
	}
	else {
		shoot(false);
	}
}

void ThiefBot::takeDamage() {
	this->damage();
	if (getHitPoints() <= 0) {
		//Overloaded function accounts for dropping of stolen goodies
		if (stolenGoodie != nullptr) {
			dropGoodie();
		}
		getWorld()->playSound(SOUND_ROBOT_DIE);
		this->kill();
		getWorld()->increaseScore(10);
	}
	else {
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
	}
}

//update the location of the stolen goodie and make it reappear
void ThiefBot::dropGoodie() {
	stolenGoodie->moveTo(getX(), getY());
	stolenGoodie->setVisible(true);
}

//*******MEAN THEIF BOT*******//
MeanThiefBot::MeanThiefBot(double x, double y, StudentWorld* sw)
	:ThiefBot(IID_MEAN_THIEFBOT, 8, x, y, sw)
{}

//Mean Thief Bots act identically to Thief Bots except they have the option to shoot
void MeanThiefBot::doSomething() {
	ThiefBot::doSomething();
}

void MeanThiefBot::takeDamage() {
	ThiefBot::takeDamage();
	//add an additional 10 points for killing a mean thief bot
	if(getHitPoints() <= 0)
		getWorld()->increaseScore(10);
}

//*******MARBLE*******//
Marble::Marble(double x, double y, StudentWorld* sw)
	:ActorWithHealth(IID_MARBLE, x, y, none, 10, sw)
{}

void Marble::doSomething() {}

//push is only called if a pit/empty space has been located in the direction it's being pushed
void Marble::push(double x, double y, int direction) {
	if (direction == up) {
		moveTo(x, y + 1);
	}
	else if (direction == down) {
		moveTo(x, y - 1);
	}
	else if (direction == left) {
		moveTo(x - 1, y);
	}
	else if (direction == right) {
		moveTo(x + 1, y);
	}
}


//*******RETRIEVABLE*******//
//Retrievable is a base class for all goodies and crystals
Retrievable::Retrievable(int id, double x, double y, StudentWorld* myWorld)
:Actor(id, x, y, none, myWorld)	
{}

//Compares the goodie's location to the location of the player
bool Retrievable::isPlayerPresent() const{
	if (this->getX() == getWorld()->getPlayer()->getX() && this->getY() == getWorld()->getPlayer()->getY() && this->isVisible() == true)
		return true;
	return false;
}

void Retrievable::claimPoints(int amt) {
	getWorld()->increaseScore(amt);
	this->kill();
}


//*******CRYSTAL*******//
Crystal::Crystal(double x, double y, StudentWorld* sw)
	:Retrievable(IID_CRYSTAL, x, y, sw)
{}

void Crystal::doSomething() {
	if (!isAlive())
		return;
	if (isPlayerPresent()) {
		claimPoints(50);
		//notifies the student world that the number of remaining crystals is one less
		getWorld()->obtainCrystal();
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
}


//*******GOODIE*******//
Goodie::Goodie(int id, double x, double y, StudentWorld* sw)
	:Retrievable(id, x, y, sw)
{}

void Goodie::claimPoints(int amt) {
	getWorld()->increaseScore(amt);
	this->kill();
	getWorld()->playSound(SOUND_GOT_GOODIE);
}
void Goodie::disable() {
	this->setVisible(false);
}

//*******EXTRA LIFE GOODIE*******//
ExtraLifeGoodie::ExtraLifeGoodie(double x, double y, StudentWorld* sw)
	:Goodie(IID_EXTRA_LIFE, x, y, sw)
{}

void ExtraLifeGoodie::doSomething() {
	if (!isAlive() || !isVisible())
		return;
	if (isPlayerPresent()) {
		claimPoints(1000);
		getWorld()->incLives();
	}
}

//*******RESTORE HEALTH GOODIE*******//
RestoreHealthGoodie::RestoreHealthGoodie(double x, double y, StudentWorld* sw)
	:Goodie(IID_RESTORE_HEALTH, x, y, sw)
{}

void RestoreHealthGoodie::doSomething() {
	if (!isAlive() || !isVisible())
		return;
	if (isPlayerPresent()) {
		claimPoints(500);
		getWorld()->getPlayer()->restoreHealth();
	}
}

//*******AMMO GOODIE*******//
AmmoGoodie::AmmoGoodie(double x, double y, StudentWorld* sw)
	:Goodie(IID_AMMO, x, y, sw)
{}

void AmmoGoodie::doSomething() {
	if (!isAlive() || !isVisible())
		return;
	if (isPlayerPresent()) {
		claimPoints(100);
		getWorld()->getPlayer()->addAmmo();
	}
}

//*******EXIT*******//
Exit::Exit(double x, double y, StudentWorld* sw) 
	:Retrievable(IID_EXIT, x, y, sw)
{}

void Exit::doSomething() {
	if (getWorld()->getCrystalsRemaining() > 0) {
		setVisible(false);
	}
	else {
		//reveals exit if no crystals remain
		setVisible(true);
	}
	if (isPlayerPresent() && isVisible()) {
		claimPoints(2000);
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
		getWorld()->finishLevel();
	}
}

void Exit::determineVisibility() {
	this->setVisible(false);
}

//*******PEA*******//
Pea::Pea(double x, double y, int facing, double off, StudentWorld* sw)
	:Actor(IID_PEA, x, y, facing, sw), offset(off)
{}

void Pea::doSomething() {
	if (!isAlive()) {
		return;
	}
	//checks if the Pea collided, removing it and damaging necessary actors
	if (getWorld()->isPeaObstaclePresent(this->getX(), this->getY()) != 1) {
		getWorld()->damageAllObstaclesPresent(this->getX(), this->getY());
		this->kill();
	}
	if (!isAlive()) {
		return;
	}
	int dir = getDirection();
	//Updates position based on set direction
	switch (dir)
	{
	case up:
		this->moveTo(this->getX(), this->getY() + 1);
		break;
	case down:
		this->moveTo(this->getX(), this->getY() - 1);
		break;
	case left:
		this->moveTo(this->getX() - 1, this->getY());
		break;
	case right:
		this->moveTo(this->getX() + 1, this->getY());
		break;
	}
	//checks if the new location has an obstacle for the pea to collide with
	if (getWorld()->isPeaObstaclePresent(this->getX(), this->getY()) != 1) {
		getWorld()->damageAllObstaclesPresent(this->getX(), this->getY());
		this->kill();
	}
}

//*******UNPASSABLE OBJECT*******//
UnpassableObject::UnpassableObject(int id, double x, double y, bool traversability, StudentWorld* sw)
	:Actor(id, x, y, none, sw)
{
	traversable = traversability;
}
bool UnpassableObject::isTraversable() const{ return traversable; }
bool UnpassableObject::canSwallowMarble() const { return getPassable() == 1; }

//*******FACTORY*******//
Factory::Factory(double x, double y, StudentWorld* sw, int robotType) 
	:UnpassableObject(IID_ROBOT_FACTORY, x, y, false, sw)
{
	thiefBotSpawnType = robotType;
}

void Factory::doSomething() {
	if (getWorld()->getNumThiefBots(getX(), getY()) < 3 && randInt(1, 50) == 1) {
		if (thiefBotSpawnType == 0) {
			getWorld()->spawnThiefBot(getX(), getY(), 0);
		}
		else {
			getWorld()->spawnThiefBot(getX(), getY(), 1);
		}
		getWorld()->playSound(SOUND_ROBOT_BORN);
	}
}

//*******PIT*******//
Pit::Pit(double x, double y, StudentWorld* sw) 
	:UnpassableObject(IID_PIT, x, y, false, sw)
{}
void Pit::doSomething(){
	if (!isAlive()) {
		return;
	}
	//Checks if a marble has been moved onto the pit, signaling it to despawn
	if (getWorld()->isHolePatched(this->getX(), this->getY())) {
		this->kill();
		getWorld()->removeMarble(this->getX(), this->getY());
	}
}

//*******WALL*******//
Wall::Wall(double x, double y, StudentWorld* sw) 
	:UnpassableObject(IID_WALL, x, y, false, sw)
{}
void Wall::doSomething() {}

