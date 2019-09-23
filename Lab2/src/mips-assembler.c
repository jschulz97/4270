#include "mips-assembler.h"


/**************************************************************/
// Load program into memory
/**************************************************************/
void load_program(char* prog_file) {                   
	FILE * fp;
	char* word;
	ssize_t read;
	size_t len = 0;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */
	while((read = getline(&word,&len,fp)) != EOF ) {
		//printf("From file read: %s",word);
		handle_instruction(word);
	}
	printf("\n\n");
	fclose(fp);
}



/**************************************************************/
// Handle string instruction, convert to hex instr
/**************************************************************/
void handle_instruction(char* instr) {                  
	char* opcode = strtok(instr," ");
	char params[4][6];
	//params = malloc(4*5*sizeof(char));
	//printf("opcode: %s\n",opcode);
	int dim;
	uint32_t op;
	uint32_t code = 0; 

	opcode = to_lower(opcode);
	//printf("\n%s %s %s %s",opcode,params[0],params[1],params[2]);

	// I Type
	if(!strncmp(opcode,"addiu",5)) {
		dim = 3;
		op = 0x09;
		parse_params(params,dim);
		i_type_data data = parse_registers_i(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type
	else if(!strncmp(opcode,"addi",4)) {
		dim = 3;
		op = 0x08;
		parse_params(params,dim);
		i_type_data data = parse_registers_i(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"addu",4)) {
		dim = 3;
		op = 0x21;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"add",3)) {
		dim = 3;
		op = 0x20;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// I Type
	else if(!strncmp(opcode,"andi",4)) {
		dim = 3;
		op = 0x0c;
		parse_params(params,dim);
		i_type_data data = parse_registers_i(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"and",3)) {
		dim = 3;
		op = 0x24;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// I Type
	else if(!strncmp(opcode,"beq",3)) {
		dim = 3;
		op = 0x08;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_rs(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2 Special
	else if(!strncmp(opcode,"bgez",4)) {
		dim = 2;
		op = 0x08;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i_special(data,op);
		output_instr(code);

	}
	// I Type 2
	else if(!strncmp(opcode,"bgtz",4)) {
		dim = 2;
		op = 0x07;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"blez",4)) {
		dim = 2;
		op = 0x06;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2 Special
	else if(!strncmp(opcode,"bltz",4)) {
		dim = 2;
		op = 0x0;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i_special(data,op);
		output_instr(code);

	}
	// I Type
	else if(!strncmp(opcode,"bne",3)) {
		dim = 3;
		op = 0x05;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_rs(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type 2
	else if(!strncmp(opcode,"divu",4)) {
		dim = 2;
		op = 0x1b;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_2(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 2
	else if(!strncmp(opcode,"div",3)) {
		dim = 2;
		op = 0x1a;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_2(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 2
	else if(!strncmp(opcode,"jalr",4)) {
		dim = 2;
		op = 0x09;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_2(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// J Type
	else if(!strncmp(opcode,"jal",3)) {
		dim = 1;
		op = 0x03;
		parse_params(params,dim);
		j_type_data data = parse_registers_j(params);
		code = create_mach_code_j(data,op);
		output_instr(code);

	} 
	// R Type 1
	else if(!strncmp(opcode,"jr",2)) {
		dim = 1;
		op = 0x08;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_1(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// J Type
	else if(!strncmp(opcode,"j",1)) {
		dim = 1;
		op = 0x02;
		parse_params(params,dim);
		j_type_data data = parse_registers_j(params);
		code = create_mach_code_j(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"lb",2)) {
		dim = 2;
		op = 0x20;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"lh",2)) {
		dim = 2;
		op = 0x21;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"lui",3)) {
		dim = 2;
		op = 0x0f;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"lw",2)) {
		dim = 2;
		op = 0x23;
		parse_params(params,dim);
		i_type_data data = parse_registers_i_2(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type 1 rd
	else if(!strncmp(opcode,"mfhi",4)) {
		dim = 1;
		op = 0x10;
		parse_params(params,dim);
		r_type_data data;
		data.rs 	= 0;
		data.rt 	= 0;
		data.shamt 	= 0;
		data.rd		= get_int_dec(params[0],1);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 1 rd
	else if(!strncmp(opcode,"mflo",4)) {
		dim = 1;
		op = 0x12;
		parse_params(params,dim);
		r_type_data data;
		data.rs 	= 0;
		data.rt 	= 0;
		data.shamt 	= 0;
		data.rd		= get_int_dec(params[0],1);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 1
	else if(!strncmp(opcode,"mtlo",4)) {
		dim = 1;
		op = 0x13;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_1(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 1
	else if(!strncmp(opcode,"mthi",4)) {
		dim = 1;
		op = 0x11;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_1(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 2
	else if(!strncmp(opcode,"multu",5)) {
		dim = 2;
		op = 0x19;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_2(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type 2
	else if(!strncmp(opcode,"mult",4)) {
		dim = 2;
		op = 0x18;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_2(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"nor",3)) {
		dim = 3;
		op = 0x27;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// I Type
	else if(!strncmp(opcode,"ori",3)) {
		dim = 3;
		op = 0x0d;
		parse_params(params,dim);
		i_type_data data = parse_registers_i(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type
	else if(!strncmp(opcode,"or",2)) {
		dim = 3;
		op = 0x25;
		parse_params(params,dim);
		i_type_data data = parse_registers_i(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"sb",2)) {
		dim = 2;
		op = 0x28;
		parse_params_s(params,dim);
		i_type_data data;
		data.rt 		= get_int_dec(params[0],1);
		data.immediate	= get_int(params[1],2);
		data.rs		 	= get_int_dec(params[2],1);
		printf("%s %x %s %x %s %x",params[0],data.rt,params[1],data.immediate,params[2],data.rs);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"sh",2)) {
		dim = 2;
		op = 0x29;
		parse_params_s(params,dim);
		i_type_data data;
		data.rt 		= get_int_dec(params[0],1);
		data.immediate	= get_int(params[1],1);
		data.rs		 	= get_int_dec(params[2],1);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type w/ shamt
	else if(!strncmp(opcode,"sll",3)) {
		dim = 3;
		op = 0x00;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_shamt(params);
		code = create_mach_code_r_shamt(data,op);
		output_instr(code);

	} 
	// I Type
	else if(!strncmp(opcode,"slti",4)) {
		dim = 3;
		op = 0x0a;
		parse_params(params,dim);
		i_type_data data = parse_registers_i(params);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"slt",3)) {
		dim = 3;
		op = 0x2a;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type w/ shamt
	else if(!strncmp(opcode,"sra",3)) {
		dim = 3;
		op = 0x03;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_shamt(params);
		code = create_mach_code_r_shamt(data,op);
		output_instr(code);

	} 
	// R Type w/ shamt
	else if(!strncmp(opcode,"srl",3)) {
		dim = 3;
		op = 0x02;
		parse_params(params,dim);
		r_type_data data = parse_registers_r_shamt(params);
		code = create_mach_code_r_shamt(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"subu",4)) {
		dim = 3;
		op = 0x23;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// R Type
	else if(!strncmp(opcode,"sub",3)) {
		dim = 3;
		op = 0x22;
		parse_params(params,dim);
		r_type_data data = parse_registers_r(params);
		code = create_mach_code_r(data,op);
		output_instr(code);

	} 
	// I Type 2
	else if(!strncmp(opcode,"sw",2)) {
		dim = 2;
		op = 0x2b;
		parse_params_s(params,dim);
		i_type_data data;
		data.rt 		= get_int_dec(params[0],1);
		data.immediate	= get_int(params[1],1);
		data.rs		 	= get_int_dec(params[2],1);
		code = create_mach_code_i(data,op);
		output_instr(code);

	} 
	// R Type - SYSCALL
	else if(!strncmp(opcode,"SYSCALL",6)) {
		dim = 3;
		op = 0xc;
		output_instr(op);

	} 


	printf("\ncode: %x",code);

}


/********************************************/
// Creates machine code for i type instruction
/********************************************/
uint32_t create_mach_code_i(i_type_data data, uint32_t op) {
	uint32_t instr = 0;
	instr = instr + op 		* 0x4000000;
	instr = instr + data.rs * 0x200000;
	printf("\n%x %x", (data.rs * 0x200000),instr);
	instr = instr + data.rt * 0x10000;
	instr = instr + data.immediate; 
	return instr;
}


/********************************************/
// Creates machine code for i special type instruction
/********************************************/
uint32_t create_mach_code_i_special(i_type_data data, uint32_t op) {
	uint32_t instr = 0;
	instr = instr + op 		 * 0x10000;
	instr = instr + data.rs  * 0x200000;
	instr = instr + 0x000001 * 0x4000000;
	instr = instr + data.immediate; 
	return instr;
}

/********************************************/
// Creates machine code for r type instruction
/********************************************/
uint32_t create_mach_code_r(r_type_data data, uint32_t op) {
	uint32_t instr = 0;
	instr = instr + op;
	instr = instr + data.rd * 0x800;
	instr = instr + data.rt * 0x10000;
	instr = instr + data.rs * 0x200000;
	return instr;
}

/********************************************/
// Creates machine code for r w/ shamt type instruction
/********************************************/
uint32_t create_mach_code_r_shamt(r_type_data data, uint32_t op) {
	uint32_t instr = 0;
	instr = instr + op;
	instr = instr + data.rt 	* 0x800;
	instr = instr + data.rd 	* 0x10000;
	instr = instr + data.shamt  * 0x40;
	return instr;
}

/********************************************/
// Creates machine code for j type instruction
/********************************************/
uint32_t create_mach_code_j(j_type_data data, uint32_t op) {
	uint32_t instr = 0;
	instr = instr + op * 0x4000000;
	instr = instr + data.target;
	return instr;
}


/********************************************/
// Parses registers for i type instruction
/********************************************/
i_type_data parse_registers_i(char params[4][6]) {
	i_type_data data;

	int neg = 1;
	int neg_off = 2;
	if(params[2][0] == '-') {
		neg = -1;
		neg_off = 3;
	}

	data.rt 		= get_int_dec(params[0],1);
	data.rs 		= get_int_dec(params[1],1);
	data.immediate 	= (neg * get_int(params[2],neg_off)) & 0xFFFF;
	return data;
}


/********************************************/
// Parses registers for i type instruction w/ rs first
/********************************************/
i_type_data parse_registers_i_rs(char params[4][6]) {
	i_type_data data;

	int neg = 1;
	int neg_off = 2;
	if(params[2][0] == '-') {
		neg = -1;
		neg_off = 3;
	}

	data.rt 		= get_int_dec(params[1],1);
	data.rs 		= get_int_dec(params[0],1);
	data.immediate 	= (neg * get_int(params[2],neg_off)) & 0xFFFF;
	return data;
}


/********************************************/
// Parses registers for i type w/ 2 instruction
/********************************************/
i_type_data parse_registers_i_2(char params[4][6]) {
	i_type_data data;
	data.rs 		= get_int_dec(params[0],1);
	data.immediate 	= get_int(params[1],2);
	data.rt 		= 0;
	return data;
}

/********************************************/
// Parses registers for r type instruction
/********************************************/
r_type_data parse_registers_r(char params[4][6]) {
	r_type_data data;
	data.rt 	= get_int_dec(params[2],1);
	data.rs 	= get_int_dec(params[1],1);
	data.rd		= get_int_dec(params[0],1);

	return data;
}


/********************************************/
// Parses registers for r type instruction
/********************************************/
r_type_data parse_registers_r_shamt(char params[4][6]) {
	r_type_data data;
	data.shamt 	= get_int(params[2],2);
	data.rt 	= get_int_dec(params[1],1);
	data.rd		= get_int_dec(params[0],1);

	return data;
}


/********************************************/
// Parses registers for r w/ 2 
/********************************************/
r_type_data parse_registers_r_2(char params[4][6]) {
	r_type_data data;
	data.rs 	= get_int_dec(params[0],1);
	data.rt 	= get_int_dec(params[1],1);
	data.shamt 	= 0;
	data.rd		= 0;

	return data;
}


/********************************************/
// Parses registers for r w/ 1 param 
/********************************************/
r_type_data parse_registers_r_1(char params[4][6]) {
	r_type_data data;
	data.rs 	= get_int_dec(params[0],1);
	data.rt 	= 0;
	data.shamt 	= 0;
	data.rd		= 0;

	return data;
}


/********************************************/
// Parses registers for j type instruction
/********************************************/
j_type_data parse_registers_j(char params[4][6]) {
	j_type_data data;
	data.target	 = get_int(params[0],2);
	
	return data;
}


/********************************************/
// 
/********************************************/
int get_int(char* str,int offset) {
	char temp[strlen(str)];
	strncpy(temp, " ", sizeof(temp));

	int i = 0;
	while(isxdigit(str[offset]) != 0) {
		temp[i] = str[offset];
		offset++;
		i++;
	}
	return strtol(temp,NULL,16);
}

/********************************************/
// 
/********************************************/
int get_int_dec(char* str,int offset) {
	char temp[strlen(str)];
	strncpy(temp, " ", sizeof(temp));

	int i = 0;
	while(isxdigit(str[offset]) != 0) {
		temp[i] = str[offset];
		offset++;
		i++;
	}
	return strtol(temp,NULL,10);
}


/**************************************************************/
/**/
/**************************************************************/
void parse_params(char params[4][6],int dim) {
	int i;
	//char params[4][6];
	for(i=0; i<dim; i++) {
		strcpy(params[i],strtok(NULL," "));
	}
}


/**************************************************************/
/**/
/**************************************************************/
void parse_params_s(char params[4][6],int dim) {
	int i;
	//char params[4][6];
	for(i=0; i<dim-1; i++) {
		strcpy(params[i],strtok(NULL," "));
	}
	strcpy(params[i],strtok(NULL,"("));
	i++;
	strcpy(params[i],strtok(NULL,")"));
}


/**************************************************************/
/**/
/**************************************************************/
char* to_lower(char* str) {
	for(int i = 0; str[i]; i++) {
  		str[i] = tolower(str[i]);
	}
	return str;
}


/********************************************/
// 
/********************************************/
void output_instr(uint32_t instr) {
	FILE * fp;
	char* fn = "output.txt";

	/* Open program file. */
	fp = fopen(fn, "a");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", fn);
		exit(-1);
	}

	/* write to the program. */
	char temp[9];
	sprintf(temp, "%x\n", instr);
	fputs(temp,fp);
	fclose(fp);
}


/***************************************************************/
/*main*/
/***************************************************************/
int main(int argc, char *argv[]) {                              
	printf("\n**************************\n");
	printf("MIPS-Assembler\n");
	printf("**************************\n\n");
	
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}

	char prog_file[32];
	strcpy(prog_file, argv[1]);
	load_program(prog_file);
	
	return 0;
}
