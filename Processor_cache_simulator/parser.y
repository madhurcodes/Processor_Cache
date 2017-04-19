%{
	#include <stdio.h>
	#include "structures.h"
	#include "Processor.c"
	extern int yylex ();
	extern void yyerror ( char *);
%}

%union {
	char* str;
}

%token <str> INST
%token END_OF_FILE
%start root

%%

root:
	calc END_OF_FILE	{  DoComputations(); return 0;}      
;
		
calc:		
|exp calc       		    {;}
;


exp: 
  INST          { addNode($1);}
%%
