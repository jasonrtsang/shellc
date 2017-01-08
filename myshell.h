/*
 * Filename:        myshell.h
 * 
 * Description:     Primative shell application in Linux.
 *
 * Reference:       http://cmpt-300.sfucloud.ca/cmpt-300/wp-content/uploads/2016/09/A1P2.pdf
 *
 * Creation date:   October 1, 2016
 * Author:          Jason Tsang
 */


#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#include "myjobs.h"


/* Sizes */
#define BUFFERSIZE_WORD 8
#define BUFFERSIZE_CHAR 256
#define BUFFERSIZE_DONE 4096
#define DIRECTORYSIZE 1024

/* Internal command parsing */
#define COMMAND_CD "cd"
#define COMMAND_BG "bg"
#define COMMAND_FG "fg"
#define COMMAND_JOBS "jobs"
#define COMMAND_EXIT "exit"
#define COMMAND_PIPING "|"
#define COMMAND_BACKGROUND "&"

/* myshell init variables */
static pid_t myshellPGID;
static int myshellterminal;


/********* Signal Handlers *********/
/*
 * Name: signalhandler_terminate
 * Description: Terminates process SIGINT (CTRL + C)
 * Return Value: None
 * Contraints: None
*/
void signalhandler_terminate(int p);

/*
 * Name: signalhandler_suspend
 * Description: Suspends process SIGSTP (CTRL + Z), signalhandler_child will be loaded
 * Return Value: None
 * Contraints: None
*/
void signalhandler_suspend(int p);

/*
 * Name: signalhandler_child
 * Description: Deals with signals from child processes SIGCHLD
 * Return Value: None
 * Contraints: None
*/
void signalhandler_child(int p);


/********* Functions *********/
/*
 * Name: myshell_init
 * Description: Initializes myShell ensuring that it's runnning interactively in foreground
 * Return Value: None
 * Contraints: None
 * Note: Adapted from GNU Free Documentation Library
 *       https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
*/
void myshell_init();

/*
 * Name: myshell_input
 * Description: Reads user input from STDIN
 * Return Value: The user input
 * Contraints: None
*/
char *myshell_input();

/*
 * Name: myshell_arguements
 * Description: Splits input into arguements using whitespace (tabs or spaces)
 * Return Value: Arguements
 * Contraints: None
*/
char **myshell_arguements(char *input);

/*
 * Name: myshell_setparent
 * Description: Adds process to jobs list and configures parent progress
 * Return Value: None
 * Contraints: None
*/
void myshell_setparent(pid_t pid, char **arguements, bool backgroundprocess);

/*
 * Name: myshell_setchild
 * Description: Configures child process signals and execute fork
 * Return Value: None
 * Contraints: None
*/
void myshell_setchild(char **arguements);

/*
 * Name: myshell_foreground
 * Description: Puts a job in the foreground, if continue is true, it will send the process
 *              group SIGCONT to wake it.
 * Return Value: None
 * Contraints: None
*/
void myshell_foreground(struct job *newjob, bool continuejob);

/*
 * Name: myshell_piping
 * Description: Executes and runs piped user commands.
 * Return Value: None
 * Contraints: None
*/
void myshell_piping(char **arguements, bool backgroundprocess);

/*
 * Name: myshell_handler
 * Description: Checks for the execution of internal commands first, else runs command
 * Return Value: None
 * Contraints: None
*/
void myshell_handler(char **arguements);
