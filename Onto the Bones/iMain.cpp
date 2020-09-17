#ifndef __INIT
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <queue>
#include <functional>
#include <fstream>
#include "iGraphics.h"
#define __INIT
#endif

/*
|-------|------|----|-------|
| Enemy | Char | HP | Moves |
|-------|------|----|-------|
|  Bat  |  a   | 1  |   3   |
| Hound |  b   | 2  |   2   |
| Snail |  c   | 3  |   1   |
|-------|------|----|-------|
*/

// ------------------- Important Stuff

#pragma region Macros
#define INFINITY 2147483647

#define WIN_WIDTH  960
#define WIN_HEIGHT 540
#define WIN_TITLE  "Onto the Bones"

#define GAME_STEP_TIMER 100
#define PLAYER_MAX_HP 5

#define CELL_WIDTH 48
#define CELL_HEIGHT 32
#define LEVEL_LAYOUT_DELIMITER ' '

#define MAX_HIGHSCORE_COUNT 8
#define FILE_HIGHSCORE "highscores.sav"
#pragma endregion

#pragma region Dependencies
#include "otb_utils.h"
#include "otb_strings.h"
#include "otb_sprites.h"
#include "otb_graphics.h"
#include "otb_input.h"
#include "otb_instances.h"
#include "otb_rooms.h"
#include "otb_levels.h"
#include "otb_highscores.h"

using namespace std;
#pragma endregion

#pragma region Globals
// Timers
int Timer1, Timer100, Timer500;

// Background
int skyX[2];
int skySpeed;

// World
int GridRows, GridCols;
vector<vector<int>> Grid, HeatMap;

// Game
int GamePos[2], GameBasePos[2], GameAmplitudeRange[2];
int GameTimer, GameDir, GameLen;
vector<int> GameTimers, AllTimers;

int BonePos[2], BoneScale;
int Level, Tries, Kills, CurrKills;
bool Paused = false;

// Menu
vector<pair<string, function<void(void)>>> options;
int optionSelected, optionsCount;

// Instances
GameInstance *player, *noone;
queue<int> InputQueue;
#pragma endregion

// ------------------- Functions

#pragma region Cells
bool cell_in_bounds(int row, int col, int totalRows = GridRows, int totalCols = GridCols) {
	return (((0 <= row) && (row < totalRows)) && ((0 <= col) && (col < totalCols)));
}
bool cell_is_valid(int row, int col, int totalRows = GridRows, int totalCols = GridCols) {
	return (cell_in_bounds(row, col, totalRows, totalCols) && Grid[row][col]);
}
int coord_x(int col) {
	return (GamePos[0] + col*CELL_WIDTH);
}
int coord_y(int row) {
	return (GamePos[1] + row*CELL_HEIGHT);
}
#pragma endregion

#pragma region Instances
GameInstance* instance_get_at_cell(int row, int col) {
	for (int i = 0; i < int(InstancesList.size()); ++i) {
		GameInstance* inst = InstancesList[i];
		if (inst == noone) continue;
		if ((inst->get_row() == row) && (inst->get_col() == col)) return inst;
	}
	return noone;
}
int instance_get_number(GameInstance* inst) {
	for (int i = 0; i < int(InstancesList.size()); ++i) {
		if (InstancesList[i] == inst) return i;
	}
	return -1;
}
void hit_instance(GameInstance *inst, int damage) {
	if (!instance_exists(inst)) return;
	inst->hit(damage);
	if (inst->get_hp() <= 0) {
		InstancesList[instance_get_number(inst)] = noone;
		++CurrKills;
	}
}
void instance_face_towards(GameInstance* inst, GameInstance* target) {
	int dir = sign(target->get_col()-inst->get_col());
	if (abs(dir)) inst->set_face(dir);
}
#pragma endregion

