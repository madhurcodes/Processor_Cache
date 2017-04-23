#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

#include "cache.h"
#include "main.h"

FILE* fpout, *fpout2;
int InstructionsExecuted = 0;
instruction* instHead = NULL;
long program_counter = 0,numberOfInstructions=0; 
int ActivatedThreads[5];
int Register[32][32];
int Memory[65536][8]; // Assuming words are stored in the big endian manner i.e. most significant bytes are stored first followed by least significant bytes
int InstructionMemory[16384][32];
long MULSPECIAL;
char* filename;
int pcFlag=0;
int contFlag = 0;
char *print1 = "",*print2 = "",*print3 = "",*print4 = "",*print5 = "";
int flush=0;
DecodedInstruction emptyStruct;
IF_ID_Data emptyStruct1;
EX_MEM_DATA emptyStruct3;
Mem_WB_Data emptyStruct4;
int listOfBreakPoints[16364];
int breakpoint;
IF_ID_T IF_ID;
ID_EX_T ID_EX;
EX_MEM_T EX_MEM;
MEM_WB_T MEM_WB;
CheckSWithF dataHazardCheckingUnit;
CheckSWithF emptyStruct5;
DataForwardingUnit PATH1;
DataForwardingUnit PATH2;
int numberofInst=0;
int fileWrite=0;
int numberofCycles;
int numOfCaches = 0;
int mul_hi[32];
int mul_lo[32];
int p_c[32];
int perIFlag = 0;
int perDFlag = 0;
DataForwardingUnit emptyStruct6;
int path2flag=0;
int write(int is_g1,int is_g2, int is_g3, int is_g4, int is_g5);
void step();
void writeInstructionMemory();

void setIFlag(int fl){
	perIFlag = fl;
}

void setDFlag(int fl){
	perDFlag = fl;
}
 
void addNode(char* s){
	numberOfInstructions++;
	instruction* temp;
	temp = instHead;
	instruction* newNode = (instruction*)malloc(sizeof(instruction)); newNode->next = NULL; newNode->value = s;
	if (instHead==NULL)
	{
		instHead = newNode;
		program_counter = 0;
	}
	else{
		while((temp->next)!=NULL){
			temp = temp->next;
		}
		temp->next = newNode;
	}
	return;
}

long getMemoryIndex(int Z[8]){
	long x = 0;
	int i=0;
	for (i = 0; i < 8; i++)
	{
		x = x + Z[7-i]*pow(16,i);
	}
	x = x - 268500992;
	return x; 
}

long getMemoryIndex32(int Z[32]){
	long x = 0;
	int i=0;
	for (i = 0; i < 32; i++)
	{
		x = x + Z[i]*pow(2,i);
	}
	x = x - 268500992;
	return x; 
}


char getHexValue(int m){
	char ch;
	if (m==0)
	{
		return '0';
	}
	else if (m==1)
	{
		return '1';
	}
	else if (m==2)
	{
		return '2';
	}
	else if (m==3)
	{
		return '3';
	}
	else if (m==4)
	{
		return '4';
	}
	else if (m==5)
	{
		return '5';
	}
	else if (m==6)
	{
		return '6';
	}
	else if (m==7)
	{
		return '7';
	}
	else if (m==8)
	{
		return '8';
	}
	else if (m==9)
	{
		return '9';
	}
	else if (m==10)
	{
		return 'A';
	}
	else if (m==11)
	{
		return 'B';
	}
	else if (m==12)
	{
		return 'C';
	}
	else if (m==13)
	{
		return 'D';
	}
	else if (m==14)
	{
		return 'E';
	}
	else{
		return 'F';
	}
}


void printValueinRegister(int d){
	int z=0;
	int sum=0;
	for (z = 7; z >=0; z--)
	{
		if((d>=0)&&(d<32)){
		sum = Register[d][z*4 +3]*8 + Register[d][z*4 +2]*4 + Register[d][z*4 +1]*2 + Register[d][z*4];
		}
		char ch = getHexValue(sum);
		printf("%c",ch);
	}
	return;
}
void DoComputations(){
	int i=0,j=0;
	contFlag = 0;
	MULSPECIAL = 0;
	breakpoint = 163400;
	emptyStruct1.instr = NULL;
	emptyStruct1.rd = 60;
	emptyStruct1.rs = 60;
	emptyStruct1.rt = 77;
	IF_ID.left = emptyStruct1;
	IF_ID.right = emptyStruct1;


	emptyStruct.instr = NULL;
	emptyStruct.name = NULL;
	emptyStruct.rd = 60;
	emptyStruct.rs = 60;
	emptyStruct.rt = 77;
	ID_EX.left = emptyStruct;
	ID_EX.right = emptyStruct;

	emptyStruct3.instr = NULL;
	emptyStruct3.insname = NULL;
	emptyStruct3.desRegister = 60;
	emptyStruct3.RegStore = 60;
	EX_MEM.left = emptyStruct3;
	EX_MEM.right = emptyStruct3;

	emptyStruct4.instr = NULL;
	emptyStruct4.insname = NULL;
	emptyStruct4.desRegister = 60;
	emptyStruct4.RegStore = 60;
	MEM_WB.left = emptyStruct4;
	MEM_WB.right = emptyStruct4;

	emptyStruct5.name = "";
	dataHazardCheckingUnit = emptyStruct5;

	emptyStruct6.rd = 60;
	for (i = 0; i < 32; i++)
	{
		emptyStruct6.value[i] = 2;
	}

	PATH1 = emptyStruct6;

	fpout = fopen(filename,"w");
	for (i = 0; i < 32; i++)
	{
		for (j=0;j<32;j++)
		{
			Register[i][j] = 0;	
		}
	}
	for (i = 0; i < 5; i++)
	{
		ActivatedThreads[i] = 0;
	}
	for (i = 0; i < 65536; i++)
	{
		for (j = 0; j < 8; j++)
		{
			Memory[i][j] = 0;
		}
	}
	for (i = 0; i < 16384; i++)
	{
		listOfBreakPoints[i] = 0;
		for (j = 0; j < 32; j++)
		{
			InstructionMemory[i][j] = 0;
		}
	}
	for (i = 0; i < 32; i++)
	{
		mul_lo[i] = 0;
		mul_hi[i] = 0;
		p_c[i] = 0;
	}

	int ca_ll = write(ActivatedThreads[0],ActivatedThreads[1],ActivatedThreads[2],ActivatedThreads[3],ActivatedThreads[4]);	
	
	fclose(fpout);

	writeInstructionMemory();

	char inp[100] = "", *mem_add;
	int mem_no, iter_var, memi;
	long insmemindex;
	int MAD[8], ma, am;
	while(strcmp("exit",inp)!=0){
		printf("\n>>");
		scanf("%s",inp);
		if(strcmp("step",inp)==0){
			if (program_counter<(numberOfInstructions+4))
			{
				step();
			}
			else{
				printf("All the cycles are completed\n");
			}			
		}
		else if (strcmp("regdump",inp)==0){
			for(iter_var=0;iter_var<32;iter_var++){
				printf("$%2d: 0x",iter_var);
				printValueinRegister(iter_var);
				printf("\n");
			}
			printf("HI: 0x");
			int z=0;
			int sum=0;
			for (z = 7; z >=0; z--)
			{
				sum = mul_hi[z*4 +3]*8 + mul_hi[z*4 +2]*4 + mul_hi[z*4 +1]*2 + mul_hi[z*4];
				char ch = getHexValue(sum);
				printf("%c",ch);
			}
			printf("\nLO: 0x");
			for (z = 7; z >=0; z--)
			{
				sum = mul_lo[z*4 +3]*8 + mul_lo[z*4 +2]*4 + mul_lo[z*4 +1]*2 + mul_lo[z*4];
				char ch = getHexValue(sum);
				printf("%c",ch);
			}
			long baseAddress = 4194304 + program_counter*4;
			z=0;
			for (z = 0; z < 32; z++)
			{
				p_c[z] = 0;
			}
			z=0;
			while(baseAddress!=0){
				p_c[z] = baseAddress%2;
				z++;
				baseAddress = baseAddress/2;
			}
			printf("\nPC: 0x");
			for (z = 7; z >=0; z--)
			{
				sum = p_c[z*4 +3]*8 + p_c[z*4 +2]*4 + p_c[z*4 +1]*2 + p_c[z*4];
				char ch = getHexValue(sum);
				printf("%c",ch);
			}
		}
		else if(strcmp("memdump",inp)==0){
			scanf("%s",mem_add);
			scanf("%d", &mem_no);
			for (ma = 0; ma < 8; ma++)
			{
				MAD[ma] = (int)(*(mem_add + 2 + ma) - '0');
			}
			long md = getMemoryIndex(MAD);
			long tw = md;
			tw = tw + 268500992;
			int qw;
			for (ma = 0; ma < mem_no; ma++)
			{
				tw = md;
				tw = tw + 268500992;
				qw=0;
				tw = tw + ma;
				char H[20];
				int fr,xy=0;
				while(tw!=0){
					fr = tw%16;
					tw = tw/16;
					char c1 = getHexValue(fr);
					H[xy] = c1;
					xy++;
				}
				printf("0x");
				for (xy = 7; xy >=0 ; xy--)
				{
					printf("%c",H[xy]);
				}				
				printf(": 0x");
				char c3;
				for (am = 7; am >= 4; am--)
				{
					if(((md+ma)>=0) && ((md+ma)<65536)){
					qw = qw + Memory[md+ma][am]*pow(2,am-4);
					c3 = getHexValue(qw);
					}
					else{
					printf("\nTrying to read Memory Address Out of Range\n");
					exit(1);
					}
				}
				
				printf("%c",c3);
				qw=0;
				for (am = 3; am >= 0; am--)
				{
					if(((md+ma)>=0) && ((md+ma)<65536)){
					qw = qw + Memory[md+ma][am]*pow(2,am);
					c3 = getHexValue(qw);
					}
					else{
					printf("\nTrying to read Memory Address Out of Range\n");
					exit(1);
					}

				}
				printf("%c",c3);
				printf("\n");
			}
		}
		else if(strcmp("break",inp)==0){
			insmemindex = 0;
			scanf("%s",mem_add);
			for (ma = 0; ma < 8; ma++)
			{
				MAD[ma] = (int)(*(mem_add + 2 + ma) - '0');
			}
			for (memi = 0; memi < 8; memi++)
			{
				insmemindex = insmemindex + MAD[7-memi]*pow(16,memi);
			}
			insmemindex = insmemindex - 4194304;
			if (insmemindex>=0 && insmemindex%4==0)
			{
				insmemindex = insmemindex/4;
				listOfBreakPoints[insmemindex] = 1;
				printf("Breakpoint inserted successfully\n");
			}
			else{
				printf("Error: no instruction here\n");
			}
			
		}
		else if (strcmp("cache",inp)==0)
		{
			print_stats();
		}
		else if (strcmp("continue",inp)==0)
		{
			contFlag = 1;
		}
		
		else if (strcmp("delete",inp)==0)
		{
			insmemindex = 0;
			scanf("%s",mem_add);
			for (ma = 0; ma < 8; ma++)
			{
				MAD[ma] = (int)(*(mem_add + 2 + ma) - '0');
			}
			for (memi = 0; memi < 8; memi++)
			{
				insmemindex = insmemindex + MAD[7-memi]*pow(16,memi);
			}
			insmemindex = insmemindex - 4194304;
			if (insmemindex>=0 && insmemindex%4==0)
			{
				insmemindex = insmemindex/4;
				listOfBreakPoints[insmemindex] = 0;
				printf("Breakpoint deleted successfully\n");
			}
			else{
				printf("Error: no breakpoint here\n");
			}
		}
		else if (strcmp("run",inp)==0)
		{
			for (ma = 0; ma < 16384; ma++)
			{
				listOfBreakPoints[ma] = 0;
			}
		}
		else if(strcmp("exit",inp)==0){
			break;
		}
		else{
			printf("Invalid Input\n");
		}
		fflush(stdin);
	}
	flush2();

	init_cache();
	IF_ID.left = emptyStruct1;
	IF_ID.right = emptyStruct1;
	ID_EX.left = emptyStruct;
	ID_EX.right = emptyStruct;
	EX_MEM.left = emptyStruct3;
	EX_MEM.right = emptyStruct3;
	MEM_WB.left = emptyStruct4;
	MEM_WB.right = emptyStruct4;
	dataHazardCheckingUnit = emptyStruct5;
	PATH1 = emptyStruct6;
	numOfCaches = 0;

	for (i = 0; i < 32; i++)
	{
		for (j=0;j<32;j++)
		{
			Register[i][j] = 0;	
		}
	}
	for (i = 0; i < 5; i++)
	{
		ActivatedThreads[i] = 0;
	}
	for (i = 0; i < 65536; i++)
	{
		for (j = 0; j < 8; j++)
		{
			Memory[i][j] = 0;
		}
	}
	for (i = 0; i < 32; i++)
	{
		mul_lo[i] = 0;
		mul_hi[i] = 0;
		p_c[i] = 0;
	}
	for (i = 0; i < 16384; i++)
	{
		listOfBreakPoints[i] = 0;
	}
	numberofCycles = 0;
	program_counter = 0;
	pcFlag = 0;
	flush = 0;
	fileWrite = 1;
	numberofInst = 0;
	InstructionsExecuted =0;
	while(program_counter<(numberOfInstructions+4)){
		step();
		numberofCycles++;
	}
	flush2();
	int *returned = ret_stuff(); //returned[4] = latency
	float freq = giveFreq();
	float ipc = InstructionsExecuted*1.0/numberofCycles; // change to InstructionsExecuted if needed
	float time = (1.0*numberofCycles/freq) + (returned[4]*(returned[1]+returned[3]))*1.0;
	float idle = (numberofCycles-InstructionsExecuted)*1.0/freq + (returned[4]*(returned[1]+returned[3]))*1.0; // change to InstructionsExecuted if needed
	fpout2 = fopen(filename2,"w");
	fprintf(fpout2,"Instructions,%d\n",InstructionsExecuted); // change to InstructionsExecuted if needed
	fprintf(fpout2,"Cycles,%d\n",numberofCycles);
	fprintf(fpout2,"IPC,%.4f\n",ipc);
	fprintf(fpout2,"Time (ns),%.4f\n",time);
	fprintf(fpout2,"Idle time (ns),%.4f\n",idle);
	idle = (idle/time)*100;
	fprintf(fpout2,"Idle time (%%),%.4f%%\n",idle);
	fprintf(fpout2,"Cache Summary\n");
	fprintf(fpout2,"Cache L1-I\n");
	fprintf(fpout2,"num cache accesses,%d\n",returned[0]);
	if (perIFlag==0)
	{
		fprintf(fpout2,"num cache misses,%d\n",returned[1]);
		fprintf(fpout2,"miss rate,%.4f%%\n",100.0*returned[1]/returned[0]);
	}
	else{
		fprintf(fpout2,"num cache misses,0\n");
		fprintf(fpout2,"miss rate,100%%\n");
	}
	fprintf(fpout2,"Cache L1-D\n");
	fprintf(fpout2,"num cache accesses,%d\n",returned[2]);
	if (perDFlag==0)
	{
		fprintf(fpout2,"num cache misses,%d\n",returned[3]);
		fprintf(fpout2,"miss rate,%.4f%%\n",100.0*returned[3]/returned[2]);
	}
	else{
		fprintf(fpout2,"num cache misses,0\n");
		fprintf(fpout2,"miss rate,100%%\n");
	}
	fprintf(fpout2,"DRAM Summary\n");
	fprintf(fpout2,"num cache accesses,%d\n",returned[0]+returned[2]);
	fprintf(fpout2,"average dram access latency (ns),%.0f\n",(returned[4]*(returned[1]+returned[3]) + (1.0/freq)*(returned[0]+ returned[2]-returned[1]-returned[3]) )/ (returned[0]+ returned[2]) );
	fclose(fpout2);
	return;
}

