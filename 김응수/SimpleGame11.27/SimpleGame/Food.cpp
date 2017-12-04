#include "stdafx.h"
#include "Food.h"


Food::Food(float time, POINT locate) {
	this->prevTime = time;
	this->locateFood = locate;

	this->score = 2;//기본 2점으로 
	this->size = score * 10;//스코어가 큰 먹이는 사이즈가 큼
}
void Food::Update() {
	return;
}
void Food::Draw() {
	return;
}
Food::Food() {

}