#pragma region World
vector<vector<int>> world_init(vector<string>& temp, char delimiter = LEVEL_LAYOUT_DELIMITER) {
	int rows = int(temp.size()), cols = (int(temp[0].size())>>1)+1;
	vector<vector<int>> grid(rows, vector<int>(cols,0));
	
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < (cols<<1); j += 2) {
			int r = i, c = (j>>1);

			// Put a ground tile
			grid[r][c] = 1;

			// Put a character or tile
			switch (temp[i][j]) {
				case 'P': player = instance_create(r, c, 2, PLAYER_MAX_HP, sPlayer, PLAYER); break;
				case 'G': BonePos[0] = r; BonePos[1] = c; break;
				
				case 'a': instance_create(r, c,    3, 1,    sBat,   ENEMY); break;
				case 'b': instance_create(r, c,    2, 2,    sHound, ENEMY); break;
				case 'c': instance_create(r, c,    1, 3,    sSnail, ENEMY); break;

				default: grid[r][c] = int(temp[i][j]-'0'); break;
			}
		}
	}
	
	GridRows = rows;
	GridCols = cols;
	return grid;
}
vector<vector<int>> generate_heatmap(GameInstance* instance) {
	int rows = GridRows, cols = GridCols;
	vector<vector<int>> map(rows, vector<int>(cols,INFINITY));
	vector<vector<bool>> visited(rows, vector<bool>(cols,false));

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (Grid[i][j] == 0) {
				map[i][j] = INFINITY;
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
	map[r][c] = 0;
	visited[r][c] = true;

	while (!q.empty()) {
		p = q.front(); q.pop();
		r = p.first;
		c = p.second;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				int rr = r + dr[i], cc = c + dc[j];
				if (!cell_is_valid(rr,cc) || visited[rr][cc] || (instance_get_at_cell(rr,cc) != noone)) continue;
				visited[rr][cc] = true;
				map[rr][cc] = map[r][c] + 1;
				q.push(make_pair(rr,cc));
			}
		}
	}

	return map;
}
#pragma endregion

#pragma region Drawing
void draw_rectangle_at_cell(int row, int col, int color = draw_get_color(), bool filled = true) {
	draw_rectangle_color(coord_x(col), coord_y(row), CELL_WIDTH, CELL_HEIGHT, color, filled);
}
void draw_circle_at_cell(int row, int col, int color = draw_get_color(), bool filled = true) {
	double w = CELL_WIDTH/2., h = CELL_HEIGHT/2.;
	draw_circle_color(coord_x(col)+w, coord_y(row)+h, min(w,h), color, filled);
}
void draw_sprite_at_cell_offset(int row, int col, Sprite* spr, int dx, int dy, int face) {
	draw_sprite_ext(coord_x(col)+dx, coord_y(row)+dy, spr, face, 1);
}
void draw_sprite_at_cell(int row, int col, Sprite* spr, int face) {
	draw_sprite_at_cell_offset(row, col, spr, round((CELL_WIDTH-spr->get_width())/2.), round((CELL_HEIGHT-spr->get_height())/2.), face);
}
void draw_instance(GameInstance *inst, int dx = 0, int dy = 0) {
	if (!instance_exists(inst)) return;
	int row = inst->get_row(), col = inst->get_col();
	Sprite* spr = inst->get_sprite();
	draw_sprite_at_cell_offset(row, col, spr, dx+spr->get_xoffset(), dy, inst->get_face());
	//draw_circle_color(coord_x(col), coord_y(row)+32, 3, c_green, true);
}
#pragma endregion

#pragma region Timers
void pause_game_timers() {
	for (int i = 0; i < int(GameTimers.size()); ++i) iPauseTimer(GameTimers[i]);
}

void resume_game_timers() {
	for (int i = 0; i < int(GameTimers.size()); ++i) iResumeTimer(GameTimers[i]);
}

void pause_all_timers() {
	for (int i = 0; i < int(AllTimers.size()); ++i) iPauseTimer(AllTimers[i]);
}

void resume_all_timers() {
	for (int i = 0; i < int(AllTimers.size()); ++i) iResumeTimer(AllTimers[i]);
}

void timer_step1() {
	// Game
	++GameTimer;

	// Move sky
	for (int i = 0; i < 2; ++i) {
		int sw = bgSky->get_width();
		if ((skyX[i] -= skySpeed) <= -sw) skyX[i] = sw;
	}
}
void timer_step100() {
	room->step();
}
void timer_step500() {
	// Move game world
	GamePos[1] += GameDir;
	if (!--GameLen) {
		GameDir *= -1;
		GameLen = irandom_range(GameAmplitudeRange[0], GameAmplitudeRange[1]);
	}
	if (abs(GamePos[1]-GameBasePos[1]) >= 20) GameDir *= -1;
}
#pragma endregion

