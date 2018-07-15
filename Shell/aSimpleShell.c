/*

	Raul Muniz
	CMPE 4334-01
	Dr. Egle
 */
#include	<stdio.h>
#include	<stdlib.h>
#include 	<sys/types.h>	//necessary.
#include	<sys/wait.h>
#include	<string.h>
#include	<unistd.h> //Fork
#include	"aSimpleShell.h"

char *lookupPath(char **, char **);
int parseCommand(char *, struct command_t *);
int parsePath(char **);
void printPrompt();
void readCommand(char *);

char prompt[] = "myShell>";
extern FILE *stdin;

int main() 
{
    char commandInput[LINE_LEN];
    char *pathv[MAX_PATHS];
    int numPaths;
    int i, j, len;
    int childPID;
    int stat;
    pid_t theChPID;
    struct command_t command;

//start-------------
    for(i=0; i<MAX_ARGS; i++)
        command.argv[i] = (char *) malloc(MAX_ARG_LEN);

    parsePath(pathv);

 
    while(TRUE) 
    {
        printPrompt();

 //parse here........
        readCommand(commandInput);
        printf("Command:: %s\n", commandInput);

        if( (strcmp(commandInput, "exit") == 0) ||
            (strcmp(commandInput, "quit") == 0))
        {
            break;
        }

        parseCommand(commandInput, &command);

#ifdef DEBUG
        printf("returned from parseCommmand!\n");

        for(i=0; i<command.argc; i++)
            printf("	argv[%d] = %s\n", i,  command.argv[i]);
#endif

        
        command.name  = lookupPath(command.argv, pathv);

#ifdef DEBUG
        printf("returned from lookupPath!\n");
        printf("command path = %s\n", command.name);
        for(i=0; i<command.argc; i++)
	    printf("argv[%d] = %s\n", i,  command.argv[i]);
#endif

        if(command.name == NULL) 
        {
            fprintf(stderr, "Command %s ..Error: 404\n", command.argv[0]);
            continue;
        }
/*--------------------------------------------------
fork -- create a new process

SYNOPSIS
     #include <unistd.h>

     pid_t
     fork(void);

DESCRIPTION
     fork() causes creation of a new process.  The new process (child process)
     is an exact copy of the calling process (parent process) except for the
     following:

           o   The child process has a unique process ID.

           o   The child process has a different parent process ID (i.e., the
               process ID of the parent process).

           o   The child process has its own copy of the parent's descriptors. 		       			These descriptors reference the same underlying objects, so
               that, for instance, file pointers in file objects are shared
               between the child and the parent, so that an lseek(2) on a
               descriptor in the child process can affect a subsequent read or
               write by the parent.  This descriptor copying is also used by
               the shell to establish standard input and output for newly cre-
               ated processes as well as to set up pipes.

           o   The child processes resource utilizations are set to 0; see
               setrlimit(2).

RETURN VALUES
     Upon successful completion, fork() returns a value of 0 to the child
     process and returns the process ID of the child process to the parent
     process.  Otherwise, a value of -1 is returned to the parent process, no child process is created, and the global variable errno is set to indi-
     cate the error.

ERRORS
     fork() will fail and no child process will be created if:

     [EAGAIN]           The system-imposed limit on the total number of pro-
                        cesses under execution would be exceeded.  This limit
                        is configuration-dependent.

     [EAGAIN]           The system-imposed limit MAXUPRC (<sys/param.h>) on
                        the total number of processes under execution by a
                        single user would be exceeded.

     [ENOMEM]           There is insufficient swap space for the new process.

LEGACY SYNOPSIS
     #include <sys/types.h>		*remember to include this*
     #include <unistd.h>

     The include file <sys/types.h> is necessary.
----------------------------------------------------*/
        if((childPID = fork()) == 0) /* This is the child process */
        {


#ifdef DEBUG
            printf("child in execution: %s\n", command.name);
            for(i=1; i<command.argc; i++)
	            printf("argv[%d] = %s\n", i,  command.argv[i]);
#endif

            execv(command.name, command.argv);
       }

//Parent should wait-----
#ifdef DEBUG
       printf("Parent waiting...\n");
#endif

        theChPID = wait(&stat);
  }


}


