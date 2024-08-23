#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

#define CHECK_ERR(err, msg)                           \
    if (err != CL_SUCCESS)                            \
    {                                                 \
        fprintf(stderr, "%s failed: %d\n", msg, err); \
        exit(EXIT_FAILURE);                           \
    }

void NaiveMatrixMultiply(Matrix *input0, Matrix *input1, Matrix *result)
{
    int blockSize;
    //@@ Insert code to implement naive matrix multiply here
    int row0 = input0->shape[0];
    int col0 = input0->shape[1];
    int row1 = input1->shape[0];
    int col1 = input1->shape[1];

     // Compute block size
    if( row0 < 4 |col1 < 4){
        blockSize = 2;
    }
    else{
        blockSize = 4;
    }

   for (int i = 0; i < row0; i++){
        for(int j = 0; j < col1; j++){

           // Compute Block
            for(int blockRow = i; blockRow < i+blockSize; blockRow+=blockSize){ //incrementing by block size (got segfault in top loops)
                for(int blockCol = j; blockCol < j+blockSize; blockCol+=blockSize){
                    for(int k = 0; k < row1; k+=4){
                        result->data[blockRow*col1 + blockCol] += input0->data[col0*blockRow + k]*input1->data[col1*k +blockCol];
                        result->data[blockRow*col1 + blockCol] += input0->data[col0*blockRow + k+1]*input1->data[col1*(k+1) +blockCol];
                        result->data[blockRow*col1 + blockCol] += input0->data[col0*blockRow + k+2]*input1->data[col1*(k+2) +blockCol];
                        result->data[blockRow*col1 + blockCol] += input0->data[col0*blockRow + k+3]*input1->data[col1*(k+3) +blockCol];
                    }
                }
            }

        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <input_file_0> <input_file_1> <answer_file> <output_file>\n", argv[0]);
        return -1;
    }

    const char *input_file_a = argv[1];
    const char *input_file_b = argv[2];
    const char *input_file_c = argv[3];
    const char *input_file_d = argv[4];

    // Host input and output vectors and sizes
    Matrix host_a, host_b, host_c, answer;
    
    cl_int err;

    err = LoadMatrix(input_file_a, &host_a);
    CHECK_ERR(err, "LoadMatrix");

    err = LoadMatrix(input_file_b, &host_b);
    CHECK_ERR(err, "LoadMatrix");

    err = LoadMatrix(input_file_c, &answer);
    CHECK_ERR(err, "LoadMatrix");

    int rows, cols;
    //@@ Update these values for the output rows and cols of the output
    //@@ Do not use the results from the answer matrix
    rows = host_a.shape[0];
    cols = host_b.shape[1];


    // Allocate the memory for the target.
    host_c.shape[0] = rows;
    host_c.shape[1] = cols;
    host_c.data = (float *)malloc(sizeof(float) * host_c.shape[0] * host_c.shape[1]);

    // Call your matrix multiply.
    NaiveMatrixMultiply(&host_a, &host_b, &host_c);

    // // Call to print the matrix
    // PrintMatrix(&host_c);

    // Check the result of the matrix multiply
    CheckMatrix(&answer, &host_c);

    // Save the matrix
    SaveMatrix(input_file_d, &host_c);

    // Release host memory
    free(host_a.data);
    free(host_b.data);
    free(host_c.data);
    free(answer.data);

    return 0;
}