#pragma region Debugging
void log_vec2d(vector<vector<int>> grid) {
	cout << ">>>>>>>>>>>>>>>>>\n";
	for (int i = 0; i < int(grid.size()); ++i) {
		for (int j = 0; j < int(grid[i].size()); ++j) cout << grid[i][j] << " ";
		cout << "\n";
	}
	cout << "<<<<<<<<<<<<<<<<<\n";
}
#pragma endregion


// ------------------- Game Init

void timers_init() {
	Timer1   = iSetTimer(1,   timer_step1);
	Timer100 = iSetTimer(100, timer_step100);
	Timer500 = iSetTimer(500, timer_step500);
	
	// All Timers
	AllTimers.push_back(Timer1);
	AllTimers.push_back(Timer100);
	AllTimers.push_back(Timer500);


	// Game Timers
	GameTimers.push_back(Timer500);
}

void rooms_init() {
	#pragma region rMenu
	rMenu = new Room("Menu",
		// Create
		function<void(void)>([](void) {
			options.clear();

			// Sound
			PlaySound("Audio/aMenubg.wav", NULL, SND_ASYNC | SND_LOOP);

			// Menu Init	
			options.push_back(make_pair("Play", function<void(void)>([](void){
					Level = 1;
					Tries = 0;
					Kills = 0;
					room_goto(rGame);
				})
			));
					
			options.push_back(make_pair("Highscores", function<void(void)>([](void){
					room_goto(rHighscore);
				})
			));
					
			options.push_back(make_pair("Credits", function<void(void)>([](void){
					room_goto(rCredits);
				})
			));

			options.push_back(make_pair("Quit", function<void(void)>([](void){
					exit(0);
				})
			));
				
			optionSelected = 0;
			optionsCount = options.size();
			
			// Pause timer
			pause_game_timers();
		}),
		// Step
		function<void(void)>([](void) {
			// Handle input
			if (abs(vdir)) {
				optionSelected -= vdir;
				input_refresh();

				if (optionSelected < 0) optionSelected = optionsCount-1;
					else if (optionSelected >= optionsCount) optionSelected = 0;
			}

			if (interact) {
				options[optionSelected].second();
				input_refresh();
			}
		}),
		// Draw
		function<void(void)>([](void) {
			// Text
			int tx = WIN_WIDTH/2, ty = WIN_HEIGHT/2;
			int xx;
			char* text;

			for (int i = 0; i < optionsCount; ++i) {
				text = to_cstring(options[i].first);

				xx = tx - string_width(text)/2;
				if (i == 0) xx += 3;

				if (i == optionSelected) draw_text_color(xx-20, ty+2, ">", c_black);
				draw_text_color(xx, ty, text, c_black);

				ty -= 40;
			}
		})
	);
	#pragma endregion

	#pragma region rGame
	rGame = new Room("Game",
		// Create
		function<void(void)>([](void) {
			// Get layout
			vector<string> layout = get_level_layout(Level);

			// Check for game end
			if (layout.empty()) {
				room_goto(rHighscore);
				return;
			}

			// Globals
			CurrKills = 0;

			// Sound
			PlaySound("Audio/aWind.wav", NULL, SND_ASYNC | SND_LOOP);

			// Clear Instances
			InstancesList.clear();

			// Reset bone position
			BonePos[0] = BonePos[1] = -1;

			// Initialize Input
			while (!InputQueue.empty()) InputQueue.pop();
			input_refresh();

			// Initialize World
			Grid = world_init(layout);
			HeatMap = vector<vector<int>>(GridRows, vector<int>(GridCols, INFINITY));
			HeatMap = generate_heatmap(player);
	
			// Game
			GameBasePos[0] = (WIN_WIDTH-GridCols*CELL_WIDTH)/2;
			GameBasePos[1] = (WIN_HEIGHT-GridRows*CELL_HEIGHT)/2+10;
			GamePos[0] = GameBasePos[0];
			GamePos[1] = GameBasePos[1];
			GameAmplitudeRange[0] = 3;
			GameAmplitudeRange[1] = 4;
			GameTimer = 0;
			GameDir = irandom(1)*2-1;
			GameLen = irandom_range(GameAmplitudeRange[0], GameAmplitudeRange[1]);

			// Initialize Instances
			BoneScale = irandom(1)*2-1;

			// Put player on top of instance list/ input queue and sort
			for (int i = 0; i < int(InstancesList.size()); ++i) {
				if (InstancesList[i]->get_object_type() == PLAYER) {
					swap(InstancesList[0], InstancesList[i]);
					break;
				}
			}
			sort(InstancesList.begin()+1, InstancesList.end(), [](GameInstance* inst1, GameInstance* inst2) {
				return (inst1->get_moves() < inst2->get_moves());
			});

			// Set faces
			for (int i = 0; i < int(InstancesList.size()); ++i) {
				instance_face_towards(InstancesList[i], player);
			}

			// Start timer
			resume_game_timers();
		}),
		// Step
		function<void(void)>([](void) {
			if (restart) {
				input_refresh();
				++Tries;
				room_goto(rGame);
			}
			// Handle movement
			if (InputQueue.empty()) {
				// Feed inputs
				for (int i = 0; i < int(InstancesList.size()); ++i) {
					GameInstance* inst = InstancesList[i];
					if (inst == noone) continue;
					for (int j = 0; j < inst->get_moves(); ++j) InputQueue.push(i);
				}
			} else {
				int curr = InputQueue.front();
				GameInstance *inst = InstancesList[curr];
				//cout << curr << endl;
		
				if (!instance_exists(inst)) {
					// Instance is destroyed
					InputQueue.pop();
				} else {
					// Instance exists
					if (inst->get_object_type() == PLAYER) {
						if (skip) {
							// Skipped de turn
							input_refresh();
							while (!InputQueue.empty()) {
								if (InputQueue.front() != curr) break;
								InputQueue.pop();
								inst->animate();
							}
						} else {
							int r = inst->get_row()+vdir, c = inst->get_col()+hdir;
							if ((hdir || vdir) && cell_is_valid(r,c)) {
								// Move
								GameInstance* other = instance_get_at_cell(r,c);
								if (other != noone) hit_instance(other,1);
									else inst->set_pos(r,c);
					
								// Direction
								if (hdir) inst->set_face(hdir);

								// Heatmap
								HeatMap = generate_heatmap(inst);
								//log_vec2d(HeatMap);

								// Handle Inputs
								input_refresh();
								InputQueue.pop();
								inst->animate();

							} else if (interact) {
								// Check for bone
								if ((r == BonePos[0]) && (c == BonePos[1])) {
									++Level;
									Kills += CurrKills;
									CurrKills = 0;
									room_goto(rGame);
								}
							}
						}
					} else {
						int mr = INFINITY, mc = INFINITY, dr = 0, dc = 0,
							row = inst->get_row(), col = inst->get_col();

						if (cell_in_bounds(row-1, col)) mr = min(mr, HeatMap[row-1][col]);
						if (cell_in_bounds(row+1, col)) mr = min(mr, HeatMap[row+1][col]);
						if (cell_in_bounds(row, col-1)) mc = min(mc, HeatMap[row][col-1]);
						if (cell_in_bounds(row, col+1)) mc = min(mc, HeatMap[row][col+1]);

						// Player is near
						if ((mr == 0) || (mc == 0)) {
							hit_instance(player, 1);
							if (player->get_hp() <= 0) {
								room_goto(rGameOver);
								return;
							}
						} else if ((mr != INFINITY) || (mc != INFINITY)) {
							if (mc < mr) {
								if (mc <= HeatMap[row][col]) {
									if (cell_in_bounds(row, col-1) && (HeatMap[row][col-1] == mc)) inst->move_relative(0, -1);
										else if (cell_in_bounds(row, col+1) && (HeatMap[row][col+1] == mc)) inst->move_relative(0, 1);
								}
							} else {
								if (mr <= HeatMap[row][col]) {
									if (cell_in_bounds(row-1, col) && (HeatMap[row-1][col] == mr)) inst->move_relative(-1, 0);
										else if (cell_in_bounds(row+1, col) && (HeatMap[row+1][col] == mr)) inst->move_relative(1, 0);
								}
							}
						}
						
						// Set face
						int dir = sign(inst->get_col()-col);
						if (abs(dir)) inst->set_face(dir);

						// End move
						InputQueue.pop();
						inst->animate();

						// Face towards player
						if (instance_exists(player)) {
							if (InputQueue.empty() || (!InputQueue.empty() && (InputQueue.front() != instance_get_number(inst)))) {
								instance_face_towards(inst, player);
							}

							// Generate HeatMap
							HeatMap = generate_heatmap(player);
							log_vec2d(HeatMap);
						}
					}
				}
			}
		}),
		// Draw
		function<void(void)>([](void) {
			// World
			for (int i = GridRows-1; i >= 0; --i) {
				for (int j = 0; j < GridCols; ++j) {
					if (!Grid[i][j]) continue;
			
					Sprite* spr = sGround;

					if (!InputQueue.empty()) {
						GameInstance* instActive = InstancesList[InputQueue.front()];
						GameInstance* inst = instance_get_at_cell(i,j);
						if (instance_exists(instActive)) {
							if ((abs(i-instActive->get_row()) + abs(j-instActive->get_col())) <= 1) {
								spr = sGroundHighlighted;
								if (instance_exists(inst)) {
									if (((instActive->get_object_type() == PLAYER) && (inst->get_object_type() == ENEMY)) ||
										((instActive->get_object_type() == ENEMY) && (inst->get_object_type() == PLAYER))) {
											spr = sGroundAlert;
									}
								}
							}
						}
					}

					// Ground
					draw_sprite(coord_x(j), coord_y(i)-64, spr);

					// Bone
					if ((BonePos[0] == i) && (BonePos[1] == j)) draw_sprite_at_cell(i, j, sBone, BoneScale);
				}
			}
	
			// Instances
			for (int i = 0; i < int(InstancesList.size()); ++i) {
				GameInstance* inst = InstancesList[i];
				draw_instance(inst, 0, 12);
			}

			#pragma region UI
			// Lives
			if (instance_exists(player)) {
				int hx = 20, hy = 504;
				for (int i = 0; i < PLAYER_MAX_HP; ++i) {
					draw_sprite(hx, hy, (i < player->get_hp()) ? sHeartFull : sHeartEmpty);
					hx += 25;
				}
			}

			// Instructions
			draw_sprite(0, 0, levelInstruction);

			int rw, rh, rx, ry, buff = 10;
			string str;

			// Level
			rw = 110, rh = 30;
			rx = WIN_WIDTH-rw, ry = WIN_HEIGHT-rh;
			draw_rectangle_color(rx, ry, rw, rh, c_black, true);
			draw_text_general(rx+buff, ry+buff, "Level: " + string_format(Level,3), c_white, GLUT_BITMAP_9_BY_15);

			// Kills
			ry -= rh-buff;
			draw_rectangle_color(rx, ry, rw, rh, c_black, true);
			draw_text_general(rx+buff, ry+buff, "Kills: " + string_format(Kills,3), c_white, GLUT_BITMAP_9_BY_15);

			// Tries
			ry -= rh-buff;
			draw_rectangle_color(rx, ry, rw, rh, c_black, true);
			draw_text_general(rx+buff, ry+buff, "Tries: " + string_format(Tries,3), c_white, GLUT_BITMAP_9_BY_15);

			#pragma endregion
		})
	);
	#pragma endregion
	
	#pragma region rHighscores
	rHighscore = new Room("Highscores",
		// Create
		function<void(void)>([](void) {
			// Sound
			// PlaySound("Audio/aMenubg.wav", NULL, SND_ASYNC | SND_LOOP);

			// Highscore
			Highscores.clear();
			highscore_push(Score("ghjgh", 456, 213));
			highscore_push(Score(",mn", 999, 23));
			highscore_push(Score("jhg", 100, 213));
			highscore_push(Score("redstg", 456, 34));
			highscore_push(Score("fcvb", 999, 213));
			highscore_push(Score("nbvmh", 456, 23));
			highscore_push(Score("ytu", 999, 999));
			highscore_push(Score("fsdfsdsd", 100, 213));
			highscore_push(Score("sdfsdfsdf", 456, 123));
			highscore_push(Score("qwe", 999, 43));
			highscore_push(Score("qweq", 456, 123213));
		}),
		// Step
		function<void(void)>([](void) {
			if (pause) {
				input_refresh();
				room_goto(rMenu);
			}
		}),
		// Draw
		function<void(void)>([](void) {
			draw_sprite(0, 0, bgHighscores);

			int hy = 350, buff = 40;

			for (int i = 0; i < int(Highscores.size()); ++i) {
				draw_text_color(200, hy, Highscores[i].name, c_black);
				draw_text_color(515, hy, string_format(Highscores[i].kills,3), c_black);
				draw_text_color(715, hy, string_format(Highscores[i].tries,3), c_black);
				hy -= buff;	
			}
		})
	);
	#pragma endregion

	#pragma region rCredits
	rCredits = new Room("Credits",
		// Create
		function<void(void)>([](void) {
			
		}),
		// Step
		function<void(void)>([](void) {
			if (pause) {
				input_refresh();
				room_goto(rMenu);
			}
		}),
		// Draw
		function<void(void)>([](void) {
			draw_sprite(0, 0, bgCredits);
		})
	);
	#pragma endregion

	#pragma region rGameWin
	rGameWin = new Room("GameWin",
		// Create
		function<void(void)>([](void) {
			
		}),
		// Step
		function<void(void)>([](void) {
			
		}),
		// Draw
		function<void(void)>([](void) {
			
		})
	);
	#pragma endregion

	#pragma region rGameOver
	rGameOver = new Room("Game Over",
		// Create
		function<void(void)>([](void) {
			// Pause Timers
			pause_game_timers();
		}),
		// Step
		function<void(void)>([](void) {
			if (restart) {
				input_refresh();
				++Tries;
				room_goto(rGame);
			}
		}),
		// Draw
		function<void(void)>([](void) {
			string text;
			int xx;
			
			xx = WIN_WIDTH/2 - 68;
			text = "YOU";
			draw_text_color(xx, WIN_HEIGHT/2+20, text, c_black);
			
			xx += 60;
			text = "DIED";
			draw_text_color(xx, WIN_HEIGHT/2+20, text, c_red);
			
			xx += 60;
			text = "!";
			draw_text_color(xx, WIN_HEIGHT/2+20, text, c_black);
			
			text = "Press R to restart";
			draw_text_color((WIN_WIDTH-string_width(text))/2, WIN_HEIGHT/2-50, text, c_black);
		})
	);
	#pragma endregion
}

