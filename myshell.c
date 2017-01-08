/*
 * Filename:        myshell.c
 * 
 * Description:     Primative shell application in Linux.
 *
 * Reference:       http://cmpt-300.sfucloud.ca/cmpt-300/wp-content/uploads/2016/09/A1P2.pdf
 *                  README.md
 *
 * Creation date:   October 1, 2016
 * Author:          Jason Tsang
 */


#include "myshell.h"


/*
 * Name: signalhandler_terminate
 * Description: Terminates process SIGINT (CTRL + C)
 * Return Value: None
 * Contraints: None
*/
void signalhandler_terminate(int p) {
  printf("\n");
  exit(EXIT_SUCCESS);
}


/*
 * Name: signalhandler_suspend
 * Description: Suspends process SIGSTP (CTRL + Z), signalhandler_child will be loaded
 * Return Value: None
 * Contraints: None
*/
void signalhandler_suspend(int p) {
  /* Do nothing, signalhandler_child will handle the interrupt */
}


/*
 * Name: signalhandler_child
 * Description: Deals with signals from child processes SIGCHLD
 * Return Value: None
 * Contraints: None
*/
void signalhandler_child(int p) {
  /* Initialize variables */
  char *jobsdonebuffer;
  pid_t pid;
  int status, donebuffersize;
  struct job *childjob = NULL;

  /* Get pid status of any child that received interrupt */
  pid = waitpid(WAIT_ANY, &status, WUNTRACED|WNOHANG);
  /* If pid is greater than 0, it specifies the process ID of a single child process 
     for which status is requested */
  if (pid > 0) {
    /* Retrieve child from job list */
    struct job *childjob = myjobs_retrieve(pid, SEARCH_PID);
    /* If the job pid exists process, else return */
    if (childjob) {
      /* If the child terminated normally in the background, print Done and remove job */
      if (WIFEXITED(status)) {
        if (childjob->status == BACKGROUND) {
          childjob->status = DONE;
          /* Get size of done string to print */
          /* If childjob is jobsprevious, print with (+) flag */
          if (jobsprevious->number == childjob->number) {
            donebuffersize = snprintf(NULL, 0, "[%d]+ %d\t Done\t\t\t %s\n", childjob->number, childjob->pid, childjob->name);
          }
          else {
            donebuffersize = snprintf(NULL, 0, "[%d]  %d\t Done\t\t\t %s\n", childjob->number, childjob->pid, childjob->name);
          }
          jobsdonebuffer = (char *)malloc(donebuffersize+1);
          /* Check to see if buffer was allocated before use */
          if (jobsdonebuffer) {
            /* Add done string to buffer, and concatenate to main output buffer */
            if (jobsprevious->number == childjob->number) {
              snprintf(jobsdonebuffer, donebuffersize+1, "[%d]+ %d\t Done\t\t\t %s\n", childjob->number, childjob->pid, childjob->name);
            }
            else {
              snprintf(jobsdonebuffer, donebuffersize+1, "[%d]  %d\t Done\t\t\t %s\n", childjob->number, childjob->pid, childjob->name);
            }
            if (jobsdonelist) {
              strcat(jobsdonelist, jobsdonebuffer);
            }
            else {
              printf("Error: No allocated buffer for outputing done jobs list.\n");
            }
            /* Free temp buffer to add to list */
            free(jobsdonebuffer);
          }
          else {
            printf("Error: Could not allocate buffer for adding job to done job list.\n");
          }
          /* Remove child job */
          myjobs_remove(childjob);
        }
      }
      /* If the child terminated because it was killed, print Killed and remove job */
      else if (WIFSIGNALED(status)) {
        /* No print format change for (+) as killed job will be jobsprevious */
        printf("\n[%d]+ %d\t Killed\t\t\t %s\n", childjob->number, childjob->pid, childjob->name);
        myjobs_remove(childjob);
      }
      /* If the child receives SIGSTP signal suspend the job, print Stopped and
         return terminal control to myshell */
      else if (WIFSTOPPED(status)) {
        /* Give child control of terminal to print stopped */
        tcsetpgrp(myshellterminal, childjob->pid);
        childjob->status = STOPPED;
        /* No print format change for (+) as stopped job will be jobsprevious */
        printf("\n[%d]+ %d\t Stopped\t\t %s\n", childjob->number, childjob->pid, childjob->name);
      }
    }
    tcsetpgrp(myshellterminal, myshellPGID);
  }
}


