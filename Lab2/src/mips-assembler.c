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
		//printf("From file read: %s",word);
		handle_instruction(word);
	}
	fclose(fp);
}



/**************************************************************/
/**/
/**************************************************************/
void handle_instruction(char* instr) {                  
	char* opcode = strtok(instr," ");
	char params[4][6];
	//params = malloc(4*5*sizeof(char));
	//printf("opcode: %s\n",opcode);
	int dim;

	opcode = to_lower(opcode);

	if(opcode == "add") {
		dim = 3;
		//strcpy(params,parse_params(dim));
		printf("%s %s %s",params[0],params[1],params[2]);
		//parse_registers(2)
		//parse_immediate(1)
		//create_i_type()

	} else if(opcode == "addi") {
		;
	} else if(!strncmp(opcode,"addiu",5)) {
		dim = 3;
		parse_params(params,dim);
		printf("\n%s %s %s %s",opcode,params[0],params[1],params[2]);
		parse_registers('i',params);
	}

}


void parse_registers(char t,char params[4][6]) {
	switch(t) {
		case 'i':
			i_type_data data_i;
			data_i.rt 		 = get_int(params[0]);
			data_i.rs 		 = get_int(params[1]);
			data_i.immediate = get_int(params[2]);
			break;
		case 'r':
			r_type_data data_r;
			data_r.rt 		 = get_int(params[0]);
			data_r.rs 		 = get_int(params[1]);
			data_r.rd
			data_r.shamt 	 = get_int(params[2]);
			break;
		case 'j':
			j_type_data data_j;
			data_j.target	 = get_int(params[0]);
			break;
	}
}


int get_int(char* str) {
	char temp[strlen(str)];
	int i=1;
	while(isdigit(str[i]) != 0) {
		temp[i-1] = str[i];
		i++;
	}
	return strtol(temp,NULL,0);
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
char* to_lower(char* str) {
	for(int i = 0; str[i]; i++) {
  		str[i] = tolower(str[i]);
	}
	return str;
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
