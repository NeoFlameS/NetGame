#include "Obstacle.h"

Obstacle::Obstacle(float time, POINT locate) {
	this->prevTime = time;
	this->locate = locate;
	this->size = 10;// ũ��� �ӵ��� �ӽ�����
	this->speed = 1;// 
	this->stateDead = FALSE;
}

void Obstacle::Update(float time) {
	if (!this->stateDead) return; // �������̴� �Ű� ���� ����
	this->locate.x = this->locate.x + (this->vector.x * this->speed*time);
	this->locate.y = this->locate.y + (this->vector.y * this->speed*time);
}

void Obstacle::Draw() {
	return;
}

//11.28�߰�
Obstacle::Obstacle() {
}