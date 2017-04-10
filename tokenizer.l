%{	
	#include "structures.h"
	#include "parser.tab.h"
	void yyerror ( char *);
	int yyparse ( void );

%}

%%

" "					{;}
"\n"				{;}
"\r"				{;}
[a-fA-F0-9]{8}    {yylval.str= (char*)malloc(sizeof(char)*yyleng + 2);
	strcpy(yylval.str, yytext);return INST;}
.			{printf("Wrong Input\n"); exit(0);}
<<EOF>>					{return END_OF_FILE;}
%%


void yyerror(char* str){
	fprintf(yyout, "SynErr");
}

int main(int argc, char* argv[]){
	if (argc!=4) {
		printf("Enter the Correct Format.\n");
		exit(0);
	}
	fpin= fopen(argv[1], "r");
	if(fpin==NULL) {printf("File Not Exist"); exit(0);}
	filename = argv[2];
	filename2 = argv[3];
	yyin= fpin;	
	yyout= fpout;
	yyparse();
	fclose(fpin);
	return 0;
}