/*
 * Name: myshell_init
 * Description: Initializes myShell ensuring that it's runnning interactively in foreground
 * Return Value: None
 * Contraints: None
 * Note: Adapted from GNU Free Documentation Library
 *       https://www.gnu.org/software/libc/manual/html_node/Initializing-the-Shell.html
*/
void myshell_init() {
  /* Get PID of parent myshell */
  pid_t pid;
  pid = getpid();
  /* Set terminal as STDIN */
  myshellterminal = STDIN_FILENO;

  /* Check if myshell is interactive (is STDIN the terminal) */
  if (isatty(myshellterminal)) {
    /* Loop until we are in foreground */
    while (tcgetpgrp(myshellterminal) != getpgrp()) {
      kill(pid, SIGTTIN);
    }

    /* Ignore all interactive and job control signals, use custom handler for child */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, &signalhandler_child);

    /* Put myshell in its own process group, abort if failed */
    myshellPGID = getpid();
    if (setpgid(myshellPGID, myshellPGID) < 0) {
      perror("Error: Couldn't put myShell in its own process group.\n");
      exit(EXIT_FAILURE);
    }

    /* Get control of terminal */
    tcsetpgrp(myshellterminal, myshellPGID);
  }
  else {
    printf("Error: Couldn't make my_shell interactive.\n");
    exit(EXIT_FAILURE);
  }

  /* Allocate memory for "Done" buffer to be printed at next enter */
  jobsdonelist = (char *)malloc(BUFFERSIZE_DONE*sizeof(char *));

  /* Check to see if buffer was allocated before use */
  if (!jobsdonelist) {
    printf("Error: Could not allocate buffer for outputing done jobs list.\n");
  }

}


/*
 * Name: myshell_input
 * Description: Reads user input from STDIN
 * Return Value: The user input
 * Contraints: None
*/
char *myshell_input() {
  /* Initialize buffer */
  int bufferindex = 0, buffersize = BUFFERSIZE_CHAR;
  char *readbuffer = (char *)malloc(buffersize*sizeof(char *));
  char inputchar;

  if (!readbuffer) {
    printf("Error: Could not allocate buffer for user input.\n");
    goto myshell_input_return;
  }

  /* Read input from STDIN */
  while (inputchar = getchar()) {
    /* If user enters EOF (CTRL + D) terminate myshell */
    if (inputchar == EOF) {
      printf("\n");
      exit(EXIT_SUCCESS);
    } 
    /* If input is "enter" return input sentence with NULL terminate, else add char to buffer */
    else if (inputchar == '\n') {
      readbuffer[bufferindex] = '\0';
      /* Print out all done jobs queued in buffer after user hits enter (mimic terminal) */
      if (jobsdonelist) {
        printf("%s", jobsdonelist);
        memset(jobsdonelist, 0, sizeof(jobsdonelist));
      }

      goto myshell_input_return;
    }
    else {
      readbuffer[bufferindex++] = inputchar;
    }

    /* Check if the buffer is full, if so double its size */
    if (bufferindex >= buffersize) {
      readbuffer = realloc(readbuffer, (buffersize*=2)*sizeof(char*));
      if (!readbuffer) {
        printf("Error: Could not reallocate buffer for user input.\n");
        goto myshell_input_return;
      }
    }
  }

myshell_input_return:
  return readbuffer;
}


/*
 * Name: myshell_arguements
 * Description: Splits input into arguements using whitespace (tabs or spaces)
 * Return Value: Arguements
 * Contraints: None
*/
char **myshell_arguements(char *input) {
  /* Initialize buffers */
  int bufferindex = 0, buffersize = BUFFERSIZE_WORD;
  char **arguements = malloc(buffersize*sizeof(char *));
  char *arguementstoken;

  /* Check to see if buffer was allocated before use */
  if (!arguements) {
    printf("Error: Could not allocate buffer for getting arguements.\n");
    goto myshell_arguements_return;
  }

  while (arguementstoken = strtok_r(input, " \t", &input)) {
    arguements[bufferindex++] = arguementstoken;

    /* Check if the buffer is full, if so double its size */
    if (bufferindex >= buffersize) {
      arguements = realloc(arguements, (buffersize*=2)*sizeof(char*));
      if (!arguements) {
        printf("Error: Could not reallocate buffer for getting arguements.\n");
        goto myshell_arguements_return;
      }
    }
  }

  /* NULL terminate arguements if there was user input */
  if (arguements[0]) {
    arguements[bufferindex] = '\0';
  }

myshell_arguements_return:
  return arguements;
}


