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
#include <stdio.h>
#endif

#ifndef XmConst
#if defined(__STDC__) || !defined( NO_CONST )
#define XmConst const
#else
#define XmConst
#endif /* __STDC__ */
#endif /* XmConst */

#ifdef YY_BUFFER_SIZE
#define BUF_SIZE YY_BUFFER_SIZE
#else
#define BUF_SIZE 8192
#endif

char            yystringval[BUF_SIZE];      /* any string value */
char            yynameval[BUF_SIZE];         /* any identifier (name) */
int             yytknval1;              /* terminal token value 1 */
int             yytknval2;              /* terminal token value 2 */

/* Flex can assemble a token across refill boundaries. Keep the legacy
 * fixed-size semantic buffers, but reject a token that cannot fit instead of
 * letting strcpy/strncpy overwrite the next parser buffer. */
static int wml_copy_token(char *destination, size_t capacity,
                          const char *source, size_t length)
{
        if (length >= capacity) {
                fprintf(stderr, "WML token is too long (maximum %lu bytes)\n",
                        (unsigned long)(capacity - 1));
                return 0;
        }
        memcpy(destination, source, length);
        destination[length] = '\0';
        return 1;
}

%}

%p 3000
%%

[ \t\f]		{}

[\n]		{ wml_line_count += 1; }

"Class"			{ return CLASS; }
"Resource"		{ return RESOURCE; }
"DataType"		{ return DATATYPE; }
"ControlList"		{ return CONTROLLIST; }
"EnumerationSet"	{ return ENUMERATIONSET; }
"EnumerationValue"	{ return ENUMERATIONVALUE; }
"CharacterSet"		{ return CHARACTERSET; }
"Child"			{ return CHILD; }

"DocName"		{ return DOCNAME; }
"ConvenienceFunction"	{ return CONVFUNC; }
"Alias"			{ return ALIAS; }
"Type"			{ return TYPE; }
"ResourceLiteral"	{ return RESOURCELITERAL; }
"Related"		{ return RELATED; }
"InternalLiteral"	{ return INTERNALLITERAL; }
"Constraint"		{ return CONSTRAINT; }
"Exclude"		{ return EXCLUDE;}
"Resources"		{ return RESOURCES; }
"SuperClass"		{ return SUPERCLASS; }
"ParentClass"		{ return PARENTCLASS; }
"Controls"		{ return CONTROLS; }
"WidgetClass"		{ return WIDGETCLASS; }
"DialogClass"		{ return DIALOGCLASS; }
"Default"		{ return DEFAULT; }
"EnumLiteral"		{ return ENUMLITERAL; }
"XmStringCharsetName"	{ return XMSTRINGCHARSETNAME; }
"FontListElementTag"	{ return XMSTRINGCHARSETNAME; }
"Direction"		{ return DIRECTION; }
"ParseDirection"	{ return PARSEDIRECTION; }
"CharacterSize"		{ return CHARACTERSIZE; }
"ControlsMapToResource"	{ return CTRLMAPSRESOURCE; }
"Children"		{ return CHILDREN; }

"MetaClass"		{ return METACLASS;}
"Widget"		{ return WIDGET;}
"Gadget"		{ return GADGET;}
"Argument"		{ return ARGUMENT;}
"Reason"		{ return REASON;}
"SubResource"		{ return SUBRESOURCE;}
"True"			{ return ATTRTRUE; }
"False"			{ return ATTRFALSE; }
"LeftToRight"		{ return LEFTTORIGHT; }
"RightToLeft"		{ return RIGHTTOLEFT; }
"OneByte"		{ return ONEBYTE; }
"TwoByte"		{ return TWOBYTE; }
"MixedOneAndTwoByte"	{ return MIXED1_2BYTE; }

":"			{ return COLON; }
";"			{ return SEMICOLON; }
"="			{ return EQUALS; }
"{"			{ return LBRACE; }
"}"			{ return RBRACE; }

"!"[^\n]*	{}

"#"[^\n]*	{}

[a-zA-Z_][a-zA-Z0-9$_]* {	/* string without quotes	*/
		if (!wml_copy_token(yystringval, sizeof(yystringval),
		                    (XmConst char *) yytext, (size_t)yyleng))
			return ERRORTOKEN;
	return STRING;
	}

\"[^"\n]*\\	{	/* escaped character in the string 	*/
		yymore();
		}

\"[^"\n]*\"	{	/* String in quotes	*/
			if (!wml_copy_token(yystringval, sizeof(yystringval),
			                    (XmConst char *) yytext + 1,
			                    (size_t)yyleng - 2))
				return ERRORTOKEN;
		return STRING;
		}
\"[^"\n]*	{
		printf ("\nUnterminated string near %s, line %d",
			yytext, wml_line_count);
		return ERRORTOKEN;
		}

.	{
	printf ("\nEncountered illegal character '%c', line %d",
		yytext[0], wml_line_count);
	return ERRORTOKEN;
	}
