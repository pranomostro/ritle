#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <time.h>

struct back
{
	int cursor;
	int buf[BUFSIZ];
};

/*returns 1 if everything is alright, and 0 if ^D or ^C*/

int dortl(void)
{
	struct back in;
	/*while(no newline)*/
	/*read new rune || arrow key, use libtermkey. Fuck that 'deprecated!' thing.*/
	/*display the current thing || move the cursor position*/
	printrtl(in);
	/*repeat*/
	/*print the content of in to stdout, not on the screen*/
}

/*prints in with the correct cursor position*/

void printrtl(int align, struct back in)
{
	printf("\33[2K\r");
	/*print the line*/
	/*set cursor position*/
}

int main(int argc, char* argv[])
{
	int c;

	/*argument handling here, maybe some sort of alignment control*/
	/*with -a [lmr] (left, middle, right)*/

	do
		c=dortl();
		/*newline*/
	while(c);

	return 0;
}