/*
 * Name: myshell_setparent
 * Description: Adds process to jobs list and configures parent progress
 * Return Value: None
 * Contraints: None
*/
void myshell_setparent(pid_t pid, char **arguements, bool backgroundprocess) {
  struct job *newjob = NULL;
  /* Set child as new process group leader */
  setpgid(pid, pid);

  /* Add job to job list with either background or foreground status, if background, set most recent flag */
  if (backgroundprocess) {
    newjob = myjobs_insert(pid, BACKGROUND, arguements[0]);
    if (!newjob) {
      printf("Error: Couldn't add new job to job list.\n");
    }
    jobsprevious = newjob;
  }
  else {
    newjob = myjobs_insert(pid, FOREGROUND, arguements[0]);
    if (!newjob) {
      printf("Error: Couldn't add new job to job list.\n");
    }
    else {
      myshell_foreground(newjob, false);
    }
  }
}


/*
 * Name: myshell_setchild
 * Description: Configures child process signals and execute fork
 * Return Value: None
 * Contraints: None
*/
void myshell_setchild(char **arguements) {
  /* Set custom signal handler */
  signal(SIGINT, &signalhandler_terminate);
  signal(SIGTSTP, &signalhandler_suspend);
  signal(SIGCHLD, &signalhandler_child);

  /* Set child as new process group leader for signals */
  setpgrp();

  /* Execute command, termination successful regardless */
  if (execvp(arguements[0], arguements) == -1) {
    perror("Error");
  }
  exit(EXIT_SUCCESS);
}


/*
 * Name: myshell_foreground
 * Description: Puts a job in the foreground, if continue is true, it will send the process
 *              group SIGCONT to wake it.
 * Return Value: None
 * Contraints: None
*/
void myshell_foreground(struct job *newjob, bool continuejob) {
  /* Set status as foreground and give it control of myshell */
  newjob->status = FOREGROUND;
  tcsetpgrp(myshellterminal, newjob->pid);
  /* Send newJob SIGCONT if continue job */
  if (continuejob) {
    if (kill(-newjob->pid, SIGCONT) < 0) {
      perror("kill (SIGCONT");
    }
  }
  /* Wait while there if a child not exited, and remove after it is executed */
  while (waitpid(newjob->pid, NULL, WNOHANG) == 0) {
    /* If job is suspended ignore and return */
    if (newjob->status == STOPPED)
      goto myshell_foreground_return;
  }
  myjobs_remove(newjob);
  /* Return shell control back to myshell */
  tcsetpgrp(myshellterminal, myshellPGID);

myshell_foreground_return:
  return;
}


