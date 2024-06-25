#define _GNU_SOURCE // This is needed so that in the multiprocess version of second exercise we can set pipe size to a custom max size to store a line
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "scanner.h"
#include "AVLTree.h"
#include "opener.h"


void compito_1_multi(char* fInputName, char* fOutputName);
void compito_1_mono(char* fInputName, char* fOutputName);
void compito_2_mono(char* fInputName, char* fOutputName, int numWords, char* startWord);
void compito_2_multi(char* fInputName, char* fOutputName, int numWords, char* startWord);

int main(){
    char fInput[100];
    char fOutput[100];
    char firstWord[61];
    int multi;
    int num;

    // Set locale to the system's default locale
    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "Error: Unable to set locale.\n");
        return 1;
    }
    printf("Please select the exercise to execute[1: primo compito, 2: secondo compito]:\n");
    int num_compito;
    while (scanf("%d", &num_compito) == 1)    
    {
        if(num_compito == 1)
        {
            printf("Please enter the input text file name or absolute path:\n");

            if(scanf("%s", fInput) != 0){

                printf("Please enter the output file name with .csv at the end or absolute path:\n");

                if(scanf("%s", fOutput) != 0){

                    printf("Do you want to execute the multiprocess version[1: multiprocess, any other number: monoprocesso]:\n");

                    if(scanf("%d", &multi) != 0){

                        if(multi == 1){
                            compito_1_multi(fInput, fOutput);
                            break;
                        } else {
                            compito_1_mono(fInput, fOutput);
                            break;
                        }
                    } else {
                        printf("Wrong format!\n");
                    }
                } else {
                    printf("Wrong format!\n");
                }
            } else {
                printf("Wrong format!\n");
            }

        } else if (num_compito == 2) {
            printf("Please enter the input csv file name or absolute path:\n");

            if(scanf("%s", fInput) != 0){

                printf("Please enter the output text file name or absolute path:\n");

                if(scanf("%s", fOutput) != 0){

                    printf("How many words to generate:\n");

                    if(scanf("%d", &num) != 0){

                        printf("Please enter a word to start with(optional enter a single comma , for start randomly):\n");

                        if(scanf("%s", firstWord) != 0){

                            printf("Do you want to execute the multiprocess version[1: multiprocess, any other number: monoprocesso]:\n");

                            if(scanf("%d", &multi) != 0){
                                if(multi == 1){
                                    if(strcmp(firstWord, ",") == 0){
                                        compito_2_multi(fInput, fOutput, num, NULL);
                                    } else {
                                        compito_2_multi(fInput, fOutput, num, firstWord);
                                    }
                                    break;
                                } else {
                                    if(strcmp(firstWord, ",") == 0){
                                        compito_2_mono(fInput, fOutput, num, NULL);
                                    } else {

                                        compito_2_mono(fInput, fOutput, num, firstWord);
                                    }
                                    break;
                                }
                            } else {
                                printf("Wrong format!\n");
                            }
                        } else {
                            printf("Wrong format!\n");
                        }    
                    } else{
                    printf("Wrong Format!");
                    }
                } else {
                    printf("Wrong format!\n");
                }
            } else {
                printf("Wrong format!\n");
            }

        } 
        printf("Please Retry!\n\nPlease select the exercise to execute[1: primo compito, 2: secondo compito]:\n");
        continue;
    }

}
void compito_1_mono(char* fInputName, char* fOutputName)
{
    FILE* in_fp = fopener_read(fInputName);
    FILE* out_fp = fopener_write(fOutputName);
    char buffer[61];
    MainTreeNode* root = NULL;
    MainTreeNode* prevNode = NULL;
    MainTreeNode* currentNode = NULL;
    MainTreeNode* firstWordNode = NULL;

    while((getWord(in_fp, buffer, sizeof(buffer)) != EOF))
    {

        root = insertMainNode(root, buffer, &currentNode);
        if(!firstWordNode){
            firstWordNode = currentNode;
        }

        if (prevNode) {
            prevNode->subtree = insertSubNode(prevNode->subtree, currentNode, 0, compareByPointer);
        }

        prevNode = currentNode;
    }
    currentNode->subtree = insertSubNode(currentNode->subtree, firstWordNode, 0, compareByPointer);

    generateFrequencyTable(root, out_fp, 0, 0);

    // Free the trees recursively
    freeMainTree(root);

    // Close files
    fclose(in_fp);
    fclose(out_fp);

    exit(EXIT_SUCCESS);
}
void compito_1_multi(char* fInputName, char* fOutputName)
{
    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;
    
    if (pipe(pipefd1) == -1) {
        perror("pipe1");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == -1) {
        perror("fork1");
        exit(EXIT_FAILURE);
    }


    if (pid1 == 0) {    // Child process 1: Read words from text file and send to parent
        // Close unused read end
        close(pipefd1[0]);
        // Open the text file
        FILE* in_fp = fopener_read(fInputName);

        char word[61];
        size_t len_of_word = 0;

        while ((getWord(in_fp, word, sizeof(word)) != EOF)) {
            // Size of the word including /0
            len_of_word = strlen(word) + 1;
            // Send the size of the string
            if(write(pipefd1[1], &len_of_word, sizeof(size_t)) < 0){
                fprintf(stderr,"There was a problem trying to write to the pipe: %ld\n", len_of_word);
                exit(EXIT_FAILURE);
            }

            write(pipefd1[1], word, len_of_word);   // Send the string 
            
        }
        len_of_word = 62;   // Exit condition: WE WILL NEVER HAVE A WORD OF 62 chars

        write(pipefd1[1], &len_of_word, sizeof(size_t));    // SEND THE EXIT CONDITION

        close(pipefd1[1]);  // Close pipe after writing is done

        fclose(in_fp);
        exit(EXIT_SUCCESS);

    } else {    // Parent process
        close(pipefd1[1]);  // Close unused write end

        MainTreeNode* root = NULL;
        MainTreeNode* prevNode = NULL;
        MainTreeNode* currentNode = NULL;
        MainTreeNode* firstWordNode = NULL;

        char buffer[61];
        size_t word_len_read=0;  
        int status_code;

        while (word_len_read < 62){
            status_code = read(pipefd1[0], &word_len_read, sizeof(size_t));
            if(word_len_read >= 62){
                break;
            } else if((status_code = read(pipefd1[0], buffer, word_len_read)) < 0){
                fprintf(stderr,"There was a prolem when reading from he pipe last thing read:%s",buffer);
                exit(EXIT_FAILURE);
            } else {
                root = insertMainNode(root, buffer, &currentNode);
                if(!firstWordNode){
                    firstWordNode = currentNode;
                }

                if (prevNode) {
                    prevNode->subtree = insertSubNode(prevNode->subtree, currentNode, 0, compareByPointer);
                }

                prevNode = currentNode;
            }
        }
        if(status_code <= 0){
            fprintf(stderr,"There was a prolem when reading from the pipe!");
            perror("Read error1");
            exit(EXIT_FAILURE);
        } else {
            currentNode->subtree = insertSubNode(currentNode->subtree, firstWordNode, 0, compareByPointer);
        }

        close(pipefd1[0]);  // Close read end after done

        if (pipe(pipefd2) == -1) {
            perror("pipe2");
            exit(EXIT_FAILURE);
        }

        pid2 = fork();
        if (pid2 == -1) {
            perror("fork2");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {  // Child process 2: Write words to output file
            close(pipefd2[1]);  // Close unused write end
            FILE* out_fp = fopener_write(fOutputName);

            char buff[125];
            size_t csv_word_len_read = 0;
            while (csv_word_len_read < 125){

                status_code = read(pipefd2[0], &csv_word_len_read, sizeof(size_t));
                if(csv_word_len_read >= 125){
                    break;
                } else if((status_code = read(pipefd2[0], buff, csv_word_len_read)) < 0){
                    fprintf(stderr,"There was a prolem when reading from he pipe last thing read:%s",buffer);
                    exit(EXIT_FAILURE);
                } else {
                    fprintf(out_fp, "%s", buff);
                }
            }

            close(pipefd2[0]);  // Close read end after done
            fclose(out_fp);

            // Free the trees recursively
            freeMainTree(root);
            if(status_code <= 0){
                fprintf(stderr,"There was a prolem when reading from the pipe!");
                perror("Read error1");
                exit(EXIT_FAILURE);
            } else {
                exit(EXIT_SUCCESS);

            }
        } else {  // Parent process
            generateFrequencyTable(root, NULL, pipefd2[1], 1);

            size_t exit_num = 126;
            write(pipefd2[1],&exit_num,sizeof(size_t));
            // Free the trees recursively
            freeMainTree(root);
            close(pipefd2[1]);  // Close write end after done
            waitpid(pid2, NULL, 0);  // Wait for child process 2 to finish
            close(pipefd2[0]);  // Close unused read end
            
        }
    }
}

void compito_2_mono(char* fInputName, char* fOutputName, int numWords, char* startWord){
    FILE* in_fp = fopener_read(fInputName);
    FILE* out_fp = fopener_write(fOutputName);

    // While scaning the text add the punctuations seen in the text
    char terminator[4];
    terminator[0] = '\0';

    MainTreeNode* root = NULL;
    char* row = NULL;
    // Check if maximum frequency of any row has surpassed 1
    int surpassFreqRow = 0;
    // Value to show which row has surpassed the max frequency
    int rowNumb = 0;
    // Read a new line each time till find a surpassing frequency value
    while (getRow(in_fp, &row) != -1 && !surpassFreqRow){
        rowNumb++;
        // Populate the tree with the new row and keep track of cumulative frequency of it
        surpassFreqRow = PopulateTree(strtok(row, "\n"), &root, terminator);
        free(row);
    }
    // Generate random sentence if max frequncy is not surpassed
    if(!surpassFreqRow)
    {
        // printInOrder(root);
        randomSenteceGen(root, numWords, startWord, terminator, out_fp, 0, 0);
    }

    free(row);
    // Free the tree recursively
    freeMainTree(root);
    // CLose files
    fclose(in_fp);
    fclose(out_fp);
    // If max cumulative frequency has surpassed 1 then throw error and print the faulty row number
    if(surpassFreqRow){
        printf("Row %d, has cumulative frequency value > 1\n", rowNumb++);
        exit(EXIT_FAILURE);
    }
}

void compito_2_multi(char* fInputName, char* fOutputName, int numWords, char* startWord){
    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;

    if (pipe(pipefd1) == -1) {
        perror("pipe1");
        exit(EXIT_FAILURE);
    }

    int pipe_size;
    int max_line_size = 2500;

    // Set the new pipe size to maximum size of line
    pipe_size = fcntl(pipefd1[1], F_SETPIPE_SZ, max_line_size);
    if (pipe_size == -1) {
        perror("Fcntl error!");
        exit(EXIT_FAILURE);
    }

    // Create child process 1
    pid1 = fork();

    if (pid1 == -1) {
        perror("fork1");
        exit(EXIT_FAILURE);
    }


    if (pid1 == 0) {    // Child process 1: Read rows from csv file and send to parent

        // Close unused read end
        close(pipefd1[0]);
        // Open the text file
        FILE* in_fp = fopener_read(fInputName);
        
        char* row = NULL;
        size_t len_of_line = 0;

        // Read a new line each time
        while (getRow(in_fp, &row) != -1){

            // Size of the line including /0
            len_of_line = strlen(row) + 1;

            // Send the size of the string
            if(write(pipefd1[1], &len_of_line, sizeof(size_t)) < 0){
                fprintf(stderr,"There was a problem trying to write to the pipe: %ld\n", len_of_line);
                exit(EXIT_FAILURE);
            }

            write(pipefd1[1], row, len_of_line);   // Send the row

            free(row);
        }
        free(row);

        len_of_line = max_line_size + 1;   // Exit condition: WE WILL NEVER HAVE A LINE WITH MORE THAN 2500 CHARS

        write(pipefd1[1], &len_of_line, sizeof(size_t));    // SEND THE EXIT CONDITION

        close(pipefd1[1]);  // Close pipe after writing is done

        fclose(in_fp);
        exit(EXIT_SUCCESS);

    } else {    // Parent process
        close(pipefd1[1]);  // Close unused write end


        // While scaning the text add the unique punctuations seen in the text
        char terminator[4];
        terminator[0] = '\0';

        MainTreeNode* root = NULL;

        // Check if maximum frequency of any row has surpassed 1
        int surpassFreqRow = 0;

        // Value to show which row has surpassed the max frequency
        int rowNumb = 0;

        char buffer[max_line_size];
        size_t line_len_read = 0;  
        int status_code;

        while (line_len_read < max_line_size + 1){
            // Get line length
            status_code = read(pipefd1[0], &line_len_read, sizeof(size_t));
            // If greater than max
            if(line_len_read >= max_line_size + 1){
                break;
            // Read the Line itself and check for error
            } else if((status_code = read(pipefd1[0], buffer, line_len_read)) < 0){
                fprintf(stderr,"There was a prolem when reading from the pipe last line read:%d", rowNumb);
                exit(EXIT_FAILURE);
            } else {
                rowNumb++;
                // Populate the tree with the new row and keep track of cumulative frequency of it
                surpassFreqRow = PopulateTree(strtok(buffer, "\n"), &root, terminator);
            }
        }

        if(status_code <= 0){
            fprintf(stderr,"There was a prolem when reading from the pipe!");
            perror("Read error1");
            exit(EXIT_FAILURE);
        } else {
            // Generate random sentence if max frequncy is not surpassed
            if(surpassFreqRow)
            {
                fprintf(stderr,"Maximum cumulative frequency of %d line has surpassed 1!", rowNumb);
                perror("Read error1");
                exit(EXIT_FAILURE);
            }
        }

        close(pipefd1[0]);  // Close read end after done

        if (pipe(pipefd2) == -1) {
            perror("pipe2");
            exit(EXIT_FAILURE);
        }
        // Create child process 2
        pid2 = fork();
        if (pid2 == -1) {
            perror("fork2");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {  // Child process 2: Write words to output file
            close(pipefd2[1]);  // Close unused write end
            FILE* out_fp = fopener_write(fOutputName);

            char buff[62];
            size_t word_len_read = 0;
            while (word_len_read < 62){

                status_code = read(pipefd2[0], &word_len_read, sizeof(size_t));
                if(word_len_read >= 62){
                    break;
                } else if((status_code = read(pipefd2[0], buff, word_len_read)) < 0){
                    fprintf(stderr,"There was a prolem when reading from he pipe last thing read:%s", buffer);
                    exit(EXIT_FAILURE);
                } else {
                    fprintf(out_fp, "%s", buff);
                }
            }

            close(pipefd2[0]);  // Close read end after done
            fclose(out_fp);

            // Free the trees recursively
            freeMainTree(root);
            if(status_code <= 0){
                fprintf(stderr,"There was a problem when reading from the pipe!");
                perror("Read error1");
                exit(EXIT_FAILURE);
            } else {
                exit(EXIT_SUCCESS);

            }
        } else {  // Parent process
            close(pipefd2[0]);  // Close unused read end
            // Generate random sentence if max frequncy is not surpassed
            if(!surpassFreqRow)
            {
                randomSenteceGen(root, numWords, startWord, terminator, NULL, pipefd2[1], 1);
            }
        
            // printInOrder(root);
            size_t exit_num = 63;
            write(pipefd2[1], &exit_num, sizeof(size_t));
            // Free the trees recursively
            freeMainTree(root);
            close(pipefd2[1]);  // Close write end after done
            waitpid(pid2, NULL, 0);  // Wait for child process 2 to finish
            
        }
    }
}