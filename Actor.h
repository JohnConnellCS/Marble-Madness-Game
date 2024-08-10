#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int id, double x, double y, int direction, StudentWorld* myWorld);
	virtual ~Actor() {}
	virtual void doSomething() = 0;
	bool isAlive() const;
	void kill();
	virtual void takeDamage();
	virtual int getHitPoints() const;
	virtual int getPassable() const { return  1; }
	bool isPushable() const;
	virtual bool isTraversable() const;
	virtual bool canSwallowMarble() const;
	virtual bool isGoodie() const { return false; }
	virtual void disable() { return; }
	virtual void push(double x, double y, int direction);
	virtual bool hasGun() { return false; }
	virtual bool canSteal() { return false; }
	StudentWorld* getWorld() const;
	virtual void determineVisibility();
private:
	bool lifeStatus;	
	StudentWorld* playersWorld;
};

//ActorWithHealth is the base class for the player, rage bots, thief bots, mean thief bots, and marbles
class ActorWithHealth : public Actor
{
public:
	ActorWithHealth(int id, double x, double y, int direction, int hp, StudentWorld* myWorld);
	//some sort of shoot function once peas are real lol
	virtual void takeDamage();
	void restoreHealth();
	virtual int getPassable() const { return  2; }
	int getHitPoints() const;
	void damage() { hitPoints -= 2; }
	virtual void shoot(bool isAlly) const;
private:
	int hitPoints;
};

class Player : public ActorWithHealth
{
public:
	Player(double x, double y, StudentWorld* sw);
	void increaseAmmo(int amt);
	void doSomething();
	int getAmmo() const;
	void addAmmo();
	int getHealthPercentage() const;
	virtual bool hasGun() { return true; }
private:
	int ammo;
};

class Robot : public ActorWithHealth
{
public:
	Robot(int id, double x, double y, int direction, int hp, StudentWorld* sw);
	int getTickCounter() const { return tickCounter; }
	void addTick() { tickCounter++; }
	void resetTick() { tickCounter = 0; }
	bool checkTick();
	bool attemptToShoot();
	bool moveRobot(bool hasLinearMovement);
	virtual void takeDamage();
private:
	int tickCounter;
};

class RageBot : public Robot
{
public:
	RageBot(double x, double y, bool isVert, StudentWorld* sw);
	void doSomething();
	virtual bool hasGun() { return true; }
	virtual void takeDamage();
private:
};

class ThiefBot : public Robot
{
public:
	ThiefBot(int id, int hp, double x, double y, StudentWorld* sw);
	virtual void doSomething();
	void dropGoodie();
	virtual void takeDamage();
	virtual bool canSteal() { return true; }
private:
	Actor* stolenGoodie;
	int distBeforeTurn;
	int distCounter;
};

class MeanThiefBot : public ThiefBot
{
public:
	MeanThiefBot(double x, double y, StudentWorld*sw);
	void doSomething();
	virtual bool hasGun() { return true; }
	virtual void takeDamage();
	virtual bool canSteal() { return true; }
private:
};

class Marble : public ActorWithHealth
{
public:
	Marble(double x, double y, StudentWorld* sw);
	void doSomething();
	void push(double x, double y, int direction);
	virtual int getPassable() const { return  3; }
private:
};

//Retrievable is the base class for all goodie types and crystals
class Retrievable : public Actor
{
public:
	Retrievable(int id, double x, double y, StudentWorld* myWorld);
	bool isPlayerPresent() const;
	virtual int getPassable() const { return  1; }
	virtual void claimPoints(int amt);
private:
};

class Crystal : public Retrievable
{
public:
	Crystal(double x, double y, StudentWorld* sw);
private:
	void doSomething();
};

//Goodie is the base class for the extra life, restore health, and ammo goodies
class Goodie : public Retrievable
{
public:
	Goodie(int id, double x, double y, StudentWorld* sw);
	virtual void claimPoints(int amt);
	bool isGoodie() const{return true;}
	void disable();
private:
};

class ExtraLifeGoodie : public Goodie
{
public:
	ExtraLifeGoodie(double x, double y, StudentWorld* sw);
	void doSomething();
private:
};

class RestoreHealthGoodie : public Goodie
{
public:
	RestoreHealthGoodie(double x, double y, StudentWorld* sw);
	void doSomething();
private:
};

class AmmoGoodie : public Goodie
{
public:
	AmmoGoodie(double x, double y, StudentWorld* sw);
	void doSomething();
private:
};

class Exit : public Retrievable
{
public:
	Exit(double x, double y, StudentWorld* sw);
	virtual void determineVisibility();
private:
	void doSomething();
};

class Pea : public Actor
{
public:
	Pea(double x, double y, int facing, double off, StudentWorld* sw);
	void doSomething();
	virtual int getPassable() const { return  1; }
private:
	double offset;
};

//UnpassableObject is the base class for factories, pits, and walls
class UnpassableObject : public Actor
{
public:
	UnpassableObject(int id, double x, double y, bool traversable, StudentWorld* sw);
	bool isTraversable() const;
	virtual int getPassable() const { return  2; }
	bool canSwallowMarble() const;
private:
	bool traversable;
};

class Factory : public UnpassableObject
{
public:
	Factory(double x, double y, StudentWorld* sw, int robotType);
	void doSomething();
private:
	int thiefBotSpawnType;
};

class Pit : public UnpassableObject
{
public:
	Pit(double x, double y, StudentWorld* sw);
	void doSomething();
	virtual int getPassable() const { return  1; }
private:
};

class Wall : public UnpassableObject
{
public: 
	Wall(double x, double y, StudentWorld* sw);
	void doSomething();
private:
};

#endif // ACTOR_H_
