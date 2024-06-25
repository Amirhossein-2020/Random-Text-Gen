#ifndef SCANNER_H
#define SCANNER_H

// Header guard to prevent multiple inclusions
// Function prototypes

// Function to read a word from a file into the provided buffer
// Parameters:
// - in_fp: Pointer to the input file
// - buffer: Buffer to store the read word
// - buffSize: Size of the buffer
// Returns 0 on success, EOF if the end of the file is reached
int getWord(FILE* in_fp, char* buffer, const size_t buffSize);

// Function to check if a character is alphanumeric and optionally convert case
// Parameters:
// - ch: Pointer to the character to check
// - toLow: Convert to lowercase if set to 1
// - toUp: Convert to uppercase if set to 1
// Returns 1 if the character is alphanumeric, 0 otherwise
int is_AlphaNumeric(char* ch, int toLow, int toUp);

// Function to check if a character is a sentence terminator
// Parameters:
// - ch: Character to check
// Returns the terminator character if true, 0 otherwise
int is_TERMINATOR(char ch);

// Function to read a row from a file into the provided line buffer
// Parameters:
// - in_fp: Pointer to the input file
// - line: Pointer to the buffer to store the read row
// Returns the number of characters read, or -1 on failure
int getRow(FILE* in_fp, char** line);

#endif // SCANNER_H
