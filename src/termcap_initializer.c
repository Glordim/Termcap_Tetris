#include "termcap_initializer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*#define USE_TERMCAP*/

struct s_termcap_cmd tc_cmd;

int init_termcap()
{
#ifdef USE_TERMCAP
	int ret;
	char *term_name = getenv("TERM");

	if (term_name == NULL)
	{
		fprintf(stderr, "Specify a terminal type with 'TERM=<type>'.\n");
		return -1;
	}

	ret = tgetent(NULL, term_name);

	if (ret == -1)
	{
		fprintf(stderr, "Could not access the termcap data base.\n");
		return -1;
	}
	else if (ret == 0)
	{
		fprintf(stderr, "Terminal type '%s' is not defined in termcap database (or too little information).\n", term_name);
		return -1;
	}

	tc_cmd.cl = tgetstr("cl", NULL);
	tc_cmd.cm = tgetstr("cm", NULL);
	tc_cmd.vi = tgetstr("vi", NULL);
	tc_cmd.ve = tgetstr("ve", NULL);
	tc_cmd.ab = tgetstr("AB", NULL);
	tc_cmd.reset = tgetstr("me", NULL);

	tc_cmd.ku = tgetstr("ku", NULL);
	tc_cmd.kd = tgetstr("kd", NULL);
	tc_cmd.kl = tgetstr("kl", NULL);
	tc_cmd.kr = tgetstr("kr", NULL);
#else

	if (setupterm(NULL, STDOUT_FILENO, NULL) != 0)
	{
		return -1;
	}

	tc_cmd.cl = tigetstr("clear");
	tc_cmd.cm = tigetstr("cup");
	tc_cmd.vi = tigetstr("civis");
	tc_cmd.ve = tigetstr("cnorm");
	tc_cmd.ab = tigetstr("setab");
	tc_cmd.reset = tigetstr("sgr0");

	tc_cmd.ku = tigetstr("kcuu1");
	tc_cmd.kd = tigetstr("kcud1");
	tc_cmd.kl = tigetstr("kcub1");
	tc_cmd.kr = tigetstr("kcuf1");

#endif
	return 0;
}

