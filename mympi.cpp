#include "mympi.h"
#include "functions.h"

#include <ctime>
#include <stdlib.h>
#include <assert.h>

void MPIerror(int error_code, int myrank)
{
    if(error_code != MPI_SUCCESS)
    {
        char error_string[BUFSIZ];
        int length_of_error_string;

        MPI_Error_string(error_code, error_string, &length_of_error_string);
        fprintf(stderr, "%3d: %s\n", myrank, error_string);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
}

void GetColorsForProcesses(int *colors, const int nGroups, const int nProcesses)
{
    assert(colors && "colors nullptr\n");
    srand(time(NULL));

    int iColor;
    for(int i = 0; i < nProcesses; i++)
    {
        iColor = rand() % nGroups;
        colors[iColor] += 1;
    }
    qsort(colors, nGroups, sizeof(int), compare);

    while(colors[0] == 0)
    {
        colors[0] = colors[nGroups - 1] / 2;
        if(colors[nGroups - 1] % 2 == 0)
        {
            colors[nGroups - 1] /= 2;
        }
        else
        {
            colors[nGroups - 1] = (colors[nGroups - 1] / 2) + 1;
        }

        qsort(colors, nGroups, sizeof(int), compare);
    }
}
