#include "matrix.h"

#include <assert.h>
#include <stdlib.h>
#include <iostream>

int* NewMatrix(const int rowNum, const int colNum)
{
   /* int** matrix = new int*[rowNum];
    for(int iRow = 0; iRow < rowNum; iRow++)
    {
        matrix[iRow] = new int[colNum];
    }*/

    int* matrix = (int*)calloc(rowNum * colNum, sizeof(rowNum));

    return matrix;
}

void FreeMatrix(int **matrix, const int rowNum)
{
    assert(matrix && "matrix null pointer\n");

    for(int iRow = 0; iRow < rowNum; iRow++)
    {
        delete[] matrix[iRow];
    }
    delete[] matrix;
}

void MatrixFilling(int* matrix, const int rowNum, const int colNum, const int val)
{
    int v = 1;
    for(int i = 0; i < rowNum; i++)
    {
        for(int j = 0; j < colNum; j++)
        {
            matrix[i * colNum + j] = val;
        }
    }
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

