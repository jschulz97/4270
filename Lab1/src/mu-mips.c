#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "mu-mips.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/
void help() {        
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********MU-MIPS Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("high <val>\t-- set the HI register to <val>\n");
	printf("low <val>\t-- set the LO register to <val>\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                                                                            */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return  (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                                                                                */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {                                                
	handle_instruction();
	CURRENT_STATE = NEXT_STATE;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/* Simulate MIPS for n cycles                                                                                       */
/***************************************************************/
void run(int num_cycles) {                                      
	
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                                                                               */
/***************************************************************/
void runAll() {                                                     
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/***************************************************************/ 
/* Dump a word-aligned region of memory to the terminal                              */
/***************************************************************/
void mdump(uint32_t start, uint32_t stop) {          
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal                                              */   
/***************************************************************/
void rdump() {                               
	int i; 
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < MIPS_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	printf("[HI]\t: 0x%08x\n", CURRENT_STATE.HI);
	printf("[LO]\t: 0x%08x\n", CURRENT_STATE.LO);
	printf("-------------------------------------\n");
}

/***************************************************************/
/* Read a command from standard input.                                                               */  
/***************************************************************/
void handle_command() {                         
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;
	int hi_reg_value, lo_reg_value;

	printf("MU-MIPS SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			runAll(); 
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting MU-MIPS! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'H':
		case 'h':
			if (scanf("%i", &hi_reg_value) != 1){
				break;
			}
			CURRENT_STATE.HI = hi_reg_value; 
			NEXT_STATE.HI = hi_reg_value; 
			break;
		case 'L':
		case 'l':
			if (scanf("%i", &lo_reg_value) != 1){
				break;
			}
			CURRENT_STATE.LO = lo_reg_value;
			NEXT_STATE.LO = lo_reg_value;
			break;
		case 'P':
		case 'p':
			print_program(); 
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                                                    */
/***************************************************************/
void reset() {   
	int i;
	/*reset registers*/
	for (i = 0; i < MIPS_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	CURRENT_STATE.HI = 0;
	CURRENT_STATE.LO = 0;
	
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
	
	/*load program*/
	load_program();
	
	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/***************************************************************/
/* Allocate and set memory to zero                                                                            */
/***************************************************************/
void init_memory() {                                           
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                                                                      */
/**************************************************************/
void load_program() {                   
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */

	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}

/************************************************************/
/* decode and execute instruction                                                                     */ 
/************************************************************/
void handle_instruction()
{
	/*IMPLEMENT THIS*/
	/* execute one instruction at a time. Use/update CURRENT_STATE and and NEXT_STATE, as necessary.*/
	uint32_t instruction;
	instruction = mem_read_32(CURRENT_STATE.PC);

	NEXT_STATE = CURRENT_STATE;

	//printf("%d",CURRENT_STATE.PC);

	/*printf("\n\ninstruction:  ");
	printf("%x",instruction);
	printf("\n");*/

	r_type_data	data_r = parse_r_type(instruction);
	i_type_data data_i = parse_i_type(instruction);
	j_type_data data_j = parse_j_type(instruction);

	uint32_t opcode;
	uint32_t msb;
	uint32_t temp;
	uint64_t temp64;
	int jump = 0;
	opcode = instruction & 0xFC000000;

	/*printf("\n\nopcode:  ");
	printf("%x",opcode);
	printf("\n\n");*/

	switch(opcode) {
		case 0x00000000:
		    ;// RType
			//Handle rtypes
			uint32_t func;
			func = instruction & 0x0000003F;

			switch(func) {
				case 0x00000010: // MFHI
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.HI;
					break;
				case 0x00000012: //MFLO
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.LO;
					break;
				case 0x00000011: //MTHI
					NEXT_STATE.HI = CURRENT_STATE.REGS[data_r.rs];
					break;
				case 0x00000013: //MTLO
					NEXT_STATE.LO = CURRENT_STATE.REGS[data_r.rs];
					break;
				case 0x00000018: //MULT
					temp64 = CURRENT_STATE.REGS[data_r.rs] * CURRENT_STATE.REGS[data_r.rt];
					NEXT_STATE.LO = (uint32_t)(temp64 & 0x00000000FFFFFFFF);
					NEXT_STATE.HI = (uint32_t)(temp64 & 0xFFFFFFFF00000000 / 0x100000000);
					break;
				case 0x00000019: //MULTU
					temp64 = CURRENT_STATE.REGS[data_r.rs] * CURRENT_STATE.REGS[data_r.rt];
					NEXT_STATE.LO = (uint32_t)(temp64 & 0x00000000FFFFFFFF);
					NEXT_STATE.HI = (uint32_t)(temp64 & 0xFFFFFFFF00000000 / 0x100000000);
					break;
				case 0x00000027: //NOR
					NEXT_STATE.REGS[data_r.rd] = ~(CURRENT_STATE.REGS[data_r.rs] | CURRENT_STATE.REGS[data_r.rt]);
					break;
				case 0x00000025: //OR
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] | CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x00000000: // SLL
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rt] << data_r.shamt;
					break;
				case 0x0000002A: // SLT
					if(CURRENT_STATE.REGS[data_r.rs] < CURRENT_STATE.REGS[data_r.rt]) {
						temp = 1;
					} else {
						temp = 0;
					}
					NEXT_STATE.REGS[data_r.rd] = temp;
					break;
				case 0x00000003: // SRA  - Sign extending????
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rt] / pow(2,data_r.shamt);
					break;
				case 0x00000002: // SRL
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rt] >> data_r.shamt;
					break;
				case 0x00000022: // SUB
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] - CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x00000023: // SUBU
					//if(CURRENT_STATE.REGS[data_r.rt] > CURRENT_STATE.REGS[data_r.rs])
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] - CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x0000000C: //SYSCALL
					if(CURRENT_STATE.REGS[0x2] == 0xA) {
						RUN_FLAG = 0;
					}
					break;
				case 0x00000026: // XOR
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] ^ CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x00000020: // ADD
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] + CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x00000021: // ADDU
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] + CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x00000024: // AND
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.REGS[data_r.rs] & CURRENT_STATE.REGS[data_r.rt];
					break;
				case 0x0000001A: // DIV
					if(CURRENT_STATE.REGS[data_r.rt] == 0) {
						//undefined
					} else {
						NEXT_STATE.LO = CURRENT_STATE.REGS[data_r.rs] / CURRENT_STATE.REGS[data_r.rt];
						NEXT_STATE.HI = CURRENT_STATE.REGS[data_r.rs] % CURRENT_STATE.REGS[data_r.rt];
					}
					break;
				case 0x0000001B: // DIVU
					if(CURRENT_STATE.REGS[data_r.rt] == 0) {
						//undefined
					} else {
						NEXT_STATE.LO = CURRENT_STATE.REGS[data_r.rs] / CURRENT_STATE.REGS[data_r.rt];
						NEXT_STATE.HI = CURRENT_STATE.REGS[data_r.rs] % CURRENT_STATE.REGS[data_r.rt];
					}
					break;
				case 0x00000009: // JALR
					NEXT_STATE.REGS[data_r.rd] = CURRENT_STATE.PC + 0x8;
					NEXT_STATE.PC = CURRENT_STATE.REGS[data_r.rs];
					jump = 1;
					break;
				case 0x00000008: // JR
					NEXT_STATE.PC = CURRENT_STATE.REGS[data_r.rs];
					jump = 1;
					break;
				default:
					printf("\n\nUnknown instruction in R type.\n\n");
					break;
			}
			break;

		case 0x04000000: // BGEZ & BLTZ
			;
			uint32_t rt;
			rt = instruction & 0x001F0000;

			switch(rt) {
				case 0x00010000: // BGEZ
					msb = (CURRENT_STATE.REGS[data_i.rs] & 0x80000000) / 0x80000000;
					if(msb == 0) {
						NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend(data_i.immediate * 0x4,16);
						jump = 1;
					}
					break;
				case 0x00000000: // BLTZ
					msb = (CURRENT_STATE.REGS[data_i.rs] & 0x80000000) / 0x80000000;
					if(msb == 1) {
						NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend(data_i.immediate * 0x4,16);
						jump = 1;
					}
					break;
				default:
					printf("\n\nUnknown instruction in B case.\n\n");
					break;
			}
			break;

		case 0x3C000000: // LUI
			;//parse i type
			NEXT_STATE.REGS[data_i.rt] = data_i.immediate * 0x10000;
			break;
		case 0x84000000: // LH
			temp = mem_read_32(sign_extend(data_i.immediate,16) + CURRENT_STATE.REGS[data_i.rs]);
			NEXT_STATE.REGS[data_i.rt] = sign_extend(temp,16);
			break;
		case 0x8C000000: // LW
			NEXT_STATE.REGS[data_i.rt] = mem_read_32(sign_extend(data_i.immediate,16) + CURRENT_STATE.REGS[data_i.rs]);
			break;
		case 0x34000000: // ORI
			NEXT_STATE.REGS[data_i.rt] = CURRENT_STATE.REGS[data_i.rs] | data_i.immediate;
			break;
		case 0xA0000000: // SB
			temp = sign_extend(data_i.immediate, 16) + CURRENT_STATE.REGS[data_i.rs];
			mem_write_32(temp,CURRENT_STATE.REGS[data_i.rt] & 0xFF);
			break;
		case 0xA4000000: // SH
			temp = sign_extend(data_i.immediate, 16) + CURRENT_STATE.REGS[data_i.rs];
			mem_write_32(temp,CURRENT_STATE.REGS[data_i.rt] & 0xFFFF);
			break;
		case 0x28000000: // SLTI
			if(CURRENT_STATE.REGS[data_i.rs] < sign_extend(data_i.immediate, 16)) {
				temp = 1;
			} else {
				temp = 0;
			}
			NEXT_STATE.REGS[data_i.rt] = temp;
			break;
		case 0xAC000000: // SW
			temp = sign_extend(data_i.immediate, 16) + CURRENT_STATE.REGS[data_i.rs];
			mem_write_32(temp,CURRENT_STATE.REGS[data_i.rt]);
			break;
		case 0x38000000: // XORI
			NEXT_STATE.REGS[data_i.rt] = CURRENT_STATE.REGS[data_i.rs] ^ data_i.immediate;
			break;
		case 0x20000000: // ADDI
			NEXT_STATE.REGS[data_i.rt] = sign_extend(data_i.immediate,16) + CURRENT_STATE.REGS[data_i.rs];
			break;
		case 0x24000000: // ADDIU
			//printf("%x\n",data_i.immediate );
			//printf("%x\n",sign_extend(data_i.immediate,16) );
			//printf("\n\n\n\n\n");
			NEXT_STATE.REGS[data_i.rt] = sign_extend(data_i.immediate,16) + CURRENT_STATE.REGS[data_i.rs];
			break;
		case 0x30000000: // ANDI
			NEXT_STATE.REGS[data_i.rt] = CURRENT_STATE.REGS[data_i.rs] & data_i.immediate;
			break;
		case 0x10000000: // BEQ
			if(CURRENT_STATE.REGS[data_i.rs] == CURRENT_STATE.REGS[data_i.rt]) {
				NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend(data_i.immediate * 0x4,16);
				jump = 1;
			}			
			break;
		case 0x1C000000: // BGTZ
			msb = (CURRENT_STATE.REGS[data_i.rs] & 0x80000000) / 0x80000000;
			if(msb == 0 || CURRENT_STATE.REGS[data_i.rs] != 0) {
				NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend(data_i.immediate * 0x4,16);
				jump = 1;
			}
			break;
		case 0x18000000: // BLEZ
			msb = (CURRENT_STATE.REGS[data_i.rs] & 0x80000000) / 0x80000000;
			if(msb == 1 || CURRENT_STATE.REGS[data_i.rs] == 0) {
				NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend(data_i.immediate * 0x4,16);
				jump = 1;
			}
			break;
		case 0x14000000: // BNE
			if(CURRENT_STATE.REGS[data_i.rs] != CURRENT_STATE.REGS[data_i.rt]) {
				NEXT_STATE.PC = CURRENT_STATE.PC + sign_extend(data_i.immediate * 0x4,16);
				jump = 1;
			}
			break;
		case 0x08000000: // J
			NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (data_j.target * 0x4);
			jump = 1;
			break;
		case 0x0C000000: // JAL
			NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 0x8;
			NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) + (CURRENT_STATE.REGS[data_j.target] * 0x4);
			jump = 1;
			break;
		case 0x80000000: // LB
			temp = mem_read_32(sign_extend(data_i.immediate,16) + CURRENT_STATE.REGS[data_i.rs]);
			NEXT_STATE.REGS[data_i.rt] = sign_extend(temp,24);
			break;
		default:
			printf("\n\nUnknown instruction in other case.\n\n");
			break;		
	}	

	if(!jump) {
		NEXT_STATE.PC = CURRENT_STATE.PC + 4;
	}

}


