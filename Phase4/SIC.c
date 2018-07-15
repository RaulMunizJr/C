
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "symbolAVL.h"
#include "sicengine.c"

// Structure for mnemonics.
struct opcode {
  char mnemo [6];
  char opcode [3];
} ;

// Structure for objectValues.
struct objectFV {
	char line [100];
	int address;
	int opValue;
	char label [10];
	char errors [100];
};

// Structure for objectFile
struct objectLine {
	int address;
	int length;
	int error;
	int newLine;
	char objectCode[50];
	char line[90];
};

struct opcode opcoTable[25]; // Global array for opcode table.

char errorMesages[25][100]={"Missing START directive.",
			"Illegal or missing operand on Start directive.",
			"Cannot access memory at that address.",
			"Duplicated label.","More than 500 labels.",
			"Illegal type on operand for BYTE directive.",
			"No even number of digits on operand.",
			"Invalid operand on BYTE directive.",
			"Invalid operand on BYTE directive.",
			"Exceed maxlengthof Char bytes.",
			"Illegal or missing operand on instruction(mnemonics).",
			"No hexadecimal address of operand on instruction(mnemonics).",
			"Invalid form of operand on instruction(mnemonics).",
			"Missing operand on END directive.",
			"Illegal operand on END directive.","Program too long.",
			"Invalid instruction.","Illegal label.",
			"Illegal or missing operand on data storage directive.",
			"No hexadecimal value for BYTE directive on operand type X.",
			"Duplicate or misplaced START statement.",
			"Missing or misplaced operand in BYTE statement.",
			"Undefined symbol in operand.","Unrecognized operation code."};

void SIC();void welcome();
int command(char[], char **);
char ** breakCommand(char[],int, char **);
void help(char[]);
void load(char[],char[]);
void executes(char[]);
void debug(char[]);
void dump(char[],char[],char[]);
void assemble(char[],char[]);
void directory(char[]);
void noComm();
int findWord(char[],char[],int,int);
void getOpcodeTable(struct opcode[]);
int findOpcode(char[]);
void pass1(char[]);
void tokenizingLine(char[], char **);
int isComment(char[]);
void handleLinePass1(char **,FILE *,int*,struct symbol,int*);
int lengthConstants(char[], int*);
void writeErrors(int, FILE *);
void errorOperStart(int *,char []);
void errorOperInstruc(int*,char[]);
void errorOperDirective(int*,char[]);
void errorEnd(int*, int,char[]);
int errorLabel(int *, char[]);
int isHex(char[]);
void defaultArgs(char**);
int strIsalnum(char[]);
void pass2(int);
void handleStartObjectFile(FILE *,FILE *, FILE*, struct objectFV,int,int*);
void generateObjCode(struct objectFV,char[],int*);
void getCharHex(char[],char[]);
void objectFile(FILE*,struct objectLine*, struct objectFV,int*);
void listingFile(FILE*, struct objectFV,int*);
void getErrorsMessages(FILE* fl,char [],int*);
void handleEndObjectFile(FILE *, FILE*,struct objectLine *, struct objectFV,int*);
void loadFile(char[]);
void dumpMem(int,int);

int main()
{
	/* Start the computer simulation. */
	SIC();
}

/* Ask for a command while the user does not type "exit"*/
void SIC()
{
	getOpcodeTable(opcoTable); // Get the opcode table from the InstructionSet file.

	int flag=0;	// Flag to check if the user wants to end the SIC
	char line[100];	// This array will holds the line that the user input.
	char ** args;	// Double pointer to store the command and parameters.

	args = (char **) malloc(sizeof(char*) * 4);	// Set the number of parameters. 
	//It is going to be set it at four because a user will only type at most one command, and two parameters, 
	//and we are going to give an extraparameter to check for execess of parameters.  
	int i;
	for (i=0; i<4; i++)	// Set the size of 20 to each parameter.
	{
		args[i] = (char *) malloc(sizeof(char) * 20);
	}

	welcome();	// Displays a welcome message. 
	while(flag!=1)	// Loops until the user type exit.
	{
		flag=command(line, args);	//Ask for a command, gets if the user asked to exit.
	}	

	free(args);	//Deallocates the memory from args.

	return;
}

/* Prompt a welcome message */
void welcome()
{
	printf("\nHello, This is a computer simulation\n\n");
}

