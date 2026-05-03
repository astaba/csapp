/* =========================================================================
 * Created on: <Sat May 02 01:23:46 +01 2026>
 * Time-stamp: <Sun May  3 16:17:21 +01 2026 by owner>
 * Author    : Copyright (c) 2002, R. Bryant and D. O'Hallaron,
 *	       All rights reserved.
 * Desc      : ~/coding/c_prog/csapp/conc/rw0.c -
 *
 * Naive readers-writers solution
 * See analysis at [[file:README.org::#the-naive-solution-rw0]]
 * ========================================================================= */
#include "../include/csapp.h"

/* Global variable */
sem_t w; /* Initially = 1 */

void reader(void) {
  while (1) {
    P(&w);

    /* Critical section: */
    /* Reading happens   */

    V(&w);
   }
}

void writer(void) {
  while (1) {
    P(&w);

    /* Critical section: */
    /* Writing happens   */

    V(&w);
  }
}
