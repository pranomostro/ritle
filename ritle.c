#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <termkey.h>

#define LEN(x) (sizeof (x) / sizeof*(x))

size_t pos, lim;
char line[2048][7] = { 0 };

TermKey* tk;

int dortl(FILE* ttyout);
void printrtl(FILE* ttyout);
void printl(FILE* out);

/* returns 1 if everything is alright, and 0 if ^D or ^C */

int dortl(FILE* ttyout)
{
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
			if(lim<LEN(line))
			{
				if(pos<LEN(line))
					pos++;
				lim++;
			}
			continue;
		}
		else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Tab"))
		{
			pos--;
			lim--;
			memcpy(line[pos], "\t", strlen("\t"));
			continue;
		}
		else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Left"))
		{
			if(pos>lim)
				pos--;
			continue;
		}
		else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Right"))
		{
			if(pos<LEN(line))
				pos++;
			continue;
		}

		if(key.type!=TERMKEY_TYPE_UNICODE)
			continue;

		if(key.modifiers&TERMKEY_KEYMOD_CTRL&&
		   (key.code.codepoint=='C'||key.code.codepoint=='c'||
		    key.code.codepoint=='D'||key.code.codepoint=='d'))
			return 0;
		else
		{
			lim--;
			if(pos>LEN(line)) /* instead have an error? */
				break;
			memcpy(line[lim], key.utf8, LEN(key.utf8));
		}
	}

	return 1;
}

/* prints the line with the correct cursor position */

void printrtl(FILE* ttyout)
{
	fprintf(ttyout, "\33[2K\r");
	printl(ttyout);
	fprintf(ttyout, "\33[4096D"); /* lets just hope the user has no wider terminal */
	if(pos-lim>0)
		fprintf(ttyout, "\33[%liC", pos-lim);
}

/* prints the current line to the file */

void printl(FILE* out)
{
	size_t i;

	for(i=lim; i<LEN(line); i++)
		fprintf(out, "%s", line[i]);
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
		lim=LEN(line);
		ret=dortl(ttyout);
		printrtl(ttyout);
		fprintf(ttyout, "\n");
		printl(stdout);
		fprintf(stdout, "\n");
	}
	while(ret);

	fclose(ttyout);
	termkey_destroy(tk);

	return 0;
}