/* Get the command, and performes a function,
if the command is exit return 1 otherwise 0*/
int command(char line[], char ** args)
{
	int i;
	for (i=0; i<4; i++)
	{
		strncpy(args[i],"",strlen(args[i])); //Set an empty string to each argument.
	}

	printf("Command> "); // Ask for a command.

	fgets(line,99,stdin);	// Gets the line that the user typed as a command.
	int lineSize=strlen(line)-1;	// Gets the size of the line.

	char ** comm=breakCommand(line,lineSize,args);	// Gets each argument from the line.

	int len=strlen(comm[0]);	// Gets the size of the command.

	if(len==0)	/* If the command is empty, asks again for a command */
	{
		return 0;
	}
	/* Since SIC can recognize abbreviated commands, 
	I set it that a command should be at least 3 or more characters. */
	else if(len < 3)
	{
		printf("Cannot recognize any command. "
			"Need at least three characters.\n\n");
	}
	/* Performe the given command if the programm recognizes it.
	If not it will prompt and error message. 
	Also it command checks for the prescence and abscence. 
	Compares up to the length of the given command with the list of valids commands in SIC. 
	So it will recognize the first few characters of the command.*/
	else
	{
		if(strncmp(comm[0],"help",len)==0)
			{help(comm[1]);}
		else if(strncmp(comm[0],"directory",len)==0)
			{directory(comm[1]);}
		else if (strncmp(comm[0],"load",len)==0)
			{load(comm[1],comm[2]);}
		else if(strncmp(comm[0],"dump",len)==0)
			{dump(comm[1],comm[2],comm[3]);}
		else if (strncmp(comm[0],"assemble",len)==0)
			{assemble(comm[1],comm[2]);}
		else if (strncmp(comm[0],"executes",len)==0)
			{executes(comm[1]);}
		else if (strncmp(comm[0],"debug",len)==0)
			{debug(comm[1]);}
		else if(strncmp(comm[0],"exit",len)==0)
			{return 1;}
		else
			{noComm();}
	}    

	return 0;
}

/* Prints out a list of availables commands. */
void help(char par[])
{
	if(strlen(par)>0) // Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	printf("\n");
	printf("Commands available: \n"
		"load filename\n"
		"executes\n"
		"debug\n"
		"dump start end\n"
		"help\n"
		"assemble filename\n"
		"directory\n"
		"exit\n\n");
	return;
}

/* Loads an specidied file */
void load(char par1[],char par2[])
{
	if(strlen(par1)==0)	// Check for abscense of paramters.
	{	
		printf("\nThis command needs a parameter.\n\n");
		return;
	}
	else if (strlen(par2)>0) // Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	loadFile(par1);
	return;
}

/*  */
void dump(char par1[],char par2[],char par3[])
{
	if(strcmp(par1,"")==0 || strcmp(par2,"")==0 )	// Check for abscense of paramters.
	{	
		printf("\nThis command takes at least 2 parameters.\n\n");
		return;
	}
	else if (strlen(par3)>0)	// Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	int start;
	int end;
	sscanf(par1,"%X",&start);
	sscanf(par2,"%X",&end);
	dumpMem(start,end);
	return;
}

/* Assemble and SIC assembly language program into a load module, 
and store it in a file. */
void assemble(char par1[], char par2[])
{
	if(strcmp(par1,"")==0)	// Check for abscense of paramters.
	{	
		printf("\nThis command needs a parameter.\n\n");
		return;
	}
	else if (strlen(par2)>0)	// Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	pass1(par1);
	return;
}

/* Allows to execute in debug mode. */
void debug(char par[])
{
	if (strlen(par)>0)	// Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	printf("debug\n");
	return;
}

/* Calls the computer simulation prgram to execute the program 
that was previously loaded in memory. */
void executes(char par[])
{
	if (strlen(par)>0)	// Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	printf("executes\n");
	return;
}

/* Lists the files stored in the current diretory. */
void directory(char par[])
{
	if (strlen(par)>0)	// Get if the user input extraparameters.
	{
		printf("Too many parameters\n\n");
		return;
	}
	system("ls");
	printf("\n");
	return;
}

/* Displays a message if the typed command is not recognized,
and suggest to type help command. */
void noComm()
{
	printf("\nUnrecognized command. Type 'help' to see the commands available\n\n");
	return;
}

/* Receive the line, arrays for the command and parametersm and the size of the line */
char ** breakCommand(char line[], int lineSize, char ** comm)
{
	int start=0;	// Set the start to 0 for the index for line.
	int i;
	for (i=0; i<4; i++)
	{
		start=findWord(line,comm[i],start,lineSize); // Finds a word and set it to the array of arguments. Gets the index for the next word.
	}

	return comm; // Return the array of arguments.
}

/* Gets a string between spaces or tabs.
Return the starting index for the next string, if there is any. */
int findWord(char line[],char word[],int start,int end)
{
	int flag=0;	// Flag that checks if the function found a string. 0 no string found. 1 string found.
	int i=start;	// Index for line array.
	int j=0;	// Index for word array.
	
	while(i<=end)
	{
		if (( (line[i]==' ' || line[i]=='\t') && flag==1)||i==end)	 // Checks if line[i] is a spaces or a tab, and if it found a string.
			{return i;}	// Returns the index of the end of the found string, if it is not the end of the line.
		else if(line[i]!=' ' && line[i]!='\t') // Check if the line[i] is not a space or a tab.
		{
			word[j]=line[i];	// Set what is in line at index i, and put it in word array.
			flag=1;	// Set it to 1 because it found an string.
			i++;	// Increment i for line.
			j++;	// Increment j for word.
		}
		else if(line[i]==' ' || line[i]=='\t')	// Checks if line[i] is a space or a tab.
			{i++;}	// Ignores spaces or tabs.
	}
}

/******************************************** PASS 1 FUNCTIONS ***********************************************************/
/*************************************************************************************************************************/

