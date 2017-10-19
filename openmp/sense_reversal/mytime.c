/*
 * CS 6210 - Fall 2017
 * Project - 2
 * Time calculator
 * Palak Choudhary
 */

#include <stdint.h>
#include <stdio.h>

# include "mytime.h"

double mysecond()
{
   struct timespec tp;
   clock_gettime(CLOCKID, &tp);

   return (((double) tp.tv_sec) + ((double) tp.tv_nsec / 1.e9));
}