char* singleCharConvert(char ch){
	char *s;
	switch(ch){
		case '0':
			s = "0000";
			break;
		case '1':
			s = "0001";
			break;
		case '2':
			s = "0010";
			break;
	
		case '3':
			s = "0011";
			break;
		case '4':
			s = "0100";
			break;
		case '5':
			s = "0101";
			break;
		case '6':
			s = "0110";
			break;
		case '7':
			s = "0111";
			break;
		case '8':
			s = "1000";
			break;
		case '9':
			s = "1001";
			break;
		case 'a':
			s = "1010";
			break;
		case 'A':
			s = "1010";
			break;
		case 'b':
			s = "1011";
			break;
		case 'B':
			s = "1011";
			break;
		case 'c':
			s = "1100";
			break;
		case 'C':
			s = "1100";
			break;
		case 'd':
			s = "1101";
			break;
		case 'D':
			s = "1101";
			break;
		case 'e':
			s = "1110";
			break;	
		case 'E':
			s = "1110";
			break;
		case 'f':
			s = "1111";
			break;
		case 'F':
			s = "1111";
			break;
		default:
			s = NULL;		
	}
	return s;
}

int covertToInteger(int X[32],int l, int r){
	int y =0;
	int j=0;
	int z=0;
	for (j = r; j <=l; j++)
	{
		y = y + X[j]*pow(2,z);
		z++;
	}
	return y;
}


void decodeTheInstruction(int instBin[32]){

	int k=0,fn,rd_temp,rs_temp,rt_temp,shamt_temp,offset_temp;

	//Decode The Instruction
	for (k = 0; k < 16; k++)
	{
		IF_ID.left.offsetArray[k] = instBin[k];
	}
	k = covertToInteger(instBin,31,26);
	rd_temp = covertToInteger(instBin,15,11);
	rt_temp = covertToInteger(instBin,20,16);
	rs_temp = covertToInteger(instBin,25,21);
	offset_temp = covertToInteger(instBin,14,0) - instBin[15]*pow(2,15);

	if (k==0)
	{
		if (covertToInteger(instBin,20,0)==8)
		{
			IF_ID.left.name = "jr";
			IF_ID.left.rs = rs_temp;
			IF_ID.left.identifier = "s";
		}
		else if (covertToInteger(instBin,31,16)==0 && covertToInteger(instBin,10,0)==18)
		{
			IF_ID.left.name = "mflo";
			IF_ID.left.rd = rd_temp;
			IF_ID.left.identifier = "d";
		}
		else{
		fn = covertToInteger(instBin,5,0);
		shamt_temp = covertToInteger(instBin,10,6);
		IF_ID.left.rd = rd_temp;
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		if (fn==43 && shamt_temp==0)
		{
			IF_ID.left.identifier = "std";
			IF_ID.left.name = "sltu";
		}
		else if (fn==32 && shamt_temp==0)
		{
			IF_ID.left.name = "add";
			IF_ID.left.identifier = "std";
		}
		else if (fn==33 && shamt_temp==0)
		{
			IF_ID.left.name = "move";
			IF_ID.left.identifier = "std";
		}
		else if (fn==9 && shamt_temp==0)
		{
			IF_ID.left.name = "jalr";
			IF_ID.left.identifier = "s";
		}
		else if(fn==36 && shamt_temp==0)
		{
			IF_ID.left.name = "and";
			IF_ID.left.identifier = "std";
		}
		else if(fn==39 && shamt_temp==0)
		{
			IF_ID.left.name = "nor";
			IF_ID.left.identifier = "std";
		}
		else if(fn==37 && shamt_temp==0)
		{
			IF_ID.left.name = "or";
			IF_ID.left.identifier = "std";
		}
		else if (fn==0)
		{
			IF_ID.left.shamt = shamt_temp;
			IF_ID.left.name = "sll";
			IF_ID.left.identifier = "tdh";
		}
		else if (fn==4 && shamt_temp==0)
		{
			IF_ID.left.name = "sllv";
			IF_ID.left.identifier = "std";
		}
		else if (fn==34 && shamt_temp==0)
		{
			IF_ID.left.name = "sub";
			IF_ID.left.identifier = "std";
		}
		else if (fn==42 && shamt_temp==0)
		{
			IF_ID.left.name = "slt";
			IF_ID.left.identifier = "std";
		}
		else if (fn==24 && rd_temp==0 && shamt_temp==0)
		{
			IF_ID.left.name = "mult";
			IF_ID.left.identifier = "std";
		}
		else{
			printf("Invalid Instruction\n");
			exit(0);
		}
	}
	}
	else if(k==28 && offset_temp==0){
		IF_ID.left.name = "madd";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.identifier = "std";
	}
	else if (k==4)
	{
		IF_ID.left.name = "beq";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "beq";
	}
	else if (k==1){
		if (rt_temp==1)
		{
			IF_ID.left.name = "bgez";
			IF_ID.left.rs = rs_temp;
			IF_ID.left.label = offset_temp;	
			IF_ID.left.identifier = "bra";	
		}
		else if (rt_temp==0)
		{
			IF_ID.left.name = "bltz";
			IF_ID.left.rs = rs_temp;
			IF_ID.left.label = offset_temp;
			IF_ID.left.identifier = "bra";
		}
		else{
			printf("Invalid Instruction\n");
			exit(0);
		}
	}
	else if (k==7 && rt_temp==0)
	{
		IF_ID.left.name = "bgtz";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "bra";
	}
	else if (k==6 && rt_temp==0)
	{
		IF_ID.left.name = "blez";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "bra";
	}
	else if (k==2)
	{
		IF_ID.left.name = "j";
		IF_ID.left.identifier = "notread";
		IF_ID.left.target = covertToInteger(instBin,25,0);
	}
	else if (k==3)
	{
		IF_ID.left.name = "jal";
		IF_ID.left.identifier = "notread";
		IF_ID.left.target = covertToInteger(instBin,25,0);
	}
	else if (k==32)
	{
		IF_ID.left.name = "lb";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "l";
	}
	else if (k==35)
	{
		IF_ID.left.name = "lw";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "l";
	}
	else if (k==40)
	{
		IF_ID.left.name = "sb";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "s";
	}
	else if (k==43)
	{
		IF_ID.left.name = "sw";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "s";
	}
	else if (k==8)
	{
		IF_ID.left.name = "addi";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.identifier = "st";
	}
	else if (k==13)
	{
		IF_ID.left.name = "ori";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.identifier = "st";
	}
	else if (k==10)
	{
		IF_ID.left.name = "slti";
		IF_ID.left.rs = rs_temp;
		IF_ID.left.rt = rt_temp;
		IF_ID.left.label = offset_temp;
		IF_ID.left.identifier = "st";
	}
	else if (k==15)
	{
		IF_ID.left.name = "lui";
		IF_ID.left.rt = rt_temp;
		IF_ID.left.identifier = "notread";
	}
	else{
		printf("Invalid Instruction\n");
		exit(0);
	}
	return;
}

int binaryToInteger(int D[32]){
	int f;
	int sum = 0;
	for (f = 0; f < 32; f++)
	{
		sum = sum + D[f]*pow(2,f);
	}

	sum = sum - D[32]*pow(2,31);

	return sum;
}


int and(int a, int b){
	if (a==1 && b==1)
	{
		return 1;
	}
	else{
		return 0;
	}
}

int or(int a, int b){
	if (a==0 && b==0)
	{
		return 0;
	}
	else{
		return 1;
	}
}

int nor(int a, int b){
	if (a==0 && b==0)
	{
		return 1;
	}
	else{
		return 0;
	}
}

void flushTheInstructions(int i){
	if (i==0)
	{
		flush =0;
	}
	else if (i==1)
	{
		flush = 1;
	}
	else
	{
		flush =2;
	}
	return;
}

void advanceProgramCounter(int R){

	program_counter = program_counter + R;
	if (R==0)
	{
		flushTheInstructions(0);
		pcFlag = 0;
	}
	else if (R==1)
	{
		program_counter = program_counter - 1;
		pcFlag = 1;
		flushTheInstructions(1);
	}
	else
	{
		program_counter = program_counter - 1;
		pcFlag = 1;
		flushTheInstructions(2);
	}
	
	return;
}

void advanceJumpProgramCounter(int tar){
		long tem = program_counter-2;
		program_counter = program_counter - 2;
		program_counter = program_counter*4 + 4194304;
		program_counter = program_counter / pow(2,28);
		program_counter = program_counter * pow(2,28);
		program_counter = program_counter + tar*4 - 4194304;
		program_counter = program_counter/4;
		int R = program_counter - tem;
		if (R==1)
		{
			flushTheInstructions(0);
			pcFlag = 0;
		}
		else if (R==2)
		{
			pcFlag = 1;
			flushTheInstructions(1);
		}
		else
		{
			pcFlag = 1;
			flushTheInstructions(2);
		}
}