/* Store mnemonis and values from InstructionSet file into opcoTable[] */
void getOpcodeTable (struct opcode opcoTable[])
{
	// Array for mnemonics, it is going to be used later in pass 1.
	char line[21];
	char * token; // Pointer for strtok.
	int i=0; // Index for the mnemonics array.
	int j;	// Index for the strtok.

	// Open file InstructionSet only for read.
	FILE *fp;
	if (( fp = fopen("InstructionSet", "r")) == NULL) {
		printf("cannot open file\n");
		return;
	}

	// Read each line in the file.	
	while(!feof(fp))
	{
		j=0;
		fgets(line,20,fp); // Reads the line of the file.
		token = strtok (line," \n");
		// Only looks for two tokens
		while(j<2)
		{
			if(j==0) // First token is the mnemonic
			{
				strcpy(opcoTable[i].mnemo,token);
			}
			else if(j==1) // Second token is the opcode value.
			{
				strcpy(opcoTable[i].opcode,token);
			}
			token = strtok (NULL," \n"); // Points to the next token.
			j++;
		}
		i++;
	}
	// Close file.
	fclose(fp);
}

/* Function that return the index of the found mnemonic */
int findOpcode(char mnemo[])
{
	// Binary search of Mnenomics.
	int binarySearchOptable(char target[],int low, int high)
	{
		if(low>high)
			return -1;
		int mid= (low+high)/2;
		if(strcmp(target, opcoTable[mid].mnemo)==0)
			return mid;
		else if(strcmp(target,opcoTable[mid].mnemo)<0)
			return binarySearchOptable(target, low, mid-1);
		else
			return binarySearchOptable(target, mid+1, high);
	}

	return binarySearchOptable(mnemo,0,(25-1)); 
}

/* Pass1 of an assembler. */
void pass1(char file[])
{
	int flagEnd=0; // Flag if instruction is END
	int locctr=0;	// Variable that keeps track of the current address.
	int startLocctr; // Store the initial value of locctr.
	int errors=0; // Bitter for errors. O no error. 1 error.
	int programLenght; // Store the lenght of the program.
	char line[100];	// Store the line of the assembled file.

	symbolAVL(); // Initialize the  root of the AVLtree of symbols.
	struct symbol symb; // Variable to store a symbol.

	char ** args;	// Double pointer to store the instruction format.
	args = (char **) malloc(sizeof(char*) * 3);	// Set the number of parameters.
	args[0] = (char *) malloc(sizeof(char) * 7); // Set size to store the label.
	args[1] = (char *) malloc(sizeof(char) * 7); // Set size to store the instruction
	args[2] = (char *) malloc(sizeof(char) * 50); // Set size to store the operand.
	defaultArgs(args); // Set all args to a space.

	FILE *fr; // File read.
	// Check if there is a problem opening the file.
	if (( fr = fopen(file, "r")) == NULL) {
		printf("cannot assemble file\n");
		return;
	}

	FILE *fw; //file write.
	// Check if there is a problem creating the file.
	if (( fw = fopen("IntermediateFile", "w+")) == NULL) {
		printf("cannot open or create the file\n");
		return;
	}

	FILE *fAVL; //file write.
	// Check if there is a problem creating the file.
	if (( fAVL = fopen("ElementsOnAVL", "w+")) == NULL) {
		printf("cannot open or create the file\n");
		return;
	}

	/***************** Checks for START directive *********************/
	fgets(line,99,fr); // Get first line of file.
	if(isspace(line[strlen(line)-2])) // Get rid of the closest '\n' char to the last word. 
			line[strlen(line)-2]=' ';
	while(isComment(line))
	{
		fgets(line,99,fr); // Get first line of file.
		if(isspace(line[strlen(line)-2])) // Get rid of the closest '\n' char to the last word. 
			line[strlen(line)-2]=' ';
	}

	fprintf(fw, "%s", line); // Writes line on IntermediateFile.
	tokenizingLine(line,args); // Get all label, instruction, and operand from line.
	if(strcmp("START",args[1])==0)	// Checks if the instruction of the first line is equal to START.
	{
		sscanf(args[2],"%X",&locctr); // Set location counter from the value of the operand.
		startLocctr=locctr;

		fprintf(fw, "%s\n", args[0]);
		fprintf(fw, "F0\n"); // Write the opcode value for START directive.	
		fprintf(fw, "%X\n", locctr); // Write on the IntermediateFile locctr.

		// Copy label and address to Symbol variable.
		strcpy(symb.label,args[0]);
		symb.address=locctr;
		insertSymbol(symb); // Insert symbol in symbolAVL.

		errorOperStart(&errors,args[2]); // Check for errors on the operand for START.
		writeErrors(errors, fw); // Write all errors flaged on START directive.
	}
	else if(strcmp(args[1]," ")==0) // If there is no START.
	{
		errors ^= (-1 ^ errors) & (1 << 0); 
		locctr=0; // Set location counter to 0.
		fprintf(fw, "%X\n", locctr);
	}
	else
	{
		if(strncmp(args[1],"END",3)==0) // If the instruction is END.
		{
			errorEnd(&errors,locctr,args[2]); // Check for errors on the operad for END.
			flagEnd=1; // Set flag 1. Found END directive.
		} 
		handleLinePass1(args,fw,&locctr,symb,&errors); // Handle line from file.
		writeErrors(errors,fw); // Write all errors flagged for each line.
	}
	/***********************************************************************/

	// Reads the file;
	while(!flagEnd && !feof(fr))
	{
		errors=0;
		fgets(line,99,fr); // Reads the line of the file.
		if(isspace(line[strlen(line)-2])) // Get rid of the closest '\n' char to the last word. 
			line[strlen(line)-2]='\0';
		else if(isspace(line[strlen(line)-1]))
			line[strlen(line)-1]='\0';

		if(!isComment(line)) // If it is a comment it will ignore it.
		{
			fprintf(fw, "%s\n", line); // Writes line on IntermediateFile.
			tokenizingLine(line,args); // Get all label, instruction, and operand from line.
			if(strncmp(args[1],"END",3)==0 ) // If the instruction is END.
			{
				errorEnd(&errors,locctr,args[2]); // Check for errors on the operad for END.
				flagEnd=1; // Set flag 1. Found END directive.
			} 
			handleLinePass1(args,fw,&locctr,symb,&errors); // Handle line from file.
			writeErrors(errors,fw); // Write all errors flagged for each line.
		}
	}
	
	programLenght=locctr-startLocctr; // Get the lenght of the program.
	writeAVLonFile(fAVL); // Print all the elements in the AVLtree.
	fclose(fAVL);
	fclose(fr); // Close file.
	fclose(fw); // Close file.
	free(args);	//Deallocates the memory from args.
	pass2(programLenght);
	return;
}

