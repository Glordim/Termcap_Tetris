#ifndef TERMCAP_INITIALIZER_H
#define TERMCAP_INITIALIZER_H

#include <term.h>
#include <curses.h>

int init_termcap();

struct s_termcap_cmd
{
	char *cl;
	char *cm;
	char *vi;
	char *ve;
	char *ab;
	char *reset;

	char *ku;
	char *kd;
	char *kl;
	char *kr;
};

extern struct s_termcap_cmd tc_cmd;

#endif

