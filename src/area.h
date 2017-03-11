#ifndef AREA_H
#define AREA_H

#include "tetris.h"

#define AREA_EMPTY -1
#define AREA_WIDTH 10
#define AREA_HEIGHT 22
#define WIDTH_MULTIPLICATER 2

int area[AREA_HEIGHT][AREA_WIDTH];

void init_area();
void draw_area_border();
void write_shape_in_area(struct s_shape *shape);
void check_complete_line();
void move_down_all_line(int y);

#endif

