#include "stdafx.h"
#include "Character.h"
#include"stdio.h"
Character::Character() {
	this->id = -1;
}
Character::Character(int id, float x, float y) {
	this->id = id;
	this->bodyLength = 1;
	this->char_Body[bodyLength - 1].x = x;
	this->char_Body[bodyLength - 1].y = y;
	this->char_Body[bodyLength - 1].type = 0;
	this->isDead = FALSE;

}

void Character::Update(float time, float _x, float _y) {
	return;
}
void Character::Draw() {

	printf("id : %d\n", id);
	printf("bodyLength : %d\n", bodyLength);
	printf("bodyLength : %f, %f\n", this->char_Body[0].x, this->char_Body[0].y);
	printf("***************************************\n");

	return;
}
//11.25 추가

int Character::get_id() {
	return this->id;
}




CharacterBody* Character::get_body() {
	return this->char_Body;
}

void Character::set_body(CharacterBody* s) {
	int i = 0;
	for (i = 0; i < MAX_BODY_LENGTH; i++) {
		this->char_Body[i] = s[i];
	}
}

//12.7 오전6:27
int Character::get_bodyLength() {
	return bodyLength;
}
void Character::set_bodyLength(int _bodyLength) {
	bodyLength = _bodyLength;
}