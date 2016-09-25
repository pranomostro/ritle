#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <time.h>

int getkey(void) {
	int c;
	struct termios orig, new;

	tcgetattr(fileno(stdin), &orig);
	memcpy(&new, &orig, sizeof(struct termios));
	new.c_lflag&=~(ICANON|ECHO|ISIG);
	new.c_cc[VTIME]=0;
	new.c_cc[VMIN]=1;
	tcsetattr(fileno(stdin), TCSANOW, &new);

	c=fgetc(stdin);
	fflush(stdin);

	tcsetattr(fileno(stdin), TCSANOW, &orig);

	return c;
}

int main(int argc, char* argv[])
{
	int c, buf[BUFSIZ];
	size_t len;

	while((c=getkey())!=-1&&c!=0x1B&&c!=0x04)
	{
		/*here print the current saved line*/
		printf("\33[2K\r");
	}

	return 0;
}

