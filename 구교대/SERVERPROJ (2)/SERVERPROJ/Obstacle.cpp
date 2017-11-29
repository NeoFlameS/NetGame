#include "Obstacle.h"

Obstacle::Obstacle(float time, POINT locate) {
	this->prevTime = time;
	this->locate = locate;
	this->size = 10;// 크기와 속도는 임시지정
	this->speed = 1;// 
	this->stateDead = FALSE;
}

void Obstacle::Update(float time) {
	if (!this->stateDead) return; // 죽은아이는 신경 쓰지 않음
	this->locate.x = this->locate.x + (this->vector.x * this->speed*time);
	this->locate.y = this->locate.y + (this->vector.y * this->speed*time);
}

void Obstacle::Draw() {
	return;
}

//11.28추가
Obstacle::Obstacle() {
}