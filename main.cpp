#include <ctime>
#include <memory>
#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <mpich/mpi.h>
#include <sstream>

using namespace std;

#define TAG_COLOR       5

#define EMPTY_COLOR     500

#define GROUPS_NUM 2
#define MFILE_A "mA"
#define MFILE_B "mB"
#define MFILE_C "mC"

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

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

int* NewMatrix(const int rowNum, const int colNum)
{
    int* matrix = (int*)calloc(rowNum * colNum, sizeof(rowNum));

    return matrix;
}

void PrintMatrix(const int* matrix, const int rowNum, const int colNum)
{
    for(int i = 0; i < rowNum; i++)
    {
        for(int j = 0; j < colNum; j++)
        {
            std::cout << matrix[i * colNum + j] << " ";
        }
        std::cout << std::endl;
    }
}

void MatrixMultiplication(const int *matrixA, const int *matrixB, int *matrixC, const int rowsA, const int columnsA, const int columnsB)
{
    assert(matrixA && "matrixA null pointer\n");
    assert(matrixB && "matrixB null pointer\n");
    assert(matrixC && "matrixC null pointer\n");

    for(int i = 0; i < rowsA; i++)
    {
        for(int j = 0; j < columnsB; j++)
        {
            for(int k = 0; k < columnsA; k++)
            {
                matrixC[i * columnsB + j] += matrixA[i * columnsA + k] * matrixB[k * columnsB + j];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int mpiSize;
    MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);
    int mpiRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);

    if(GROUPS_NUM > mpiSize)
    {
        cerr << "ERR1" << endl;
        return 1;
    }

    int error;

    //Matirx a
    MPI_File fileMatrixA;
    MPI_File_open(MPI_COMM_WORLD, MFILE_A, MPI_MODE_RDONLY, MPI_INFO_NULL, &fileMatrixA);

    int rowsA;
    MPI_File_read_all(fileMatrixA, &rowsA, 1, MPI_INT, MPI_STATUSES_IGNORE);

    int columnsA;
    MPI_File_read_all(fileMatrixA, &columnsA, 1, MPI_INT, MPI_STATUSES_IGNORE);

    //Matrix B
    MPI_File fileMatrixB;
    MPI_File_open(MPI_COMM_WORLD, MFILE_B, MPI_MODE_RDONLY, MPI_INFO_NULL, &fileMatrixB);

    int rowsB;
    MPI_File_read_all(fileMatrixB, &rowsB, 1, MPI_INT, MPI_STATUS_IGNORE);

    int columnsB;
    MPI_File_read_all(fileMatrixB, &columnsB, 1, MPI_INT, MPI_STATUS_IGNORE);

    if(columnsA != rowsB)
    {
        cerr << "Size mismatch" << std::endl;
        MPI_Finalize();
        return 0;
    }


    int color = 0;
    MPI_Comm comm = MPI_COMM_WORLD;
    if(GROUPS_NUM > 1)
    {
        if(mpiRank == 0)
        {
            srand((unsigned int) time(NULL));

            int* colors = new int[GROUPS_NUM];
            bzero(colors, GROUPS_NUM * sizeof(int));

            //Radom split
            GetColorsForProcesses(colors, GROUPS_NUM, mpiSize);

            //Print the number of processes on the color
            PrintMatrix(colors,1, GROUPS_NUM);

            color = 0;
            colors[0]--;

            int iColor = 0;
            int sendCouter = 0;
            for(int i = 1; i < mpiSize; i++)
            {
                if(colors[iColor] == 0)
                {
                    iColor++;
                    sendCouter = 0;
                }

                int sendValue = iColor;
                if(sendCouter >= rowsA)
                {
                    sendValue = EMPTY_COLOR;
                }
                //send color for process
                MPI_Send(&sendValue, 1, MPI_INT, i, TAG_COLOR, MPI_COMM_WORLD);
                colors[iColor] -= 1;
                sendCouter++;
            }

            delete[] colors;
        }
        else
        {
            //recv color
            MPI_Recv(&color, 1, MPI_INT, 0, TAG_COLOR, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
        }


    }
    else
    {
        if(mpiRank >= rowsA)
        {
            color = EMPTY_COLOR;
        }
    }

    MPI_Comm_split(MPI_COMM_WORLD, color, mpiRank, &comm);

    if(color != EMPTY_COLOR)
    {
        int rank;
        MPI_Comm_rank(comm, &rank);
        int size;
        MPI_Comm_size(comm, &size);

        // cout << "Rank: " << rank << endl;

        const int procRowsA = rowsA / size;
        int firstOffset = procRowsA + rowsA % size;

        const int rowsNum = (rank == 0) ? firstOffset : procRowsA;

        int* mA;
        mA = NewMatrix(rowsNum, columnsA);
        int* mB;
        mB = NewMatrix(rowsB, columnsB);
        int* mC;
        mC = NewMatrix(rowsNum, columnsB);

        //Matix A
        const int fileOffset = rowsNum * columnsA * rank + (firstOffset - rowsNum) * columnsA + 2;
        error = MPI_File_seek(fileMatrixA, fileOffset * sizeof(int), MPI_SEEK_SET);
        MPIerror(error, rank);
        error = MPI_File_read_all(fileMatrixA, mA, rowsNum * columnsA, MPI_INT, MPI_STATUSES_IGNORE);
        MPIerror(error, rank);

        //Matrix B
        error = MPI_File_seek(fileMatrixB, sizeof(int) * 2, MPI_SEEK_SET);
        MPIerror(error, rank);
        error = MPI_File_read_all(fileMatrixB, mB, rowsB * columnsB, MPI_INT, MPI_STATUS_IGNORE);
        MPIerror(error, rank);

        double startTime;
        if(rank == 0)
        {
            startTime = MPI_Wtime();
        }

        MatrixMultiplication(mA, mB, mC, rowsNum, columnsA, columnsB);

        MPI_Barrier(comm);

        double endTime;
        if(rank == 0)
        {
            endTime = MPI_Wtime();
            std::cout << "Time: " << endTime - startTime
                      << "\tGroup of " << size << " processes" << std::endl;
        }

        std::ostringstream  stringStream;
        stringStream << MFILE_C ;//<< "_" << color;
        std::string name(stringStream.str());
        MPI_File fileMatrixC;

        MPI_File_open(comm, name.data(), MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fileMatrixC);
        if(rank == 0)
        {
            error = MPI_File_write(fileMatrixC, &rowsA, 1, MPI_INT, MPI_STATUS_IGNORE);
            MPIerror(error, rank);
            error = MPI_File_write(fileMatrixC, &columnsB, 1, MPI_INT, MPI_STATUS_IGNORE);
            MPIerror(error, rank);
        }

        const int offsetMatrixC = rowsNum * columnsB * rank + (firstOffset - rowsNum) * columnsB + 2;
        error = MPI_File_seek(fileMatrixC, offsetMatrixC * sizeof(int), MPI_SEEK_SET);
        MPIerror(error, rank);
        error = MPI_File_write_all(fileMatrixC, mC, rowsNum * columnsB, MPI_INT, MPI_STATUS_IGNORE);
        MPIerror(error, rank);

        MPI_File_close(&fileMatrixC);

        delete[] mA;
        delete[] mB;
        delete[] mC;
    }

    //    MPI_Barrier(comm);

    MPI_File_close(&fileMatrixA);
    MPI_File_close(&fileMatrixB);



    MPI_Finalize();

    return 0;
}


