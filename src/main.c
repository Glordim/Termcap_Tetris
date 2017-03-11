#include <stdlib.h>
#include <stdio.h>

#include <termcap.h>
#include <termios.h>

#ifdef unix
	static char termcap_buffer[2048];
#else
	#define termcap_buffer 0
#endif

#include "tetris.h"

int init_termcap();

int main(int argc, char **argv)
{
	struct termios s_termios;
	struct termios s_termios_backup;
	int ret = 0;

	/* On evite les warnings pour variables non utilisées. */
	(void)argc;
	(void)argv;

	if (init_termcap() == 0)
	{
		if (tcgetattr(0, &s_termios) == -1)
			return (-1);

		if (tcgetattr(0, &s_termios_backup) == -1)
			return (-1);

		s_termios.c_lflag &= ~(ICANON); /* Met le terminal en mode canonique. La fonction Read recevra les entrées clavier en direct sans attendre qu'on appuie sur Enter */
		s_termios.c_lflag &= ~(ECHO); /* Les touches tapées au clavier ne s'affficheront plus dans le terminal */

		if (tcsetattr(0, 0, &s_termios) == -1)
			return (-1);

		ret = tetris();

		if (tcsetattr(0, 0, &s_termios_backup) == -1)
			return (-1);
	}

	return ret;
}

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

	return 0;
}

