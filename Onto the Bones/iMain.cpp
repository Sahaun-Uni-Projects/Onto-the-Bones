#ifndef __INIT
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "iGraphics.h"
#define __INIT
#endif

#include "otb_graphics.h"
#include "otb_instances.h"
using namespace std;

int hdir, vdir;
void input_refresh() {
	hdir = vdir = 0;
}

void iPassiveMouse(int x, int y) {}
void iMouseMove(int mx, int my) {}
void iMouse(int button, int state, int mx, int my){}
void iKeyboard(unsigned char key) {
	hdir = 0;
	vdir = 0;

	switch (key) {
		case 'd': hdir =  1; break;
		case 'a': hdir = -1; break;
		case 'w': vdir =  1; break;
		case 's': vdir = -1; break;
		default: break;
	}
}
void iSpecialKeyboard(unsigned char key) {}

// Program starts here
#define WIN_W 960
#define WIN_H 540
#define GAME_X 30
#define GAME_Y 30
#define CELL_W 48
#define CELL_H 32

#define ROWS 4
#define COLS 10

#define INFINITY 2147483647

#define DEFAULT_LEVEL_LAYOUT_DELIMITER ' '

queue<OBJECT_TYPE> InputQueue;

// Globals
Instance* player;
Instance* enemy;
int EnemyMoveTimer = -1;

vector<string> layout;

vector<vector<int>> Grid;
int GridRows, GridCols;
vector<vector<int>> HeatMap;


// Functions
bool cell_in_bounds(int row, int col, int totalRows = GridRows, int totalCols = GridCols) {
	return (((0 <= row) && (row < totalRows)) && ((0 <= col) && (col < totalCols)));
}

bool cell_is_valid(int row, int col, int totalRows = GridRows, int totalCols = GridCols) {
	return (cell_in_bounds(row, col, totalRows, totalCols) && Grid[row][col]);
}

vector<vector<int>> world_init(vector<string> temp, char delimiter = DEFAULT_LEVEL_LAYOUT_DELIMITER) {
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

void ev_step() {}

void ev_draw() {
	draw_set_color(c_white);
	for (int i = 0; i < GridRows; ++i) {
		for (int j = 0; j < GridCols; ++j) {
			if (Grid[i][j]) draw_rectangle(coord_x(j), coord_y(i), CELL_W, CELL_H);
			//draw_rectangle(coord_x(j), coord_y(i), CELL_W, CELL_H);
		}
	}
	
	draw_rectangle_at_cell(enemy->get_row(), enemy->get_col(), c_red);
	draw_circle_at_cell(player->get_row(), player->get_col(), c_green);
}

void iDraw() {
	iClear();
	ev_draw();
}

void move() {
	if (InputQueue.empty()) {
		for (int i = 0; i < int(InstancesList.size()); ++i) {
			Instance* inst = InstancesList[i];
			for (int j = 0; j < inst->get_moves(); ++j) {
				//InputQueue.push(inst->get_object_type());
				InputQueue.push(inst->get_object_type());
			}
		}
	} else {
		if (InputQueue.front() == PLAYER) {
				int r = player->get_row(), c = player->get_col();
			if ((hdir || vdir) && cell_is_valid(r+vdir, c+hdir)) {
				player->move_relative(vdir, hdir);

				input_refresh();

				InputQueue.pop();
				HeatMap = generate_heatmap(player);
				for (int i = 0; i < GridRows; ++i) {
					for (int j = 0; j < GridCols; ++j) cout << HeatMap[i][j] << " ";
					cout << endl;
				}
			}
		} else {
			int mr = INFINITY, mc = INFINITY, dr = 0, dc = 0,
				row = enemy->get_row(), col = enemy->get_col();

			if (cell_in_bounds(row-1, col)) mr = min(mr, HeatMap[row-1][col]);
			if (cell_in_bounds(row+1, col)) mr = min(mr, HeatMap[row+1][col]);
			if (cell_in_bounds(row, col-1)) mc = min(mc, HeatMap[row][col-1]);
			if (cell_in_bounds(row, col+1)) mc = min(mc, HeatMap[row][col+1]);

			cout << mr << " " << mc << endl;

			if ((mr != INFINITY) && (mc != INFINITY)) {
				if (mc < mr) {
					if (cell_in_bounds(row, col-1) && (HeatMap[row][col-1] == mc)) enemy->move_relative(0, -1);
						else if (cell_in_bounds(row, col+1) && (HeatMap[row][col+1] == mc)) enemy->move_relative(0, 1);
				} else {
					if (cell_in_bounds(row-1, col) && (HeatMap[row-1][col] == mr)) enemy->move_relative(-1, 0);
						else if (cell_in_bounds(row+1, col) && (HeatMap[row+1][col] == mr)) enemy->move_relative(1, 0);
				}
			}

			InputQueue.pop();
		}
	}
}

void init() {
	// Initialize World
	/*
	layout.push_back("1.1.1.0.0.1.0.1.1.1.0");
	layout.push_back("1.1.1.0.0.1.0.1.1.1.0");
	layout.push_back("1.1.1.1.1.1.0.1.0.1.1");
	layout.push_back("1.1.1.0.0.1.1.1.0.1.1");
	layout.push_back("1.1.1.0.0.1.1.1.0.1.1");
	layout.push_back("1.1.1.1.1.1.1.1.1.1.1");
	*/

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
	player = new Player(2, 0, 2);
	enemy  = new Enemy(3, 9, 1);

	InstancesList.push_back(player);
	InstancesList.push_back(enemy);
}

int main() {
	// Initialization
	iInitialize(WIN_W, WIN_H, "Demo", 800, 200);
	init();

	// Timers
	EnemyMoveTimer = iSetTimer(100, move);

	// Start
	iStart();

	return 0;
}
