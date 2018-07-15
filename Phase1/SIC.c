
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void SIC();
void welcome();
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

int main()
{
	/* Start the computer simulation. */
	SIC();
}

/* Ask for a command while the user does not type "exit"*/
void SIC()
{
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
	printf("\nPar: %s\n\n",par1);
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
	printf("\nPar: %s %s \n\n",par1,par2);
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
	printf("\nPar: %s\n\n",par1);
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