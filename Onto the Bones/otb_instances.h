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
	int row, col, moves;
	OBJECT_TYPE object_type;

	public:
		Instance() {}
		Instance(int row, int col, int moves, OBJECT_TYPE object_type) : row(row), col(col), moves(moves), object_type(object_type) {}
		
		int get_row() {
			return row;
		}

		int get_col() {
			return col;
		}

		int get_moves() {
			return moves;
		}

		OBJECT_TYPE get_object_type() {
			return object_type;
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
};
std::vector<Instance*> InstancesList;
std::vector<int> InstancesTypeList[2];

/**
 * Object definitions
 */

class Enemy : public Instance {
	public:
		Enemy() {}
		Enemy(int row, int col, int moves, OBJECT_TYPE object_type = ENEMY) : Instance(row, col, moves, object_type) {}
};

class Player : public Instance {
	public:
		Player() {}
		Player(int row, int col, int moves, OBJECT_TYPE object_type = PLAYER) : Instance(row, col, moves, object_type) {}
};