int checkDataDependence(){

	if ((ID_EX.right.rt==PATH1.rd || ID_EX.right.rs==PATH1.rd)  && (!((ID_EX.right.rt>31 || ID_EX.right.rs>31))))
	{
		return 1;
	}
	if ((ID_EX.right.rt==PATH2.rd || ID_EX.right.rs==PATH2.rd) && (!((ID_EX.right.rt>31 || ID_EX.right.rs>31))))
	{
		return 2;
	}
	return 0;
}
void executeTheInstruction(){
	char *nameTemp = ID_EX.right.name;
	int var1[32],var2[32],result[32],t,temp1,temp2,temp3,shiftAmountShamt,shiftAmount,offSet,desArith,desLoad,offsetArr[32];

	// var1 will contain the value of rs
	// var2 will contain the value of rt
	// result will contain the value of rd
	// offset contain the value of last 16 bits extended to sign 

	int datadependence = checkDataDependence();

	if (datadependence==2)
	{
		if (ID_EX.right.rs==PATH2.rd)
		{
			for (t = 0; t < 32; t++)
			{
				var1[t] = PATH2.value[t];
			}
		}
		else{
			for (t = 0; t < 32; t++)
			{
				if((ID_EX.right.rs>=0)&&(ID_EX.right.rs<32)){
				var1[t] = Register[ID_EX.right.rs][t];
				}
			}
		}
		if (ID_EX.right.rt==PATH2.rd)
		{
			for (t = 0; t < 32; t++)
			{
				var2[t] = PATH2.value[t];
			}
		}
		else{
			for (t = 0; t < 32; t++)
			{
				if((ID_EX.right.rt>=0)&&(ID_EX.right.rt<32)){
				var2[t] = Register[ID_EX.right.rt][t];
				}
			}
		}

	}
	else if (datadependence==1)
	{
		if (ID_EX.right.rs==PATH1.rd)
		{
			for (t = 0; t < 32; t++)
			{
				var1[t] = PATH1.value[t];
			}
		}
		else{
			for (t = 0; t < 32; t++)
			{
				if((ID_EX.right.rs>=0)&&(ID_EX.right.rs<32)){

				var1[t] = Register[ID_EX.right.rs][t];
				}
			}
		}
		if (ID_EX.right.rt==PATH1.rd)
		{
			for (t = 0; t < 32; t++)
			{
				var2[t] = PATH1.value[t];
			}
		}
		else{
			for (t = 0; t < 32; t++)
			{
				if((ID_EX.right.rt>=0)&&(ID_EX.right.rt<32)){

				var2[t] = Register[ID_EX.right.rt][t];
				}
			}			
		}
	}
	else
	{
		for (t = 0; t < 32; t++)
		{	
			if((ID_EX.right.rs>=0)&&(ID_EX.right.rs<32)){
				var1[t] = Register[ID_EX.right.rs][t];
			}
			if((ID_EX.right.rt>=0)&&(ID_EX.right.rt<32)){
				var2[t] = Register[ID_EX.right.rt][t];
			}
		}
	}
	desArith = ID_EX.right.rd;
	desLoad = ID_EX.right.rt;
	offSet = ID_EX.right.label;
	shiftAmountShamt = ID_EX.right.shamt;
	shiftAmount = 0;
	for (t = 0; t < 32; t++)
	{
		if((ID_EX.right.rs>=0)&&(ID_EX.right.rs<32)){
			shiftAmount = shiftAmount + Register[ID_EX.right.rs][t]*pow(2,t);
		}
	}
	for (t = 0; t < 16; t++)
	{
		 offsetArr[t] = ID_EX.right.offsetArray[t];
	}
	for (t = 16; t < 32; t++)
	{
		 offsetArr[t] = ID_EX.right.offsetArray[15];
	}

	if (strcmp(nameTemp,"add")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		int carry = 0;
		for (t = 0; t < 32; t++)
		{
			result[t] = (var1[t] + var2[t] + carry)%2;
			carry = (var1[t] + var2[t] + carry)/2;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"move")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		for (t = 0; t < 32; t++)
		{
			result[t] = var2[t];
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"mflo")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		for (t = 0; t < 32; t++)
		{
			result[t] = mul_lo[t];
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"and")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		for (t = 0; t < 32; t++)
		{
			result[t] = and(var1[t],var2[t]);
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];		
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"mult")==0)
	{
		EX_MEM.left.insname = "m";
		temp1 = binaryToInteger(var1);
		temp2 = binaryToInteger(var2);
		MULSPECIAL = temp1*temp2;
		int negflag = 0;
		if (MULSPECIAL<0)
		{
			negflag = 1;
			MULSPECIAL = -1*MULSPECIAL;
		}
		int yCount = 0;
		int tmult[64];

		for (yCount = 0; yCount < 64; yCount++)
		{
			tmult[yCount] = 0;
		}

		yCount = 0; 
		long u = MULSPECIAL;

		while(u!=0){
			tmult[yCount] = u%2;
			u = u/2;
			yCount++;
		}

		if (negflag==1)
		{
			MULSPECIAL = -1*MULSPECIAL;
			yCount=0;
			while(yCount<=63 && tmult[yCount]==0){
				yCount++;
			}
			yCount++;
			if (yCount<=63)
			{
				int z;
				for (z = yCount; z < 64; z++)
				{
					if (tmult[z]==0)
					{
						tmult[z] = 1;
					}
					else{
						tmult[z] = 0;
					}
				}
			}
		}
		for (yCount = 0; yCount < 32; yCount++)
		{
			mul_lo[yCount] = tmult[yCount];
			mul_hi[yCount] = tmult[yCount+32];
		}
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"madd")==0)
	{
		EX_MEM.left.insname = "mad";
		temp1 = binaryToInteger(var1);
		temp2 = binaryToInteger(var2);
		MULSPECIAL = MULSPECIAL + temp1*temp2;
	
		int negflag = 0;
		if (MULSPECIAL<0)
		{
			negflag = 1;
			MULSPECIAL = -1*MULSPECIAL;
		}
		int yCount = 0;
		int tmult[64];

		for (yCount = 0; yCount < 64; yCount++)
		{
			tmult[yCount] = 0;
		}

		yCount = 0;
		long u = MULSPECIAL; 

		while(u!=0){
			tmult[yCount] = u%2;
			u = u/2;
			yCount++;
		}

		if (negflag==1)
		{
			MULSPECIAL = -1*MULSPECIAL;
			yCount=0;
			while(yCount<=63 && tmult[yCount]==0){
				yCount++;
			}
			yCount++;
			if (yCount<=63)
			{
				int z;
				for (z = yCount; z < 64; z++)
				{
					if (tmult[z]==0)
					{
						tmult[z] = 1;
					}
					else{
						tmult[z] = 0;
					}
				}
			}
		}
		for (yCount = 0; yCount < 32; yCount++)
		{
			mul_lo[yCount] = tmult[yCount];
			mul_hi[yCount] = tmult[yCount+32];
		}

		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"nor")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		for (t = 0; t < 32; t++)
		{
			result[t] = nor(var1[t],var2[t]);
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"or")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		for (t = 0; t < 32; t++)
		{
			result[t] = or(var1[t],var2[t]);
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"sll")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		//var2 contains rt
		for (t = 0; t < shiftAmountShamt; t++)
		{
			result[t] = 0;
		}
		for (t = shiftAmountShamt; t < 32; t++)
		{
			result[t] = var2[t- shiftAmountShamt];
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"sllv")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		for (t = 0; t < shiftAmount; t++)
		{
			result[t] = 0;
		}
		for (t = shiftAmount; t < 32; t++)
		{
			result[t] = var2[t- shiftAmount];
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"sub")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		int one[32],tempres[32];
		int carry =0;
		for (t = 0; t < 32; t++)
		{
			one[t] = 0;
			if (var2[t]==1)
			{
				var2[t] = 0;
			}
			else{
				var2[t] = 1;
			}
		}
		one[0] = 1;
		for (t= 0; t < 32; t++)
		{
			tempres[t] = (var2[t] + carry + one[t])%2;
			carry = (var2[t] + carry + one[t])/2;
		}
		carry = 0;
		for (t = 0; t < 32; t++)
		{
			result[t] = (var1[t] + carry + tempres[t])%2;
			carry = (var1[t] + carry + tempres[t])/2; 
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"slt")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		temp1 = binaryToInteger(var1);
		temp2 = binaryToInteger(var2);
		for (t = 1; t < 32; t++)
		{
			result[t] = 0;
		}
		if (temp1<temp2)
		{
			result[0] = 1;
		}
		else{
			result[0] = 0;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"beq")==0)
	{
		EX_MEM.left.insname = "b";
		temp1 = binaryToInteger(var1);
		temp2 = binaryToInteger(var2);
		if (temp1==temp2)
		{
			advanceProgramCounter(offSet);
		}
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"bgez")==0)
	{
		EX_MEM.left.insname = "b";
		temp1 = binaryToInteger(var1);
		if (temp1>=0)
		{
			advanceProgramCounter(offSet);
		}
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"bgtz")==0)
	{
		EX_MEM.left.insname = "b";
		temp1 = binaryToInteger(var1);
		if (temp1>0)
		{
			advanceProgramCounter(offSet);
		}
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"blez")==0)
	{
		EX_MEM.left.insname = "b";
		temp1 = binaryToInteger(var1);
		if (temp1<=0)
		{
			advanceProgramCounter(offSet);
		}
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"bltz")==0)
	{
		EX_MEM.left.insname = "b";
		temp1 = binaryToInteger(var1);
		if (temp1<0)
		{
			advanceProgramCounter(offSet);
		}
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"j")==0)
	{
		EX_MEM.left.insname = "j";
		advanceJumpProgramCounter(ID_EX.right.target);
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"jr")==0)
	{
		EX_MEM.left.insname = "j";
		int tarad = 0;
		for (t = 0; t < 32; t++)
		{
			tarad = tarad + var1[t]*pow(2,t);
		}
		advanceJumpProgramCounter(tarad);
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"jal")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = 31;
		int nextstr = program_counter;
		nextstr = program_counter * 4;
		nextstr = nextstr + 4194304;
		for (t = 0; t < 32; t++)
		{
			result[t] = (nextstr)%2;
			nextstr = nextstr/2;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		advanceJumpProgramCounter(ID_EX.right.target);
		PATH1.rd = 31;
	}
	else if (strcmp(nameTemp,"jalr")==0)
	{
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		int nextstr = program_counter;
		nextstr = program_counter * 4;
		nextstr = nextstr + 4194304;
		for (t = 0; t < 32; t++)
		{
			result[t] = (nextstr)%2;
			nextstr = nextstr/2;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		int tarad = 0;
		for (t = 0; t < 32; t++)
		{
			tarad = tarad + var1[t]*pow(2,t);
		}
		advanceJumpProgramCounter(tarad);
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"lb")==0)
	{
		EX_MEM.left.insname = "lb";
		EX_MEM.left.desRegister = desLoad;
		int carry = 0;
		long temp_res = 0;
		for (t = 0; t < 31; t++)
		{
			temp_res = temp_res + var1[t]*pow(2,t);
		}
		temp_res = temp_res - var1[31]*pow(2,31);
		temp_res = temp_res + offSet - 268500992;

		EX_MEM.left.MemAddress = temp_res;
		PATH1.rd = 50;			
	}
	else if (strcmp(nameTemp,"lw")==0)
	{
		EX_MEM.left.insname = "lw";
		EX_MEM.left.desRegister = desLoad;
		//Assuming it to be big endian
		long temp_res = 0;
		for (t = 0; t < 31; t++)
		{
			temp_res = temp_res + var1[t]*pow(2,t);
		}
		temp_res = temp_res - var1[31]*pow(2,31);
		temp_res = temp_res + offSet - 268500992;
		EX_MEM.left.MemAddress = temp_res;
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"sb")==0)
	{
		EX_MEM.left.insname = "sb";
		long temp_res = 0;
		for (t = 0; t < 31; t++)
		{
			temp_res = temp_res + var1[t]*pow(2,t);
		}
		temp_res = temp_res - var1[31]*pow(2,31);
		temp_res = temp_res + offSet - 268500992;
		EX_MEM.left.MemAddress = temp_res;
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = var2[t];
		}
		EX_MEM.left.RegStore = ID_EX.right.rt;
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"sw")==0)
	{
		EX_MEM.left.insname = "sw";
		//Assuming it to be big endian
		long temp_res = 0;
		for (t = 0; t < 31; t++)
		{
			temp_res = temp_res + var1[t]*pow(2,t);
		}
		temp_res = temp_res - var1[31]*pow(2,31);
		temp_res = temp_res + offSet - 268500992;
		EX_MEM.left.MemAddress = temp_res;
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = var2[t];
		}
		EX_MEM.left.RegStore = ID_EX.right.rt;
		PATH1.rd = 50;
	}
	else if (strcmp(nameTemp,"addi")==0){
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desLoad;
		int carry = 0;
		for (t = 0; t < 16; t++)
		{
			result[t] = (var1[t] + offsetArr[t] + carry)%2;
			carry = (var1[t] + offsetArr[t] + carry)/2;
		}
		for (t = 16; t < 32; t++)
		{
			result[t] = (var1[t] + offsetArr[15] + carry)%2;
			carry = (var1[t] + offsetArr[15] + carry)/2;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desLoad;
	}
	else if (strcmp(nameTemp,"ori")==0){
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desLoad;
		for (t = 0; t < 16; t++)
		{
			result[t] = or(var1[t],offsetArr[t]);
			result[t+16] = or(var1[t+16],0);
		}

		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desLoad;
	}
	else if (strcmp(nameTemp,"lui")==0){
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desLoad;
		for (t = 0; t < 16; t++)
		{
			result[t] = 0;
			result[t+16] = offsetArr[t];
		}

		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desLoad;
	}
	else if (strcmp(nameTemp,"sltu")==0){
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desArith;
		temp1 = 0;
		for (t = 0; t < 32; t++)
		{
			temp1 = temp1 + var1[t]*pow(2,t);
		}
		temp2 = 0;
		for (t = 0; t < 32; t++)
		{
			temp2 = temp2 + var2[t]*pow(2,t);
			result[t] = 0;
		}
		if (temp1 < temp2)
		{
			result[0] = 1;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desArith;
	}
	else if (strcmp(nameTemp,"slti")==0){
		EX_MEM.left.insname = "a";
		EX_MEM.left.desRegister = desLoad;
		temp1 = binaryToInteger(var1);
		temp2 = offSet;
		for (t = 0; t < 32; t++)
		{
			result[t] = 0;
		}
		if (temp1 < temp2)
		{
			result[0] = 1;
		}
		for (t = 0; t < 32; t++)
		{
			EX_MEM.left.Data[t] = result[t];
			PATH1.value[t] = result[t];
		}
		PATH1.rd = desLoad;
	}
	else{
		printf("ERROR in executing the instruction\n");
	}
	InstructionsExecuted++;
	return;
}

int checkStallingWithForwarding(){
	//if needed then 1 else return 0
	if (strcmp(IF_ID.left.identifier,"notread")==0)
	{
		return 0;
	}
	else if (dataHazardCheckingUnit.name==NULL)
	{
		return 0;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rs || dataHazardCheckingUnit.rt==IF_ID.left.rt)&&(strcmp(IF_ID.left.identifier,"std")==0))
	{
		return 1;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rs)&&(strcmp(IF_ID.left.identifier,"bra")==0))
	{
		return 1;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rs || dataHazardCheckingUnit.rt==IF_ID.left.rt)&&(strcmp(IF_ID.left.identifier,"beq")==0))
	{
		return 1;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rs)&&(strcmp(IF_ID.left.identifier,"st")==0))
	{
		return 1;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rs)&&(strcmp(IF_ID.left.identifier,"l")==0))
	{
		return 1;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rs)&&(strcmp(IF_ID.left.identifier,"s")==0))
	{
		return 1;
	}
	else if (((strcmp(dataHazardCheckingUnit.name,"lw")==0)||(strcmp(dataHazardCheckingUnit.name,"lb")==0)) && (dataHazardCheckingUnit.rt==IF_ID.left.rt)&&(strcmp(IF_ID.left.identifier,"tdh")==0))
	{
		return 1;
	}
	else{
		return 0;
	}
}

void registerRead(){
	ID_EX.left.name = IF_ID.right.name;
	ID_EX.left.instr = IF_ID.right.instr;
	ID_EX.left.rd = IF_ID.right.rd;
	ID_EX.left.rs = IF_ID.right.rs;
	ID_EX.left.rt = IF_ID.right.rt;
	ID_EX.left.shamt = IF_ID.right.shamt;
	ID_EX.left.label = IF_ID.right.label;
	ID_EX.left.address = IF_ID.right.address;
	ID_EX.left.target = IF_ID.right.target;
	int t = 0;
	for (t = 0; t < 16; t++)
	{
		ID_EX.left.offsetArray[t] = IF_ID.right.offsetArray[t];
	}
	return;
}

void *instructionFetch(void *value){
	//Code to be added
	int j=0;
	int tr = program_counter;
	instruction *tp = instHead;

	if (program_counter<numberOfInstructions)
	{
		if (listOfBreakPoints[program_counter]==1 && contFlag==0)
		{
			breakpoint = program_counter;
			print1 = "";
			IF_ID.left = emptyStruct1;
			ActivatedThreads[0] = 0;
			printf("\nNo instruction fetched due to break point\n");
		}
		else{
		breakpoint = 16400;
		contFlag = 0;
		decodeTheInstruction(InstructionMemory[program_counter]);
		if (checkStallingWithForwarding())
		{
			IF_ID.left.instr = "NOP";
			IF_ID.left.rd = 50;
			IF_ID.left.rs = 50;
			IF_ID.left.rt = 50;
			print1 = "NOP";
			ActivatedThreads[0] = 0;
		}
		else{
			unsigned int addr;
			addr = program_counter*4 + 4194304;
			perform_access(addr,2);

			for (j = 0; j < 32; j++)
			{
				IF_ID.left.instructionBinary[j] = InstructionMemory[program_counter][j];
			}
			for (j = 0; j < tr; j++)
			{
				tp = tp->next;		
			}
			IF_ID.left.instr = tp->value;
			print1 = IF_ID.left.instr;
			ActivatedThreads[0] = 1;
			numberofInst++; 
		}
	}
	}
	else{
		print1 = "";
		IF_ID.left = emptyStruct1;
		ActivatedThreads[0] = 0;
	}
	pthread_exit(NULL);
}

