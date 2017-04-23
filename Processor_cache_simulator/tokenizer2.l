%{	
	#include "cache.h"
	#include "main.h"
	#include "parser2.tab.h"
	void akashMerror ( char *);
	 #define akashMSTYPE int
	int akashMparse ( void );
	//extern int akashMdebug;

%}
%s STAGE
%s ICACHE
%s EQX
%s DCACHE
%s CORE
%s DRAM
%s EQ2
%s ENDICACHE
%S ENDDCACHE

%%
<INITIAL>"# Common config file for MIPs core" {BEGIN STAGE;}
<INITIAL>" "								   {BEGIN INITIAL;}
<INITIAL>"\n"								   {BEGIN INITIAL;}
<INITIAL>"\r"								   {BEGIN INITIAL;}
<INITIAL><<EOF>>					{ BEGIN INITIAL;return END_OF_FILE;}
<INITIAL>.                            {printf("Wrong Input\n"); exit(0);}
<STAGE>"[perf_model/l1_icache]" {BEGIN ICACHE;}
<STAGE>"[perf_model/l1_dcache]" {BEGIN DCACHE;}
<STAGE>"[perf_model/core]"      {BEGIN CORE;}
<STAGE>"[perf_model/dram]"      {BEGIN DRAM;}
<STAGE>" "						{BEGIN STAGE;}
<STAGE>"\n"						{BEGIN STAGE;}
<STAGE>"\r"						{BEGIN STAGE;}
<STAGE>.                            {printf("Wrong Input\n"); exit(0);}

<ICACHE>"perfect"				{BEGIN ICACHE; return PER1; }
<ICACHE>"cache_size"            {BEGIN ICACHE; return CAC1; }
<ICACHE>"associativity"			{BEGIN ICACHE; return ASS1; }
<ICACHE>"replacement_policy"    {BEGIN ICACHE; return REP1; }
<ICACHE>"writethrough"          {BEGIN ICACHE; return WRT1; }
<ICACHE>" "						{BEGIN ICACHE;}
<ICACHE>"\n"					{BEGIN ICACHE;}
<ICACHE>"\r"					{BEGIN ICACHE;}
<ICACHE>"="                     {BEGIN ICACHE;return EQ; }
<ICACHE>"lru"                   {BEGIN ICACHE;return LRU; }
<ICACHE>("true"|"false")        {akashMlval.str= (char*)malloc(sizeof(char)*akashMleng + 2);
	strcpy(akashMlval.str, akashMtext);BEGIN ICACHE;return BOOL;}
<ICACHE>[0-9]+                  {akashMlval.intval = atoi(akashMtext);BEGIN ICACHE;return VAL; }
<ICACHE>"block_size"			{BEGIN EQX;return BS1;}
<ICACHE>.                            {printf("Wrong Input\n"); exit(0);}

<EQX>"="						{BEGIN ENDICACHE;return EQ;}
<EQX>"\n"					{BEGIN EQX;}
<EQX>"\r"					{BEGIN EQX;}
<EQX>" "                     {BEGIN EQX;}
<EQX>.                            {printf("Wrong Input\n"); exit(0);}

<ENDICACHE>[0-9]+						{akashMlval.intval = atoi(akashMtext);BEGIN STAGE;return VAL;}
<ENDICACHE>"\n"					{BEGIN ENDICACHE;}
<ENDICACHE>"\r"					{BEGIN ENDICACHE;}
<ENDICACHE>" "                     {BEGIN ENDICACHE;}
<ENDICACHE>.                            {printf("Wrong Input\n"); exit(0);}


<DCACHE>"perfect"				{BEGIN DCACHE; return PER2; }
<DCACHE>"cache_size"            {BEGIN DCACHE; return CAC2; }
<DCACHE>"associativity"			{BEGIN DCACHE; return ASS2; }
<DCACHE>"replacement_policy"    {BEGIN DCACHE; return REP2; }
<DCACHE>"writethrough"          {BEGIN DCACHE; return WRT2; }
<DCACHE>"block_size"			{ BEGIN EQ2;return BS2;}
<DCACHE>" "						{BEGIN DCACHE;}
<DCACHE>"\n"					{BEGIN DCACHE;}
<DCACHE>"\r"					{BEGIN DCACHE;}
<DCACHE>"="                     {BEGIN DCACHE;return EQ; }
<DCACHE>"lru"                   {BEGIN DCACHE; return LRU;}
<DCACHE>("true"|"false")        {akashMlval.str= (char*)malloc(sizeof(char)*akashMleng + 2);
	strcpy(akashMlval.str, akashMtext);BEGIN DCACHE;return BOOL;}
<DCACHE>[0-9]+                  {akashMlval.intval = atoi(akashMtext);BEGIN DCACHE;return VAL; }
<DCACHE>.                            {printf("Wrong Input\n"); exit(0);}


<EQ2>"="						{BEGIN ENDDCACHE;return EQ; }
<EQ2>"\n"					{BEGIN EQ2;}
<EQ2>"\r"					{BEGIN EQ2;}
<EQ2>" "                     {BEGIN EQ2;}
<EQ2>.                            {printf("Wrong Input\n"); exit(0);}


<ENDDCACHE>[0-9]+						{akashMlval.intval = atoi(akashMtext);BEGIN STAGE;return VAL; }
<ENDDCACHE>"\n"					{BEGIN ENDDCACHE;}
<ENDDCACHE>"\r"					{BEGIN ENDDCACHE;}
<ENDDCACHE>" "                     {BEGIN ENDDCACHE;}
<ENDDCACHE>.                            {printf("Wrong Input\n"); exit(0);}



<CORE>"frequency"               {BEGIN CORE;return FRQ; }
<CORE>"="						{BEGIN CORE;return EQ; }
<CORE>[0-9]+\.[0-9]*			{akashMlval.floatval = atof(akashMtext); BEGIN STAGE;return FLOAT;}
<CORE>" "						{BEGIN CORE;}
<CORE>"\n"						{BEGIN CORE;}
<CORE>"\r"						{BEGIN CORE;}
<CORE>.                            {printf("Wrong Input\n"); exit(0);}

<DRAM>"latency"                 {BEGIN DRAM;return LAT;}
<DRAM>"="						{BEGIN DRAM;return EQ; }
<DRAM>[0-9]+					{akashMlval.intval = atoi(akashMtext);BEGIN INITIAL;return VAL;}
<DRAM>" "						{BEGIN DRAM;}
<DRAM>"\n"						{BEGIN DRAM;}
<DRAM>"\r"						{BEGIN DRAM;}
<DRAM>.                            {printf("Wrong Input\n"); exit(0);}
%%


void akashMerror(char* str){
	fprintf(akashMout, "SynErr");
}

int akashMwrap (void)
{
return 1;
}

