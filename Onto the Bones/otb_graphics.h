#ifndef __INIT
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "iGraphics.h"
#define __INIT
#endif

#define c_white 0xFFFFFF
#define c_black 0x000000
#define c_red   0xFF0000
#define c_green 0x00FF00
#define c_blue  0x0000FF

// ------------------- System
namespace Sys {
	int currentColor = c_white;
	int circleSmoothness = 75;
};


// ------------------- Color
int draw_get_color() {
	return Sys::currentColor;
}

int rgb_to_hex(int red, int green, int blue) {
	return (((red&0xFF)<<16) | ((green&0xFF)<<8) | (blue&0xFF));
}

std::vector<int> hex_to_rgb(int color) {
	std::vector<int> col(3);
	col[0] = (color&c_red)>>16;
	col[1] = (color&c_green)>>8;
	col[2] = (color&c_blue);
	return col;
}

void draw_set_color(int color) {
	std::vector<int> col = hex_to_rgb(color);
	iSetColor(col[0], col[1], col[2]);
	Sys::currentColor = color;
}

void draw_set_color(int red, int green, int blue) {
	draw_set_color(rgb_to_hex(red, green, blue));
}


// ------------------- Drawing
#pragma region Shapes
void draw_rectangle_color(double left, double bottom, double dx, double dy, int color, bool filled = false) {
	int col = draw_get_color();
	draw_set_color(color);
	double xpos[4] = {left, left+dx, left+dx, left};
	double ypos[4] = {bottom, bottom, bottom+dy, bottom+dy};
	if (filled) iFilledPolygon(xpos, ypos, 4);
		else iPolygon(xpos, ypos, 4);
	draw_set_color(col);
}

void draw_rectangle(double left, double bottom, double dx, double dy, bool filled = false) {
	draw_rectangle_color(left, bottom, dx, dy, draw_get_color(), filled);
}

void draw_circle_color(double x, double y, double radius, int color, bool filled = false, int smoothness = Sys::circleSmoothness) {
	int col = draw_get_color();
	draw_set_color(color);
	if (filled) iFilledCircle(x, y, radius, smoothness);
		else iCircle(x, y, radius, smoothness);
	draw_set_color(col);
}

void draw_circle(double x, double y, double radius, bool filled = false, int smoothness = Sys::circleSmoothness) {
		draw_circle_color(x, y, radius, draw_get_color(), filled, smoothness);
}
#pragma endregion

#pragma region Text
void draw_text_color(double x, double y, char* text, int color) {
	int col = draw_get_color();
	draw_set_color(color);
	iText(x, y, text);
	draw_set_color(col);
}

void draw_text(double x, double y, char* text) {
	draw_text_color(x, y, text, Sys::circleSmoothness);
}
#pragma endregion
