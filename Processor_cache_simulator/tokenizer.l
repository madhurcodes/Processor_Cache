%{	
	#include "structures.h"
	#include "parser.tab.h"
	#include "parser2.tab.h"
	void yyerror ( char *);
	int yyparse ( void );
	extern int akashMparse ( void );
	extern FILE* akashMin ;

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
	if (argc!=6) {
		printf("Enter the Correct Format.\n");
		exit(0);
	}
	fpin= fopen(argv[1], "r");
	fpin3 = fopen(argv[2],"r");
	if(fpin==NULL) {printf("File Not Exist"); exit(0);}
	if(fpin3==NULL) {printf("File Not Exist"); exit(0);}
	filename = argv[3];
	filename2 = argv[4];
	filename3 = argv[5];
	yyin= fpin;
	akashMin = fpin3;	
	akashMparse();
	yyparse();
	fclose(fpin3);
	fclose(fpin);
	return 0;
}




