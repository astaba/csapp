/* =========================================================================
 * Created on: <Sat May 02 01:32:10 +01 2026>
 * Time-stamp: <Sun May  3 16:17:20 +01 2026 by owner>
 * Author    : Copyright (c) 2002, R. Bryant and D. O'Hallaron,
 *	       All rights reserved.
 * Desc      : ~/coding/c_prog/csapp/conc/rw2.c -
 *
 * Readers-writers solution with strong reader priority.
 * See analysis at [[file:README.org::#the-strong-solution-rw2]]
 * ========================================================================= */
#include "../include/csapp.h"

/* Global variables */
int readcount;      /* Initially = 0 */
sem_t mutex, w, wg; /* All initially = 1 */

void reader(void) {
  while (1) {
    P(&mutex);
    readcount++;
    if (readcount == 1)
      P(&w);
    V(&mutex);

    /* Critical section: */
    /* Reading happens   */

    P(&mutex);
    readcount--;
    if (readcount == 0)
      V(&w);
    V(&mutex);
  }
}

void writer(void) {
  while (1) {
    P(&wg);
    P(&w);

    /* Critical section: */
    /* Writing happens   */

    V(&w);
    V(&wg);
  }
}
