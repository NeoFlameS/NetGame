#pragma once
#include <Windows.h>
#include <windef.h>


#define MAX_BODY_LENGTH 20


struct CharacterBody {
	float x, y;
	int type;// 0 : ����, 1: �Ӹ�, 2: ����
};
class Character {
private:
	POINT vector; // ����
	int state;
	bool isDead;
	int bodyLength;

	CharacterBody char_Body[MAX_BODY_LENGTH];
	int score, Total_Score;//���� �� ����
	int id; //�÷��̾� �ĺ� id
	int speed;
	int size; //���� ũ��

public:
	Character();
	Character(int id, float x, float y);//������
										//~Character();
	void Update(float time, float _x, float _y);
	void Draw();

	// �߰� ���� ������ �ؿ� ��¥�� ���� �ۼ��� �ּ���.
	//11.25
public:
	int get_id();
	CharacterBody* get_body();
	void set_body(CharacterBody* s);

	//12.7 ���� 6:26
	int get_bodyLength();
	void set_bodyLength(int _bodyLength);
};