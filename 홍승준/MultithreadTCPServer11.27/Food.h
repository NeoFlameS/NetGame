#pragma once
#include <Windows.h>
#include <windef.h>

class Food {
private:
	POINT locateFood;
	int size;
	int score;
	float prevTime;

public :
	Food(float time, POINT locate);
	//~Food();
	void Update();//����
	void Draw();// ���̸� �׷���
//
public :
	Food();
};