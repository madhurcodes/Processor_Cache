%{
	#include <stdio.h>
	#include "cache.h"
	#include "main.h"
	extern void setIFlag(int fl);
	extern void setDFlag(int fl);
	#include<string.h>
	extern int akashMlex ();
	extern void akashMerror ( char *);
	FILE *trfile;
	extern float frequency;
	extern int latency;
%}

%union {
  char* str;
  int intval;
  float floatval;
}

%token <intval> VAL 
%token <str> BOOL 
%token <floatval> FLOAT
%token END_OF_FILE PER1 CAC1 ASS1 REP1 WRT1 BS1 EQ PER2 CAC2 ASS2 REP2 WRT2 BS2 FRQ LAT LRU
%start root

%%

root:
	calc END_OF_FILE	{  checkValues(); init_cache(); dump_settings(); return 0;}      
;
		
calc:		
|exp calc       		    {;}
;


exp: 
  PER1 EQ BOOL          {if(strcmp($3,"true")==0){setIFlag(1);} else if (strcmp($3,"false")==0){setIFlag(0);} else{printf("\nInvalid Perfect Option Specified\n");exit(0);}}
  |CAC1 EQ VAL			{set_cache_param(CACHE_PARAM_ISIZE,(1024*($3)));}
  |ASS1 EQ VAL			{set_cache_param(CACHE_PARAM_ASSOC, $3);}
  |REP1 EQ LRU			{}
  |WRT1 EQ VAL           {if($3==1) {set_cache_param(CACHE_PARAM_WRITETHROUGH, 1);} else if($3==0){set_cache_param(CACHE_PARAM_WRITEBACK,1);} else{printf("Error: Write through value is not correct\n"); exit(0);}}
  |BS1 EQ VAL            {set_cache_param(CACHE_PARAM_BLOCK_SIZE,$3);}
  |PER2 EQ BOOL          {if(strcmp($3,"true")==0){setDFlag(1);} else if (strcmp($3,"false")==0){setDFlag(0);} else{printf("\nInvalid Perfect Option Specified\n");exit(0);}}
  |CAC2 EQ VAL           {set_cache_param(CACHE_PARAM_DSIZE, (1024*($3)));}
  |ASS2 EQ VAL           {set_cache_param(CACHE_PARAM_DASSOC, $3);}
  |REP2 EQ LRU           {}
  |WRT2 EQ VAL           {if($3==1) {set_cache_param(CACHE_PARAM_DWRITETHROUGH, 1);} else if($3==0){set_cache_param(CACHE_PARAM_DWRITEBACK,1);} else{printf("Error: Write through value is not correct\n"); exit(0);}}
  |BS2 EQ VAL            {set_cache_param(CACHE_PARAM_DBLOCK_SIZE, $3);} 
  |FRQ EQ FLOAT          {frequency = $3;}
  |LAT EQ VAL            {latency = $3;}
  ;

%%
