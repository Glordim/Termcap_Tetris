#ifndef TETRIS_H
#define TETRIS_H

struct s_shape
{
	int id;
	int x;
	int y;
	int orientation;
};

int tetris();
void clear_term();
void next_shape(struct s_shape *shape);
void draw_shape(struct s_shape *shape, int color);
void check_input(struct s_shape *currentShape, struct s_shape *shadowShape, long *lastTime);
int test_collision(struct s_shape *shape);

#endif

