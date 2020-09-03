#ifndef __INIT
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "iGraphics.h"
#define __INIT
#endif

#include "otb_input.h"
#include "otb_graphics.h"
#include "otb_instances.h"
using namespace std;

// Program starts here
#define INFINITY 2147483647

#define WIN_W 960
#define WIN_H 540
#define GAME_X 30
#define GAME_Y 30
#define CELL_W 48
#define CELL_H 32

#define LEVEL_LAYOUT_DELIMITER ' '

// Globals
Instance *player, *enemy, *enemy2, *noone;

int timerStep = -1;

queue<int> InputQueue;
vector<string> layout;

int GridRows, GridCols;
vector<vector<int>> Grid, HeatMap;

// Functions
bool cell_in_bounds(int row, int col, int totalRows = GridRows, int totalCols = GridCols) {
	return (((0 <= row) && (row < totalRows)) && ((0 <= col) && (col < totalCols)));
}
bool cell_is_valid(int row, int col, int totalRows = GridRows, int totalCols = GridCols) {
	return (cell_in_bounds(row, col, totalRows, totalCols) && Grid[row][col]);
}

vector<vector<int>> world_init(vector<string> temp, char delimiter = LEVEL_LAYOUT_DELIMITER) {
	int rows = int(temp.size()), cols = (int(temp[0].size())>>1);
	vector<vector<int>> grid(rows, vector<int>(cols,0));
	
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < (cols<<1); j += 2) {
			grid[i][j>>1] = int(temp[i][j]-'0');
		}
	}
	
	GridRows = rows;
	GridCols = cols;
	return grid;
}
vector<vector<int>> generate_heatmap(Instance* instance) {
	int rows = GridRows, cols = GridCols;
	vector<vector<int>> map(rows, vector<int>(cols,0));
	vector<vector<bool>> visited(rows, vector<bool>(cols,false));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (Grid[i][j] == 0) {
				map[i][j] = 100;
				visited[i][j] = true;
			}
		}
	}

	int r = instance->get_row(),
		c = instance->get_col(),
		dr[] = {1, -1, 0, 0},
		dc[] = {0, 0, 1, -1};

	queue<pair<int,int>> q;
	pair<int,int> p;
	q.push(make_pair(r,c));
	visited[r][c] = true;

	while (!q.empty()) {
		p = q.front(); q.pop();
		r = p.first;
		c = p.second;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				int rr = r + dr[i],
					cc = c + dc[j];
				if (!cell_is_valid(rr,cc) || visited[rr][cc]) continue;
				visited[rr][cc] = true;
				map[rr][cc] = map[r][c] + 1;
				q.push(make_pair(rr,cc));
			}
		}
	}

	return map;
}

Instance* instance_get_at_cell(int row, int col) {
	for (int i = 0; i < int(InstancesList.size()); ++i) {
		Instance* inst = InstancesList[i];
		if ((inst->get_row() == row) && (inst->get_col() == col)) return inst;
	}
	return noone;
}
int instance_get_number(Instance* inst) {
	for (int i = 0; i < int(InstancesList.size()); ++i) {
		if (InstancesList[i] == inst) return i;
	}
	return -1;
}

int coord_x(int col) {
	return (GAME_X + col*CELL_W);
}
int coord_y(int row) {
	return (GAME_Y + row*CELL_H);
}

void draw_rectangle_at_cell(int row, int col, int color = draw_get_color(), bool filled = true) {
	draw_rectangle_color(coord_x(col), coord_y(row), CELL_W, CELL_H, color, filled);
}
void draw_circle_at_cell(int row, int col, int color = draw_get_color(), bool filled = true) {
	double w = CELL_W/2., h = CELL_H/2.;
	draw_circle_color(coord_x(col)+w, coord_y(row)+h, min(w,h), color, filled);
}