void *instructionDecode(void *value){
	//Code to be added

	if ((IF_ID.right.instr==NULL))
	{
		print2 = "";
		ID_EX.left = emptyStruct;
		ActivatedThreads[1] = 0;
	}
	else if (strcmp(IF_ID.right.instr,"NOP")==0)
	{
		print2 = "NOP";
		ID_EX.left.instr = "NOP";
		ID_EX.left.rd = 50;
		ID_EX.left.rs = 50;
		ID_EX.left.rt = 50;
		ActivatedThreads[1] = 0;
	}
	else{
		ID_EX.left.instr = IF_ID.right.instr;
		print2 = IF_ID.right.instr;
		registerRead();
		ActivatedThreads[1] = 1;
	}
	pthread_exit(NULL);
}

void *Execute(void *value){
	//Code to be added

	if (ID_EX.right.name==NULL)
	{
		print3 = "";
		ActivatedThreads[2] = 0;
		EX_MEM.left = emptyStruct3;
	}
	else if (strcmp(ID_EX.right.instr,"NOP")==0)
	{
		print3 = "NOP";
		ActivatedThreads[2] = 0;
		EX_MEM.left.instr = "NOP";
	}
	else{
		EX_MEM.left.instr = ID_EX.right.instr;
		print3 = ID_EX.right.instr;
		ActivatedThreads[2] = 1;
		executeTheInstruction();
	}
	pthread_exit(NULL);
}
 
void *writeMemory(void *value){
	char *nm = EX_MEM.right.insname;
	int t=0;
	long temp_es;
	unsigned int addr1;
	int dataTobeStored[32];
	
	if ((MEM_WB.right.insname!=NULL)&&((strcmp(MEM_WB.right.insname,"a")==0) || (strcmp(MEM_WB.right.insname,"lb")==0) || (strcmp(MEM_WB.right.insname,"lw")==0)))
	{
		if (MEM_WB.right.desRegister == EX_MEM.right.RegStore)
		{
			for (t = 0; t < 32; t++)
			{
				dataTobeStored[t] = MEM_WB.right.Data[t];
			}
		}
		else{
			for (t = 0; t < 32; t++)
			{
				if((EX_MEM.right.RegStore>=0)&&(EX_MEM.right.RegStore<32)){

					dataTobeStored[t] = Register[EX_MEM.right.RegStore][t];
				}
			}
		}
	}
	else{
		for (t = 0; t < 32; t++)
		{
			if((EX_MEM.right.RegStore>=0)&&(EX_MEM.right.RegStore<32)){
				dataTobeStored[t] = Register[EX_MEM.right.RegStore][t];
			}
		}
	}

	temp_es = EX_MEM.right.MemAddress;

	if (nm==NULL)
	{
		print4 = "";
		MEM_WB.left = emptyStruct4;
		ActivatedThreads[3] = 0;
	}
	else if(strcmp(EX_MEM.right.instr,"NOP")==0){
		print4 = "NOP";
		MEM_WB.left.instr = "NOP";
		MEM_WB.left.insname = "";
		MEM_WB.left.desRegister = 50;
		ActivatedThreads[3] = 0;		
	}
	else if (strcmp(nm,"sb")==0)
	{
		numOfCaches++;
		MEM_WB.left.instr = EX_MEM.right.instr;
		MEM_WB.left.insname = EX_MEM.right.insname;
		MEM_WB.left.desRegister = 50;
		print4 = EX_MEM.right.instr;
		MEM_WB.left.flag =0;
		for (t = 0; t < 7; t++)
		{
			if(((temp_es)>=0) && ((temp_es)<65536)){
			Memory[temp_es][t] = dataTobeStored[t];
			}
			else{
				printf("Trying to Access Memory Out of Range");
				exit(1);
			}
		}
		addr1 = temp_es + 268500992;
		perform_access(addr1,1);
		ActivatedThreads[3] = 1;
	}
	else if (strcmp(nm,"sw")==0)
	{
		numOfCaches++;
		MEM_WB.left.instr = EX_MEM.right.instr;
		MEM_WB.left.insname = EX_MEM.right.insname;
		MEM_WB.left.desRegister = 50;
		print4 = EX_MEM.right.instr;
		MEM_WB.left.flag =0;
		int ns;
		for (t = 0; t < 4; t++)
		{
			for (ns = 0; ns < 8; ns++)
			{
				if(((t+temp_es)>=0) && ((t+temp_es)<65536)){

				Memory[temp_es+t][ns] = dataTobeStored[8*(3-t)+ns];
				}
				else{
					printf("Trying to Access Memory Out of Range");
					exit(1);
				}

			}
		}
		addr1 = temp_es + 268500992;
		perform_access(addr1,1);
		ActivatedThreads[3] = 1;
	}
	else if (strcmp(nm,"lb")==0)
	{
		numOfCaches++;
		MEM_WB.left.instr = EX_MEM.right.instr;
		MEM_WB.left.insname = EX_MEM.right.insname;
		MEM_WB.left.desRegister = EX_MEM.right.desRegister;
		print4 = EX_MEM.right.instr;
		MEM_WB.left.flag =1;
		for (t = 31; t>=8; t--)
		{
			MEM_WB.left.Data[t] = 0;
		}
		for (t = 0; t < 8; t++)
		{
			if(((temp_es)>=0) && ((temp_es)<65536)){
			MEM_WB.left.Data[t] = Memory[temp_es][t];
			}
			else{
					printf("Trying to Access Memory Out of Range");
					exit(1);
			}
		}
		addr1 = temp_es + 268500992;
		perform_access(addr1,0);
		ActivatedThreads[3] = 1;
	}
	else if (strcmp(nm,"lw")==0)
	{
		numOfCaches++;
		MEM_WB.left.instr = EX_MEM.right.instr;
		MEM_WB.left.insname = EX_MEM.right.insname;
		MEM_WB.left.desRegister = EX_MEM.right.desRegister;
		print4 = EX_MEM.right.instr;
		MEM_WB.left.flag =1;
		int ns;
		for (t = 0; t < 4; t++)
		{
			for (ns = 0; ns < 8; ns++)
			{
			if(((t+temp_es)>=0) && ((t+temp_es)<65536)){

				MEM_WB.left.Data[8*(3-t)+ns] = Memory[temp_es + t][ns];
					}
			else{
					printf("Trying to Access Memory Out of Range");
					exit(1);
			}
			}
		}
		addr1 = temp_es + 268500992;
		perform_access(addr1,0);
		ActivatedThreads[3] = 1;
	}
	else if (strcmp(nm,"a")==0)
	{
		MEM_WB.left.instr = EX_MEM.right.instr;
		MEM_WB.left.insname = EX_MEM.right.insname;
		MEM_WB.left.desRegister = EX_MEM.right.desRegister;
		print4 = EX_MEM.right.instr;
		MEM_WB.left.flag =1;
		for (t = 0; t < 32; t++)
		{
			MEM_WB.left.Data[t] = EX_MEM.right.Data[t];
		}
		ActivatedThreads[3] = 1;
	}
	else if(strcmp(nm,"b")==0 || strcmp(nm,"j")==0){
		MEM_WB.left.instr = EX_MEM.right.instr;
		MEM_WB.left.insname = EX_MEM.right.insname;
		MEM_WB.left.flag = 0;
		MEM_WB.left.desRegister = 50;
		print4 = EX_MEM.right.instr;
		ActivatedThreads[3] = 0;
	}
	else{
		MEM_WB.left = emptyStruct4;
		ActivatedThreads[3] = 0;
	}
	pthread_exit(NULL);
}

void *writeRegister(void *value){
	//Code to be added
	int dataToBeWritten[32];
	int desRegister;
	int t;

	for (t = 0; t < 32; t++)
	{
		dataToBeWritten[t] = MEM_WB.right.Data[t];
	}

	desRegister = MEM_WB.right.desRegister;
	if ((MEM_WB.right.instr!=NULL)&&(strcmp(MEM_WB.right.instr,"NOP")==0))
	{
		print5 = "NOP";
		ActivatedThreads[4] = 0;
	}
	else if (MEM_WB.right.flag==1 && MEM_WB.right.instr!=NULL)
	{
		print5 = MEM_WB.right.instr;
		ActivatedThreads[4] = 1;
		for (t = 0; t < 32; t++)
		{
			if((desRegister>=0)&&(desRegister<32)){
				Register[desRegister][t] = dataToBeWritten[t];
			}
		} 
	}
	else if (MEM_WB.right.instr==NULL)
	{
		print5 = "";
		ActivatedThreads[4] = 0;	
	}
	else if (strcmp(MEM_WB.right.insname,"b")==0 || strcmp(MEM_WB.right.insname,"j")==0)
	{
		print5 = MEM_WB.right.instr;
		ActivatedThreads[4] = 0;
	}
	else{
		print5 = "";
		ActivatedThreads[4] = 0;
	}
	pthread_exit(NULL);
}


void step(){
	int i;
	pthread_t threads[5];
	pthread_create(&threads[0],NULL,instructionFetch,NULL);
    pthread_create(&threads[1],NULL,instructionDecode,NULL);
    //pthread_create(&threads[2],NULL,Execute,NULL);
    pthread_create(&threads[2],NULL,writeMemory,NULL);
	pthread_create(&threads[3],NULL,writeRegister,NULL);
	
	for(i=0;i<4;i++){
		pthread_join(threads[i],NULL);
	}
	pthread_create(&threads[4],NULL,Execute,NULL);
	pthread_join(threads[4],NULL);
	
	
	IF_ID.right = IF_ID.left;
	ID_EX.right = ID_EX.left;
	EX_MEM.right = EX_MEM.left;
	MEM_WB.right = MEM_WB.left;
	int t;
	PATH2.rd = MEM_WB.right.desRegister;
	for (t = 0; t < 32; t++)
	{
		PATH2.value[t] = MEM_WB.right.Data[t];
	}


	if ((IF_ID.left.name!=NULL)&&(strcmp(IF_ID.left.instr,"NOP")==0))
	{
		pcFlag=0;
		// PATH2.name = dataHazardCheckingUnit.name;
		// PATH2.rs = dataHazardCheckingUnit.rs;
		// PATH2.rt = dataHazardCheckingUnit.rt;
		// PATH2.rd = dataHazardCheckingUnit.rd;
	}
	else{
		if (pcFlag==1)
		{
			pcFlag=0;
		}
		else if(breakpoint==16400)
		{
			program_counter++;
		}
	}

	dataHazardCheckingUnit.name = IF_ID.left.name;
	dataHazardCheckingUnit.rs = IF_ID.left.rs;
	dataHazardCheckingUnit.rt = IF_ID.left.rt;
	dataHazardCheckingUnit.rd = IF_ID.left.rd;

	
	if (flush==1)
	{
		ID_EX.left = emptyStruct;
		ID_EX.right = emptyStruct;
		flush = 0;
	}
	else if (flush==2)
	{
		ID_EX.left = emptyStruct;
		ID_EX.right = emptyStruct;
		IF_ID.left = emptyStruct1;
		IF_ID.right = emptyStruct1;
		flush = 0;
	}
	if (fileWrite==0)
	{
		fpout = fopen(filename,"w");
		int call = write(ActivatedThreads[0],ActivatedThreads[1],ActivatedThreads[2],ActivatedThreads[3],ActivatedThreads[4]);	
		fclose(fpout);
	}
	
	return;
}

void writeInstructionMemory(){
	instruction* temporary = instHead;
	int instBin[32];
	int k=0,ns=0;
	char *tempS, *inst;
	while(temporary!=NULL){
	inst = temporary->value;
	for (k = 7; k >= 0; k--)
	{
		tempS = singleCharConvert(*(inst+7-k));
		if((ns>=0)&&(ns<16384)){
		InstructionMemory[ns][k*4 + 3] = *(tempS) - '0';
		InstructionMemory[ns][k*4 + 2] = *(tempS+1) - '0';
		InstructionMemory[ns][k*4 + 1] = *(tempS+2) - '0';
		InstructionMemory[ns][k*4] = *(tempS+3) - '0';
	}
	else{
		printf("Instruction Memory Invalid Access");
		exit(1);
	}
	}
	temporary = temporary->next;
	ns++;
	}
	return;
}


