#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <termkey.h>

#define LEN(x) (sizeof (x) / sizeof*(x))

size_t pos;
char line[2048][7] = { 0 };

TermKey* tk;

int dortl(FILE* ttyout);
void printrtl(FILE* ttyout);

/*returns 1 if everything is alright, and 0 if ^D or ^C*/

int dortl(FILE* ttyout)
{
	size_t i;
	TermKeyKey key;
	TermKeyResult result;

	for(;;)
	{
		printrtl(ttyout);
		result=termkey_waitkey(tk, &key);

		if(result!=TERMKEY_RES_KEY)
			return 0;

		if(key.type!=TERMKEY_TYPE_KEYSYM)
			;
		else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Enter"))
			break;
		else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Backspace"))
		{
			if(pos<LEN(line))
				pos++;
			continue;
		}
		else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Tab"))
		{
			pos--;
			memcpy(line[pos], "\t", strlen("\t"));
			continue;
		}
		else
			printf("%s\n", termkey_get_keyname(tk, key.code.sym));

		if(key.type!=TERMKEY_TYPE_UNICODE)
			continue;

		if(key.modifiers&TERMKEY_KEYMOD_CTRL&&
		   (key.code.codepoint=='C'||key.code.codepoint=='c'||
		    key.code.codepoint=='D'||key.code.codepoint=='d'))
			return 0;
		else
		{
			pos--;
			if(pos>LEN(line)) /* instead have an error? */
				break;
			memcpy(line[pos], key.utf8, LEN(key.utf8));
		}
	}
	printrtl(ttyout);
	fprintf(ttyout, "\n");

	for(i=pos; i<LEN(line); i++)
		printf("%s", line[i]);
	printf("\n");

	return 1;
}

/*prints the line with the correct cursor position*/

void printrtl(FILE* ttyout)
{
	size_t i;

	fprintf(ttyout, "\33[2K\r");
	for(i=pos; i<LEN(line); i++)
		fprintf(ttyout, "%s", line[i]);
	fprintf(ttyout, "\33[255D");
}

int main(int argc, char* argv[])
{
	int ret;
	FILE* ttyout;

	TERMKEY_CHECK_VERSION;

	tk=termkey_new(0, 0);

	if(!tk)
	{
		fprintf(stderr, "%s: could not initialize termkey.\n", argv[0]);
		exit(1);
	}

	ttyout=fopen("/dev/tty", "w");
	setbuf(stdout, NULL);
	setbuf(ttyout, NULL);

	do
	{
		pos=LEN(line);
		ret=dortl(ttyout);
	}
	while(ret);

	fclose(ttyout);
	termkey_destroy(tk);

	return 0;
}