/************************************************************/
/* Parses instruction into data struct for R-type instr                                                                                                  */ 
/************************************************************/
r_type_data parse_r_type(uint32_t instr) {
	r_type_data data;
	data.rs 	= (instr & 0x03E00000) / 0x200000;
	data.rt 	= (instr & 0x001F0000) / 0x10000;
	data.rd 	= (instr & 0x0000F800) / 0x800;
	data.shamt 	= (instr & 0x000007E0) / 0x40;
	return data;
}


/************************************************************/
/* Parses instruction into data struct for I-type instr                                                                                                  */ 
/************************************************************/
i_type_data parse_i_type(uint32_t instr) {
	i_type_data data;
	data.rs 		= (instr & 0x03E00000) / 0x200000;
	data.rt 		= (instr & 0x001F0000) / 0x10000;
	data.immediate 	= (instr & 0x0000FFFF);
	//printf("\n\n%x %x %x %x",instr,data.rs,data.rt,data.immediate);
	return data;
}


/************************************************************/
/* Parses instruction into data struct for J-type instr                                                                                                  */ 
/************************************************************/
j_type_data parse_j_type(uint32_t instr) {
	j_type_data data;
	data.target	= (instr & 0x03FFFFFF);
	return data;
}


/************************************************************/
/* Parses instruction into data struct for J-type instr                                                                                                  */ 
/************************************************************/
uint32_t sign_extend(uint32_t instr,int amt) {
	/*
	start = 32 - amt;
	tmp = pow(2,start);
	mask = 0xFFFFFFFF / tmp;
*/

	uint32_t extend_bit;
	if(amt == 16) {	
		extend_bit = (instr & 0x8000) / 0x8000;	
		if(extend_bit == 0x1) {
			instr = instr | 0xFFFF0000;
		} else {
			instr = instr & 0x0000FFFF;
		}
	} else if(amt == 24) {
		extend_bit = (instr & 0x80) / 0x80;
		if(extend_bit == 0x1) {
			instr = instr | 0xFFFFFF00;
		} else {
			instr = instr & 0x000000FF;
		}
	}
	return instr;
}


