#ifndef MATRIX_H
#define MATRIX_H


int* NewMatrix(const int rowNum, const int colNum);

void FreeMatrix(int** matrix, const int rowNum);

void MatrixFilling(int* matrix, const int rowNum, const int colNum, const int val);

void PrintMatrix(const int* matrix, const int rowNum, const int colNum);

void MatrixMultiplication(const int *matrixA, const int *matrixB, int *matrixC, const int rowsA, const int columnsA, const int columnsB);


#endif // MATRIX_H
