#include "stdafx.h"
#include "Food.h"


Food::Food(float time, POINT locate) {
	this->prevTime = time;
	this->locateFood = locate;

	this->score = 2;//�⺻ 2������ 
	this->size = score * 10;//���ھ ū ���̴� ����� ŭ
}
void Food::Update() {
	return;
}
void Food::Draw() {
	return;
}
Food::Food() {

}