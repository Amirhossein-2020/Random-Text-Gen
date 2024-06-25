#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

// Global variables to track terminators and end-of-file status
char terminator_seen = 0;
int EOF_seen = 0;

// Function to check if a character is alphanumeric and optionally convert case
int is_AlphaNumeric(char* ch, int toLow, int toUp) {
    // Check for uppercase English letter
    if(*ch >= 0x41 && *ch <= 0x5A) {
        // Convert to lowercase if toLow is set
        if(toLow)
            *ch += 32;
        return 1;
    }
    // Check for lowercase English letter
    if(*ch >= 0x61 && *ch <= 0x7A) {
        // Convert to uppercase if toUp is set
        if(toUp)
            *ch -= 32;
        return 1;
    }
    // Check for numeric characters
    if(*ch >= 0x30 && *ch <= 0x39) {
        return 1;
    }
    return 0;
}

// Function to check if a character is a sentence terminator
int is_TERMINATOR(char ch) {
    switch (ch) {
        case '.':
            return '.';
        case '?':
            return '?';
        case '!':
            return '!';
        default:
            return 0;
    }
}

// Function to read a word from a file
int getWord(FILE* fp, char* buffer, const size_t buffSize) {
    // Maximum readable word length
    int wordSize = 30;
    char ch;
    int i = 0;
    int non_ascii_firstB_seen = 0;

    // If a terminator was seen previously, add it to the buffer
    if(terminator_seen > 0) {
        buffer[i++] = terminator_seen;
        buffer[i++] = '\0';
        terminator_seen = 0;
        return 0;
    }

    // Read characters from the file
    while((i < wordSize) && (i < buffSize)) {
        ch = getc(fp);
        if(ch == EOF) {
            if(i > 0) {
                EOF_seen = 1;
                printf("%c\n", buffer[i]);
                buffer[i++] = '\0';
            } else
                return EOF;
        }
        // Check for multibyte characters
        if(ch & 128) {
            if(non_ascii_firstB_seen) {
                non_ascii_firstB_seen = 0;
                // Convert to lowercase if uppercase
                if(!(ch & 32))
                    ch += 32;
                // Exclude certain multibyte characters
                if((((unsigned char)ch) == 0xAB) || (((unsigned char)ch) == 0xBB)) {
                    wordSize--;
                    i--;
                    if(i > 0)
                         break;
                    else {
                        continue;
                    }
                }
            } else {
                non_ascii_firstB_seen = 1;
                // Increase maximum readable size
                ++wordSize;
            }
            buffer[i++] = ch;
        } else if(is_AlphaNumeric(&ch, 1, 0)) {
            buffer[i++] = ch;
        } else if(ch == '\'') {
            buffer[i++] = ch;
            if(i > 1)
                break;
        } else if(is_TERMINATOR(ch)) {
            if(i > 0) {
                terminator_seen = ch;
            } else {
                buffer[i++] = ch;
                buffer[i++] = '\0';
                return 0;
            }
            break;
        // If any other character type is seen, stop reading new characters
        } else if (i > 0) {
            break;
        }
    }

    buffer[i++] = '\0';
    return 0;
}

// Function to read a row from a file
int getRow(FILE* in_fp, char** line) {
    size_t len = 0;
    __ssize_t nchar;
    // Read until a newline character is encountered
    nchar = getdelim(line, &len, '\n', in_fp);
    return nchar;
}