void ev_step() {
	if (InputQueue.empty()) {
		// Feed inputs
		for (int i = 0; i < int(InstancesList.size()); ++i) {
			Instance* inst = InstancesList[i];
			if (inst == noone) {
				InstancesList.erase(InstancesList.begin()+i);
				continue;
			}
			for (int j = 0; j < inst->get_moves(); ++j) InputQueue.push(i);
		}
	} else {
		int curr = InputQueue.front();
		Instance *inst = InstancesList[curr];
		
		if (!instance_exists(inst)) {
			// Instance is destroyed
			InputQueue.pop();
		} else {
			// Instance exists
			if (inst->get_object_type() == PLAYER) {
				int r = inst->get_row()+vdir, c = inst->get_col()+hdir;
				if ((hdir || vdir) && cell_is_valid(r,c)) {
					// Move
					Instance* other = instance_get_at_cell(r,c);
					if (other != noone) {
						// Enemy found
						other->hit(1);
						if (other->get_hp() <= 0) {
							int pos = instance_get_number(other);
							if (pos > -1) InstancesList[pos] = noone;
						}
					} else {
						inst->set_pos(r,c);
					}
					
					// Handle Inputs
					input_refresh();
					InputQueue.pop();

					// Heatmap
					HeatMap = generate_heatmap(inst);
					for (int i = 0; i < GridRows; ++i) {
						for (int j = 0; j < GridCols; ++j) cout << HeatMap[i][j] << " ";
						cout << endl;
					}
				}
			} else {
				int mr = INFINITY, mc = INFINITY, dr = 0, dc = 0,
					row = inst->get_row(), col = inst->get_col();

				if (cell_in_bounds(row-1, col)) mr = min(mr, HeatMap[row-1][col]);
				if (cell_in_bounds(row+1, col)) mr = min(mr, HeatMap[row+1][col]);
				if (cell_in_bounds(row, col-1)) mc = min(mc, HeatMap[row][col-1]);
				if (cell_in_bounds(row, col+1)) mc = min(mc, HeatMap[row][col+1]);

				cout << mr << " " << mc << endl;

				if ((mr != INFINITY) && (mc != INFINITY)) {
					if (mc < mr) {
						if (cell_in_bounds(row, col-1) && (HeatMap[row][col-1] == mc)) inst->move_relative(0, -1);
							else if (cell_in_bounds(row, col+1) && (HeatMap[row][col+1] == mc)) inst->move_relative(0, 1);
					} else {
						if (cell_in_bounds(row-1, col) && (HeatMap[row-1][col] == mr)) inst->move_relative(-1, 0);
							else if (cell_in_bounds(row+1, col) && (HeatMap[row+1][col] == mr)) inst->move_relative(1, 0);
					}
				}

				InputQueue.pop();
			}
		}
	}
}

void ev_draw() {
	draw_set_color(c_white);
	for (int i = 0; i < GridRows; ++i) {
		for (int j = 0; j < GridCols; ++j) {
			if (Grid[i][j]) draw_rectangle(coord_x(j), coord_y(i), CELL_W, CELL_H);
		}
	}
	
	for (int i = 0; i < int(InstancesList.size()); ++i) {
		Instance* inst = InstancesList[i];
		if (!instance_exists(inst)) continue;
		draw_rectangle_at_cell(inst->get_row(), inst->get_col(), c_red);
	}
}

void iDraw() {
	iClear();
	ev_draw();
}

void init() {
	// Initialize World
	layout.push_back("1 1 1 0 0 1 0 1 1 1 0");
	layout.push_back("1 1 1 0 0 1 0 1 1 1 0");
	layout.push_back("1 1 1 1 1 1 0 1 0 1 1");
	layout.push_back("1 1 1 0 0 1 1 1 0 1 1");
	layout.push_back("1 1 1 0 0 1 1 1 0 1 1");
	layout.push_back("1 1 1 1 1 1 1 1 1 1 1");

	Grid = world_init(layout);
	HeatMap = vector<vector<int>>(GridRows, vector<int>(GridCols, 0));

	// Initialize Input
	input_refresh();
	
	// Initialize Instances
	noone  = new Noone();
	player = instance_create(2, 0, 2, 5, PLAYER);
	enemy  = instance_create(3, 9, 1, 3, ENEMY);
	enemy2 = instance_create(2, 2, 2, 2, ENEMY);
}

int main() {
	// Initialization
	iInitialize(WIN_W, WIN_H, "Demo", 800, 200);
	init();

	// Timers
	timerStep = iSetTimer(100, ev_step);

	// Start
	iStart();

	return 0;
}
