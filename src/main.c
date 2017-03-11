#include "termcap_initializer.h"
#include "tetris.h"

#include <termios.h>

int main(int argc, char **argv)
{
	struct termios s_termios;
	struct termios s_termios_backup;
	int ret = 0;

	/* Supression Unused Warning */
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