/*  Checks if the lines begins with a period. 
	Meaning that it is a comment. */
int isComment(char line[])
{
	return (line[0]=='.');
}

/* Gets all the tokens from line[] */
void tokenizingLine(char line[], char ** args)
{
	int i=0;

	char * token; // Pointer for strtok.
	token = strtok (line," \t\n");

	// If there is a space, 
	// it means that the instruction does not have a label.
	if(isspace(line[0])) 
	{
		strcpy(args[0]," "); // No label
		i++; // Increment to the next token.
	}

	while(token != NULL) // Finds all tokens in line[]
	{
		if(i==0) // First token is a label.
			strcpy(args[i],token);
		else if(i==1) // Second token is an instruction.
			strcpy(args[i],token);
		else if(i==2) // Third token is an operand.
			strcpy(args[i],token);
		
		token = strtok (NULL," \t\n"); // Points to the next token.
		i++;
	}
	for (i;i<3;i++) // Set to rest of the args to a space
		strcpy(args[i]," ");
	return;
}

/* Handles the line for pass 1 */
void handleLinePass1(char ** args, FILE * fw, int * locctr, struct symbol symb, int * errors)
{
	int plusHEX; // Varirable that will hold some hexadecimal value that will be added later to the location counter.
	char args1[3]; // Variable that will hold the value of the instruction. 
	if(!isspace(args[0][0])) // Checks if the instruction has a label.
	{
		// If there is a label strore the label and address into Symbol variable,
		// and store it in symbolAVL.
		if(!errorLabel(errors,args[0])) 
		{
			strcpy(symb.label,args[0]);
			symb.address=*locctr;
			insertSymbol(symb);
		}
	}

	fprintf(fw, "%X\n", *locctr); // Write the location counter to the IntermediateFile.

	int opIndex = findOpcode(args[1]); // Check if the instruction is a mnemonic.
	if(opIndex!=-1)
	{
		strcpy(args1,opcoTable[opIndex].opcode); // Get the value of the mnemonic from the opcode table.
		*locctr += 0X03; // Increment location counter by three. Size of instruction.
		errorOperInstruc(errors,args[2]);
	}
	else if(strcmp(args[1],"WORD")==0) // If instruction is WORD directive.
	{
		strcpy(args1,"F1"); // Set a value for the instruction.
		*locctr += 0X03; // Increment location counter by three. Size of word.
		errorOperDirective(errors,args[2]);
	}
	else if(strcmp(args[1],"RESB")==0) // If instruction is RESB directive.
	{
		strcpy(args1,"F2"); // Set a value for the instruction.
		plusHEX=atoi(args[2]); // Get the size from the operand and store it in plusHEX.
		*locctr += plusHEX;	// Increment location counter by the value of plusHEX.
		errorOperDirective(errors,args[2]); 
	}
	else if(strcmp(args[1],"RESW")==0) // If instruction is RESW directive.
	{
		strcpy(args1,"F3"); // Set a value for the instruction.
		plusHEX=atoi(args[2]); // Get the size from the operand and store it in plusHEX.
		plusHEX *= 0X03; // Multiply the given size by three.
		*locctr += plusHEX; // Increment location counter by the value of plusHEX.
		errorOperDirective(errors,args[2]);
	}
	else if(strcmp(args[1],"BYTE")==0) // If instruction is BYTE directive.
	{
		strcpy(args1,"F4"); // Set a value for the instruction.
		plusHEX=lengthConstants(args[2],errors); // Get the size from the operand and store it in plusHEX.
		*locctr += plusHEX; // Increment location counter by the value of plusHEX.
	}
	else if(strcmp(args[1],"END")==0) // If instruction is END directive.
	{
		strcpy(args1,"F5"); // Set a value for the instruction.
	}
	else // If it is an invalid instruction.
	{
		strcpy(args1,"F6"); // Set a value for the instruction.
		*errors ^= (-1 ^ *errors) & (1 << 16); // Error-Invalid instruction.
	}

	fprintf(fw, "%s\n", args1); // Write the value of the instruction.
	fprintf(fw, "%s\n", args[2]); // Write the operand to the IntermediateFile.
}

