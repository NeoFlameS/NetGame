



	//POINT locateFood; //먹이의 좌표
	//int size; //먹이의 사이즈
	//int score; // 먹을시 생기는 함수
	//float prevTime; // 먹이 생성시간
#pragma once
#include <Windows.h>
#include <windef.h>

class Food {
private:
	POINT locateFood;
	int size;
	int score;
	float prevTime;

public:
	Food(float time, POINT locate);
	//~Food();
	void Update();//갱신
	void Draw();// 먹이를 그려줌
				//


};