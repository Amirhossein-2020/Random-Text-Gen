#include <stdio.h>
#include <stdlib.h>
#include "opener.h"

// Function to open a file for reading
FILE* fopener_read(const char* in_path)
{
    // Attempt to open the file in read mode
    FILE* in_fp = fopen(in_path, "r");
    if(in_fp == NULL){
        // If opening fails, print an error message and exit
        printf("Input file opening error!\n");
        exit(EXIT_FAILURE);
    }
    // Return the file pointer if successful
    return in_fp;
}

// Function to open a file for writing
FILE* fopener_write(char* f_name)
{
    // Clear the file's previous contents by opening it in write mode
    FILE* fp = fopen(f_name, "w");
    if(fp == NULL){
        // If opening fails, print an error message and exit
        printf("Output file opening error!\n");
        exit(EXIT_FAILURE);
    }
    // Close the file to finalize the clearing process
    fclose(fp);

    // Reopen the file in append mode
    FILE* out_fp = fopen(f_name, "a");
    if(out_fp == NULL){
        // If reopening fails, print an error message and exit
        printf("Output file opening error!\n");
        exit(EXIT_FAILURE);
    }
    // Return the file pointer if successful
    return out_fp;
}
