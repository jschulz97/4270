#include "mips-assembler.h"


/**************************************************************/
/* load program into memory*/
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
		printf("From file read: %s",word);
		handle_instruction(word);
	}
	fclose(fp);
}


/**************************************************************/
/**/
/**************************************************************/
void handle_instruction(char* instr) {                  
	char* opcode = strtok(instr," ");
	char** params;
	printf("opcode: %s\n",opcode);
	int dim;

	opcode = to_lower(opcode);

	if(opcode == "add") {
		dim = 3;
		params = parse_params(dim);
		//parse_registers(2)
		//parse_immediate(1)
		//create_i_type()

	} else if(opcode == "addi") {

	}




}


/**************************************************************/
/**/
/**************************************************************/
char** parse_params(int dim) {
	int i;
	char** params;
	for(i=0; i<dim; i++) {
		params[i] = strtok(NULL," ");
	}
	return params;
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



/***************************************************************/
/* main   */                                                                                                                                */
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
