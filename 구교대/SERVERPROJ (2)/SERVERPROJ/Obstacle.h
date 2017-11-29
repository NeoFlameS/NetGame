
//private:
//	//POINT locate, vector; //장애물의 좌표와 방향
//	int size; //장애물의 사이즈
//	float prevTime; // 먹이 생성시간
//	int speed; // 장애물의 속도
//	int damage; // 장애물의 데미지
//	bool stateDead; //장애물의 존재유무
#pragma once
#include <Windows.h>
#include <windef.h>

class Obstacle {
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