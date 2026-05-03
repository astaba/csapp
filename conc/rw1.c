/* =========================================================================
 * Created on: <Sat May 02 01:29:10 +01 2026>
 * Time-stamp: <Sun May  3 16:17:20 +01 2026 by owner>
 * Author    : From Courtois et al, CACM, 1971.
 *	       Copyright (c) 2002, R. Bryant and D. O'Hallaron,
 *	       All rights reserved.
 * Desc      : ~/coding/c_prog/csapp/conc/rw1.c -
 *
 * Figure 12.26 Solution to the first readers-writers problem.
 * Readers-writers solution with weak reader priority.
 * See analysis at [[file:README.org::#the-weak-solution-rw1]]
 * ========================================================================= */
#include "../include/csapp.h"

/* Global variables */
int readcnt;    /* Initially = 0 */
sem_t mutex, w; /* Both initially = 1 */

void reader(void) {
  while (1) {
    P(&mutex);
    readcnt++;
    if (readcnt == 1) /* First in */
      P(&w);
    V(&mutex);

    /* Critical section */
    /* Reading happens  */

    P(&mutex);
    readcnt--;
    if (readcnt == 0) /* Last out */
      V(&w);
    V(&mutex);
  }
}

void writer(void) {
  while (1) {
    P(&w);

    /* Critical section */
    /* Writing happens  */

    V(&w);
  }
}
