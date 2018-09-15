%{ /* user defined code needed for the rules */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "rcl.h"

/* prototypes */
extern int yylex(void);												/* yyparse() needs it */
extern int yyparse(struct S_RCL_TYPE * parse_param);				/* because we define a custom "main()" */
void yyerror(struct S_RCL_TYPE * parse_param, const char* s, ...);	/* custom error report */

int yylineno;	/* global variable for the line number, used in error riport */
extern char rcl_error_msg[RCL_MSG_SIZE];	/* buffer for the error messages, defined in rcl.h */

%} /* end of the user defined code */

/* bison definitions */

%error-verbose										/* verbose error messages */
%parse-param { struct S_RCL_TYPE * parse_param }	/* parameter definition for yyparse() */

%union { char * s; int i; struct S_RCL_TYPE * rcl_val }	/* lex return types */

/* token + types */
%token <i>OPERATOR
%token <s>ATTRIBUTE
%token <rcl_val>VALUE
%token <i>OP_GET <i>OP_SET <i>OP_RESET <i>OP_START <i>OP_STOP
%token <i>UNKNOWN

/**/
%left L_BEGIN L_END
%left COMMA NLINE

/* type of the productions */
%type <rcl_val>PARAM
%type <rcl_val>LIST
%type <rcl_val>LIST_ELEMENTS

%start COMMANDS		/* root of the parse tree */

%%	/* start of the rules */

/* productions*/

COMMANDS : COMMAND NLINE				{ ; }
		 | COMMANDS COMMAND NLINE		{ ; }
		 ;

COMMAND : OPERATOR ATTRIBUTE			{ rcl_execute(parse_param, $1, $2, rcl_null()); }
		| OPERATOR ATTRIBUTE PARAM		{ rcl_execute(parse_param, $1, $2, $3);   }
		;

/* possible parameters */
PARAM : VALUE							{ $$ = $1; }
	  | LIST							{ $$ = $1; }
	  ;

/* definition of a list */
LIST : L_BEGIN L_END					{ $$ = rcl_null(); 	}	/* empty list */
	 | L_BEGIN LIST_ELEMENTS L_END		{ $$ = $2;	 		}
	 ;

/* definition of the elements of a list */
LIST_ELEMENTS : PARAM						{ $$ = rcl_list($1); 			}	/* create list */
			  | LIST_ELEMENTS COMMA PARAM	{ $$ = rcl_list_add($1, $3); 	}	/* add element to list */
			  ;

%% /* end of the rules */

/* user defined code */

void yyerror(struct S_RCL_TYPE * parse_param, const char* s, ...) {
	va_list args;
	char msg[128];

    va_start(args, s);
	vsprintf(msg, s, args);
	va_end(args);

	sprintf(rcl_error_msg, "line %d: %s\n", yylineno, msg);
}