char *lookupPath(char **argv, char **dir) 
{

    int i;
    char *result;
    char pName[MAX_PATH_LEN];

     for(i = 0; i < MAX_PATHS; i++) 
    {
        if(dir[i] == NULL) break;
        strcpy(pName, dir[i]);
        strcat(pName, "/");
        strcat(pName, argv[0]);

#ifdef DEBUG
        printf("lookupPath: Searching. . .  %s\n", pName);
#endif

        if(access(pName, X_OK | F_OK) != -1) 
        {

#ifdef DEBUG
            printf("lookupPath: located %s in %s --Location: %s)\n",
			argv[0], dir[i], pName);
#endif
            result = (char *) malloc(strlen(pName)+1);
            strcpy(result, pName);
            return result;		// Return with success
        }
    }

//file already path?
    if(*argv[0] == '/') 
    {
        result = (char *) malloc(strlen(argv[0])+1);
        strcpy(result, argv[0]);
        return result;
    }

 //directories for path to find
   

    fprintf(stderr, "%s: Invalid Command\n", argv[0]);
    return NULL;

}


int parseCommand(char *cLine, struct command_t *cmd) 
{
 
    int argc;
    int i, j;
    char **clPtr;


    clPtr = &cLine;
    argc = 0;

    while((cmd->argv[argc++] = strsep(clPtr, WHITESPACE)) != NULL) ;

    cmd->argv[argc--] = '\0';	//end
    cmd->argc = argc;

    return  1;	
}


int parsePath(char *dirs[]) 
{
 
    int i;
    char *pathEnvVar;
    register char *thePath, *oldPath;

    for(i=0; i<MAX_ARGS; i++)
	dirs[i] = NULL;

    pathEnvVar = (char *) getenv("PATH");
    thePath = (char *) malloc(strlen(pathEnvVar) + 1);
    strcpy(thePath, pathEnvVar);

    i = 0;
    oldPath = thePath;

    for(;; thePath++) 
    {
	if((*thePath == ':') || (*thePath == '\0')) 
        {
            dirs[i] = oldPath;
            i++;

            if(*thePath == '\0') break;

            *thePath = '\0';
            oldPath = thePath + 1;
        }
    }

#ifdef DEBUG
    printf("Directories in PATH variable\n");
        for(i=0; i<MAX_PATHS; i++)
            if(dirs[i] != '\0')
		printf("	Directory[%d]: %s\n", i, dirs[i]);
#endif

}


void printPrompt() 
{
    printf("%s ", prompt);
}

void readCommand(char *buffer) 
{
    // original dangerous stdio call - can over-run buffer, see man gets
    // gets(buffer); 

  
    // suggested safer replacement call - can't go beyond length provided,
    // but we must strip off the line feed included in the buffer unlike gets
    // 
    fgets(buffer, LINE_LEN, stdin);

    buffer[strlen(buffer)-1] = '\0';  // overwrite the line feed with null term

    printf("Command Buffering.. [\" %s \"]\n", buffer);
}
/*
ls: Show directory contents, lists names of files.
mkdir: Creates a directory of the specified name.
cat: Display contents of a file.
cd: Change directory. Change to certain directory name if provided.
pwd: Displays the name of the working directory.
touch: Creates a blank file with a specified name.
less: View contents of specified file, page by page.
head/tail: Displays the first/ last 10 lines of a file.
rm: Removes a specified file. This action is permanent. There is no recycle bin.
rmdir: Removes a directory.

cp: Copy specified file to a new named file. Use -r flag to copy a directory.
mv: Rename a specified file or directory.
find: search files and directories. Can use with wildcards (* ? [ ]).
quota: Print the amount of space available and used on all shares for the current user.
*/