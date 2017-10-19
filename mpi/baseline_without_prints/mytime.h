/*
 * CS 6210 - Fall 2017
 * Time calculator - Header
 * Palak Choudhary
 */

# include <time.h>

# define HAVE_POSIX_TIMER

# ifdef CLOCK_MONOTONIC
#  define CLOCKID CLOCK_MONOTONIC
# else
#  define CLOCKID CLOCK_REALTIME
# endif

unsigned long long mysecond();