/* Finds out the number of bytes for BYTE directive */
int lengthConstants(char operand[], int * errors)
{
	char * pch;
	if(strcmp(operand,"")==0)
	{
		printf("nada\n");
		*errors ^= (-1 ^ *errors) & (1 << 21);
	}
	if(operand[0]=='X'||operand[0]=='C') // Check if the operand is a valid type. Char or Hex.
	{
		int len= strlen(operand); // Get the lenght of the opernad.
		int i=1; // Count of single quote. Assume there is at least one single quote. 
		int pos=-1; // Variable that will hold the position of the last single quote found.
		int numBytes; // Variable that will hold the number of bytes 
		pch=strchr(operand,'\'');
		while (pch!=NULL) // Check for single quotes
	 	{
	 		if(i==2)
		   		pos=pch-operand+1; // returns the position of the last single quote on the operand. 
		    pch=strchr(pch+1,'\'');
		    i++; // Increment the count of single quotes.
		}

		if(len==pos) // checks if the last char is a single quote.
		{
			if(operand[0]=='C') // If it is type Char.
			{
				numBytes = pos-3; // Store the number of bytes that would be the size of the operand minus the constant and the two single quotes.
				if(numBytes>30) 
					*errors ^= (-1 ^ *errors) & (1 << 8); // Error-Exceed maxlenght of Char bytes.
				else
					return numBytes; // Retutn the number of bytes.
			}
			else // If it is type Hex.
			{
				int c=2; 
				while(c<strlen(operand)-1) // Loops trough all the digits between single quotes on the operand.
				{
					if(!isxdigit(operand[c]))
					{
						*errors ^= (-1 ^ *errors) & (1 << 19); // Error-No Hexadecimal value on operant type X.
						break;
					}
					c++;
				}

				numBytes = pos-3; // Store the number of bytes that would be the size of the operand minus the constant and the two single quotes.
				if(numBytes%2==0) // Check if the number of digits is even.
				{
					numBytes /= numBytes; // Divide the number of digits by 2, because 2 digits make a byte.
					if(numBytes>16)
						*errors ^= (-1 ^ *errors) & (1 << 9); // Error-Exceed maxlenght of Hex bytes.
					else
						return numBytes; // Return the number of bytes.
				}
				else
					*errors ^= (-1 ^ *errors) & (1 << 6); // Error-No even number of digits on operand.
			}
		}
		else
			*errors ^= (-1 ^ *errors) & (1 << 7); // Error-Invalid operand on BYTE directive.
	}
	else
		*errors ^= (-1 ^ *errors) & (1 << 5); // Error-Illegal type on operand for BYTE directive.
	
	return 0;
}

/* Print all the errors flaged for each line of assemble file */
void writeErrors(int errors, FILE * fw)
{
	int i=0;
	fprintf(fw,"<");
	while (errors) // Loops every bit in errors.
	{
	    if (errors & 1)
	    	fprintf(fw, "/%d", (i+10));
	    errors >>= 1;
	    i++;
	}
	fprintf(fw,"/>\n");
	return;
}

/* Check and sets errors for operand on Start directive */
void errorOperStart(int *errors,char operand[])
{
	int address;
	sscanf(operand,"%X",&address);
	if(!isHex(operand)||strcmp(operand," ")==0) // check for character that not belong in hexa
		*errors ^= (-1 ^ *errors) & (1 << 1); // Error-Illegal or missing operand on Start.
	else if(address>0X7FFF)
		*errors ^= (-1 ^ *errors) & (1 << 2); // Error-Cannot acces memory at that address.
}

/* Checks and sets errors for operand on instructions. */
void errorOperInstruc(int* errors,char arg[])
{
	char operand[7];
	int i=0;
	
	if(arg[0]!='0' && !isalpha(arg[0]))
		*errors ^= (-1 ^ *errors) & (1 << 10); // Error-Illegal operand on instruction(mnemonic).
	else if(arg[0]=='0')
	{
		if(!isHex(arg))
			*errors ^= (-1 ^ *errors) & (1 << 11); // Error-No hexadecimal address on operand.
	}
	while(i<strlen(arg))
	{
		if(arg[i]!=',' && !isalnum(arg[i]))
		{	
			*errors ^= (-1 ^ *errors) & (1 << 12); // Error-Invalid form on operand.
			break;
		}
		i++;
	}
}

