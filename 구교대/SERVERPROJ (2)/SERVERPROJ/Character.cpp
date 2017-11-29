#include "Character.h"
Character::Character() {
	this->id = -1;
}
Character::Character(int id, POINT _locate) {
	this->id = id;
	this->bodyLength = 1;

	this->char_Body[bodyLength - 1].locate = _locate;
	this->char_Body[bodyLength - 1].type = 0;
	this->isDead = FALSE;

}

void Character::Update(float time, POINT vector) {
	return;
}
void Character::Draw() {
	return;
}
//11.25 추가

int Character::get_id() {
	return this->id;
}

CharacterBody* Character::get_body() {
	return this->char_Body;
}
//11.28 추가
int* Character::getbodylength(void)
{
	return &bodyLength;
}

bool Character::GetDead(void)
{
	return isDead;
}

int* Character::getTotalScore(void)
{
	return &Total_Score;
}

void Character::set_body(CharacterBody* s) {
	int i = 0;
	for (i = 0; i < MAX_BODY_LENGTH; i++) {
		this->char_Body[i] = s[i];
	}
}