/*
 * Name: myshell_piping
 * Description: Executes and runs piped user commands.
 * Return Value: None
 * Contraints: None
*/
void myshell_piping(char **arguements, bool backgroundprocess) {
  /* File descriptors (0 = output) (1 = input) */
  int fdP[2]; /* Pipe Child to contact Parent */
  int fdC[2]; /* Pipe Parent to contact Child */

  /* Initialize buffer */
  int bufferindex = 0, buffersize = BUFFERSIZE_WORD;
  char **pipingbuffer = malloc(buffersize*sizeof(char *));

  /* Counters and other intializations */
  bool pipingcomplete = false;
  int arguementsindex = 0, pipingindex = 0, numcommands = 0;
  pid_t pid;

  /* Check to see if buffer was allocated before use */
  if (!pipingbuffer) {
    printf("Error: Could not allocate buffer for piping arguements.\n");
    goto myshell_piping_return;
  }

  /* Count number of commands between "|" for piping */
  while (arguements[arguementsindex]) {
    if (strcmp(arguements[arguementsindex++], COMMAND_PIPING) == 0) {
      numcommands++;
      if (!arguements[arguementsindex]) {
        printf("Error: Must input command after '|' token for piping.\n");
        goto myshell_piping_return;
      }
    }
  }

  /* Loop and configure pipes for each arguement (command) that was counted
     configure STDIN/ STDOUT for each child process and execute */
  arguementsindex = 0;
  while (!pipingcomplete) {
    /* Parse arguements and seperate each command into new buffer */
    while (strcmp(arguements[arguementsindex], COMMAND_PIPING) != 0) {
      pipingbuffer[bufferindex++] = arguements[arguementsindex++];
      /* Check if end of all arguements, if so raise loop flag */
      if (!arguements[arguementsindex]) {
        pipingcomplete = true;
        break;
      }
    }
    /* NULL terminate arguements */
    pipingbuffer[bufferindex] = '\0';

    /* Determine if iteration is even else odd. Using this we can correctly assign
       the parent-child inputs/ outputs shared between the interation and iteration-1,
       p0-c1 -> p1-c2 -> p2-c3 -> ... */
    if (pipingindex%2 == 0) {
      pipe(fdC);
    }
    else {
      pipe(fdP);
    }
    /* Create child process */
    pid = fork();

    /* Failed to fork */
    if (pid < 0) {
      if (pipingindex != numcommands) {
        if (pipingindex%2 == 0) {
          close(fdC[1]);
        }
        else {
          close(fdP[1]);
        }
      }
      printf("Error: Couldn't create child process for piping.\n");
      goto myshell_piping_return;
    }
    /* Configure piping for child process, if fail print error */ 
    else if (pid == 0) {
      /* If this is the first command in the pipe */
      if (pipingindex == 0) {
        dup2(fdC[1], STDOUT_FILENO);
      }
      /* Else if it is the last command in the pipe, leave STDOUT untouched */
      else if (pipingindex == numcommands) {
        /* If the last command+1 (STDOUT) is even, pipe parent to child */
        if ((pipingindex+1)%2 == 0) {
          dup2(fdC[0], STDIN_FILENO);
        }
        else {
          dup2(fdP[0], STDIN_FILENO);
        }
      }
      /* Else middle command terms we connect two pipes (I/O) */
      else {
        /* If the command is even, enable child to parent out as in, and parent to child in as out */
        if (pipingindex%2 == 0) {
          dup2(fdP[0], STDIN_FILENO); 
          dup2(fdC[1], STDOUT_FILENO);
        }
        else { 
          dup2(fdC[0], STDIN_FILENO); 
          dup2(fdP[1], STDOUT_FILENO);
        }
      }
      /* Child process */
      myshell_setchild(pipingbuffer);
    }
    /* Parent process */
    else {
      myshell_setparent(pid, pipingbuffer, backgroundprocess);
    }

    /* Close unused file descriptors for all*/
    /* If this is the first command in the pipe, only close parent to child in */
    if (pipingindex == 0) {
      close(fdC[1]);
    }
    /* Else if it is the last command+1 (STDOUT) in the pipe */
    else if (pipingindex == numcommands) {
      if ((pipingindex+1)%2 == 0) {
        close(fdC[0]);
      }
      else {
        close(fdP[0]);
      }
    }
    /* Else middle command */
    else {
      if (pipingindex%2 == 0) {
        close(fdP[0]);
        close(fdC[1]);
      }
      else { 
        close(fdC[0]);
        close(fdP[1]);
      }
    }

    /* Reset and increment indexes, and increment iteration */
    bufferindex = 0;
    arguementsindex++;
    pipingindex++;
  }

  /* Make sure all file descriptors are closed */
  /* Check @ ls -l /proc/<myshellPGID>/fd */
  close(fdP[0]);
  close(fdP[1]);
  close(fdC[0]);
  close(fdC[1]);

myshell_piping_return:
  /* Deallocate dynamic memory */
  free(pipingbuffer);
  return;
}


