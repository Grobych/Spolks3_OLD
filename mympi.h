#ifndef MYMPI_H
#define MYMPI_H

#include <mpi/mpi.h>


void MPIerror(int error_code, int myrank);

void GetColorsForProcesses(int *colors, const int nGroups, const int nProcesses);

#endif // MYMPI_H