/************************************************************/
/* Initialize Memory                                                                                                    */ 
/************************************************************/
void initialize() { 
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/************************************************************/
/* Print the program loaded into memory (in MIPS assembly format)    */ 
/************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	
	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		//printf("\n%x",addr);
		print_instruction(addr);
	}
}

/************************************************************/
/* Print the instruction at given memory address (in MIPS assembly format)    */
/************************************************************/
void print_instruction(uint32_t addr){
	uint32_t instruction = mem_read_32(addr);
	r_type_data	data_r = parse_r_type(instruction);
	i_type_data data_i = parse_i_type(instruction);
	j_type_data data_j = parse_j_type(instruction);

	uint32_t opcode;
	opcode = instruction & 0xFC000000;

	switch(opcode) {
		case 0x00000000:
		    ;// RType
			//Handle rtypes
			uint32_t func;
			func = instruction & 0x0000003F;

			switch(func) {
				case 0x00000010: // MFHI
					printf("MFHI R%d\n",data_r.rd);
					break;
				case 0x00000012: //MFLO
					printf("MFLO R%d\n",data_r.rd);
					break;
				case 0x00000011: //MTHI
					printf("MTHI R%d\n",data_r.rs);
					break;
				case 0x00000013: //MTLO
					printf("MTLO R%d\n",data_r.rs);
					break;
				case 0x00000018: //MULT
					printf("MULT R%d R%d\n",data_r.rs,data_r.rt);
					break;
				case 0x00000019: //MULTU
					printf("MULTU R%d R%d\n",data_r.rs,data_r.rt);
					break;
				case 0x00000027: //NOR
					printf("NOR R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000025: //OR
					printf("OR R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000000: // SLL
					printf("SLL R%d R%d %d\n",data_r.rd,data_r.rt,data_r.shamt);
					break;
				case 0x0000002A: // SLT
					printf("SLT R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000003: // SRA  - Sign extending????
					printf("SRA R%d R%d %d\n",data_r.rd,data_r.rt,data_r.shamt);
					break;
				case 0x00000002: // SRL
					printf("SRL R%d R%d %d\n",data_r.rd,data_r.rt,data_r.shamt);
					break;
				case 0x00000022: // SUB
					printf("SUB R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000023: // SUBU
					printf("SUBU R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x0000000C: //SYSCALL
					printf("SYSCALL\n");
					break;
				case 0x00000026: // XOR
					printf("XOR R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000020: // ADD
					printf("ADD R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000021: // ADDU
					printf("ADDU R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x00000024: // AND
					printf("AND R%d R%d R%d\n",data_r.rd,data_r.rs,data_r.rt);
					break;
				case 0x0000001A: // DIV
					printf("DIV R%d R%d\n",data_r.rs,data_r.rt);
					break;
				case 0x0000001B: // DIVU
					printf("DIVU R%d R%d\n",data_r.rs,data_r.rt);
					break;
				case 0x00000009: // JALR
					printf("JALR R%d R%d\n",data_r.rd,data_r.rs);
					break;
				case 0x00000008: // JR
					printf("JR R%d\n",data_r.rs);
					break;
				default:
					printf("\n\nUnknown instruction in R type.\n\n");
					break;
			}
			break;

		case 0x04000000: // BGEZ & BLTZ
			;
			uint32_t rt;
			rt = instruction & 0x001F0000;

			switch(rt) {
				case 0x00010000: // BGEZ
					printf("BGEZ R%d %d\n",data_i.rs,data_i.immediate);
					break;
				case 0x00000000: // BLTZ
					printf("BLTZ R%d %d\n",data_i.rs,data_i.immediate);
					break;
				default:
					printf("\n\nUnknown instruction in B case.\n\n");
					break;
			}
			break;

		case 0x3C000000: // LUI
			printf("LUI R%d %d\n",data_i.rt,data_i.immediate);
			break;
		case 0x84000000: // LH
			printf("LH R%d %d(R%d)\n",data_i.rt,data_i.immediate,data_i.rs);
			break;
		case 0x8C000000: // LW
			printf("LW R%d %d(R%d)\n",data_i.rt,data_i.immediate,data_i.rs);
			break;
		case 0x34000000: // ORI
			printf("ORI R%d R%d %d\n",data_i.rt,data_i.rs,data_i.immediate);
			break;
		case 0xA0000000: // SB
			printf("SB R%d %d(R%d)\n",data_i.rt,data_i.immediate,data_i.rs);
			break;
		case 0xA4000000: // SH
			printf("SH R%d %d(R%d)\n",data_i.rt,data_i.immediate,data_i.rs);
			break;
		case 0x28000000: // SLTI
			printf("SLTI R%d R%d %d\n",data_i.rt,data_i.rs,data_i.immediate);
			break;
		case 0xAC000000: // SW
			printf("SW R%d %d(R%d)\n",data_i.rt,data_i.immediate,data_i.rs);
			break;
		case 0x38000000: // XORI
			printf("XORI R%d R%d %d\n",data_i.rt,data_i.rs,data_i.immediate);
			break;
		case 0x20000000: // ADDI
			printf("ADDI R%d R%d %d\n",data_i.rt,data_i.rs,data_i.immediate);			
			break;
		case 0x24000000: // ADDIU
			printf("ADDIU R%d R%d %d\n",data_i.rt,data_i.rs,data_i.immediate);
			break;
		case 0x30000000: // ANDI
			printf("ANDI R%d R%d %d\n",data_i.rt,data_i.rs,data_i.immediate);
			break;
		case 0x10000000: // BEQ
			printf("BEQ R%d R%d %d\n",data_i.rs,data_i.rt,data_i.immediate);
			break;
		case 0x1C000000: // BGTZ
			printf("BGTZ R%d %d\n",data_i.rs,data_i.immediate);
			break;
		case 0x18000000: // BLEZ
			printf("BLEZ R%d %d\n",data_i.rs,data_i.immediate);
			break;
		case 0x14000000: // BNE
			printf("BNE R%d R%d %d\n",data_i.rs,data_i.rt,data_i.immediate);
			break;
		case 0x08000000: // J
			printf("J %d\n",data_j.target);
			break;
		case 0x0C000000: // JAL
			printf("JAL %d\n",data_j.target);
			break;
		case 0x80000000: // LB
			printf("LB R%d %d(R%d)\n",data_i.rt,data_i.immediate,data_i.rs);
			break;
		default:
			printf("\n\nUnknown instruction in other case.\n\n");
			break;		
	}	
}

/***************************************************************/
/* main                                                                                                                                   */
/***************************************************************/
int main(int argc, char *argv[]) {                              
	printf("\n**************************\n");
	printf("Welcome to MU-MIPS SIM...\n");
	printf("**************************\n\n");
	
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}

	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
