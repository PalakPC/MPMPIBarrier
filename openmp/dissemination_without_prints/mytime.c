/*
 * CS 6210 - Fall 2017
 * Project - 2
 * Time calculator
 * Palak Choudhary
 */

# include <stdint.h>
# include <stdio.h>

# include "mytime.h"

unsigned long long mysecond()
{
   struct timespec tp;
   clock_gettime(CLOCKID, &tp);

   return ((unsigned long long)(tp.tv_sec * 1.e9) + (unsigned long long)tp.tv_nsec);
}