/* Checks and sets errors for labels. */
int errorLabel(int * errors, char label[])
{
	int error=0;
	
	if(searchSymbol(label))
	{
		*errors ^= (-1 ^ *errors) & (1 << 3); // Error-Duplicated label.
		error=1;
	}
	else if(countAVL()>=500)
	{
		*errors ^= (-1 ^ *errors) & (1 << 4); // Error-More than 500 labels.
		error=1;
	}
	else if(!isalpha(label[0]) || !strIsalnum(label)) // The first letter of the label is not a letter or string is not alphanumeric.
	{
		*errors ^= (-1 ^ *errors) & (1 << 17); // Error-Illegal label.
		error=1;
	}
	return error;
}

/* Returns true, if all characters of a string 
	belong to hexadecimal digit. */
int isHex(char arg[])
{
	int i=0;
	int flag=1;
	while (i<strlen(arg))
  	{
  		if(!isxdigit(arg[i])) // All characters must be alphanumeric
  			flag=0;
  		i++;
	}
	return flag;
}

/* Set all the args to space. */
void defaultArgs(char** args)
{
	int i=0;
	for(i;i<3;i++)
		strcpy(args[i]," ");
}

/* Checks and sets errors for the operand on directives. */
void errorOperDirective(int* errors,char arg[])
{
	int i=0;
	while (i<strlen(arg))
  	{
  		if(!isdigit(arg[i])) // should be a digit
  		{
  			*errors ^= (-1 ^ *errors) & (1 << 18); // Error-Missing or illegal operand on data storage directive.
  			break;
  		}
  		i++;
	}
}

/* Check and sets errors for end directive */
void errorEnd(int * errors, int locctr, char operand[])
{
	if(strcmp(operand," ")==0)
		*errors ^= (-1 ^ *errors) & (1 << 13); // Error-Missing operand on end directive.
	else if(!isalpha(operand[0]) || !strIsalnum(operand)) // the first letter of the label is not a letter
		*errors ^= (-1 ^ *errors) & (1 << 14); // Error-Illegal operan on end directive.
	if (locctr>0X7FFF)
		*errors ^= (-1 ^ *errors) & (1 << 15); // Error-Program too long.
}

/* Return true if all the characters of a string are alphanumeric. */
int strIsalnum(char str[])
{
	int i=0;
	int flag=1;
	while(i<strlen(str))
	{
		if(!isalnum(str[i])) // check if currenct char is alphanumeric. 
		{
			flag=0;
			break;
		}
		i++;
	}
	return flag;
}
/****************************************************************************************************************/
/****************************************************************************************************************/

/********************************************* PASS 2 FUNCTIONS **************************************************/
/****************************************************************************************************************/
/* Pass2 of an assembler. */
void pass2 (int PL)
{
	struct objectFV infoLine; // Varible that will contain all the information of the line.
	struct objectLine objLine; // Variable with information for the ObjectFile.
	objLine.newLine=1;	// Set the newLine variable to true, since the pass 2 just started.
	objLine.error=0;	// By defult it assumes that the programs has no errors.
	int i=0;	
	int errors2=0;	// Error flagger for the errors generated in pass 2.
	char line[100];

	FILE *fr; // File read.
	if (( fr = fopen("IntermediateFile", "r")) == NULL) {
		printf("cannot assemble file\n");
		return;
	}
	FILE *fo; //file write.
	// Check if there is a problem creating the file.
	if (( fo = fopen("ObjectFile", "w+")) == NULL) {
		printf("cannot open or create the file\n");
		return;
	}
	FILE *fl; //file write.
	// Check if there is a problem creating the file.
	if (( fl = fopen("ListingFile", "w+")) == NULL) {
		printf("cannot open or create the file\n");
		return;
	}

	handleStartObjectFile(fr,fo,fl,infoLine,PL,&errors2); // Handles the first line of the IntermediateFile.

	// Reads all the lines of the IntermidiateFile.
	while(!feof(fr))
	{
		fgets(line,99,fr);
		line[strlen(line)-1]='\0'; 
		if(i%5==0)
			strcpy(infoLine.line,line);	// Reads the line of the assembled file.
		else if(i%5==1)
			sscanf(line,"%X",&infoLine.address); // Reads the address of the statement.
		else if(i%5==2)
			sscanf(line,"%X",&infoLine.opValue); // Reads the opcode value.
		else if(i%5==3)
			strcpy(infoLine.label,line); // Reads the operand.
		else if(i%5==4)
		{
			strcpy(infoLine.errors,line); // Reads the errors of the line.
			if(infoLine.opValue == 0XF5) 
			{
				handleEndObjectFile(fo,fl,&objLine,infoLine,&errors2); // Handles the end line of the IntermediateFile.
				break;
			}
			objectFile(fo,&objLine,infoLine,&errors2);
			listingFile(fl,infoLine,&errors2);
		}
		i++;
	}
	if(objLine.error) // If there are any errors in the IntermediateFile.
		remove("ObjectFile"); // Remove it.

	writeAVLonFile(fl);
	// Close all the files.
	fclose(fr);
	fclose(fo);
	fclose(fl);
} 

