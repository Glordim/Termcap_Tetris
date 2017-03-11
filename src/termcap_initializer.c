#include "termcap_initializer.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef unix
	static char termcap_buffer[2048];
#endif

struct s_termcap_cmd tc_cmd;

int init_termcap()
{
	int ret;
	char *term_name = getenv("TERM");

	if (term_name == NULL)
	{
		fprintf(stderr, "Specify a terminal type with 'TERM=<type>'.\n");
		return -1;
	}

	ret = tgetent(termcap_buffer, term_name);

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

	tc_cmd.cl = tgetstr("cl", termcap_buffer);
	tc_cmd.cm = tgetstr("cm", termcap_buffer);
	tc_cmd.vi = tgetstr("vi", termcap_buffer);
	tc_cmd.ve = tgetstr("ve", termcap_buffer);
	tc_cmd.ab = tgetstr("AB", termcap_buffer);
	tc_cmd.reset = tgetstr("me", termcap_buffer);

	tc_cmd.ku = tgetstr("ku", termcap_buffer);
	tc_cmd.kd = tgetstr("kd", termcap_buffer);
	tc_cmd.kl = tgetstr("kl", termcap_buffer);
	tc_cmd.kr = tgetstr("kr", termcap_buffer);

	return 0;
}

