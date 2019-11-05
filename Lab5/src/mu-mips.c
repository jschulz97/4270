#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

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
	printf("show\t-- print the current content of the pipeline registers\n");
	printf("?\t-- display help menu\n");
	printf("f <0/1>\t-- enable forwarding\n\n");
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
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
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
	handle_pipeline();
	CURRENT_STATE = NEXT_STATE;
	CYCLE_COUNT++;
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
	printf("# Cycles Executed\t: %u\n", CYCLE_COUNT);
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

	printf("\nMU-MIPS SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			if (buffer[1] == 'h' || buffer[1] == 'H'){
				show_pipeline();
			}else {
				runAll(); 
			}
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
		case 'f':
			if (scanf("%d", &ENABLE_FORWARDING) != 1) {
				break;
			}
			ENABLE_FORWARDING == 0 ? printf("Forwarding OFF\n") : printf("Forwarding ON\n");
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
/* maintain the pipeline            */ 
/************************************************************/
void handle_pipeline()
{
	/*INSTRUCTION_COUNT should be incremented when instruction is done*/
	/*Since we do not have branch/jump instructions, INSTRUCTION_COUNT should be incremented in WB stage */
	
	WB();
	MEM();
	EX();
	ID();
	IF();
}


/************************************************************/
/* writeback (WB) pipeline stage:            */ 
/************************************************************/
void WB()
{
	if(WB_FLAG == 1) {
		uint32_t instruction = MEM_WB.IR;
		uint32_t opcode = (instruction & 0xFC000000) >> 24;
		uint32_t function = instruction & 0x0000003F;

		switch(opcode) {
			case 0x00: //R type
				if(function == 0xc && MEM_WB.ALUOutput == 0xA) {
					RUN_FLAG = 0;
				} else {
					NEXT_STATE.REGS[MEM_WB.D] = MEM_WB.ALUOutput;
				}
				break;
			case 0x80: //LB
				NEXT_STATE.REGS[MEM_WB.D] = MEM_WB.LMD;
				break;
			case 0x84: //LH
				NEXT_STATE.REGS[MEM_WB.D] = MEM_WB.LMD;
				break;
			case 0x8C: //LW
				NEXT_STATE.REGS[MEM_WB.D] = MEM_WB.LMD;
				break;
			case 0xA0: //SB

				break;
			case 0xA4: //SH

				break; 
			case 0xAC: //SW

				break;
			default: //I Type
				NEXT_STATE.REGS[MEM_WB.D] = MEM_WB.ALUOutput;
				break;
		}
		CURRENT_STATE = NEXT_STATE;
	}
}


/************************************************************/
/* memory access (MEM) pipeline stage:           */ 
/************************************************************/
void MEM()
{
	if(MEM_FLAG == 1) {
		uint32_t instruction = EX_MEM.IR;
		uint32_t opcode = (instruction & 0xFC000000) >> 24;
		uint32_t function = instruction & 0x0000003F;
		MEM_WB.IR = EX_MEM.IR;
		MEM_WB.D  = EX_MEM.D;
		MEM_WB.B  = EX_MEM.B;
		MEM_WB.A  = EX_MEM.A;
		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
		MEM_WB.rs = EX_MEM.rs;
		MEM_WB.rd = EX_MEM.rd;
		MEM_WB.rt = EX_MEM.rt;
		MEM_WB.RegWrite = EX_MEM.RegWrite;

		switch(opcode) {
			case 0xA0: //SB
				MEM_WB.D =  MEM_WB.D & 0x000000FF;
				mem_write_32(MEM_WB.ALUOutput,MEM_WB.D);
				break;
			case 0xA4: //SH
				MEM_WB.D = MEM_WB.D & 0x0000FFFF;
				mem_write_32(MEM_WB.ALUOutput,MEM_WB.D);
				break;
			case 0xAC: //SW
				//MEM_WB.D = MEM_WB.D;
				mem_write_32(MEM_WB.ALUOutput,MEM_WB.D);
				break;
			case 0x80: //LB
				MEM_WB.LMD = mem_read_32(MEM_WB.ALUOutput) & 0x000000FF;
				break;
			case 0x84: //LH
				MEM_WB.LMD = mem_read_32(MEM_WB.ALUOutput) & 0x0000FFFF;
				break;
			case 0x8C: //LW
				MEM_WB.LMD = mem_read_32(MEM_WB.ALUOutput);
				break;
			default:
				break;
		}


		WB_FLAG = 1;
	}
}

