#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <termkey.h>

#define LEN(x) (sizeof (x) / sizeof*(x))

char *prompt;

size_t pos, lim;
char line[2048][7]={ 0 };

TermKey* tk;

int dortl(FILE* ttyout);
void printrtl(FILE* ttyout);
void printl(FILE* out);

/* returns 1 if everything is alright, and 0 if ^D or ^C */

int dortl(FILE* ttyout)
{
	TermKeyKey key;
	TermKeyResult result;

	while(1)
	{
		result=termkey_waitkey(tk, &key);

		if(result!=TERMKEY_RES_KEY)
			return 0;

		if(key.type==TERMKEY_TYPE_KEYSYM)
		{
			if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Enter"))
				break;
			else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Backspace"))
			{
				if(pos<LEN(line))
				{
					if(pos!=lim)
						memmove(line+lim+1, line+lim, (pos-lim)*sizeof(*line));
					memset(line[lim], '\0', sizeof(line[lim]));
					if(pos<LEN(line))
						pos++;
					lim++;
				}
			}
			else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Tab"))
			{
				pos--;
				lim--;
				memcpy(line[pos], "\t", strlen("\t"));
			}
			else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Left"))
			{
				if(pos>lim)
					pos--;
			}
			else if(!strcmp(termkey_get_keyname(tk, key.code.sym), "Right"))
				if(pos<LEN(line))
					pos++;
		}
		else if(key.type==TERMKEY_TYPE_UNICODE)
		{
			if(key.modifiers&TERMKEY_KEYMOD_CTRL&&
			  (key.code.codepoint=='C'||key.code.codepoint=='c'||
			   key.code.codepoint=='D'||key.code.codepoint=='d'))
				return 0;
			else
			{
				if(lim>LEN(line)||lim==0)
				{
					fprintf(stderr, "error: input line length limit hit (workaround: edit source and recompile)\n");
					continue;
				}
				lim--;
				pos--;
				if(pos>lim)
					memmove(line+lim, line+lim+1, (pos-lim)*sizeof(*line));
				memcpy(line[pos], key.utf8, LEN(key.utf8));
			}
		}
		printrtl(ttyout);
	}

	return 1;
}

/* prints the line with the correct cursor position */

void printrtl(FILE* ttyout)
{
	fprintf(ttyout, "\33[2K\r");
	fprintf(ttyout, "%s", prompt);
	printl(ttyout);
	fprintf(ttyout, "\33[4096D"); /* lets just hope the user has no wider terminal */
	if(strlen(prompt)>0)
		fprintf(ttyout, "\33[%liC", strlen(prompt));
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

	if(argc>1)
	{
		prompt=malloc(strlen(argv[1])+1);
		memcpy(prompt, argv[1], strlen(argv[1])+1);
		prompt[strlen(argv[1])+1]='\0';
	}
	else
		prompt="";

	ttyout=fopen("/dev/tty", "w");
	setbuf(stdout, NULL);
	setbuf(ttyout, NULL);

	while(1)
	{
		pos=LEN(line);
		lim=LEN(line);
		ret=dortl(ttyout);
		if(!ret)
			break;
		printrtl(ttyout);
		fprintf(ttyout, "\n");
		printl(stdout);
		fprintf(stdout, "\n");
	}

	fclose(ttyout);
	if(argc>1)
		free(prompt);
	termkey_destroy(tk);

	return 0;
}

