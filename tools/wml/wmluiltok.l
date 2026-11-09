%top{
/* Feature macros must precede the scanner skeleton system headers. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
}
%{
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/*
 * HISTORY
 */

#if defined(__STDC__)
#include <string.h>
#include <stdlib.h>
#endif

#ifndef XmConst
#if defined(__STDC__) || !defined( NO_CONST )
#define XmConst const
#else
#define XmConst
#endif /* __STDC__ */
#endif /* XmConst */

	/*
	 * lex program to construct token data for WML to generate token
	 * table data.
 	 *
	 * stdin is the file Uil.y
	 *
	 * it is searched for each occurance of a line starting with '%token'
	 * when found various fields are extracted.
	 *
	 * when Uil.y has been scanned the collected data is written
	 * to stdout.
	 */

%}

%a 9999						/* transitions */

%{

#include <stdio.h>

#define TRUE  1
#define FALSE 0

#define MAX_SYN 10
#define MAX_TOKENS 1000

#ifdef yywrap
#undef yywrap
#endif

typedef struct _token
{
    char * name;
    char * id;
    char * class;
    int    num_syn;
    char * syn[MAX_SYN];
}
    TokenRec, *Token;

TokenRec token[1000];
TokenRec temp;

void add_token(Token t);
char *copy_token_text(const char *text);
int yywrap(void);

int phase		= 0;
int keyword_count;
int line_num		= 0;
int in_comment		= 0;
int in_include		= 0;
int in_token		= 0;
int state		= 0;
int used		= 0;
int token_error		= 0;

%}



%%


[\n]		{
		    if (in_token == TRUE)
		    {
			add_token (&temp);
		    }

		    in_token = FALSE;
		    line_num++;
		}

[ \t]		{				/* swallow */
		} 


"/\*"		{				/* swallow */
		}


^%token		{
		    in_token = TRUE;
		    state = 0;
		}

[a-zA-Z0-9_]+	{
		    if (in_token == TRUE)
		    {
			switch (state)
			{
			    case 0:
				temp.name = copy_token_text ((XmConst char *)yytext);

				temp.num_syn = 0; 
				break;

			    case 1:
				temp.id = copy_token_text ((XmConst char *)yytext);
				break;

			    case 2:
				temp.class = copy_token_text ((XmConst char *)yytext);
				break;

			    case 3:
				if (temp.num_syn >= MAX_SYN)
				{
				    fprintf (stderr,
					"ERROR, too many synonyms, line %d\n",
					line_num);
				    token_error = 1;
				}
				else
				{
				    temp.syn[temp.num_syn] =
					copy_token_text ((XmConst char *)yytext);
				    temp.num_syn++;
				}
				break;
			}

			state++;
		    }
		}

.		{				/* swallow */
		}

%%


char *
copy_token_text (const char *text)
{
    char *copy;
    size_t length;

    length = strlen (text);
    if (length == (size_t)-1)
	{
	fprintf (stderr, "ERROR, WML token is too long, line %d\n", line_num);
	exit (EXIT_FAILURE);
	}
    copy = (char *) malloc (length + 1);
    if (copy == NULL)
	{
	fprintf (stderr, "ERROR, unable to copy WML token, line %d\n", line_num);
	exit (EXIT_FAILURE);
	}
    memcpy (copy, text, length + 1);
    return copy;
}


void
add_token(Token t) /* keep sorted by name */
{
    int i, j;

    if (used >= MAX_TOKENS)
	{
	    fprintf (stderr, "ERROR, too many tokens, line %d\n", line_num);
	    token_error = 1;
	    free (t->name); free (t->id); free (t->class);
	    for (i = 0; i < t->num_syn; i++) free (t->syn[i]);
	    return;
	}

    for (j=0; j<used; j++)			/* for each token */
    {
	if (strcmp (token[j].name, t->name) > 0)	/* goes here */
  	{
	    for (i=used++; i>j; i--)		/* make hole */
		token[i] = token[i-1];

	    token[j] = *t;			/* insert it */

	    return;
	}
    }

    /*
     * if we get there then it goes at the end of the list 
     */

    token[used++] = *t;
}



int
main (void)
{
    yylex ();
    return token_error ? 1 : 0;
}

int yywrap(void)
{
    int i;

    for (i=0; i<used; i++)
	 { 
	 printf ("%s %s %s\n", token[i].name, token[i].id, token[i].class);
	 }
    return (1);
}