/************************************************************/
/* execution (EX) pipeline stage:     */ 
/************************************************************/
void EX()
{
	if(EX_FLAG == 1) {
		uint32_t instruction = ID_EX.IR;
		uint32_t opcode = (instruction & 0xFC000000) >> 26;
		uint32_t function = instruction & 0x0000003F;

		EX_MEM.D = ID_EX.D;
		EX_MEM.IR = ID_EX.IR;
		EX_MEM.rs = ID_EX.rs;
		EX_MEM.rd = ID_EX.rd;
		EX_MEM.rt = ID_EX.rt;
		EX_MEM.RegWrite = ID_EX.RegWrite;

		if(opcode == 0x00){
			switch(function){
				case 0x00: //SLL
					EX_MEM.ALUOutput = ID_EX.B << ID_EX.sa;
					break;
				case 0x02: //SRL
					EX_MEM.ALUOutput = ID_EX.B >> ID_EX.sa;
					break;
				case 0x03: //SRA 
					if ((ID_EX.B & 0x80000000) == 1)
					{
						EX_MEM.ALUOutput =  ~(~ID_EX.B >> ID_EX.sa );
					}
					else {
						EX_MEM.ALUOutput =  ~(~ID_EX.B >> ID_EX.sa );
					}
					break;
				case 0x08: //JR
					NEXT_STATE.PC = ID_EX.A;
					BRANCH_FLAG = 1;
					 
					break;
				case 0x09: //JALR
					EX_MEM.ALUOutput = CURRENT_STATE.PC + 4;
					NEXT_STATE.PC    = ID_EX.A;
					BRANCH_FLAG = 1;
					 
					break;
				case 0x0C: //SYSCALL
					EX_MEM.ALUOutput = CURRENT_STATE.REGS[2];
					break;
				case 0x10: //MFHI
					EX_MEM.ALUOutput = CURRENT_STATE.HI;
					break;
				case 0x11: //MTHI
					NEXT_STATE.HI = ID_EX.A;
					break;
				case 0x12: //MFLO
					EX_MEM.ALUOutput = CURRENT_STATE.LO;
					break;
				case 0x13: //MTLO
					NEXT_STATE.LO = ID_EX.A;
					break;
				case 0x18: //MULT
					; uint32_t p1,p2,product;
					if ((ID_EX.A & 0x80000000) == 0x80000000){
						p1 = 0xFFFFFFFF00000000 | ID_EX.A;
					}else{
						p1 = 0x00000000FFFFFFFF & ID_EX.A;
					}
					if ((ID_EX.B & 0x80000000) == 0x80000000){
						p2 = 0xFFFFFFFF00000000 | ID_EX.B;
					}else{
						p2 = 0x00000000FFFFFFFF & ID_EX.B;
					}
					product = p1 * p2;
					NEXT_STATE.LO = (product & 0X00000000FFFFFFFF);
					NEXT_STATE.HI = (product & 0XFFFFFFFF00000000)>>32;
					break;
				case 0x19: //MULTU
					product = (uint64_t)ID_EX.A * (uint64_t)ID_EX.B;
					NEXT_STATE.LO = (product & 0X00000000FFFFFFFF);
					NEXT_STATE.HI = (product & 0XFFFFFFFF00000000)>>32;
					break;
				case 0x1A: //DIV 
					if(ID_EX.B != 0)
					{
						NEXT_STATE.LO = (int32_t)ID_EX.A / (int32_t)ID_EX.B;
						NEXT_STATE.HI = (int32_t)ID_EX.A % (int32_t)ID_EX.B;
					}
					 
					break;
				case 0x1B: //DIVU
					if(ID_EX.B != 0)
					{
						NEXT_STATE.LO = ID_EX.A / ID_EX.B;
						NEXT_STATE.HI = ID_EX.A % ID_EX.B;
					}
					 
					break;
				case 0x20: //ADD
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.B;
					 
					break;
				case 0x21: //ADDU 
					EX_MEM.ALUOutput = ID_EX.B + ID_EX.A;
					 
					break;
				case 0x22: //SUB
					EX_MEM.ALUOutput = ID_EX.A - ID_EX.B;
					 
					break;
				case 0x23: //SUBU
					EX_MEM.ALUOutput = ID_EX.A - ID_EX.B;
					 
					break;
				case 0x24: //AND
					EX_MEM.ALUOutput = ID_EX.A & ID_EX.B;
					 
					break;
				case 0x25: //OR
					EX_MEM.ALUOutput = ID_EX.A | ID_EX.B;
					 
					break;
				case 0x26: //XOR
					EX_MEM.ALUOutput = ID_EX.A ^ ID_EX.B;
					 
					break;
				case 0x27: //NOR
					EX_MEM.ALUOutput = ~(ID_EX.A | ID_EX.B);
					 
					break;
				case 0x2A: //SLT
					if(ID_EX.A < ID_EX.B){
						EX_MEM.ALUOutput = 0x1;
					}
					else{
						EX_MEM.ALUOutput = 0x0;
					}
					 
					break;
				default:
					printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
					break;
			}
		}
		else{
			switch(opcode){
				case 0x01:
					if(EX_MEM.rt == 0x00000){ //BLTZ
						if((ID_EX.A & 0x80000000) > 0){
							NEXT_STATE.PC = ID_EX.PC + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000)<<2 : (ID_EX.imm & 0x0000FFFF)<<2);
							BRANCH_FLAG = 1;
						}
						STALL_COUNT = 1;
						 
					}
					else if(EX_MEM.rt == 0x00001){ //BGEZ
						if((ID_EX.A & 0x80000000) == 0x0){
							NEXT_STATE.PC = ID_EX.PC + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000)<<2 : (ID_EX.imm & 0x0000FFFF)<<2);
							BRANCH_FLAG = 1;
						}
						STALL_COUNT = 1;
					}
					break;
				case 0x02: //J
					NEXT_STATE.PC = (ID_EX.PC & 0xF0000000) | (ID_EX.target << 2);
					BRANCH_FLAG = 1;
					STALL_COUNT = 1;
					 
					break;
				case 0x03: //JAL
					NEXT_STATE.PC = (ID_EX.PC & 0xF0000000) | (ID_EX.target << 2);
					NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
					BRANCH_FLAG = 1;
					STALL_COUNT = 1;
					 
					break;
				case 0x04: //BEQ
					if(ID_EX.A == ID_EX.B){
						NEXT_STATE.PC = ID_EX.PC + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000)<<2 : (ID_EX.imm & 0x0000FFFF)<<2);
						BRANCH_FLAG = 1;
					}
					STALL_COUNT = 1;
					 
					break;
				case 0x05: //BNE
					if(ID_EX.A != ID_EX.B){
						NEXT_STATE.PC = ID_EX.PC + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000)<<2 : (ID_EX.imm & 0x0000FFFF)<<2);
						BRANCH_FLAG = 1;
					}
					STALL_COUNT = 1;
					 
					break;
				case 0x06: //BLEZ
					if((ID_EX.A & 0x80000000) > 0 || ID_EX.A == 0){
						NEXT_STATE.PC = ID_EX.PC +  ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000)<<2 : (ID_EX.imm & 0x0000FFFF)<<2);
						BRANCH_FLAG = 1;
					}
					STALL_COUNT = 1;
					 
					break;
				case 0x07: //BGTZ
					if((ID_EX.A & 0x80000000) == 0x0 || ID_EX.A != 0){
						NEXT_STATE.PC = ID_EX.PC +  ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000)<<2 : (ID_EX.imm & 0x0000FFFF)<<2);
						BRANCH_FLAG = 1;
					}
					STALL_COUNT = 1;
					 
					break;
				case 0x08: //ADDI
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					break;
				case 0x09: //ADDIU
					EX_MEM.ALUOutput = ID_EX.A + ( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF));
					 
					break;
				case 0x0A: //SLTI
					if ( (  (int32_t)ID_EX.A - (int32_t)( (ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm | 0xFFFF0000) : (ID_EX.imm & 0x0000FFFF))) < 0){
						EX_MEM.ALUOutput = 0x1;
					}else{
						EX_MEM.ALUOutput = 0x0;
					}
					break;
				case 0x0C: //ANDI
					EX_MEM.ALUOutput = ID_EX.A & (ID_EX.imm & 0x0000FFFF);
					 
					break;
				case 0x0D: //ORI
					EX_MEM.ALUOutput = ID_EX.A | ID_EX.imm;
					EX_MEM.D 		 = ID_EX.D;
					break;
				case 0x0E: //XORI
					EX_MEM.ALUOutput = ID_EX.A ^ (ID_EX.imm & 0x0000FFFF);
					 
					break;
				case 0x0F: //LUI
					EX_MEM.ALUOutput = ID_EX.imm << 16;
					 
					break;
				case 0x20: //LB
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					EX_MEM.D = ID_EX.D;					 
					break;
				case 0x21: //LH
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					EX_MEM.D = ID_EX.D;
					break;
				case 0x23: //LW
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					EX_MEM.D = ID_EX.D;
					break;
				case 0x28: //SB
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					EX_MEM.D = ID_EX.D;
					break;
				case 0x29: //SH
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					EX_MEM.D = ID_EX.D;
					break;
				case 0x2B: //SW
					EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
					EX_MEM.D = ID_EX.D;
					break;
				default:
					// put more things here
					printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
					break;
			}
		}
		MEM_FLAG = 1;

	}
}


