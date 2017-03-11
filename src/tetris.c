#define _POSIX_C_SOURCE 199309L

#include "tetris.h"
#include "area.h"
#include "shapes.h"

#include <termcap.h>
#include <curses.h>

#include <string.h>
#include <unistd.h>
#include <time.h>

#include <stdlib.h>
#include <signal.h>

#include <sys/select.h>

#ifdef unix
	extern static char termcap_buffer[2048];
#else
	#define termcap_buffer 0
#endif

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

	char *cm_cmd = tgetstr("cm", termcap_buffer);
	char *vi_cmd = tgetstr("vi", termcap_buffer);
	char *ve_cmd = tgetstr("ve", termcap_buffer);

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 5000;

	srand(time(0));
	tputs(vi_cmd, 1, putchar); /* Masque le curseur */

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

	tputs(ve_cmd, 1, putchar);
	tputs(tgoto(cm_cmd, 0, 2 + AREA_HEIGHT), 1, putchar);
	fflush(stdout);

	return 0;
}

void check_input(struct s_shape *current_shape, struct s_shape *shadow_shape, long *last_time)
{
	char *kl_cmd = tgetstr("kl", termcap_buffer);
	char *kr_cmd = tgetstr("kr", termcap_buffer);
	char *kd_cmd = tgetstr("ku", termcap_buffer);
	char *ku_cmd = tgetstr("kd", termcap_buffer);

	int i = 0;
	char read_buffer[64];
	struct timeval timeout;

	fd_set read_fd_set;

	FD_ZERO(&read_fd_set);
	FD_SET(0, &read_fd_set);

	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	i = select(1, &read_fd_set, NULL, NULL, &timeout);

	if (i == -1)
	{

	}
	else if (i == 0)
	{

	}
	else
	{
		i = 0;
		while (i < FD_SETSIZE)
		{
			if (FD_ISSET(i, &read_fd_set) != 0)
			{
				memset(read_buffer, 0, 64);
				read(0, read_buffer, 64);
				
				if (strncmp(read_buffer, kl_cmd, strlen(kl_cmd)) == 0 || read_buffer[0] == 'a') /* q for azerty layout */
				{
					--current_shape->x;
				}
				else if (strcmp(read_buffer, kr_cmd) == 0 || read_buffer[0] == 'd')
				{
					++current_shape->x;
				}
				else if (strcmp(read_buffer, kd_cmd) == 0 || read_buffer[0] == 's')
				{
					*last_time = 0;
				}
				else if (strcmp(read_buffer, ku_cmd) == 0 || read_buffer[0] == 'w') /* z for azerty layout */
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
			++i;
		}
	}
}

void clear_term()
{
	char *cl_cmd = tgetstr("cl", termcap_buffer);

	tputs(cl_cmd, 1, putchar);
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

	char *ab_cmd = tgetstr("AB", termcap_buffer);
	char *reset_cmd = tgetstr("me", termcap_buffer);
	char *cm_cmd = tgetstr("cm", termcap_buffer);

	tputs(tparm(ab_cmd, color), 1, putchar);

	y = 0;
	while (y < SHAPE_SIZE)
	{
		x = 0;
		while (x < SHAPE_SIZE)
		{
			if (shapes[shape->id][shape->orientation][y][x] != 0)
			{
				tputs(tgoto(cm_cmd, 1 + shape->x * WIDTH_MULTIPLICATER + x * WIDTH_MULTIPLICATER, shape->y + 1 + y), 1, putchar);

				printf("  ");
			}
			++x;
		}
		++y;
	}

	tputs(reset_cmd, 1, putchar);
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

