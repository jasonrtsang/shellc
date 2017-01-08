/*
 * Filename:        myjobs.h
 * 
 * Description:     Methods utilized by myshell regarding it's job structure management.
 *
 * Reference:       http://cmpt-300.sfucloud.ca/cmpt-300/wp-content/uploads/2016/09/A1P2.pdf
 *
 * Creation date:   October 1, 2016
 * Author:          Jason Tsang
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Job retrieval search type */
enum searchtype {
  SEARCH_PID,
  SEARCH_NUMBER
};

/* Job status definitions */
enum jobstatus {
  FOREGROUND,
  BACKGROUND,
  STOPPED,
  DONE
};

/* Job information stucture */
struct job {
  int number;
  pid_t pid;
  enum jobstatus status;
  char *name;
  struct job *next;
};

/* Universal job list */
static struct job *jobslist = NULL;

/* Universal job done list */
static char *jobsdonelist = NULL;

/* Lasted backgrounded job */
static struct job *jobsprevious = NULL;

/* Universal job number tracker */
static int numjobs = 0;


/********* Functions *********/
/*
 * Name: myjobs_print
 * Description: Displays all jobs in jobsList.
 * Return Value: None
 * Contraints: None
*/
void myjobs_print(struct job *jobsbefore);

/*
 * Name: myjobs_getnum
 * Description: Retrieves numjobs.
 * Return Value: numjobs
 * Contraints: None
*/
int myjobs_getnum();

/*
 * Name: myjobs_retrieve
 * Description: Retrieves a job into the job  list.
 * Return Value: The job found with its searchtype.
 * Contraints: None
*/
struct job *myjobs_retrieve(int number, enum searchtype type);

/*
 * Name: myjobs_insert
 * Description: Inserts a job into the job  list.
 * Return Value: The new job that was just inserted.
 * Contraints: None
*/
struct job *myjobs_insert(pid_t pid, enum jobstatus status, char* name);

/*
 * Name: myjobs_remove
 * Description: Removes a job from the jobs list.
 * Return Value: None
 * Contraints: None
*/
void myjobs_remove(struct job *removeJob);