/* Writes all the Text line in the objectFile */
void objectFile(FILE * fo, struct objectLine *objLine, struct objectFV infoLine,int *errors)
{
	generateObjCode(infoLine, objLine->objectCode,errors); // Generates the objectCode.

	// Checks if the are errors in the line
	if(strlen(infoLine.errors)>3)
	{
		objLine->error=1; // If there are errors, set the flag of errors to true;
		return;
	}
	if(objLine->newLine) // Check if it is a new line.
	{
		if(strcmp(objLine->objectCode,"      ")==0)
			return;
		objLine->address=infoLine.address; // Get the address of the first instruction of the text file.
		objLine->length=0;	// Set the length of the line to 0
		objLine->newLine=0;	// Set the flag of newLine to false.
		strcpy(objLine->line,""); // Initialize the line to empty.
		strcat(objLine->line,objLine->objectCode);	
		objLine->length += (strlen(objLine->objectCode)/2); // Get the length of bytes of the first instrucion on the line.
	}
	else
	{
		// Check if the current length of the text line plus the lenght of the new instruction, and the objectCode is not a space.
		if(objLine->length+(strlen(objLine->objectCode)/2) <= 0X1E && strcmp(objLine->objectCode,"      ")!=0) 
		{
			strcat(objLine->line,objLine->objectCode); // Add the new instruction to the line.
			objLine->length += strlen(objLine->objectCode)/2; // Increment the length of the line.
		}
		else
		{
			fprintf(fo,"T%06X%02X%s\n", objLine->address,objLine->length,objLine->line); // Print the line on the ObjectFile.
			objLine->length=0; // Set the length of the line to 0;
			objLine->newLine=1;	// Set the flag of new line to true.
			objectFile(fo,objLine,infoLine,errors);	// Call the function itself to handle the same line, because we did not store it somewhere.
		}
	}
}

/* Write all the information to ListingFile, except for START and END statements. */
void listingFile(FILE* fl, struct objectFV infoLine, int *errors)
{
	char oc[50];
	generateObjCode(infoLine, oc,errors); // Generates objectCode.
	fprintf(fl,"%06X  %s  %s\n",infoLine.address, oc,infoLine.line); // Prints all the information to the ListingFile.
	getErrorsMessages(fl,infoLine.errors,errors); // Prints all the errors that the line has.
}

/* Handles the START statement or the first uncommented line of the assembled file. */
void handleStartObjectFile(FILE *fr,FILE *fo, FILE* fl, struct objectFV infoLine,int PL,int *errors)
{
	int i=0;
	char line[100];
	char name[7];
	char oc[50];

	for (i;i<5;i++) // Get the first line
	{
		fgets(line,99,fr);
		line[strlen(line)-1]='\0';
		if(i%5==0)
			strcpy(infoLine.line,line); // Get the source line.
		else if(i%5==1)
			strcpy(name,line); // Get the name of the program if any.
		else if(i%5==2)
			sscanf(line,"%06X",&infoLine.opValue); // Get the opcode value.
		else if(i%5==3)
			sscanf(line,"%06X",&infoLine.address); // Get the address.
		else if(i%5==4)
			strcpy(infoLine.errors,line); // Get the errors.
	}

	if(infoLine.opValue==0x0000F0) // If there is a START directive.
	{
		fprintf(fo,"H%-6s%06X%06X\n",name,infoLine.address,PL); // Print the header line to the objectFile.
		fprintf(fl,"                %s\n",infoLine.line); // Print the first line to the listingFile.
		getErrorsMessages(fl,infoLine.errors,errors); // Print all the errors to the listingFile.
	}
	else
	{
		infoLine.address=atoi(name); // Get the address.
		generateObjCode(infoLine, oc,errors); // Generate objectCode.
		fprintf(fo,"H     %06X%06X\n",infoLine.address,PL); // Print the header line to the objectFile.
		fprintf(fl,"%06X  %s  %s\n",infoLine.address, oc,infoLine.line); // Print the first line to the listingFile
		getErrorsMessages(fl,infoLine.errors,errors); // Print all the errors to the listingFile.
	}
}

/* Handles the END directive */
void handleEndObjectFile(FILE * fo, FILE* fl,struct objectLine *objLine, struct objectFV infoLine,int *errors)
{
	fprintf(fo,"T%06X%02X%s\n", objLine->address,objLine->length,objLine->line); // Print the last text line to the objectFile.
	generateObjCode(infoLine, objLine->objectCode, errors); // Generate the objectCode.
	fprintf(fo, "E%s", objLine->objectCode); // Print the end line to the objectFile.
	fprintf(fl,"%06X          %s\n",infoLine.address, infoLine.line); // Print the last line to the listingFile.
	getErrorsMessages(fl,infoLine.errors,errors); // Print all the errors.
}

