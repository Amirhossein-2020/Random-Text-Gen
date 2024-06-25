#ifndef OPENER_H
#define OPENER_H

/*
* Function to open a file in read mode
* Parameters:
*   - in_path: Path to the input file
* Returns:
*   - A pointer to the opened file
*   - If the file cannot be opened, the program prints an error message and exits
*/
FILE* fopener_read(const char* in_path);

/*
* Function to create or open a file in write mode
* Parameters:
*   - f_name: Name of the file to open or create
* Returns:
*   - A pointer to the opened file in append mode
*   - If the file cannot be opened, the program prints an error message and exits
* Note:
*   - This function first opens the file in write mode to clear its contents, then reopens it in append mode
*/
FILE* fopener_write(char* f_name);

#endif // OPENER_H
