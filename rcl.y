%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "rcl.h"

extern int yylex(void);												/* yyparse() needs it */
extern int yyparse(struct S_RCL_TYPE * parse_param);				/* because we define a custom "main()" */
void yyerror(struct S_RCL_TYPE * parse_param, const char* s, ...);	/* custom error report */

int yylineno;	/* global variable for error riport */

extern char rcl_error_msg[RCL_MSG_SIZE];	/* buffer for error messages */

%}

%error-verbose										/* verbose error messages */
%parse-param { struct S_RCL_TYPE * parse_param }	/* parameter definition for yyparse() */

%union { char * s; int i; struct S_RCL_TYPE * rcl_val }	/* lex return types */
%token <i>OPERATION
%token <s>ATTRIBUTE
%token <rcl_val>VALUE
%token <i>OP_GET <i>OP_SET <i>OP_RESET <i>OP_START <i>OP_STOP
%left L_BEGIN L_END
%left COMMA NLINE
%token <i>UNKNOWN

%type <rcl_val>PARAM
%type <rcl_val>LIST
%type <rcl_val>LIST_ELEMENTS

%start COMMANDS

%%	/* start of rules */

COMMANDS : COMMAND NLINE				{ ; }
		 | COMMANDS COMMAND NLINE		{ ; }
		 ;

COMMAND : OPERATION ATTRIBUTE			{ rcl_execute(parse_param, $1, $2, NULL); }
		| OPERATION ATTRIBUTE PARAM		{ rcl_execute(parse_param, $1, $2, $3);   }
		;

PARAM : VALUE							{ $$ = $1; }
	  | LIST							{ $$ = $1; }
	  ;


LIST : L_BEGIN L_END					{ $$ = NULL; }	/* empty list */
	 | L_BEGIN LIST_ELEMENTS L_END		{ $$ = $2;	 }
	 ;

LIST_ELEMENTS : PARAM						{ $$ = rcl_list($1); 			}
			  | LIST_ELEMENTS COMMA PARAM	{ $$ = rcl_list_add($1, $3); 	}
			  ;

%% /* end of rules */

void yyerror(struct S_RCL_TYPE * parse_param, const char* s, ...) {
	va_list args;
	char msg[128];

    va_start(args, s);
	vsprintf(msg, s, args);
	va_end(args);

	sprintf(rcl_error_msg, "line %d: %s\n", yylineno, msg);
}
