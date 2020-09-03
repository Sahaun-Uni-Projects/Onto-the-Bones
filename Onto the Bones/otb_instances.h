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
	int row, col, moves, hp;
	OBJECT_TYPE type;

	public:
		Instance() {}
		Instance(int row, int col, int moves, int hp, OBJECT_TYPE type) : row(row), col(col), moves(moves), hp(hp), type(type) {}
		
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

		void hit(int damage) {
			this->hp -= damage;
			std::cout << hp << " Instance hit.\n";
			//if (this->hp < 0) instance_destroy(this);
		}

		~Instance() {
			std::cout << "Instance destroyed.\n";
		}
};
std::vector<Instance*> InstancesList;


/**
 * Object definitions
 */

class Enemy : public Instance {
	public:
		Enemy() {}
		Enemy(int row, int col, int moves, int hp, OBJECT_TYPE type = ENEMY) : Instance(row, col, moves, hp, type) {}
};

class Player : public Instance {
	public:
		Player() {}
		Player(int row, int col, int moves, int hp, OBJECT_TYPE type = PLAYER) : Instance(row, col, moves, hp, type) {}
};

class Noone : public Instance {
	public:
		Noone() : Instance(0, 0, 0, 9999, NOONE) {}
};

/**
 * Functions
 */

Instance* instance_create(int x, int y, int moves, int hp, OBJECT_TYPE type) {
	Instance* inst;
	
	switch (type) {
		case PLAYER: inst = new Player(x, y, moves, hp); break;
		case ENEMY : inst = new  Enemy(x, y, moves, hp); break;
		default: break;
	}

	InstancesList.push_back(inst);
	return inst;
}

bool instance_exists(Instance* inst) {
	return (inst->get_object_type() != NOONE);
}