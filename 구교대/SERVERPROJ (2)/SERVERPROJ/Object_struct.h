#pragma once


#define MAX_BODY_LENGTH 20
#define MAX_CHARACTER	4
#define MAX_OBSTACLE	4
#define MAX_FOOD		30
#define LOCATE_SIZE     10


//#include <Windows.h>
#include "Character.h"
#include "Obstacle.h"
#include "Food.h"


struct WaitRoom
{
	int max_player;
	int current_player;
	int room_state;
};


struct ObjectGroup
{
	Character		player[MAX_CHARACTER];
	Obstacle		obs[MAX_OBSTACLE];
	Food			food[MAX_FOOD];
	//11.28 Ãß°¡
	int playerCnt;
	int obsCnt;
	int foodCnt;
};