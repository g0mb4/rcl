%{

#include <stdio.h>
#include <string.h>

#include "rcl.h"
#include "rcl.tab.h"

extern void yyerror(const char * s, ...);

char *strdupquotes(const char * s);
uint32_t bin2int(const char * s);
uint32_t hex2int(const char * s);

%}
%option yylineno

CHARS		[[:print:]]
DIGITS 		[0-9]
INT	 		[-+]?{DIGITS}+
DBL 		{INT}[.]{DIGITS}+
STR			[\"]{CHARS}+[\"]
ATTR        [a-zA-Z_][0-9a-zA-Z_]+

BININT		0[bB][0-1]+
HEXINT		0[xX][0-9a-fA-F]+

%%
{STR}		{	yylval.rcl_val = rcl_string(strdupquotes(yytext));	/* string */
				return VALUE; };
{INT}  		{	yylval.rcl_val = rcl_int(atoi(yytext));	/* integer */
				return VALUE; };
{DBL}  		{	yylval.rcl_val = rcl_double(atof(yytext));	/* double */
				return VALUE; };
{BININT}	{	yylval.rcl_val = rcl_uint(bin2int(yytext));	/* binary integer */
				return VALUE; };
{HEXINT}	{	yylval.rcl_val = rcl_uint(hex2int(yytext));	/* binary integer */
				return VALUE; };

"get"		{	yylval.i = OP_GET;
				return OPERATION; 	};
"set"		{ 	yylval.i = OP_SET;
				return OPERATION;	};
"reset"		{ 	yylval.i = OP_RESET;
				return OPERATION;	};
"start"		{	yylval.i = OP_START;
				return OPERATION;	};
"stop"		{	yylval.i = OP_STOP;
				return OPERATION;	};

{ATTR}		{  yylval.s = strdup(yytext);
			   return ATTRIBUTE;	};

\[ 			{	return L_BEGIN;	};			/* list */
\] 			{	return L_END; 	};
, 			{	return COMMA; 	};
\n			{	return NLINE;	};
[ \t\r]+ 	{ 	; } 						/* ignore whitespace */
. 			{	yyerror("Unrecognized input: %s\n", yytext);
				exit(0); }
%%

char *strdupquotes(const char * s){
	int i, j = 0, len = strlen(s);

	/*  empty string */
	if(len <= 2){
		return NULL;
	}

	char *d = malloc(strlen(s) - 1);   /* lenght - 2 quotes + 1 terminator */

   	if (d == NULL)
		return NULL;

	for(i = 1; i < (len - 1); i++){
		d[j++] = s[i];
	}
	d[j] = '\0';

   	return d;
}

uint32_t bin2int(const char * s){
	return (uint32_t)strtol((s+2), NULL, 2);
}

uint32_t hex2int(const char * s){
	return (uint32_t)strtol((s+2), NULL, 16);
}

/* for the compiler */
int yywrap(void){
   return 1;
}