/* Generates the objectCode */
void generateObjCode(struct objectFV infoLine, char objectCode[], int *errors)
{
	int instruction; // Variable that will hold the objectCode.
	int address; 
	char label[7];
	int X=0; // Flax X for instruction.
	int i=0;
	char * pch;

	if(isspace(infoLine.label[0])) // Check if the operand is a space.
		strcpy(infoLine.label,"AAAAA"); // If it is a space replace it, by some other value. (AVL search would not work with spaces)
	pch = strtok (infoLine.label,","); 
	while (pch != NULL)
	{
		if(i==0)
			strcpy(label,pch); // Get whatever is in the operand field.
		else
		{
			if(strcmp(pch,"X")==0)
				X=1; // If there is an X set flag X to 1.
			else
				*errors ^= (-1 ^ *errors) & (1 << 12); 
		}
	  pch = strtok (NULL, ",");
	  i++;
	}

	if(infoLine.opValue == 0X4C) // Check for RSUB mnemonic.
		address=0;
	else if(infoLine.opValue == 0XF0) // Check for START directive.
	{
		printf("%s\n", label);
		*errors ^= (-1 ^ *errors) & (1 << 20);
	}
	else if(infoLine.opValue == 0XF1) // Check for WORD directive.
	{
		infoLine.opValue=0;
		address=atoi(label);
	}
	else if(infoLine.opValue==0XF2||infoLine.opValue==0XF3) // Check for RESW, and RESB directives.
	{
		strcpy(objectCode,"      ");
		return;
	}
	else if(infoLine.opValue == 0XF4) // Check for BYTE directive.
	{
		getCharHex(label,objectCode);
		return;
	}
	else if (infoLine.opValue==0XF5) // Check for END directive.
	{
		infoLine.opValue=0;
		address=getValueAVL(label);
	}
	else // Instruction.
		address=getValueAVL(label);

	if(address==-1)
	{
		address=0;
		*errors ^= (-1 ^ *errors) & (1 << 22);
	}

	// Set the objectCode.
	instruction = infoLine.opValue <<16; // Add the opcode value.
	instruction |= X<<15; // Add the value of the flag X.
	instruction |= address; // Add the address of the label in the operand.

	sprintf (objectCode, "%06X", instruction); // Convert instruction to string.
	if(strncmp(objectCode,"F6",2)==0)
		*errors ^= (-1 ^ *errors) & (1 << 23);

	return;
}

/* Function that what is inside of the single quotes, on the operand field for the BYTE directive. */
void getCharHex(char label[], char objectCode[])
{
	int i=0;
	int result=0;
	char type[2];
	char charHex[50];
	char * pch;
	pch = strtok (label,"'");
	while (i<2) // divide the operand.
	{
		if(i==0)
			strcpy(type,pch); // Get the type. It can be either a C or X.
		else if (i==1)
			strcpy(charHex,pch); // Get the value inside the quotes.
	  pch = strtok (NULL, "'");
	  i++;
	}

	if(strcmp(type,"X")==0) // If it is type X.
	{
		strcpy(objectCode,charHex); // objectCode would be the value inside the quotes.
		return;
	}
	else // If type C.
	{
		int x;
		int pos;
		int len=strlen(charHex)-1; 
		for(i=len;i>=0;i--) // Get the ascii values of each letter.
		{
			pos=(len-i)*8;
			x=charHex[i];
			result |= x << pos; // Store them in result.
		}
		sprintf (objectCode, "%06X", result); // Convert result to a string.
		return;
	}
}

void getErrorsMessages(FILE* fl,char errorsList[],int *errors)
{
	int i=0;
	int error;
	if(strlen(errorsList)<=3) // If there are no errors print a new line.
		fprintf(fl, "\n");
	else
	{
		char * pch;
		pch = strtok (errorsList,"</>");
		while (pch != NULL) // Print all the errors from pass 1.
		{
			error=atoi(pch)-10;
		 	fprintf(fl, "%s\n", errorMesages[error]);
		 	pch = strtok (NULL, "</>");
 		}

 		while (*errors) // Print all the errors from pass 2.
		{
		    if (*errors & 1)
		    	fprintf(fl,"%s\n", errorMesages[i]);
		    *errors >>= 1;
		    i++;
		}
		fprintf(fl,"\n");
		*errors=0; // Erase all errors.
	}
}
/****************************************************************************************************************/
/*******************************************Look HEre RiGo*************************************************************/

/****************************************** Phase 4 *************************************************************/
/****************************************************************************************************************/

void loadFile(char fileName[])
{
	char type;
	int lineSize;
	int i;
	ADDRESS address;
	BYTE byte;
	char instructions[61];
	char copy[100];
	char line[100];
	FILE *fr; // File read.
	if (( fr = fopen(fileName, "r")) == NULL) {
		printf("cannot open file\n");
		return;
	}
	fgets(line,99,fr);
	fgets(line,99,fr);

	while(line[0]!='E')
	{
		sscanf(line,"%c%6X%2X%s",&type,&address,&lineSize,instructions);
		for(i=0;i<lineSize;i++)
		{
			strcpy(copy,instructions);
			sscanf(instructions,"%2X",&byte);
			PutMem(address,&byte,0);
			copy[0]='-';copy[1]=' ';
			sscanf(copy,"%*s %s",instructions);
			address++;
		}
		fgets(line,99,fr);
	}
	//get end
}

void dumpMem(int start, int end)
{
	BYTE byte;
	for(start;start<end;start++)
	{
		GetMem(start,&byte,0);
		printf("%02X ", byte);
	}
}