/************************************************************/
/* instruction decode (ID) pipeline stage:                                                         */ 
/************************************************************/
void ID()
{


	if(ID_FLAG == 1) {
		if(STALL_COUNT > 0) {
			;
		} else if(BRANCH_FLAG == 1) {
			;
		} else {
			uint32_t instruction, opcode, function, rs, rt, rd, sa, immediate, target;
			uint64_t product, p1, p2;
			
			uint32_t addr, data;
			
			//int branch_jump = FALSE;
			
			instruction = IF_ID.IR;
			printf("\n\n[0x%x]\t", instruction);
			
			opcode = (instruction & 0xFC000000) >> 26;
			function = instruction & 0x0000003F;
			rs = (instruction & 0x03E00000) >> 21;
			rt = (instruction & 0x001F0000) >> 16;
			rd = (instruction & 0x0000F800) >> 11;
			sa = (instruction & 0x000007C0) >> 6;
			immediate = instruction & 0x0000FFFF;
			target = instruction & 0x03FFFFFF;

			ID_EX.IR = IF_ID.IR;
			ID_EX.PC = IF_ID.PC;
			ID_EX.rs = rs;
			ID_EX.rd = rd;
			ID_EX.rt = rt;
			ID_EX.RegWrite = 0;

			ID_EX_Prev = ID_EX;
			
			if(opcode == 0x00){
				switch(function){
					case 0x00: //SLL
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.sa 	= sa;
						ID_EX.RegWrite = 1;
						break;
					case 0x02: //SRL
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.sa 	= sa;
						ID_EX.RegWrite = 1;
						break;
					case 0x03: //SRA
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.sa 	= sa;
						ID_EX.RegWrite = 1;
						break;
					case 0x08: //JR
						ID_EX.A     = CURRENT_STATE.REGS[rs];
						break;
					case 0x09: //JALR
						ID_EX.A     = CURRENT_STATE.REGS[rs];
						ID_EX.D   	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x0C: //SYSCALL
						;
						break;
					case 0x10: //MFHI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						break;
					case 0x11: //MTHI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						break;
					case 0x12: //MFLO
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	=rd;
						break;
					case 0x13: //MTLO
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						break;
					case 0x18: //MULT
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x19: //MULTU
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x1A: //DIV 
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x1B: //DIVU
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;

						break;
					case 0x20: //ADD
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x21: //ADDU 
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x22: //SUB
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x23: //SUBU
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x24: //AND
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x25: //OR
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x26: //XOR
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x27: //NOR
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					case 0x2A: //SLT
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.B 	= CURRENT_STATE.REGS[rt];
						ID_EX.D 	= rd;
						ID_EX.RegWrite = 1;
						break;
					default:
						printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
						break;
				}
			}
			else{
				switch(opcode){
					case 0x01:
						if(rt == 0x00000){ //BLTZ
							ID_EX.A     = CURRENT_STATE.REGS[rs];
							ID_EX.B     = CURRENT_STATE.REGS[rt];
							ID_EX.imm   = immediate;
						}
						else if(rt == 0x00001){ //BGEZ
							ID_EX.A     = CURRENT_STATE.REGS[rs];
							ID_EX.B     = CURRENT_STATE.REGS[rt];
							ID_EX.imm   = immediate;
						}
						break;
					case 0x02: //J
						ID_EX.target = target;
						break;
					case 0x03: //JAL
						ID_EX.target = target;
						break;
					case 0x04: //BEQ
						ID_EX.A     = CURRENT_STATE.REGS[rs];
						ID_EX.B     = CURRENT_STATE.REGS[rt];
						ID_EX.imm   = immediate;
						break;
					case 0x05: //BNE
						ID_EX.A     = CURRENT_STATE.REGS[rs];
						ID_EX.B     = CURRENT_STATE.REGS[rt];
						ID_EX.imm   = immediate;
						break;
					case 0x06: //BLEZ
						ID_EX.A     = CURRENT_STATE.REGS[rs];
						ID_EX.B     = CURRENT_STATE.REGS[rt];
						ID_EX.imm   = immediate;
						break;
					case 0x07: //BGTZ
						ID_EX.A     = CURRENT_STATE.REGS[rs];
						ID_EX.B     = CURRENT_STATE.REGS[rt];
						ID_EX.imm   = immediate;
						break;
					case 0x08: //ADDI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x09: //ADDIU
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x0A: //SLTI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x0C: //ANDI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x0D: //ORI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x0E: //XORI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x0F: //LUI
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x20: //LB
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x21: //LH
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x23: //LW
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= rt;
						ID_EX.rd    = rt;
						ID_EX.imm 	= immediate;
						ID_EX.RegWrite = 1;
						break;
					case 0x28: //SB
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= CURRENT_STATE.REGS[rt];
						ID_EX.imm 	= immediate;	
						break;
					case 0x29: //SH
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= CURRENT_STATE.REGS[rt];
						ID_EX.imm 	= immediate;
						break;
					case 0x2B: //SW
						ID_EX.A 	= CURRENT_STATE.REGS[rs];
						ID_EX.D 	= CURRENT_STATE.REGS[rt];
						ID_EX.imm 	= immediate;
						break;
					default:
						// put more things here
						printf("Instruction at 0x%x is not implemented!\n", CURRENT_STATE.PC);
						break;
				}
			}
			EX_FLAG = 1;


			if(ENABLE_FORWARDING == 1) {
				//uint32_t prev_op = (EX_MEM.IR & 0xFC000000) >> 26;
				if (EX_MEM.RegWrite && (EX_MEM.D != 0) && (EX_MEM.D == ID_EX.rs)) {
					controlA = 2;
					if(prev_op == 0x20 || prev_op == 0x21 || prev_op == 0x23) {	
						STALL_COUNT = 1;
					}
				}
				else if (EX_MEM.RegWrite && (EX_MEM.D != 0) && (EX_MEM.D == ID_EX.rt)) {
					controlB = 2;
					if(prev_op == 0x20 || prev_op == 0x21 || prev_op == 0x23) {	
						STALL_COUNT = 1;
					}
				}
				else if (MEM_WB.RegWrite && (MEM_WB.D != 0) && !(EX_MEM.RegWrite && (EX_MEM.D != 0) && (EX_MEM.D == ID_EX.rs)) && (MEM_WB.D == ID_EX.rs)) {
					controlA = 1;
				}
				else if (MEM_WB.RegWrite && (MEM_WB.D != 0) && !(EX_MEM.RegWrite && (EX_MEM.D != 0) && (EX_MEM.D == ID_EX.rt)) && (MEM_WB.D == ID_EX.rt)) {
					controlB = 1;
				}

				if(controlA == 2 && STALL_COUNT == 0) {
					if(prev_op == 0x20 || prev_op == 0x21 || prev_op == 0x23) {	
						ID_EX.A = MEM_WB.LMD;
					} else {
						ID_EX.A = EX_MEM.ALUOutput;
					}
					controlA = 0;
				}
				if(controlB == 2 && STALL_COUNT == 0) {
					if(prev_op == 0x20 || prev_op == 0x21 || prev_op == 0x23) {	
						ID_EX.B = MEM_WB.LMD;
					} else {
						switch(opcode) {
							case 0x28:
								ID_EX.D = EX_MEM.ALUOutput;
								break;
							case 0x29:
								ID_EX.D = EX_MEM.ALUOutput;
								break;
							case 0x2B:
								ID_EX.D = EX_MEM.ALUOutput;
								break;
							default:
								ID_EX.B = EX_MEM.ALUOutput;
								break;
						}
					}
					controlB = 0;
				}
				if(controlA == 1 && STALL_COUNT == 0) {
					if(prev_op == 0x20 || prev_op == 0x21 || prev_op == 0x23) {	
						ID_EX.A = MEM_WB.LMD;
					} else {
						ID_EX.A = EX_MEM.ALUOutput;
					}
					controlA = 0;
				}
				if(controlB == 1 && STALL_COUNT == 0) {
					ID_EX.B = EX_MEM.ALUOutput;
					controlB = 0;
				}

			} else {
				// If load-use hazard exists, set flag and clear out pipeline
				if ( (EX_MEM.RegWrite && (EX_MEM.rd != 0) && (EX_MEM.rd == ID_EX.rs)) ||
				 	 (EX_MEM.RegWrite && (EX_MEM.rd != 0) && (EX_MEM.rd == ID_EX.rt)) ) {
					STALL_COUNT = 2;
					EX_HAZARD = 1;
					ID_EX = Empty;
				} else {
					EX_HAZARD = 0;
				}

				// If prod-con hazard exists, set flag and clear out pipeline
				if ( (MEM_WB.RegWrite && (MEM_WB.rd != 0) && (MEM_WB.rd == ID_EX.rs)) ||
					 (MEM_WB.RegWrite && (MEM_WB.rd != 0) && (MEM_WB.rd == ID_EX.rt)) ) {
					STALL_COUNT = 1;
					MEM_HAZARD = 1;
					ID_EX = Empty;
				} else {
					MEM_HAZARD = 0;
				}
			}

			if(STALL_COUNT == 0) {
				prev_op = opcode;
			}
		}
	}
}

