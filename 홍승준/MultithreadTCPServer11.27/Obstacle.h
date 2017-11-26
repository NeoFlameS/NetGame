#pragma once
#include <Windows.h>
#include <windef.h>

class Obstacle{
private:
	POINT locate, vector;
	int size;
	float prevTime;
	int speed;
	int damage;
	bool stateDead;

public:
	Obstacle(float time, POINT locate);
	void Update(float time);
	void Draw();
//
	Obstacle();
};