/*
 * Name: myshell_handler
 * Description: Checks for the execution of internal commands first, else runs command
 * Return Value: None
 * Contraints: None
*/
void myshell_handler(char **arguements) {
  /* Initialize variables */
  bool backgroundprocess = false;
  int arguementsindex = 0;
  pid_t pid;
  struct job *retrievejob = NULL;

  /* "exit" - terminate myshell */
  if (strcmp(arguements[0], COMMAND_EXIT) == 0) {
    exit(EXIT_SUCCESS);
  }

  /* "&" - background process */
  /* Check if first arguement is just "&", if so return */
  if (strcmp(arguements[0], COMMAND_BACKGROUND) == 0) {
    printf("Error: Must input command before background process '&'\n");
    goto myshell_handler_return;
  }
  while (arguements[arguementsindex]) {
    if (strcmp(arguements[arguementsindex++], COMMAND_BACKGROUND) == 0) {
      arguements[arguementsindex-1] = NULL;
      backgroundprocess = true;
    }
  }

  /* "|" - piping */
  /* Check if first arguement is "|", if so return */
  if (strcmp(arguements[0], COMMAND_PIPING) == 0) {
    printf("Error: Must input command before '|' token for piping.\n");
    goto myshell_handler_return;
  }
  arguementsindex = 0;
  while (arguements[arguementsindex]) {
    if (strcmp(arguements[arguementsindex++], COMMAND_PIPING) == 0) {
      myshell_piping(arguements, backgroundprocess);
      goto myshell_handler_return;
    }
  }

  /* "cd" - change directory */
  if (strcmp(arguements[0], COMMAND_CD) == 0) {
    if (arguements[1]) {
      if (chdir(arguements[1]) != 0) {
        perror("cd");
      }
    }
    else {
      printf("cd: Missing directory\n");
    }
  }

  /* "bg" - background job */
  else if (strcmp(arguements[0], COMMAND_BG) == 0) {
    /* Use job number as arguement, else background previous job */
    if (arguements[1]) {
      /* Convert string job number is arguement 1 to int */
      retrievejob = myjobs_retrieve((int)atoi(arguements[1]), SEARCH_NUMBER);
    }
    else {
      retrievejob = jobsprevious;
    }
    /* if the job number exists, and was previously stopped, then it will signal it to run */
    if (retrievejob) {
      if (retrievejob->status == STOPPED) {
        retrievejob->status = BACKGROUND;
        if (kill(-retrievejob->pid, SIGCONT) < 0) {
          perror("kill (SIGCONT)");
        }
      }
    }
    else {
      printf("bg: no such job\n");
    }
  }

  /* "fg" - foreground job */
  else if (strcmp(arguements[0], COMMAND_FG) == 0) {
    /* Use job number as arguement, else foreground previous job */
    if (arguements[1]) {
      /* Convert string job number is arguement 1 to int */
      retrievejob = myjobs_retrieve((int)atoi(arguements[1]), SEARCH_NUMBER);
    }
    else {
      retrievejob = jobsprevious;
    }
    /* If the job exists, and was previously stopped, signal it to run. Regardless foreground job */
    if (retrievejob) {
      /* Set new previous job (in case arguement was used) */
      jobsprevious = retrievejob;
      if (retrievejob->status == STOPPED) {
        myshell_foreground(retrievejob, true);
      }
      else {
        myshell_foreground(retrievejob, false);
      }
    }
    else {
      printf("fg: no such job\n");
    }
  }

  /* "jobs" - print out all jobs */
  else if (strcmp(arguements[0], COMMAND_JOBS) == 0) {
    myjobs_print(jobsprevious);
  }

  /* Execute non-internal, non-piping */
  else {
    /* Create child process */
    pid = fork();
    /* Failed to fork */
    if (pid < 0) {
      printf("Error: Could not fork process.\n");
    }
    /* Child process */
    else if (pid == 0) {
      myshell_setchild(arguements);
    }
    /* Parent process */
    else {
      myshell_setparent(pid, arguements, backgroundprocess);
    }
  }

myshell_handler_return:
  return;
}


/*
 * Name: main
 * Description: Main loop that runs the shell.
 * Return Value: Sucessful always.
 * Contraints: None
*/
int main() {
  /* Initialize variables */
  char *currentdirectory, *input;
  char **arguements;

  /* Initialize myshell */
  myshell_init();

  /* Infinite program loop */
  while (true) {
    /* Prints the myshell prompt on screen, displays number of jobs and current directory */
    printf("my_shell > Jobs: %d - %s$ ", myjobs_getnum(), getcwd(currentdirectory, DIRECTORYSIZE));

    /* Get inputs from user */
    input = myshell_input();
    if (!input) {
      printf("Error: Could not read input.\n");
    }
    /* Process inputs into arguements */
    arguements = myshell_arguements(input);
    if (!arguements) {
      printf("Error: Could not get arguements from input.\n");
    }
    /* Execute arguement(s) if there is one */
    if (arguements[0]) {
      myshell_handler(arguements);
    }

    /* Deallocate dynamic memory */
    free(input);
    free(arguements);
  }

  return EXIT_SUCCESS;
}
