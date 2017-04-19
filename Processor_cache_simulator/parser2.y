%{
	#include <stdio.h>
	#include "cache.h"
	#include "main.h"
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
	calc END_OF_FILE	{  init_cache(); dump_settings(); return 0;}      
;
		
calc:		
|exp calc       		    {;}
;


exp: 
  PER1 EQ BOOL          {set_cache_param(CACHE_PARAM_WRITEALLOC, 1);}
  |CAC1 EQ VAL			{set_cache_param(CACHE_PARAM_ISIZE,$3);}
  |ASS1 EQ VAL			{set_cache_param(CACHE_PARAM_ASSOC, $3);}
  |REP1 EQ LRU			{printf("_");}
  |WRT1 EQ VAL           {if($3==1) {set_cache_param(CACHE_PARAM_WRITETHROUGH, 1);} else{set_cache_param(CACHE_PARAM_WRITEBACK,1);}}
  |BS1 EQ VAL            {set_cache_param(CACHE_PARAM_BLOCK_SIZE,$3);}
  |PER2 EQ BOOL          {set_cache_param(CACHE_PARAM_WRITEALLOC, 1);}
  |CAC2 EQ VAL           {set_cache_param(CACHE_PARAM_DSIZE, $3);}
  |ASS2 EQ VAL           {set_cache_param(CACHE_PARAM_ASSOC, $3);}
  |REP2 EQ LRU           {printf("_");}
  |WRT2 EQ VAL           {if($3==1) {set_cache_param(CACHE_PARAM_WRITETHROUGH, 1);} else{set_cache_param(CACHE_PARAM_WRITEBACK,1);}}
  |BS2 EQ VAL            {set_cache_param(CACHE_PARAM_BLOCK_SIZE, $3);} 
  |FRQ EQ FLOAT          {frequency = $3;}
  |LAT EQ VAL            {latency = $3;}
  ;

%%
