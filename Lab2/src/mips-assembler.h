#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

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

void load_program(char* prog_file);
void handle_instruction(char* instr);
char* to_lower(char* str);
void parse_params(char params[4][6],int dim);
void parse_params_s(char params[4][6],int dim);
i_type_data parse_registers_i(char params[4][6]); 
i_type_data parse_registers_i_2(char params[4][6]); 
i_type_data parse_registers_i_rs(char params[4][6]); 
r_type_data parse_registers_r(char params[4][6]); 
r_type_data parse_registers_r_2(char params[4][6]); 
r_type_data parse_registers_r_1(char params[4][6]); 
r_type_data parse_registers_r_shamt(char params[4][6]); 
j_type_data parse_registers_j(char params[4][6]); 
int get_int(char* str, int offset);
int get_int_dec(char* str, int offset);
uint32_t create_mach_code_i(i_type_data data,uint32_t op);
uint32_t create_mach_code_i_special(i_type_data data, uint32_t op);
uint32_t create_mach_code_r(r_type_data data,uint32_t op);
uint32_t create_mach_code_r_shamt(r_type_data data,uint32_t op);
uint32_t create_mach_code_j(j_type_data data,uint32_t op);
void output_instr(uint32_t instr);

