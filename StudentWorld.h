#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <list>
class Actor;
class Player;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  ~StudentWorld();

  virtual int init();
  virtual int move();
  virtual void cleanUp();

  //helper functions for init/move/cleanUp
  int loadLevel();
  void removeDeadActors();
  void finishLevel();

  //collision detection
  int isObstaclePresent(double x, double y);
  int isObstaclePresentIncludingPlayer(double x, double y);

  //marble/pit interaction
  bool isHolePatched(double x, double y);
  bool attemptToPush(double x, double y);
  void pushMarble(double x, double y, int direction);
  void removeMarble(double x, double y);

  //functions related to score and top of screen statistic tracking
  std::string formatScoreText(int level, int bonus, int score, int lives, int health, int ammo);
  void updateDisplayText();

  //functions related to pea shooting
  int isPeaObstaclePresent(double x, double y);
  void damageAllObstaclesPresent(double x, double y);
  void createPea(double x, double y, double peaOffset, int direction);
  
  //Managing crystals count
  void obtainCrystal();
  int getCrystalsRemaining() const;

  //function to allow player access by an actor
  Player* getPlayer();

  //functions for rage and mean thief bots shooting/movement
  int getTickMax();
  bool playerIsAbove(double x, double y);
  bool playerIsBelow(double x, double y);
  bool playerIsLeft(double x, double y);
  bool playerIsRight(double x, double y);

  //helper function for thief bot stealing
  Actor* getGoodieAt(double x, double y);

  //functions for thief bot factories
  int getNumThiefBots(double x, double y);
  void spawnThiefBot(double x, double y, int spawnType);

private:
	int bonusPoints;
	int crystalsRemaining;
	bool isLevelComplete;
	Player* player;
	std::list<Actor*> actorList;
};

#endif // STUDENTWORLD_H_
