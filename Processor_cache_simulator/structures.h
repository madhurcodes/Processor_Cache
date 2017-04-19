struct instruct {
	char* value;
	struct instruct* next;
};
typedef struct instruct instruction;

struct DecodedInstructionStruct{
	char *name;
	char *instr;
	int rd;
	int rs;
	int rt;
	int shamt;
	int label;
	int address;
	int target;
	int offsetArray[16];
};
typedef struct DecodedInstructionStruct DecodedInstruction;

struct IF_ID_Data_Struct{
	int instructionBinary[32];
	char *instr;
	char *name;
	int rd;
	int rs;
	int rt;
	int shamt;
	int label;
	int address;
	int target;
	int offsetArray[16];
	char *identifier;
};
typedef struct IF_ID_Data_Struct IF_ID_Data;

struct IF_ID_Struct{
	IF_ID_Data left;
	IF_ID_Data right;
};
typedef struct IF_ID_Struct IF_ID_T;

struct ID_EX_Struct{
	DecodedInstruction left;
	DecodedInstruction right;
};
typedef struct ID_EX_Struct ID_EX_T;

struct EX_MEM_DATA_Struct{
	char *insname; //a for arithmentic //m for multiplication //l for load //b for branch //s for store
	int desRegister;
	int RegStore;
	int Data[32];
	int MemAddress;
	char *instr;
};
typedef struct EX_MEM_DATA_Struct EX_MEM_DATA;

struct EX_Mem_Struct{
	EX_MEM_DATA left;
	EX_MEM_DATA right;
};
typedef struct EX_Mem_Struct EX_MEM_T;

struct Mem_WB_DATA_Struct{
	int flag;
	char *insname;
	int desRegister;
	int RegStore;
	int Data[32];
	char *instr;
};
typedef struct Mem_WB_DATA_Struct Mem_WB_Data;

struct Mem_WB_Struct{
	Mem_WB_Data left;
	Mem_WB_Data right;
};
typedef struct Mem_WB_Struct MEM_WB_T;

struct Data_Element_Struct{
	int Data[32];
	int dest;
};
typedef struct Data_Element_Struct DataElement;

struct Data_Forwarding_struct{
	char* name;
	int rd;
	int rt;
	int rs;
	int value[32];
};
typedef struct Data_Forwarding_struct DataForwardingUnit;

struct StallWithForwardStruct{
	char *name;
	int rt;
	int rs;
	int rd;
};
typedef struct StallWithForwardStruct CheckSWithF;

FILE* fpin, *fpin3, *fpout, *fpout2, *fpout3;
char *filename, *filename2, *filename3;
