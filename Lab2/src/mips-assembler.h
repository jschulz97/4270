#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>



void load_program(char* prog_file);
void handle_instruction(char* instr);
char* to_lower(char* str);
void parse_params(char params[4][6],int dim);
void parse_registers(char t,char params[4][6]);
int get_int(char* str);

typedef struct R_Type_Instruction {
	uint32_t rs;
	uint32_t rt;
	uint32_t rd;
	uint32_t shamt;
} r_type_data;

typedef struct I_Type_Instruction {
	uint32_t rs;
	uint32_t rt;
	uint32_t immediate;
} i_type_data;

typedef struct J_Type_Instruction {
	uint32_t target;
} j_type_data;