int write(int is_g1,int is_g2, int is_g3, int is_g4, int is_g5){
  char* hide_special = " <g style=\"display:inline;opacity:0.3\"" ;
  char *beginning = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
"<!-- Created with Inkscape (http://www.inkscape.org/) -->\n"
"\n"
"<svg\n"
"   xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
"   xmlns:cc=\"http://creativecommons.org/ns#\"\n"
"   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n"
"   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
"   xmlns=\"http://www.w3.org/2000/svg\"\n"
"   xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\"\n"
"   xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\"\n"
"   height=\"420mm\"\n"
"   width=\"594mm\"\n"
"   viewBox=\"0 0 2104.7244 1488.189\"\n"
"   id=\"svg6542\"\n"
"   version=\"1.1\"\n"
"   inkscape:version=\"0.91 r13725\"\n"
"   sodipodi:docname=\"try_grouping.svg\" onload= \"refresh()\">\n"
"  <defs\n"
"     id=\"defs6548\" />\n"
"  <sodipodi:namedview\n"
"     pagecolor=\"#ffffff\"\n"
"     bordercolor=\"#666666\"\n"
"     borderopacity=\"1\"\n"
"     objecttolerance=\"10\"\n"
"     gridtolerance=\"10000\"\n"
"     guidetolerance=\"10\"\n"
"     inkscape:pageopacity=\"0\"\n"
"     inkscape:pageshadow=\"2\"\n"
"     inkscape:window-width=\"1301\"\n"
"     inkscape:window-height=\"744\"\n"
"     id=\"namedview6546\"\n"
"     showgrid=\"true\"\n"
"     showguides=\"true\"\n"
"     showborder=\"false\"\n"
"     inkscape:zoom=\"1.2685914\"\n"
"     inkscape:cx=\"686.51\"\n"
"     inkscape:cy=\"501.12\"\n"
"     inkscape:window-x=\"-8\"\n"
"     inkscape:window-y=\"-8\"\n"
"     inkscape:window-maximized=\"1\"\n"
"     inkscape:current-layer=\"svg6542\"\n"
"     inkscape:snap-grids=\"true\"\n"
"     inkscape:snap-perpendicular=\"false\"\n"
"     inkscape:object-nodes=\"true\"\n"
"     inkscape:snap-nodes=\"true\"\n"
"     inkscape:snap-intersection-paths=\"false\"\n"
"     units=\"mm\">\n"
"    <inkscape:grid\n"
"       type=\"xygrid\"\n"
"       id=\"grid6550\" />\n"
"  </sodipodi:namedview>\n"
"  <metadata\n"
"     id=\"metadata6544\">\n"
"    <rdf:RDF>\n"
"      <cc:Work\n"
"         rdf:about=\"\">\n"
"        <dc:format>image/svg+xml</dc:format>\n"
"        <dc:type\n"
"           rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />\n"
"        <dc:title />\n"
"      </cc:Work>\n"
"    </rdf:RDF>\n"
"  </metadata>\n"
"  <g transform=\"translate(-102,-444.18898)\"><g\n"
"     id=\"g2572\"\n"
"     transform=\"translate(545,553.18898)\">\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2578\"\n"
"       y=\"-33.682617\"\n"
"       x=\"365.20868\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"\n"
"       transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"         y=\"-33.682617\"\n"
"         x=\"365.20868\"\n"
"         id=\"tspan2580\"\n"
"         sodipodi:role=\"line\" /></text>\n"
"  </g>";
  char *gr1 = " id=\"g4377\"\n"
"    >\n"
"   <text x=\"1265\" y=\"670\" font-family=\"Verdana\" font-size=\"25\"> Instruction Fetch Thread</text>  <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 450,676.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-5-4\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5528\"\n"
"       d=\"m 260,678.18898 0,200\"\n"
"       style=\"opacity:1;fill:#339dfa;fill-opacity:1;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path4899\"\n"
"       d=\"m 375,738.18898 0,-15 -205,0 0,195 25,0\"\n"
"       style=\"fill:none;fill-rule:evenodd;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5488-3\"\n"
"       d=\"m 260,678.18898 195,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#339dfa;stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <g\n"
"       transform=\"translate(205,703.18898)\"\n"
"       id=\"g2657\">\n"
"      <rect\n"
"         style=\"fill:#ffaeae;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"         id=\"rect2395\"\n"
"         width=\"75\"\n"
"         height=\"100\"\n"
"         x=\"100\"\n"
"         y=\"175\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:125%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"         x=\"112.67969\"\n"
"         y=\"228.12598\"\n"
"         id=\"text2397\"\n"
"         sodipodi:linespacing=\"125%\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2399\"\n"
"           x=\"112.67969\"\n"
"           y=\"228.12598\">Memory</tspan></text>\n"
"    </g>\n"
"    <g\n"
"       transform=\"translate(220,703.18898)\"\n"
"       id=\"g2523\">\n"
"      <rect\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#ffaeae;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         id=\"rect2411\"\n"
"         width=\"25\"\n"
"         height=\"100\"\n"
"         x=\"25\"\n"
"         y=\"175\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"         x=\"29.425781\"\n"
"         y=\"229.36816\"\n"
"         id=\"text2398\"\n"
"         sodipodi:linespacing=\"100%\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2400\"\n"
"           x=\"29.425781\"\n"
"           y=\"229.36816\">PC</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccccccc\"\n"
"       id=\"path2402\"\n"
"       d=\"m 320,728.18898 30,15 0,40 -30,15 0,-30 10,-5 -10,-5 0,-30 z\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#ffaeae;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       transform=\"matrix(0,1,-1,0,0,0)\"\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2414\"\n"
"       y=\"-336.75\"\n"
"       x=\"761.07178\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"-336.75\"\n"
"         x=\"761.07178\"\n"
"         id=\"tspan2416\"\n"
"         sodipodi:role=\"line\">Adder</tspan></text>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       style=\"overflow:visible;visibility:visible;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 380,928.18898 20,0\"\n"
"       id=\"path2545\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2438\"\n"
"       d=\"m 270,928.18898 23,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.62128699;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccc\"\n"
"       id=\"path2442\"\n"
"       d=\"m 280,928.18898 0,-145 35,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <rect\n"
"       y=\"728.18896\"\n"
"       x=\"405\"\n"
"       height=\"349.99997\"\n"
"       width=\"10\"\n"
"       id=\"rect2531\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#ffaeae;fill-opacity:1;stroke:#000000;stroke-width:1.26499987;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\" />\n"
"    <circle\n"
"       id=\"path2602\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"280\"\n"
"       cy=\"928.18896\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 234.0686,926.01655 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       id=\"path2811\"\n"
"       d=\"m 350,763.18898 25,0 0,-25 20,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2833\"\n"
"       y=\"923.18896\"\n"
"       x=\"385\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"923.18896\"\n"
"         x=\"385\"\n"
"         id=\"tspan2835\"\n"
"         sodipodi:role=\"line\">IR</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2837\"\n"
"       y=\"520.35693\"\n"
"       x=\"325.0625\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:16px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"520.35693\"\n"
"         x=\"325.0625\"\n"
"         id=\"tspan2839\"\n"
"         sodipodi:role=\"line\">Instruction Fetch</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2885\"\n"
"       y=\"558.18896\"\n"
"       x=\"316.05859\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"558.18896\"\n"
"         x=\"316.05859\"\n"
"         id=\"tspan2887\"\n"
"         sodipodi:role=\"line\">IF</tspan></text>\n"
"    <g\n"
"       transform=\"translate(-269.25,739.18898)\"\n"
"       id=\"g2727-7\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#ffaeae;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-14.97149 0,-15.02851 z\"\n"
"         id=\"path2729-5\"\n"
"         sodipodi:nodetypes=\"cccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2731-9\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2733-7\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 292,926.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-8\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 307,781.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 393.99971,736.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-3\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 392,926.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-90\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 187,916.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-2\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path4937\"\n"
"       d=\"m 220,928.18898 6.02377,0 8.97623,0\"\n"
"       style=\"fill:none;fill-rule:evenodd;stroke:#090909;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 307,741.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-5-9\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path4960\"\n"
"       d=\"m 306,743.18898 -16,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\" />\n"
"    <text\n"
"       transform=\"scale(1.0082157,0.99185125)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text4962\"\n"
"       y=\"755.9549\"\n"
"       x=\"274.22879\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:19.06993866px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"755.9549\"\n"
"         x=\"274.22879\"\n"
"         id=\"tspan4964\"\n"
"         sodipodi:role=\"line\">4</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2885-5\"\n"
"       y=\"582.88965\"\n"
"       x=\"265.49155\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"582.88965\"\n"
"         x=\"265.49155\"\n"
"         id=\"tspan2887-7\"\n"
"         sodipodi:role=\"line\">";
char* gr_1 = "</tspan></text>\n"
"  </g>";
  char *gr2 = "  id=\"g4423\"\n"
"     >\n"
"   <text x=\"1265\" y=\"770\" font-family=\"Verdana\" font-size=\"25\"> Instruction Decode Thread  </text>  <path\n"
"       sodipodi:nodetypes=\"ccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5601\"\n"
"       d=\"m 415,723.18898 0,-35 45,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <rect\n"
"       y=\"808.18896\"\n"
"       x=\"490\"\n"
"       height=\"94.999992\"\n"
"       width=\"135\"\n"
"       id=\"rect2418\"\n"
"       style=\"fill:#00ffed;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:3.00870109;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text2420\"\n"
"       y=\"890.99182\"\n"
"       x=\"582.40015\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:13.75px;line-height:125%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         id=\"tspan2424\"\n"
"         y=\"890.99182\"\n"
"         x=\"582.40015\"\n"
"         sodipodi:role=\"line\">Registers</tspan></text>\n"
"    <g\n"
"       transform=\"translate(258.68645,705.43001)\"\n"
"       id=\"g2558\">\n"
"      <ellipse\n"
"         style=\"overflow:visible;visibility:visible;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:1.99839556;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         id=\"path2426\"\n"
"         transform=\"matrix(1.0016064,0,0,1,-20.52208,22.5)\"\n"
"         cx=\"350\"\n"
"         cy=\"237.5\"\n"
"         rx=\"25\"\n"
"         ry=\"37.5\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:125%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"         x=\"329.99622\"\n"
"         y=\"256.97363\"\n"
"         id=\"text2432\"\n"
"         sodipodi:linespacing=\"125%\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2434\"\n"
"           x=\"329.99622\"\n"
"           y=\"256.97363\">Sign</tspan><tspan\n"
"           sodipodi:role=\"line\"\n"
"           x=\"329.99622\"\n"
"           y=\"271.97363\"\n"
"           id=\"tspan2436\">Extend</tspan></text>\n"
"    </g>\n"
"    <rect\n"
"       style=\"opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:1.27413368;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\"\n"
"       id=\"rect4975\"\n"
"       width=\"10\"\n"
"       height=\"349.99997\"\n"
"       x=\"415\"\n"
"       y=\"728.18896\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2549\"\n"
"       d=\"m 425,928.18898 20,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       id=\"path2551\"\n"
"       d=\"m 480,818.18898 -35,0 0,149.99999 105,10e-6\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2568\"\n"
"       d=\"m 445,843.18898 35,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.85214341;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <circle\n"
"       id=\"path2600\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"445\"\n"
"       cy=\"818.18896\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2604\"\n"
"       d=\"m 615,968.18898 75,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2704\"\n"
"       d=\"m 425,733.18898 270,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2845\"\n"
"       y=\"510.80618\"\n"
"       x=\"567.45245\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:16px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"510.80618\"\n"
"         x=\"567.45245\"\n"
"         id=\"tspan2847\"\n"
"         sodipodi:role=\"line\">Instruction Decode</tspan><tspan\n"
"         id=\"tspan2849\"\n"
"         y=\"526.80615\"\n"
"         x=\"567.45245\"\n"
"         sodipodi:role=\"line\">Register Fetch</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2889\"\n"
"       y=\"558.18896\"\n"
"       x=\"556.77179\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"558.18896\"\n"
"         x=\"556.77179\"\n"
"         id=\"tspan2891\"\n"
"         sodipodi:role=\"line\">ID</tspan></text>\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccc\"\n"
"       id=\"path2913\"\n"
"       d=\"m 445,968.18898 0,85.00002 255,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <circle\n"
"       id=\"path2921\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"445\"\n"
"       cy=\"968.18896\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 476.96743,816.15459 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 477.09023,841.24436 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-378\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 552,966.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-371\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,966.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-380\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,1051.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-380-3\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,731.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6\" />\n"
"    <rect\n"
"       y=\"728.18896\"\n"
"       x=\"705\"\n"
"       height=\"349.99997\"\n"
"       width=\"10\"\n"
"       id=\"rect2531-2\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:1.26499987;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\" />\n"
"    <rect\n"
"       y=\"703.18903\"\n"
"       x=\"705\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5112-2-3\"\n"
"       style=\"opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <rect\n"
"       y=\"678.18903\"\n"
"       x=\"705\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5112-2-2\"\n"
"       style=\"opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <rect\n"
"       y=\"653.18903\"\n"
"       x=\"705\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5112-2-1\"\n"
"       style=\"opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5402\"\n"
"       y=\"820.13989\"\n"
"       x=\"492.21301\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:8.75px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"820.13989\"\n"
"         x=\"492.21301\"\n"
"         id=\"tspan5404\"\n"
"         sodipodi:role=\"line\">Read Register 1</tspan></text>\n"
"    <text\n"
"       id=\"text5406\"\n"
"       y=\"845.97595\"\n"
"       x=\"492.16333\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"845.97595\"\n"
"         x=\"492.16333\"\n"
"         id=\"tspan5408\"\n"
"         sodipodi:role=\"line\">Read Register 2</tspan></text>\n"
"    <text\n"
"       id=\"text5410\"\n"
"       y=\"870.97595\"\n"
"       x=\"492.21301\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"870.97595\"\n"
"         x=\"492.21301\"\n"
"         id=\"tspan5412\"\n"
"         sodipodi:role=\"line\">Write Register</tspan></text>\n"
"    <text\n"
"       id=\"text5414\"\n"
"       y=\"895.71381\"\n"
"       x=\"491.85153\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"895.71381\"\n"
"         x=\"491.85153\"\n"
"         id=\"tspan5416\"\n"
"         sodipodi:role=\"line\">Write Data</tspan></text>\n"
"    <text\n"
"       id=\"text5418\"\n"
"       y=\"831.84332\"\n"
"       x=\"566.50232\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"831.84332\"\n"
"         x=\"566.50232\"\n"
"         id=\"tspan5420\"\n"
"         sodipodi:role=\"line\">Read Data 1</tspan></text>\n"
"    <text\n"
"       id=\"text5422\"\n"
"       y=\"857.52448\"\n"
"       x=\"566.93585\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"857.52448\"\n"
"         x=\"566.93585\"\n"
"         id=\"tspan5424\"\n"
"         sodipodi:role=\"line\">Read Data 2</tspan></text>\n"
"    <path\n"
"       id=\"path5426\"\n"
"       d=\"m 445,1033.1895 250,0 -250,0 z\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,1031.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-380-3-5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 693,826.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-98\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 693,851.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-98-6\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5464\"\n"
"       d=\"m 625,828.18898 75,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5466\"\n"
"       d=\"m 625,853.18898 75,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <rect\n"
"       ry=\"18.750004\"\n"
"       y=\"658.18896\"\n"
"       x=\"460\"\n"
"       height=\"45\"\n"
"       width=\"90\"\n"
"       id=\"rect5470\"\n"
"       style=\"opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:1.53226173;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       id=\"text5472\"\n"
"       y=\"678.18896\"\n"
"       x=\"472.21304\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"678.18896\"\n"
"         x=\"472.21304\"\n"
"         id=\"tspan5474\"\n"
"         sodipodi:role=\"line\">Hazard </tspan><tspan\n"
"         id=\"tspan5476\"\n"
"         y=\"689.12646\"\n"
"         x=\"472.21304\"\n"
"         sodipodi:role=\"line\">Detection unit</tspan></text>\n"
"    <ellipse\n"
"       inkscape:transform-center-y=\"-3.3107277\"\n"
"       inkscape:transform-center-x=\"30.498995\"\n"
"       ry=\"29.367109\"\n"
"       rx=\"25.646555\"\n"
"       cy=\"767.78223\"\n"
"       cx=\"543.66101\"\n"
"       id=\"path5468\"\n"
"       style=\"opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:0.96279413;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       transform=\"scale(0.82944537,1.2056249)\"\n"
"       id=\"text5484\"\n"
"       y=\"639.01019\"\n"
"       x=\"639.97064\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.92857647px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         style=\"stroke-width:0\"\n"
"         y=\"639.01019\"\n"
"         x=\"639.97064\"\n"
"         id=\"tspan5486\"\n"
"         sodipodi:role=\"line\">Control</tspan></text>\n"
"    <text\n"
"       transform=\"scale(0.98850986,1.0116237)\"\n"
"       id=\"text5484-3\"\n"
"       y=\"673.02716\"\n"
"       x=\"611.20648\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20.94321442px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         style=\"stroke-width:0\"\n"
"         y=\"673.02716\"\n"
"         x=\"611.20648\"\n"
"         id=\"tspan5486-1\"\n"
"         sodipodi:role=\"line\">0</tspan></text>\n"
"    <path\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       d=\"m 549.99999,678.18899 30,0 10e-6,-35 70,0 0,14.99999\"\n"
"       id=\"path5793\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       sodipodi:nodetypes=\"ccccc\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5626\"\n"
"       d=\"m 445,818.18898 0,-100 20.47692,0 -0.47692,-10\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 463,711.18898 4,0 -2,-8 -2.22263,8.00371 0.22263,-0.004\"\n"
"       id=\"use2809-38-5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5665\"\n"
"       d=\"m 445,768.18898 70,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 507,766.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-7\" />\n"
"    <g\n"
"       transform=\"matrix(1,0,0,1.20625,170.75,460.05149)\"\n"
"       id=\"g2727-7-4\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#00ffed;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-14.97149 0,-15.02851 z\"\n"
"         id=\"path2729-5-4\"\n"
"         sodipodi:nodetypes=\"cccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2731-9-3\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2733-7-0\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5706\"\n"
"       d=\"m 660,688.18898 c 0,0 25,0 25,0 l 0,-25 15,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5708\"\n"
"       d=\"m 700,688.18898 -15,0 0,25 15,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,711.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-7\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,686.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-8\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 692,661.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-6\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5737\"\n"
"       d=\"m 570,768.18898 50,0 0,-65 15,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 627,701.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-88\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5754\"\n"
"       d=\"m 635,673.18898 -15,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 627,671.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-88-4\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 652,650.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"       id=\"use2809-38-6-6-4\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5810\"\n"
"       d=\"m 565,753.18898 10,0 0,-135 -395,0 0,439.60132 220,0.3987\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 417,715.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"       id=\"use2809-5-4-2\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 392,1056.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-5-4-2-9-5\" />\n"
"    <text\n"
"       id=\"text6761-9\"\n"
"       y=\"1052.5081\"\n"
"       x=\"322.96164\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"1052.5081\"\n"
"         x=\"322.96164\"\n"
"         id=\"tspan6763-6\"\n"
"         sodipodi:role=\"line\">IF.Flush</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2885-5-8\"\n"
"       y=\"582.92505\"\n"
"       x=\"498.41577\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"582.92505\"\n"
"         x=\"498.41577\"\n"
"         id=\"tspan2887-7-5\"\n"
"         sodipodi:role=\"line\">";
char *gr_2 = "</tspan></text>\n"
"    <circle\n"
"       id=\"path2600-0-4-6-3\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"445.01608\"\n"
"       cy=\"1053.1121\"\n"
"       r=\"2.5\" />\n"
"    <circle\n"
"       id=\"path2600-0-4-6-33\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"445.13889\"\n"
"       cy=\"1032.85\"\n"
"       r=\"2.5\" />\n"
"    <circle\n"
"       id=\"path2600-0-4-6-86\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"445.26169\"\n"
"       cy=\"842.98932\"\n"
"       r=\"2.5\" />\n"
"    <circle\n"
"       id=\"path2600-0-4-6-0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"445.13889\"\n"
"       cy=\"767.84998\"\n"
"       r=\"2.5\" />\n"
"  </g>\n"
"  ";
  char *gr3 =  " id=\"g3688\">\n"
"    <text x=\"1265\" y=\"870\" font-family=\"Verdana\" font-size=\"25\" >  Instruction Execute Thread </text> <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 452,691.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-7-3\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5643\"\n"
"       d=\"m 750,688.18898 0,-50 -215,0 0,15\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccccccc\"\n"
"       id=\"path2619\"\n"
"       d=\"m 895,788.18898 40,20 0,50 -40,20 0,-30 10,-5 0,-20 -10,-5 0,-30 z\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2621\"\n"
"       y=\"837.47803\"\n"
"       x=\"920.33594\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"837.47803\"\n"
"         x=\"920.33594\"\n"
"         id=\"tspan2623\"\n"
"         sodipodi:role=\"line\">ALU</tspan></text>\n"
"    <g\n"
"       transform=\"translate(340.75,609.18898)\"\n"
"       id=\"g2625\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-30 -20,-10 z\"\n"
"         id=\"path2627\"\n"
"         sodipodi:nodetypes=\"ccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2629\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2631\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2633\"\n"
"       d=\"m 830,803.18898 55,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.99999988;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       id=\"path2651\"\n"
"       d=\"m 935,833.18898 25,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <g\n"
"       transform=\"translate(380.75,669.18898)\"\n"
"       id=\"g2727\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-30 -20,-10 z\"\n"
"         id=\"path2729\"\n"
"         sodipodi:nodetypes=\"ccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2731\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2733\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       id=\"path2735\"\n"
"       d=\"m 870,863.18898 15,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.99999988;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2851\"\n"
"       y=\"510.1402\"\n"
"       x=\"859.39606\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:16px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         id=\"tspan2855\"\n"
"         y=\"510.1402\"\n"
"         x=\"859.39606\"\n"
"         sodipodi:role=\"line\">Execute</tspan><tspan\n"
"         id=\"tspan2859\"\n"
"         y=\"526.1402\"\n"
"         x=\"859.39606\"\n"
"         sodipodi:role=\"line\">Address Calc.</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2893\"\n"
"       y=\"556.21832\"\n"
"       x=\"844.74371\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"556.21832\"\n"
"         x=\"844.74371\"\n"
"         id=\"tspan2895\"\n"
"         sodipodi:role=\"line\">EX</tspan></text>\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 883,801.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-7\" />\n"
"    <rect\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:1.27400005;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       id=\"rect4975-9\"\n"
"       width=\"10\"\n"
"       height=\"349.99997\"\n"
"       x=\"715\"\n"
"       y=\"728.18896\" />\n"
"    <rect\n"
"       y=\"703.18903\"\n"
"       x=\"715\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5114-0-6\"\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;image-rendering:auto\" />\n"
"    <rect\n"
"       y=\"678.18903\"\n"
"       x=\"715\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5114-0-9\"\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;image-rendering:auto\" />\n"
"    <rect\n"
"       y=\"653.18903\"\n"
"       x=\"715\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5114-0-94\"\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;image-rendering:auto\" />\n"
"    <rect\n"
"       y=\"728.18903\"\n"
"       x=\"970\"\n"
"       height=\"349.99997\"\n"
"       width=\"10\"\n"
"       id=\"rect2531-2-1\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:1.26499987;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\" />\n"
"    <rect\n"
"       y=\"702.59778\"\n"
"       x=\"970\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5112-2-8\"\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <rect\n"
"       y=\"677.59778\"\n"
"       x=\"970\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5112-2-0\"\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <g\n"
"       id=\"g6043\"\n"
"       transform=\"matrix(0.74993958,0,0,0.76065516,19.468341,589.61843)\">\n"
"      <ellipse\n"
"         style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:1.27475405;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"         id=\"path5468-2\"\n"
"         cx=\"1123.7429\"\n"
"         cy=\"507.522\"\n"
"         rx=\"34.198162\"\n"
"         ry=\"38.607651\"\n"
"         inkscape:transform-center-x=\"30.498995\"\n"
"         inkscape:transform-center-y=\"-3.3107277\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:11.8215704px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"         x=\"1325.7069\"\n"
"         y=\"421.16376\"\n"
"         id=\"text5484-6\"\n"
"         transform=\"scale(0.83535015,1.1971028)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan5486-6\"\n"
"           x=\"1325.7069\"\n"
"           y=\"421.16376\"\n"
"           style=\"stroke-width:0\">  ALU</tspan><tspan\n"
"           id=\"tspan6041\"\n"
"           sodipodi:role=\"line\"\n"
"           x=\"1325.7069\"\n"
"           y=\"435.94073\"\n"
"           style=\"stroke-width:0\">Control</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5490\"\n"
"       d=\"m 725,688.18898 85,0 0,25 155,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 537,645.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"       id=\"use2809-5-4-2-4\" />\n"
"    <rect\n"
"       ry=\"18.750004\"\n"
"       y=\"1058.189\"\n"
"       x=\"825\"\n"
"       height=\"45\"\n"
"       width=\"90\"\n"
"       id=\"rect5470-2\"\n"
"       style=\"opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:1.53226173;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       id=\"text5472-0\"\n"
"       y=\"941.7616\"\n"
"       x=\"958.33466\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:11.58738613px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-linejoin:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"\n"
"       transform=\"scale(0.86747182,1.1527752)\"><tspan\n"
"         id=\"tspan5476-8\"\n"
"         y=\"941.7616\"\n"
"         x=\"958.33466\"\n"
"         sodipodi:role=\"line\"\n"
"         style=\"stroke-width:0\">Forwarding unit</tspan></text>\n"
"    <g\n"
"       transform=\"translate(340.75,669.18898)\"\n"
"       id=\"g2727-4\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-30 -20,-10 z\"\n"
"         id=\"path2729-9\"\n"
"         sodipodi:nodetypes=\"ccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2731-5\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2733-0\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <g\n"
"       transform=\"translate(285.75,854.18898)\"\n"
"       id=\"g2727-48\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-30 -20,-10 z\"\n"
"         id=\"path2729-7\"\n"
"         sodipodi:nodetypes=\"ccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2731-1\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2733-72\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6096\"\n"
"       d=\"m 725,1053.189 20,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6098\"\n"
"       d=\"m 725,1033.189 20,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 742,1031.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-84-9-7\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 742,1051.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-84-9-2\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6121\"\n"
"       d=\"m 775,1043.189 190,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 957,1041.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-84-9-26\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6140\"\n"
"       d=\"m 725,968.18898 105,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 825,966.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6203\"\n"
"       d=\"m 725,788.18898 75,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6205\"\n"
"       d=\"m 725,843.18898 75,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6209\"\n"
"       d=\"m 830,848.86917 10,0\"\n"
"       style=\"opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 837,846.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 797,786.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-4\" />\n"
"    <path\n"
"       inkscape:transform-center-y=\"-36.209987\"\n"
"       inkscape:transform-center-x=\"19.842118\"\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 797,841.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-4-9\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6371\"\n"
"       d=\"m 725,913.18898 109,0 0,-45 6,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 837,866.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-9\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 884,861.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-7-7\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6403\"\n"
"       d=\"m 875,803.18898 0,85 85,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 957,886.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-9-7\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 957,831.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-9-6\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6426\"\n"
"       d=\"m 725,1003.189 80,0 0,70 15,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6428\"\n"
"       d=\"m 725,1013.189 70,0 0,70 25,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 812,1071.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-84-9-1-7\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 812,1081.189 0,4 8,-2 -8.00371,-2.2227 0.004,0.2227\"\n"
"       id=\"use2809-84-9-1-3\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6494\"\n"
"       d=\"m 725,663.18898 135,0 0,25 100,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 957,686.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-6-6\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 957,711.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-38-6-6-3\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6553\"\n"
"       d=\"m 825,1068.189 -10,0 0,-95.00002 -65,0 0,-215 70,0 0,15\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6555\"\n"
"       d=\"m 830,1063.189 -10,0 0,-115.00002 0,-65\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 822,765.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"       id=\"use2809-5-4-2-4-8\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 818,891.18898 4,0 -2,-8 -2.22263,8.00371 0.22263,-0.004\"\n"
"       id=\"use2809-5-4-2-4-1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6605\"\n"
"       d=\"m 735,1033.189 0,60 -300,0 0,-400.00002 20,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <circle\n"
"       id=\"path2921-9\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"735.10181\"\n"
"       cy=\"1033.4149\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6677\"\n"
"       d=\"m 886,963.18898 24,0 0,-85\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 908,884.18927 4,0 -2,-8 -2.22263,8.00371 0.22263,-0.004\"\n"
"       id=\"use2809-5-4-2-4-1-8\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6696\"\n"
"       d=\"m 725,708.18898 75,0 0,10 150,0 0,265 -55,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6698\"\n"
"       d=\"m 725,718.18898 65,0 0,20 70,0 0,90\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6700\"\n"
"       d=\"m 725,723.18898 10,0 0,260 30,0 0,35.00002\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 862,825.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"       id=\"use2809-5-4-2-4-8-5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 898,985.18898 0,-4 -8,2 8.00371,2.22263 -0.004,-0.22263\"\n"
"       id=\"use2809-5-4-2-4-8-0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 767,1010.189 -4,0 2,8 2.22263,-8.0037 -0.22263,0\"\n"
"       id=\"use2809-5-4-2-4-8-9\" />\n"
"    <text\n"
"       id=\"text6761\"\n"
"       y=\"777.52448\"\n"
"       x=\"861.84686\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"777.52448\"\n"
"         x=\"861.84686\"\n"
"         id=\"tspan6763\"\n"
"         sodipodi:role=\"line\">ALUSrc</tspan></text>\n"
"    <text\n"
"       id=\"text6761-8\"\n"
"       y=\"994.76556\"\n"
"       x=\"904.33545\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"994.76556\"\n"
"         x=\"904.33545\"\n"
"         id=\"tspan6763-5\"\n"
"         sodipodi:role=\"line\">ALUOp</tspan></text>\n"
"    <text\n"
"       transform=\"scale(0.99965812,1.000342)\"\n"
"       id=\"text6761-6\"\n"
"       y=\"997.74207\"\n"
"       x=\"733.87653\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:7.11169004px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"997.74207\"\n"
"         x=\"733.87653\"\n"
"         id=\"tspan6763-1\"\n"
"         sodipodi:role=\"line\">RegDst</tspan></text>\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6897\"\n"
"       d=\"m 935,813.18898 25,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 957,811.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-5-4-2-4-8-7\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2885-5-9\"\n"
"       y=\"582.75031\"\n"
"       x=\"788.07672\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"582.75031\"\n"
"         x=\"788.07672\"\n"
"         id=\"tspan2887-7-7\"\n"
"         sodipodi:role=\"line\">";
char *gr_3 = "</tspan></text>\n"
"    <circle\n"
"       id=\"path2600-0-4-6-4\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"874.83966\"\n"
"       cy=\"803.22687\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccccccc\"\n"
"       id=\"path2402-3\"\n"
"       d=\"m 898.33333,718.31296 20,10.66115 0,28.42976 -20,10.66115 0,-21.32232 6.66667,-3.55372 -6.66667,-3.55371 0,-21.32231 z\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#b0e57f;fill-opacity:1;stroke:#000000;stroke-width:1.37670493;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path4013\"\n"
"       d=\"m 725,733.18898 170,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <circle\n"
"       id=\"path2600-0-7-6\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"765.1601\"\n"
"       cy=\"968.21356\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 888,751.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-7-75\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 888,731.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-7-3\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path4057\"\n"
"       d=\"m 918,748.18898 42,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.87082887;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 958,746.18898 0,4 7,-2 -7.00325,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-7-75-5\" />\n"
"    <text\n"
"       transform=\"matrix(0,1.1157117,-0.89628888,0,0,0)\"\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2414-6\"\n"
"       y=\"-1012.5134\"\n"
"       x=\"665.1731\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:9.57165146px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"-1012.5134\"\n"
"         x=\"665.1731\"\n"
"         id=\"tspan2416-2\"\n"
"         sodipodi:role=\"line\">Adder</tspan></text>\n"
"    <g\n"
"       transform=\"matrix(0.20345696,0,0,0.20945946,666.11153,446.69615)\"\n"
"       id=\"g5176\">\n"
"      <circle\n"
"         style=\"opacity:1;fill:#b0e57f;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"         id=\"path5166\"\n"
"         cx=\"485\"\n"
"         cy=\"1533.189\"\n"
"         r=\"35\" />\n"
"      <text\n"
"         transform=\"scale(0.71805623,1.3926486)\"\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:16.30879211px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"         x=\"632.52826\"\n"
"         y=\"1107.8468\"\n"
"         id=\"text5172\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan5174\"\n"
"           x=\"632.52826\"\n"
"           y=\"1107.8468\">Shift Left 2</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       id=\"use2809-84-9-15-4-91\"\n"
"       d=\"m 763.47364,784.18898 3.05272,0 -1.52636,-6 -1.69627,6.00278 0.16991,-0.003\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:1.51312304;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       sodipodi:nodetypes=\"ccccc\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5196\"\n"
"       d=\"m 765,968.18898 0,-189\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5198\"\n"
"       d=\"m 765,760.18898 0,-7 123,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"  </g>\n"
"  ";
  char *gr4 = "  id=\"g5065\">\n"
"   <text x=\"1265\" y=\"970\" font-family=\"Verdana\" font-size=\"25\">  Memory Access Thread </text>  <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 928,1085.189 0,-4 -8,2 8.00371,2.2226 -0.004,-0.2226\"\n"
"       id=\"use2809-5-4-2-4-2\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6578\"\n"
"       d=\"m 995,688.18898 0,400.00002 -50,0 0,-5 -20,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <g\n"
"       id=\"g4750\">\n"
"      <path\n"
"         sodipodi:nodetypes=\"cc\"\n"
"         inkscape:connector-curvature=\"0\"\n"
"         id=\"path6317\"\n"
"         d=\"m 789,808.18898 11,0\"\n"
"         style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"      <path\n"
"         inkscape:connector-curvature=\"0\"\n"
"         id=\"path6319\"\n"
"         d=\"m 790,873.18898 10,0\"\n"
"         style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"      <g\n"
"         id=\"g4628\">\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1002,791.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-5-4-2-4-8-7-1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1002,801.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-5-4-2-4-8-7-2\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"cccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6969\"\n"
"           d=\"m 990,813.18898 9,0 0,-10 11,0\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6811\"\n"
"           d=\"m 990,708.18898 45,0 0,50 80,0 0,175 -55,0 0,-15\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6813\"\n"
"           d=\"m 990,718.18898 35,0 0,60 35,0 0,30\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"cccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6971\"\n"
"           d=\"m 990,723.18898 9,0 0,70 11,0\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <g\n"
"           transform=\"translate(925,638.18898)\"\n"
"           id=\"g2662\">\n"
"          <rect\n"
"             style=\"fill:#efdf23;fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"             id=\"rect2664\"\n"
"             width=\"75\"\n"
"             height=\"100\"\n"
"             x=\"100\"\n"
"             y=\"175\" />\n"
"          <text\n"
"             xml:space=\"preserve\"\n"
"             style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:125%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"             x=\"112.67969\"\n"
"             y=\"228.12598\"\n"
"             id=\"text2666\"\n"
"             sodipodi:linespacing=\"125%\"><tspan\n"
"               sodipodi:role=\"line\"\n"
"               id=\"tspan2668\"\n"
"               x=\"112.67969\"\n"
"               y=\"228.12598\">Memory</tspan></text>\n"
"        </g>\n"
"        <path\n"
"           sodipodi:nodetypes=\"cc\"\n"
"           id=\"path2757\"\n"
"           d=\"m 985,838.18898 35,0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           inkscape:connector-curvature=\"0\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccc\"\n"
"           id=\"path2761\"\n"
"           d=\"m 985,888.18898 10,0 25,0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           inkscape:connector-curvature=\"0\" />\n"
"        <path\n"
"           id=\"path2781\"\n"
"           d=\"m 1100.44,863.18898 24.8,0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           inkscape:connector-curvature=\"0\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"cccccc\"\n"
"           id=\"path2807\"\n"
"           d=\"m 990,748.18898 100,0 0,-120 -930,0 0,310 35,0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2.10743213;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           inkscape:connector-curvature=\"0\" />\n"
"        <text\n"
"           sodipodi:linespacing=\"100%\"\n"
"           id=\"text2861\"\n"
"           y=\"518.56012\"\n"
"           x=\"1062.1094\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:16px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             id=\"tspan2869\"\n"
"             y=\"518.56012\"\n"
"             x=\"1062.1094\"\n"
"             sodipodi:role=\"line\">Memory Access</tspan></text>\n"
"        <text\n"
"           sodipodi:linespacing=\"100%\"\n"
"           id=\"text2897\"\n"
"           y=\"558.18903\"\n"
"           x=\"1038.9062\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             y=\"558.18903\"\n"
"             x=\"1038.9062\"\n"
"             id=\"tspan2899\"\n"
"             sodipodi:role=\"line\">MEM</tspan></text>\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1122,861.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-84-99-20\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1012,836.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-84-99-21\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1012,886.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-84-99-75\" />\n"
"        <rect\n"
"           style=\"opacity:1;fill:#efdf23;fill-opacity:1;stroke:#000000;stroke-width:1.27413368;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\"\n"
"           id=\"rect4975-9-9\"\n"
"           width=\"10\"\n"
"           height=\"349.99997\"\n"
"           x=\"980\"\n"
"           y=\"728.18903\" />\n"
"        <rect\n"
"           y=\"702.59778\"\n"
"           x=\"980\"\n"
"           height=\"25\"\n"
"           width=\"10\"\n"
"           id=\"rect5114-0-4\"\n"
"           style=\"opacity:1;fill:#efdf23;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;image-rendering:auto\" />\n"
"        <rect\n"
"           y=\"677.59778\"\n"
"           x=\"980\"\n"
"           height=\"25\"\n"
"           width=\"10\"\n"
"           id=\"rect5114-0-3\"\n"
"           style=\"opacity:1;fill:#efdf23;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;image-rendering:auto\" />\n"
"        <path\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"           d=\"m 1000,1063.189 0,20 -40,0 0,-15 -15,0 -25,0\"\n"
"           id=\"path6471\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           sodipodi:nodetypes=\"cccccc\" />\n"
"        <rect\n"
"           y=\"728.18903\"\n"
"           x=\"1135\"\n"
"           height=\"349.99997\"\n"
"           width=\"10\"\n"
"           id=\"rect2531-2-6\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#efdf23;fill-opacity:1;stroke:#000000;stroke-width:1.26499987;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\" />\n"
"        <rect\n"
"           y=\"703.18903\"\n"
"           x=\"1135\"\n"
"           height=\"25\"\n"
"           width=\"10\"\n"
"           id=\"rect5112-2-10\"\n"
"           style=\"opacity:1;fill:#efdf23;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"cccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6313\"\n"
"           d=\"m 1005,838.18898 0,270.00002 -215,0 0,-300.00002\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <circle\n"
"           id=\"path2600-0-7\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           cx=\"789.96301\"\n"
"           cy=\"873.14435\"\n"
"           r=\"2.5\" />\n"
"        <path\n"
"           inkscape:transform-center-y=\"-36.209987\"\n"
"           inkscape:transform-center-x=\"19.842118\"\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 797,871.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"           id=\"use2809-84-9-15-4-9-1\" />\n"
"        <path\n"
"           inkscape:transform-center-y=\"-36.209987\"\n"
"           inkscape:transform-center-x=\"19.842118\"\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 797,806.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"           id=\"use2809-84-9-15-4-9-1-1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"cc\"\n"
"           id=\"path2909\"\n"
"           d=\"m 1130,1063.189 -140,0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           inkscape:connector-curvature=\"0\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1122,1061.189 0,4 8,-2 -8.0037,-2.2227 0,0.2227\"\n"
"           id=\"use2809-84-99-17\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 926,1070.189 0,-4 -8,2 8.00371,2.2226 -0.004,-0.2226\"\n"
"           id=\"use2809-84-9-26-6\" />\n"
"        <path\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6517\"\n"
"           d=\"m 990,688.18898 55,0 0,25 80,0\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1122,711.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-38-6-6-6-9\" />\n"
"        <circle\n"
"           id=\"path2600-0-4\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           cx=\"1004.8974\"\n"
"           cy=\"838.20605\"\n"
"           r=\"2.5\" />\n"
"        <circle\n"
"           id=\"path2600-0-4-6\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           cx=\"1005.1389\"\n"
"           cy=\"937.84998\"\n"
"           r=\"2.5\" />\n"
"        <path\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6744\"\n"
"           d=\"m 1005,938.18898 125,0\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1123,936.18898 0,4 8,-2 -8.0037,-2.22263 0,0.22263\"\n"
"           id=\"use2809-84-99-20-3\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1062,800.18898 -4,0 2,8 2.2226,-8.00371 -0.2226,0.004\"\n"
"           id=\"use2809-5-4-2-4-8-9-8\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 1058,926.18898 4,0 -2,-8 -2.2226,8.0038 0.2226,0\"\n"
"           id=\"use2809-5-4-2-4-8-9-4\" />\n"
"        <text\n"
"           transform=\"scale(0.96223599,1.0392461)\"\n"
"           id=\"text6761-81\"\n"
"           y=\"763.54346\"\n"
"           x=\"1102.6793\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:7.68450642px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             y=\"763.54346\"\n"
"             x=\"1102.6793\"\n"
"             id=\"tspan6763-0\"\n"
"             sodipodi:role=\"line\">MemWrite</tspan></text>\n"
"        <text\n"
"           id=\"text6761-3\"\n"
"           y=\"927.52448\"\n"
"           x=\"1066.8468\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             y=\"927.52448\"\n"
"             x=\"1066.8468\"\n"
"             id=\"tspan6763-04\"\n"
"             sodipodi:role=\"line\">MemRead</tspan></text>\n"
"        <path\n"
"           sodipodi:nodetypes=\"cccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6967\"\n"
"           d=\"m 1014,783.42708 21,0 c 4.3378,2.56667 5.2828,7.59522 6.5625,12.38095 -0.413,4.68495 -3.2554,8.60603 -6.5625,12.38095 l -21,0 0,-26\"\n"
"           style=\"opacity:1;fill:#efdf23;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2.54950976;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           id=\"path6994\"\n"
"           d=\"m 1043,796.18898 7,0 0,-198 -840,0 0,300\"\n"
"           style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"        <text\n"
"           transform=\"scale(0.78582862,1.2725421)\"\n"
"           id=\"text7016\"\n"
"           y=\"628.5564\"\n"
"           x=\"1292.2225\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.60349941px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#000000;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             y=\"628.5564\"\n"
"             x=\"1292.2225\"\n"
"             id=\"tspan7018\"\n"
"             sodipodi:role=\"line\">Branch</tspan></text>\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 187,936.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"           id=\"use2809-56\" />\n"
"        <path\n"
"           sodipodi:nodetypes=\"ccccc\"\n"
"           inkscape:connector-curvature=\"0\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           d=\"m 212,895.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"           id=\"use2809-5-4-2-9\" />\n"
"        <text\n"
"           inkscape:transform-center-y=\"-1.9706898\"\n"
"           inkscape:transform-center-x=\"-1.9706898\"\n"
"           id=\"text6761-9-7\"\n"
"           y=\"866.82074\"\n"
"           x=\"211.23853\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             y=\"866.82074\"\n"
"             x=\"211.23853\"\n"
"             id=\"tspan6763-6-8\"\n"
"             sodipodi:role=\"line\">PCSrc</tspan></text>\n"
"        <text\n"
"           sodipodi:linespacing=\"100%\"\n"
"           id=\"text2885-5-5\"\n"
"           y=\"583.34149\"\n"
"           x=\"996.93481\"\n"
"           style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"           xml:space=\"preserve\"><tspan\n"
"             y=\"583.34149\"\n"
"             x=\"996.93481\"\n"
"             id=\"tspan2887-7-3\"\n"
"             sodipodi:role=\"line\">";
char* gr_4 = "</tspan></text>\n"
"        <circle\n"
"           id=\"path2600-0-4-6-8\"\n"
"           style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"           cx=\"999.96643\"\n"
"           cy=\"1062.5879\"\n"
"           r=\"2.5\" />\n"
"      </g>\n"
"    </g>\n"
"  </g>" ;
  char *gr5 = "  id=\"g4964\">\n"
"   <text x=\"1265\" y=\"1070\" font-family=\"Verdana\" font-size=\"25\"> Write Back Thread   </text>  <g\n"
"       transform=\"translate(735.75,724.18898)\"\n"
"       id=\"g2670\">\n"
"      <path\n"
"         style=\"overflow:visible;visibility:visible;opacity:1;fill:#ff1ef2;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"         d=\"m 469.25,164 0,50 20,-10 0,-30 -20,-10 z\"\n"
"         id=\"path2672\"\n"
"         sodipodi:nodetypes=\"ccccc\"\n"
"         inkscape:connector-curvature=\"0\" />\n"
"      <text\n"
"         xml:space=\"preserve\"\n"
"         style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1\"\n"
"         x=\"188.6875\"\n"
"         y=\"-475\"\n"
"         id=\"text2674\"\n"
"         sodipodi:linespacing=\"100%\"\n"
"         transform=\"matrix(0,1,-1,0,0,0)\"><tspan\n"
"           sodipodi:role=\"line\"\n"
"           id=\"tspan2676\"\n"
"           x=\"188.6875\"\n"
"           y=\"-475\">MUX</tspan></text>\n"
"    </g>\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       id=\"path2785\"\n"
"       d=\"m 1155,873.18898 20,0 0,25 30,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccc\"\n"
"       id=\"path2787\"\n"
"       d=\"m 1205,928.18898 -30,0 0,25 -20,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccccc\"\n"
"       id=\"path2799\"\n"
"       d=\"m 1155,1038.189 35,0 0,90 -735,-5 0,-255.00002 24,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cccccc\"\n"
"       id=\"path2801\"\n"
"       d=\"m 479,893.18898 -14,0 0,220.00002 775,0 0,-195.00002 -15,0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2873\"\n"
"       y=\"518.74646\"\n"
"       x=\"1216.6167\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:16px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         id=\"tspan2875\"\n"
"         y=\"518.74646\"\n"
"         x=\"1216.6167\"\n"
"         sodipodi:role=\"line\">Write Back</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2901\"\n"
"       y=\"558.74646\"\n"
"       x=\"1200.4839\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"558.74646\"\n"
"         x=\"1200.4839\"\n"
"         id=\"tspan2903\"\n"
"         sodipodi:role=\"line\">WB</tspan></text>\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 477,891.18897 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-37\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 477,866.18897 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-9\" />\n"
"    <path\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       d=\"m 1155,1063.189 10,0 0,30 -245,0\"\n"
"       id=\"path6469\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <rect\n"
"       style=\"opacity:1;fill:#ff1ef2;fill-opacity:1;stroke:#000000;stroke-width:1.27413368;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1\"\n"
"       id=\"rect4975-9-2\"\n"
"       width=\"10\"\n"
"       height=\"349.99997\"\n"
"       x=\"1145\"\n"
"       y=\"728.18903\" />\n"
"    <rect\n"
"       y=\"703.18903\"\n"
"       x=\"1145\"\n"
"       height=\"25\"\n"
"       width=\"10\"\n"
"       id=\"rect5114-0-63\"\n"
"       style=\"opacity:1;fill:#ff1ef2;fill-opacity:1;stroke:#000000;stroke-width:0;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1;image-rendering:auto\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6271\"\n"
"       d=\"m 780,1113.189 0,-315.00002 20,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <circle\n"
"       id=\"path2600-0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"779.91119\"\n"
"       cy=\"858.03638\"\n"
"       r=\"2.5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"cc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6288\"\n"
"       d=\"m 780,858.18898 20,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#000000;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:transform-center-y=\"-36.209987\"\n"
"       inkscape:transform-center-x=\"19.842118\"\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 797,856.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-4-9-9\" />\n"
"    <path\n"
"       inkscape:transform-center-y=\"-36.209987\"\n"
"       inkscape:transform-center-x=\"19.842118\"\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 797,796.18898 0,4 8,-2 -8.00371,-2.22263 0.004,0.22263\"\n"
"       id=\"use2809-84-9-15-4-9-1-5\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 925,1095.189 0,-4 -8,2 8.00371,2.2226 -0.004,-0.2226\"\n"
"       id=\"use2809-84-9-26-5\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6588\"\n"
"       d=\"m 1155,718.18898 15,0 0,385.00002 -255,0\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 914,1105.189 0,-4 -8,2 8.00371,2.2226 -0.004,-0.2226\"\n"
"       id=\"use2809-5-4-2-4-2-9\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6637\"\n"
"       d=\"m 1170,718.18898 0,-110 -580,0 0,190\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 592,795.18898 -4,0 2,8 2.22263,-8.00371 -0.22263,0.004\"\n"
"       id=\"use2809-5-4-2-4-0\" />\n"
"    <circle\n"
"       inkscape:transform-center-y=\"1.8115345\"\n"
"       inkscape:transform-center-x=\"-2.0902321\"\n"
"       id=\"path2921-9-8\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;stroke:#339dfa;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"1169.9554\"\n"
"       cy=\"717.7373\"\n"
"       r=\"2.5\" />\n"
"    <text\n"
"       id=\"text6761-1\"\n"
"       y=\"787.22925\"\n"
"       x=\"591.53503\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:8.75px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"787.22925\"\n"
"         x=\"591.53503\"\n"
"         id=\"tspan6763-59\"\n"
"         sodipodi:role=\"line\">RegWrite</tspan></text>\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path6862\"\n"
"       d=\"m 1155,723.18898 60,0 0,160\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;fill-rule:nonzero;stroke:#339dfa;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       sodipodi:nodetypes=\"ccccc\"\n"
"       inkscape:connector-curvature=\"0\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:evenodd;stroke:#339dfa;stroke-width:2.00000024;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       d=\"m 1217,880.18898 -4,0 2,8 2.2226,-8.00371 -0.2226,0.004\"\n"
"       id=\"use2809-5-4-2-4-8-9-8-4\" />\n"
"    <text\n"
"       transform=\"scale(0.96223599,1.0392461)\"\n"
"       id=\"text6761-81-4\"\n"
"       y=\"833.63593\"\n"
"       x=\"1265.3745\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:7.68450642px;font-family:sans-serif;-inkscape-font-specification:sans-serif;opacity:1;fill:#339dfa;fill-opacity:1;fill-rule:nonzero;stroke:none;stroke-width:2;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"833.63593\"\n"
"         x=\"1265.3745\"\n"
"         id=\"tspan6763-0-4\"\n"
"         sodipodi:role=\"line\">MemtoReg</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2885-5-88\"\n"
"       y=\"582.30066\"\n"
"       x=\"1151.1793\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:20px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:start;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"582.30066\"\n"
"         x=\"1151.1793\"\n"
"         id=\"tspan2887-7-31\"\n"
"         sodipodi:role=\"line\">";
char* gr_5 = "</tspan></text>\n"
"    <circle\n"
"       id=\"path2600-0-4-6-9\"\n"
"       style=\"overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#000000;stroke-width:2;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       cx=\"780.08435\"\n"
"       cy=\"1112.7367\"\n"
"       r=\"2.5\" />\n"
"  </g>\n"
"  ";
  char *extra_g = "  id=\"g5130\">\n"
"    <text x=\"1265\" y=\"570\" font-family=\"Verdana\" font-size=\"25\" >  Threads Active : </text> <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2535\"\n"
"       y=\"1092.4589\"\n"
"       x=\"415.74554\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"1092.4589\"\n"
"         x=\"415.74554\"\n"
"         id=\"tspan2537\"\n"
"         sodipodi:role=\"line\">IF / ID</tspan></text>\n"
"    <path\n"
"       id=\"path2877\"\n"
"       d=\"m 1145,498.18906 0,89.99992\"\n"
"       style=\"display:inline;overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#cc0000;stroke-width:2.35339355;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       id=\"path2879\"\n"
"       d=\"m 980,498.18906 0,89.99992\"\n"
"       style=\"display:inline;overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#cc0000;stroke-width:2.35339355;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       id=\"path2881\"\n"
"       d=\"m 715,498.18905 0,89.99993\"\n"
"       style=\"display:inline;overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#cc0000;stroke-width:2.35339355;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <path\n"
"       id=\"path2883\"\n"
"       d=\"m 415,498.18899 0,89.99994\"\n"
"       style=\"display:inline;overflow:visible;visibility:visible;opacity:1;fill:#000000;fill-opacity:1;stroke:#cc0000;stroke-width:2.35339355;stroke-opacity:1;marker:none;enable-background:accumulate\"\n"
"       inkscape:connector-curvature=\"0\" />\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2535-7\"\n"
"       y=\"1089.2952\"\n"
"       x=\"713.71545\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"1089.2952\"\n"
"         x=\"713.71545\"\n"
"         id=\"tspan2537-0\"\n"
"         sodipodi:role=\"line\">ID / EX</tspan></text>\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5116-2-1\"\n"
"       d=\"m 725,728.18902 0,-25.00001 -20,0 0,25.00001\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.64472711;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5116-2-3\"\n"
"       d=\"m 725,703.18901 0,-25.00001 -20,0 0,25.00001\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.64472711;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5116-2-7\"\n"
"       d=\"m 725,678.18901 0,-25.00001 -20,0 0,25.00001\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.64472711;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       transform=\"scale(0.87513281,1.1426837)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5262\"\n"
"       y=\"587.29248\"\n"
"       x=\"806.86884\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:12.29445171px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"587.29248\"\n"
"         x=\"806.86884\"\n"
"         id=\"tspan5264\"\n"
"         sodipodi:role=\"line\">WB</tspan></text>\n"
"    <text\n"
"       transform=\"scale(0.84190541,1.1877819)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5270\"\n"
"       y=\"608.71893\"\n"
"       x=\"838.90753\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:15.0138998px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"608.71893\"\n"
"         x=\"838.90753\"\n"
"         id=\"tspan5272\"\n"
"         sodipodi:role=\"line\">EX</tspan></text>\n"
"    <text\n"
"       transform=\"scale(0.92096322,1.0858197)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5274\"\n"
"       y=\"645.13782\"\n"
"       x=\"766.40692\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:22.39881134px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"645.13782\"\n"
"         x=\"766.40692\"\n"
"         id=\"tspan5276\"\n"
"         sodipodi:role=\"line\">M</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2535-7-8\"\n"
"       y=\"673.92603\"\n"
"       x=\"975.73724\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"673.92603\"\n"
"         x=\"975.73724\"\n"
"         id=\"tspan2537-0-7\"\n"
"         sodipodi:role=\"line\">EX / MEM</tspan></text>\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5116-2-5\"\n"
"       d=\"m 990,727.59777 0,-25.00001 -20,0 0,25.00001\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.64472711;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5116-2-6\"\n"
"       d=\"m 990,702.59778 0,-25.00001 -20,0 0,25.00001\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.64472711;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       transform=\"scale(0.87513281,1.1426837)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5262-0\"\n"
"       y=\"609.29205\"\n"
"       x=\"1109.3269\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:12.29445171px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"609.29205\"\n"
"         x=\"1109.3269\"\n"
"         id=\"tspan5264-6\"\n"
"         sodipodi:role=\"line\">WB</tspan></text>\n"
"    <text\n"
"       transform=\"scale(0.92096322,1.0858197)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5274-1\"\n"
"       y=\"666.46234\"\n"
"       x=\"1054.5626\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:22.39881134px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"666.46234\"\n"
"         x=\"1054.5626\"\n"
"         id=\"tspan5276-5\"\n"
"         sodipodi:role=\"line\">M</tspan></text>\n"
"    <text\n"
"       sodipodi:linespacing=\"100%\"\n"
"       id=\"text2535-7-0\"\n"
"       y=\"1088.7408\"\n"
"       x=\"1132.1454\"\n"
"       style=\"font-style:normal;font-variant:normal;font-weight:normal;font-stretch:normal;font-size:12px;line-height:100%;font-family:'DejaVu Sans';-inkscape-font-specification:'DejaVu Sans';text-align:center;writing-mode:lr-tb;text-anchor:middle;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"1088.7408\"\n"
"         x=\"1132.1454\"\n"
"         id=\"tspan2537-0-6\"\n"
"         sodipodi:role=\"line\">MEM / WB</tspan></text>\n"
"    <path\n"
"       inkscape:connector-curvature=\"0\"\n"
"       id=\"path5116-2-2\"\n"
"       d=\"m 1155,728.18901 0,-25.00001 -20,0 0,25.00001\"\n"
"       style=\"opacity:1;fill:none;fill-opacity:1;stroke:#000000;stroke-width:1.64472711;stroke-linecap:round;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n"
"    <text\n"
"       transform=\"scale(0.87513281,1.1426837)\"\n"
"       sodipodi:linespacing=\"125%\"\n"
"       id=\"text5262-0-7\"\n"
"       y=\"631.73126\"\n"
"       x=\"1298.1598\"\n"
"       style=\"font-style:normal;font-weight:normal;font-size:12.29445171px;line-height:125%;font-family:sans-serif;letter-spacing:0px;word-spacing:0px;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1\"\n"
"       xml:space=\"preserve\"><tspan\n"
"         y=\"631.73126\"\n"
"         x=\"1298.1598\"\n"
"         id=\"tspan5264-6-6\"\n"
"         sodipodi:role=\"line\">WB</tspan></text>\n"
"  </g>\n"
"";
  char *hide = " <g style=\"opacity:0.3\" ";
  char *dont_hide = " <g \n";
  char *end = "</g><script type=\"text/javascript\">function refresh() {setTimeout(function () {location.reload()}, 500);}</script> </svg> ";
  fprintf(fpout,"%s",beginning);
  if(is_g1==1){
   	 fprintf(fpout,"%s",dont_hide);
 	 fprintf(fpout,"%s%s%s",gr1,print1,gr_1);
  }
  else{
    fprintf(fpout,"%s",hide_special);
    fprintf(fpout,"%s%s%s",gr1,print1,gr_1);
  }
  if(is_g2==1){
    fprintf(fpout,"%s",dont_hide);
    fprintf(fpout,"%s%s%s",gr2,print2,gr_2);
  }
  else{
    fprintf(fpout,"%s",hide_special);
    fprintf(fpout,"%s%s%s",gr2,print2,gr_2);
  }
  if(is_g3==1){
    fprintf(fpout,"%s",dont_hide);
    fprintf(fpout,"%s%s%s",gr3,print3,gr_3);
  }
  else{
    fprintf(fpout,"%s",hide);
    fprintf(fpout,"%s%s%s",gr3,print3,gr_3);
  }
  if(is_g4==1){
    fprintf(fpout,"%s",dont_hide);
    fprintf(fpout,"%s%s%s",gr4,print4,gr_4);
  }
  else{
    fprintf(fpout,"%s",hide);
    fprintf(fpout,"%s%s%s",gr4,print4,gr_4);
  }
  if(is_g5==1){
    fprintf(fpout,"%s",dont_hide);
    fprintf(fpout,"%s%s%s",gr5,print5,gr_5);
  }
  else{
    fprintf(fpout,"%s",hide);
    fprintf(fpout,"%s%s%s",gr5,print5,gr_5);
  }
  fprintf(fpout,"%s",dont_hide);
  fprintf(fpout,"%s",extra_g);
  fprintf(fpout,"%s",end);
}
