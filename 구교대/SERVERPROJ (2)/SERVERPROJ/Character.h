

	//POINT vector; //����
	//int state; //ĳ���� ����
	//bool isDead; //���� ����
	//int bodyLength; //���Ǳ���
	//CharacterBody char_Body[MAX_BODY_LENGTH];//���� ���� �ִ�ġ�� �ٵ� Ŭ���� �迭����
	//int score, Total_Score; // ���� ,�������� ; ��ŷ�� �ݿ��� 
	////2���ھ�� ��ü�ϳ�
	//
	//int id;//���̵�� �÷��̾� �ĺ�
	//int speed; // �̵��ӵ�
	//int size; // ����ũ��
#pragma once
#include <Windows.h>
#include <windef.h>

#define MAX_BODY_LENGTH 20

struct CharacterBody {
	POINT locate;
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
	Character(int id, POINT _locate);//������
									 //~Character();
	void Update(float time, POINT vector);
	void Draw();

	// �߰� ���� ������ �ؿ� ��¥�� ���� �ۼ��� �ּ���.
	//11.25
public:
	Character();
	int get_id();
	CharacterBody* get_body();
	void set_body(CharacterBody* s);
	//11.28 �߰�
	int* getbodylength(void);
	bool GetDead(void);
	int* getTotalScore(void);
	
};