#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "common.h"

char getch_blocking(void)
{
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("getch() tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("getch() tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
 		perror("getch() read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("getch() tcsetattr ~ICANON");

	return buf;
}

char getch_non_blocking(void)
{
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("getch() tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("getch() tcsetattr ICANON");

	int count;
	if (ioctl(0, FIONREAD, &count) < 0)
		perror("getch() ioctl");
	if (count > 0)
	{
		if (read(0, &buf, 1) < 0)
	 		perror("getch() read()");
	}

	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("getch() tcsetattr ~ICANON");

	return buf;
}
