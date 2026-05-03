/* =========================================================================
 * Created on: <Sat May 02 01:33:40 +01 2026>
 * Time-stamp: <Sun May  3 16:17:18 +01 2026 by owner>
 * Author    : Henri Casanova, University of Hawaii
 *             Copyright (c) 2002, R. Bryant and D. O'Hallaron,
 *	       All rights reserved.
 * Desc      : ~/coding/c_prog/csapp/conc/rw3.c -
 *
 * Readers-writers solution that is fair to both readers and
 * writers. Simplifying assumption is that the number of concurrent
 * active readers is at most N.
 * See analysis at [[file:README.org::#the-fair-solution-rw3]]
 * ========================================================================= */
#include "../include/csapp.h"

#define N 10

/* Global variables */
sem_t sem;    /* Initially = N */
sem_t wmutex; /* Initially = 1 */

void reader(void) {
  while (1) {
    P(&sem);

    /* Critical section: */
    /* Reading happens   */

    V(&sem);
  }
}

void writer(void) {
  int i;

  while (1) {
    P(&wmutex);
    for (i = 0; i < N; i++)
      P(&sem);
    V(&wmutex);

    /* Critical section: */
    /* Writing happens   */

    for (i = 0; i < N; i++)
      V(&sem);
  }
}
