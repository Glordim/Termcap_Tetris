#include "area.h"
#include "shapes.h"

#include <termcap.h>
#include <curses.h>

#include <stdio.h>

#ifdef unix
	extern static char termcap_buffer[2048];
#else
	#define termcap_buffer 0
#endif

void init_area()
{
	int y;
	int x;

	y = 0;
	while (y < AREA_HEIGHT)
	{
		x = 0;
		while (x < AREA_WIDTH)
		{
			area[y][x] = AREA_EMPTY;
			++x;
		}
		++y;
	}
}

void draw_area_border()
{
	char *cm_cmd = tgetstr("cm", termcap_buffer);

	int y;
	int x;

	y = 0;
	while (y < AREA_HEIGHT + 2)
	{
		tputs(tgoto(cm_cmd, 0, y), 1, putchar);

		if (y == 0 || y == AREA_HEIGHT + 2 - 1)
		{
			x = 0;
			while (x < AREA_WIDTH * WIDTH_MULTIPLICATER + 2)
			{
				printf("-");
				++x;
			}
		}
		else
		{
			tputs(tgoto(cm_cmd, 0, y), 1, putchar);

			printf("|");

			tputs(tgoto(cm_cmd, AREA_WIDTH * WIDTH_MULTIPLICATER + 1, y), 1, putchar);

			printf("|");
		}

		++y;
	}
}

void write_shape_in_area(struct s_shape *shape)
{
	int *line;

	int y;
	int x;

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
					continue;
				if (shape->x + x < 0)
					continue;

				if (shape->y + y >= AREA_HEIGHT)
					continue;

				line[shape->x + x] = shape->id;
			}
			++x;
		}
		++y;
	}

	check_complete_line();
}

void check_complete_line()
{
	int *line;

	int y;
	int x;

	int line_complete = 1;

	y = 0;
	while (y < AREA_HEIGHT)
	{
		line = area[y];

		line_complete = 1;

		x = 0;
		while (x < AREA_WIDTH)
		{
			if (line[x] == -1)
			{
				line_complete = 0;
				break;
			}

			++x;
		}

		if (line_complete == 1)
		{
			move_down_all_line(y);
			continue;
		}

		++y;
	}
}

void move_down_all_line(int y)
{
	char *ab_cmd = tgetstr("AB", termcap_buffer);
	char *reset_cmd = tgetstr("me", termcap_buffer);
	char *cm_cmd = tgetstr("cm", termcap_buffer);

	int *line;

	int x;

	while (y > 0)
	{
		line = area[y];

		x = 0;
		while (x < AREA_WIDTH)
		{
			if (y == 0)
				line[x] = -1;
			else
				line[x] = area[y - 1][x];

			if (line[x] != -1)
				tputs(tparm(ab_cmd, COLOR_RED + line[x]), 1, putchar);
			else
				tputs(tparm(ab_cmd, COLOR_BLACK), 1, putchar);

			tputs(tgoto(cm_cmd, 1 + x * WIDTH_MULTIPLICATER, y + 1), 1, putchar);

			printf("  ");

			++x;
		}

		--y;
	}

	tputs(reset_cmd, 1, putchar);
}

