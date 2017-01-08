/*
 * Filename:        myjobs.c
 * 
 * Description:     Methods utilized by myshell regarding it's job structure management.
 *
 * Reference:       http://cmpt-300.sfucloud.ca/cmpt-300/wp-content/uploads/2016/09/A1P2.pdf
 *
 * Creation date:   October 1, 2016
 * Author:          Jason Tsang
 */


#include "myjobs.h"


/* Job status definitions strings (order follows enum jobstatus) */
const char *jobstatustext[] = {
  "Running",
  "Running - B",
  "Stopped",
  "Done"
};


/*
 * Name: myjobs_print
 * Description: Displays all jobs in jobslist.
 * Return Value: None
 * Contraints: None
*/
void myjobs_print(struct job *jobsbefore) {
  struct job *printjob = jobslist;

  /* If job list exists, print all in list */
  if (printjob) {
    while (printjob) {
      /* If previous job, print with (+) flag */
      if (jobsbefore->number == printjob->number) {
        printf("[%d]+ %d\t %s\t\t %s\n", printjob->number, printjob->pid, jobstatustext[printjob->status], printjob->name);
      }
      else {
        printf("[%d]  %d\t %s\t\t %s\n", printjob->number, printjob->pid, jobstatustext[printjob->status], printjob->name);
      }
      printjob = printjob->next;
    }
  }
}


/*
 * Name: myjobs_getnum
 * Description: Retrieves numjobs.
 * Return Value: numjobs.
 * Contraints: None
*/
int myjobs_getnum() {
  return numjobs;
}


/*
 * Name: myjobs_retrieve
 * Description: Retrieves a job into the job  list.
 * Return Value: The job found with its searchtype.
 * Contraints: None
*/
struct job *myjobs_retrieve(int search, enum searchtype type) {
  struct job *retrievejob = jobslist;

  /* Loops through job list searching for either PID or number is job list exists */
  if (type == SEARCH_PID) {
    while (retrievejob) {
      if (retrievejob->pid == search) {
        break;
      }
      retrievejob = retrievejob->next;
    }
  }
  else if (type == SEARCH_NUMBER) {
    while (retrievejob) {
      if (retrievejob->number == search) {
        break;
      }
      retrievejob = retrievejob->next;
    }
  }
  return retrievejob;
}


/*
 * Name: myjobs_insert
 * Description: Inserts a job into the job  list.
 * Return Value: The new job that was just inserted.
 * Contraints: None
*/
struct job *myjobs_insert(pid_t pid, enum jobstatus status, char* name) {
  /* Initialize job lists */
  struct job *currentjob = jobslist;
  struct job *newjob = malloc(sizeof(struct job));

  /* Check to see if new job was allocated before use */
  if (!newjob) {
    printf("Error: Could not allocate memory for inserting new job.\n");
    goto myjobs_insert_return;
  }

  /* Name needs memory as it is a string */
  newjob->name = (char*)malloc(sizeof(name));

  /* Check to see if new job name was allocated before use */
  if (!(newjob->name)) {
    printf("Error: Could not allocate memory for inserting new job name.\n");
    goto myjobs_insert_return;
  }

  newjob->pid = pid;
  newjob->status = status;
  newjob->name = strcpy(newjob->name, name);
  newjob->next = NULL;

  /* If jobs list already exists add to end, else make it head */
  if (jobslist) {
    while (currentjob->next) {
      currentjob = currentjob->next;
    }
    newjob->number = currentjob->number+1;
    currentjob->next = newjob;
  }
  else {
    newjob->number = 1;
    jobslist = newjob;
  }
  numjobs++;

  /* If job is background, print out it's pid to user */
  if (status == BACKGROUND) {
    printf("[%d]+ %d\n", newjob->number, newjob->pid);
  }

myjobs_insert_return:
  return newjob;
}


/*
 * Name: myjobs_remove
 * Description: Removes a job from the jobs list.
 * Return Value: None
 * Contraints: None
*/
void myjobs_remove(struct job *removejob) {
  struct job *currentjob, *previousjob = NULL;

  /* Visit each job */
  for (currentjob = jobslist; currentjob; 
       previousjob = currentjob, currentjob = previousjob->next) {

    if (currentjob == removejob) {
      /* Remove from beginning */
      if (!previousjob) {
        /* Fix beginning pointer. */
        jobslist = currentjob->next;
      }
      /* Remove from middle or end */
      else {
        previousjob->next = currentjob->next;
      }

      /* Deallocate the job */
      free(currentjob->name);
      free(currentjob);

      numjobs--;
    }
  }
}