void game_init() {
	// Load sprites
	sprites_init();

	// Load timers
	timers_init();

	// Background
	skyX[0] = 0;
	skyX[1] = bgSky->get_width();
	skySpeed = 1;

	// Load rooms
	Level = 1;
	rooms_init();
	room_goto(rMenu);
}

void handle_pausing() {
	if (room != rGame) return;
	if (pause) {
		Paused = !Paused;
		if (Paused) pause_all_timers();
			else resume_all_timers();
		input_refresh();
	}
	if (Paused) {
		int rw, rh, rx, ry;

		rw = 164, rh = 44;
		rx = (WIN_WIDTH-rw)/2, ry = (WIN_HEIGHT-rh)/2;
		draw_rectangle_color(rx, ry, rw, rh, c_white, true);

		rw = 160, rh = 40;
		rx = (WIN_WIDTH-rw)/2, ry = (WIN_HEIGHT-rh)/2;
		draw_rectangle_color(rx, ry, rw, rh, c_black, true);

		string str = "PAUSED!";
		draw_text_color(WIN_WIDTH/2 - string_width(str,14)/2, WIN_HEIGHT/2-9, str, c_white);
	}
}

// ------------------- iGraphics

#pragma region iGraphics
void iDraw() {
	iClear();

	// Sky
	for (int i = 0; i < 2; ++i) {
		draw_sprite(skyX[i], 0, bgSky);
	}

	// Room
	room->draw();

	// Pause
	handle_pausing();
}

int main() {
	// Window
	iInitialize(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, 750, 200);

	// Game
	game_init();
	noone = new Noone();
	
	// Start
	iStart();

	return 0;
}
#pragma endregion
