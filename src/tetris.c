#define _POSIX_C_SOURCE 199309L

#include "termcap_initializer.h"
#include "tetris.h"
#include "area.h"
#include "shapes.h"

#include <curses.h>

#include <string.h>
#include <unistd.h>
#include <time.h>

#include <stdlib.h>
#include <signal.h>

#include <sys/select.h>

int keep_running;

void sigint_handler(int dummy)
{
	(void)dummy;
	keep_running = 0;
}

int tetris()
{
	struct s_shape current_shape;
	struct s_shape shadow_shape;

	long last_time = time(0);

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 5000;

	srand(time(0));
	tputs(tc_cmd.vi, 1, putchar); /* Masque le curseur */

	clear_term();

	init_area();
	draw_area_border();

	current_shape.id = -1;

	if (signal(SIGINT, sigint_handler) == SIG_ERR)
		return -1;

	keep_running = 1;
	while (keep_running != 0)
	{
		if (current_shape.id == -1)
		{
			next_shape(&current_shape);
			draw_shape(&current_shape, COLOR_RED + current_shape.id);

			shadow_shape = current_shape;
		}

		check_input(&current_shape, &shadow_shape, &last_time);

		if (time(0) != last_time)
		{
			last_time = time(0);
			++current_shape.y;

			if (test_collision(&current_shape) != 0)
			{
				--current_shape.y;
				draw_shape(&shadow_shape, COLOR_BLACK);
				draw_shape(&current_shape, COLOR_RED + current_shape.id);

				/* inscription dans area*/
				write_shape_in_area(&current_shape);

				current_shape.id = -1;
				shadow_shape = current_shape;
			}
		}

		if (shadow_shape.x != current_shape.x || shadow_shape.y != current_shape.y || shadow_shape.orientation != current_shape.orientation)
		{
			draw_shape(&shadow_shape, COLOR_BLACK);

			shadow_shape = current_shape;

			draw_shape(&current_shape, COLOR_RED + current_shape.id);
		}

		fflush(stdout);

		nanosleep(&ts, NULL);
	}

	tputs(tc_cmd.ve, 1, putchar);
	tputs(tgoto(tc_cmd.cm, 0, 2 + AREA_HEIGHT), 1, putchar);
	fflush(stdout);

	return 0;
}

void check_input(struct s_shape *current_shape, struct s_shape *shadow_shape, long *last_time)
{
	int i = 0;
	char read_buffer[64] = {0};
	struct timeval timeout;

	fd_set read_fd_set;

	FD_ZERO(&read_fd_set);
	FD_SET(STDIN_FILENO, &read_fd_set);

	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	i = select(STDIN_FILENO + 1, &read_fd_set, NULL, NULL, &timeout);

	if (i == -1)
	{

	}
	else if (i == 0)
	{

	}
	else
	{
		if (FD_ISSET(STDIN_FILENO, &read_fd_set) != 0)
		{
			read(STDIN_FILENO, read_buffer, sizeof(read_buffer));

			if (strncmp(read_buffer, tc_cmd.kl, strlen(tc_cmd.kl)) == 0 || read_buffer[0] == 'a') /* q for azerty layout */
			{
				--current_shape->x;
			}
			else if (strncmp(read_buffer, tc_cmd.kr, strlen(tc_cmd.kr)) == 0 || read_buffer[0] == 'd')
			{
				++current_shape->x;
			}
			else if (strncmp(read_buffer, tc_cmd.kd, strlen(tc_cmd.kd)) == 0 || read_buffer[0] == 's')
			{
				*last_time = 0;
			}
			else if (strncmp(read_buffer, tc_cmd.ku, strlen(tc_cmd.ku)) == 0 || read_buffer[0] == 'w') /* z for azerty layout */
			{
				current_shape->orientation = (current_shape->orientation + 1) % NB_ROTATIONS;
			}

			if (shadow_shape->x != current_shape->x || shadow_shape->y != current_shape->y || shadow_shape->orientation != current_shape->orientation)
			{
				if (test_collision(current_shape) != 0)
				{
					*current_shape = *shadow_shape;
				}
			}
		}
	}
}

void clear_term()
{
	tputs(tc_cmd.cl, 1, putchar);
	fflush(stdout);
}

void next_shape(struct s_shape *shape)
{
	shape->id = rand() % NB_SHAPES;
	shape->orientation = 0;
	shape->y = 0;
	shape->x = 4;
}

void draw_shape(struct s_shape *shape, int color)
{
	int y;
	int x;

	tputs(tparm(tc_cmd.ab, color), 1, putchar);

	y = 0;
	while (y < SHAPE_SIZE)
	{
		x = 0;
		while (x < SHAPE_SIZE)
		{
			if (shapes[shape->id][shape->orientation][y][x] != 0)
			{
				tputs(tgoto(tc_cmd.cm, 1 + shape->x * WIDTH_MULTIPLICATER + x * WIDTH_MULTIPLICATER, shape->y + 1 + y), 1, putchar);

				printf("  ");
			}
			++x;
		}
		++y;
	}

	tputs(tc_cmd.reset, 1, putchar);
}

int test_collision(struct s_shape *shape)
{
	int y;
	int x;

	int *line;

	y = 0;
	while (y < SHAPE_SIZE)
	{
		line = area[y + shape->y];

		x = 0;
		while (x < SHAPE_SIZE)
		{
			if (shapes[shape->id][shape->orientation][y][x] != 0)
			{
				if (shape->x + x >= AREA_WIDTH)
					return 1;
				if (shape->x + x < 0)
					return 1;

				if (shape->y + y >= AREA_HEIGHT)
					return 1;

				if (line[shape->x + x] != -1)
					return 1;
			}
			++x;
		}
		++y;
	}

	return 0;
}

