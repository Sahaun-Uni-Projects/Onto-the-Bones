#ifndef __INIT
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "iGraphics.h"
#define __INIT
#endif

enum OBJECT_TYPE {
	NOONE,
	PLAYER,
	ENEMY
};

class Instance {
	int x, y;
	Sprite* sprite;

	public:
		Instance() {}
		Instance(int x, int y, Sprite* sprite) : x(x), y(y), sprite(sprite) {}
};

class GameInstance : Instance {
	int row, col, moves, hp, face;
	Sprite* sprite;
	OBJECT_TYPE type;

	public:
		GameInstance() {}
		GameInstance(int row, int col, int moves, int hp, int face, Sprite* sprite, OBJECT_TYPE type) : row(row), col(col), moves(moves), hp(hp), face(face), sprite(sprite), type(type) {}
		
		int get_row() {
			return this->row;
		}

		int get_col() {
			return this->col;
		}

		int get_moves() {
			return this->moves;
		}

		int get_hp() {
			return this->hp;
		}

		int get_face() {
			return this->face;
		}

		Sprite* get_sprite() {
			return this->sprite;
		}

		OBJECT_TYPE get_object_type() {
			return this->type;
		}

		void set_object_type(OBJECT_TYPE type) {
			this->type = type;
		}

		bool move_relative(int dr, int dc) {
			this->row += dr;
			this->col += dc;
			return true;
		}

		bool set_pos(int row, int col) {
			this->row = row;
			this->col = col;
			return true;
		}

		void set_face(int face) {
			this->face = face;
		}

		void flip() {
			this->face = -this->face;
		}

		void hit(int damage) {
			this->hp -= damage;
			std::cout << hp << " GameInstance hit.\n";
		}

		~GameInstance() {
			std::cout << "GameInstance destroyed.\n";
		}
};
std::vector<GameInstance*> InstancesList;

/**
 * Object definitions
 */

class Enemy : public GameInstance {
	public:
		Enemy() {}
		Enemy(int row, int col, int moves, int hp, Sprite* sprite, OBJECT_TYPE type = ENEMY) : GameInstance(row, col, moves, hp, -1, sprite, type) {}
};

class Player : public GameInstance {
	public:
		Player() {}
		Player(int row, int col, int moves, int hp, Sprite* sprite, OBJECT_TYPE type = PLAYER) : GameInstance(row, col, moves, hp, 1, sprite, type) {}
};

class Noone : public GameInstance {
	public:
		Noone() : GameInstance(0, 0, 9999, 9999, 1, sNoone, NOONE) {}
};

/**
 * Functions
 */

GameInstance* instance_create(int x, int y, int moves, int hp, Sprite* sprite, OBJECT_TYPE type) {
	GameInstance* inst;
	
	switch (type) {
		case PLAYER: inst = new Player(x, y, moves, hp, sprite); break;
		case ENEMY : inst = new  Enemy(x, y, moves, hp, sprite); break;
		default: break;
	}

	InstancesList.push_back(inst);
	return inst;
}

bool instance_exists(GameInstance* inst) {
	return (inst->get_object_type() != NOONE);
}
