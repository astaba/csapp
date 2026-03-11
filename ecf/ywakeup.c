/* Practice Problem 8.5 (solution page 833) */
/* write a wrapper function for sleep, called wakeup, with the following
interface: unsigned int wakeup(unsigned int secs);
The wakeup function behaves exactly as the sleep function, except that it prints
a message describing when the process actually woke up:
> Woke up at 4 secs. */

/* In C, the `sleep()` function returns the unslept time if it is interrupted by
a signal. If it sleeps for the full duration, it returns `0`. The calculation `t
- rc + 1` is a "human-friendly" adjustment for how we count time. Here is the
breakdown:

- `t - rc` = Elapsed time (how long it actually stayed asleep).
- `+ 1`    = Formatting/Rounding adjustment to show the "current" second rather
             than the "completed" seconds.
*/

#include <stdio.h>
#include <unistd.h>

unsigned int snooze(unsigned int t) {
  unsigned int rc = sleep(t);
  printf("Wake up at %d\n", t - rc + 1);
  return rc;
}
