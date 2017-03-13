#include "functions.h"

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}


//int rand_lim(int limit)
//{
///* return a random number between 0 and limit inclusive.
// */
//    int divisor = RAND_MAX/(limit+1);
//    int retval;

//    do {
//        retval = rand() / divisor;
//    } while (retval > limit);

//    return retval;
//}