/************************************************************/
/* instruction fetch (IF) pipeline stage:                                                              */ 
/************************************************************/
void IF()
{
	if(BRANCH_FLAG == 1) {
		BRANCH_FLAG = 0;
		CURRENT_STATE.PC = NEXT_STATE.PC;
		printf("\n\nBranch taken: IF");
	}
	if(STALL_COUNT == 0) {
		IF_ID.IR = mem_read_32(CURRENT_STATE.PC);
		IF_ID.PC = CURRENT_STATE.PC;
		NEXT_STATE.PC = IF_ID.PC + 4;
	} else {
		printf("\n\nStalling!\n");
	}
	ID_FLAG = 1;
	STALL_COUNT--;
	if(STALL_COUNT < 0) {
		STALL_COUNT = 0;
	}
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
/* Print the program loaded into memory (in MIPS assembly format)    */ 
/************************************************************/
void print_instruction(uint32_t addr) {
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

/************************************************************/
/* Print the current pipeline                                                                                    */ 
/************************************************************/
void show_pipeline(){
	printf("\n\nCurrent PC: %x",CURRENT_STATE.PC);
	printf("\nIF_ID.IR: %x",IF_ID.IR);
	printf("\nIF_ID.PC: %x",IF_ID.PC);

	printf("\n\nID_EX.IR: %x",ID_EX.IR);
	printf("\nID_EX.A: %x",ID_EX.A);
	printf("\nID_EX.B: %x",ID_EX.B);
	printf("\nID_EX.D: %x",ID_EX.D);
	printf("\nID_EX.imm: %x",ID_EX.imm);

	printf("\n\nEX_MEM.IR: %x",EX_MEM.IR);
	printf("\nEX_MEM.A: %x",EX_MEM.A);
	printf("\nEX_MEM.B: %x",EX_MEM.B);
	printf("\nEX_MEM.D: %x",EX_MEM.D);
	printf("\nEX_MEM.ALUOutput: %x",EX_MEM.ALUOutput);

	printf("\n\nMEM_WB.IR: %x",MEM_WB.IR);
	printf("\nMEM_WB.ALUOutput: %x",MEM_WB.ALUOutput);
	printf("\nMEM_WB.LMD: %x",MEM_WB.LMD);
	printf("\n\n");
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
