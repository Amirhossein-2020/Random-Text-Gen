#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "AVLTree.h"
#include "scanner.h"


char* createKey(char* word){
    char* key = (char*) malloc(strlen(word)+1);
    if (!key) {
        fprintf(stderr, "Key memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return strcpy(key, word);
}

// Create a new main tree node
MainTreeNode* createMainNode(char *word) {
    MainTreeNode *node = (MainTreeNode*)malloc(sizeof(MainTreeNode));
    node->word = createKey(word);
    node->frequency = 1;
    node->totalFreq = 0;
    node->left = node->right = NULL;
    node->height = 1;
    node->subtree = NULL;
    return node;
}

// Create a new subtree node
SubTreeNode* createSubNode(MainTreeNode *mainNode) {
    SubTreeNode *node = (SubTreeNode*)malloc(sizeof(SubTreeNode));
    node->mainNode = mainNode;
    node->frequency = 1;
    node->cumulativeFreq = 0;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Get height of a main tree node
int heightMain(MainTreeNode *node) {
    return node ? node->height : 0;
}

// Get height of a subtree node
int heightSub(SubTreeNode *node) {
    return node ? node->height : 0;
}

// Get the maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Right rotate main tree node
MainTreeNode* rightRotateMain(MainTreeNode *y) {
    MainTreeNode *x = y->left;
    MainTreeNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(heightMain(y->left), heightMain(y->right)) + 1;
    x->height = max(heightMain(x->left), heightMain(x->right)) + 1;

    return x;
}

// Left rotate main tree node
MainTreeNode* leftRotateMain(MainTreeNode *x) {
    MainTreeNode *y = x->right;
    MainTreeNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(heightMain(x->left), heightMain(x->right)) + 1;
    y->height = max(heightMain(y->left), heightMain(y->right)) + 1;

    return y;
}

// Right rotate subtree node
SubTreeNode* rightRotateSub(SubTreeNode *y) {
    SubTreeNode *x = y->left;
    SubTreeNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(heightSub(y->left), heightSub(y->right)) + 1;
    x->height = max(heightSub(x->left), heightSub(x->right)) + 1;

    return x;
}

// Left rotate subtree node
SubTreeNode* leftRotateSub(SubTreeNode *x) {
    SubTreeNode *y = x->right;
    SubTreeNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(heightSub(x->left), heightSub(x->right)) + 1;
    y->height = max(heightSub(y->left), heightSub(y->right)) + 1;

    return y;
}

// Insert word into main tree
MainTreeNode* insertMainNode(MainTreeNode *node, char *word, MainTreeNode **insertedNode) {
    if (node == NULL) {
        *insertedNode = createMainNode(word);
        return *insertedNode;
    }

    if (strcmp(word, node->word) < 0)
        node->left = insertMainNode(node->left, word, insertedNode);
    else if (strcmp(word, node->word) > 0)
        node->right = insertMainNode(node->right, word, insertedNode);
    else {
        node->frequency++;
        *insertedNode = node;
        return node;
    }


    node->height = 1 + max(heightMain(node->left), heightMain(node->right));

    int balance = heightMain(node->left) - heightMain(node->right);

    if (balance > 1 && strcmp(word, node->left->word) < 0)
        return rightRotateMain(node);

    if (balance < -1 && strcmp(word, node->right->word) > 0)
        return leftRotateMain(node);

    if (balance > 1 && strcmp(word, node->left->word) > 0) {
        node->left = leftRotateMain(node->left);
        return rightRotateMain(node);
    }

    if (balance < -1 && strcmp(word, node->right->word) < 0) {
        node->right = rightRotateMain(node->right);
        return leftRotateMain(node);
    }

    return node;
}

const float Tolerance = 0.0000001;

// Compare two floats with a small Tolerance precision issue
int compareFloats(float a, float b) {
    if(a > b + Tolerance)
        return 1;
    else if(a + Tolerance < b)
        return -1;
    else
        return 0;
}

// Comparison function for SubTreeNodes by cumulative frequency
int compareByCumulativeFreq(SubTreeNode *a, SubTreeNode *b) {
    return compareFloats(a->cumulativeFreq, b->cumulativeFreq);
}

// Comparison function for SubTreeNodes by main node pointer
int compareByPointer(SubTreeNode *a, SubTreeNode *b) {
    if (a->mainNode < b->mainNode) 
        return -1;
    else if (a->mainNode > b->mainNode) 
        return 1;
    else
        return 0;
}

// Insert main node into subtree with a comparison function
SubTreeNode* insertSubNode(SubTreeNode *node, MainTreeNode *mainNode, float cumulativeFreq, int (*compare)(SubTreeNode*, SubTreeNode*)) {
    if (node == NULL) {
        SubTreeNode* inserted = createSubNode(mainNode);
        inserted->cumulativeFreq = cumulativeFreq;
        return inserted;
    }

    SubTreeNode tempNode;
    tempNode.mainNode = mainNode;
    tempNode.cumulativeFreq = cumulativeFreq;

    if (compare(&tempNode, node) < 0)
        node->left = insertSubNode(node->left, mainNode, cumulativeFreq, compare);
    else if (compare(&tempNode, node) > 0)
        node->right = insertSubNode(node->right, mainNode, cumulativeFreq, compare);
    else {
        node->frequency++;
        return node;
    }

    node->height = 1 + max(heightSub(node->left), heightSub(node->right));

    int balance = heightSub(node->left) - heightSub(node->right);

    if (balance > 1 && compare(&tempNode, node->left) < 0)
        return rightRotateSub(node);

    if (balance < -1 && compare(&tempNode, node->right) > 0)
        return leftRotateSub(node);

    if (balance > 1 && compare(&tempNode, node->left) > 0) {
        node->left = leftRotateSub(node->left);
        return rightRotateSub(node);
    }

    if (balance < -1 && compare(&tempNode, node->right) < 0) {
        node->right = rightRotateSub(node->right);
        return leftRotateSub(node);
    }

    return node;
}

// In-order traversal for printing the main tree and its subtrees
void printInOrder(MainTreeNode *root) {
    if (root != NULL) {
        printInOrder(root->left);
        
        printf("Main Tree Node: %s, Frequency: %d, Total frequncy: %f\n", root->word, root->frequency, root->totalFreq);
        printSubTree(root->subtree);
        
        printInOrder(root->right);
    }
}

// In-order traversal for printing the subtrees
void printSubTree(SubTreeNode *root) {
    if (root != NULL) {
        printSubTree(root->left);
        
        printf("  SubTree Node: %s, Frequency: %d, Cumulative Frequency: %f\n", root->mainNode->word, root->frequency, root->cumulativeFreq);
        
        printSubTree(root->right);
    }
}
// Free main tree
void freeMainTree(MainTreeNode *node) {
    if (node != NULL) {
        freeMainTree(node->left);
        freeMainTree(node->right);
        freeSubTree(node->subtree);
        free(node->word);
        free(node);
    }
}

// Free subtree
void freeSubTree(SubTreeNode *node) {
    if (node != NULL) {
        freeSubTree(node->left);
        freeSubTree(node->right);
        free(node);
    }
}
int float_no_rounding(char* freq, float frequency) {
    int i = 0;

    // Get the integer part
    int integer_part = (int)frequency;
    i += sprintf(freq + i, "%d", integer_part);
    if(!integer_part)
    {
        // Add the decimal point
        freq[i++] = '.';
        // Calculate and print the fractional part without rounding
        float fractional_part = fabsf(frequency - integer_part);
        for (int j = 0; j < 4; j++) {
            fractional_part *= 10;
            int digit = (int)fractional_part;
            freq[i++] = '0' + digit;
            fractional_part -= digit;
        }
    }

    // Null-terminate the string
    freq[i] = '\0';

    return 0;
}

// Helper function to write the main tree and its subtrees to the output file
void writeSubTree(SubTreeNode *root, int mainNodeFrequency, FILE* out_fp, int fd, int multiproc) {
    if (root != NULL) {
        writeSubTree(root->left, mainNodeFrequency, out_fp, fd, multiproc);

        float normalizedFrequency = (float)root->frequency / mainNodeFrequency;
        char freq[10];
        float_no_rounding(freq,  normalizedFrequency);
        if(multiproc == 1){
            char tmp[256];
            sprintf(tmp, ",%s,%s", root->mainNode->word, freq);
            size_t len_of_word =strlen(tmp)+1;
            write(fd,&len_of_word,sizeof(size_t));
            write(fd, tmp, len_of_word);
        } else {
            fprintf(out_fp, ",%s,%s", root->mainNode->word, freq);
        }
        
        writeSubTree(root->right, mainNodeFrequency, out_fp, fd, multiproc);
    }
}

// Function to generate the frequency table and write it to a CSV file
void generateFrequencyTable(MainTreeNode *root, FILE* out_fp, int fd, int multiproc) {
    if (root != NULL) {
        generateFrequencyTable(root->left, out_fp, fd, multiproc);

        if (multiproc == 1){
            size_t len_of_word = strlen(root->word)+1;
            write(fd,&len_of_word,sizeof(size_t));

            write(fd, root->word, len_of_word);
        } else {
            fprintf(out_fp, "%s", root->word);
        }

        writeSubTree(root->subtree, root->frequency, out_fp, fd, multiproc);
        if (multiproc == 1){
            size_t len_of_word = 2;
            write(fd,&len_of_word,sizeof(size_t));

            write(fd, "\n", len_of_word);
        } else {
            fprintf(out_fp, "\n");
        }

        generateFrequencyTable(root->right, out_fp, fd, multiproc);
    }

}


int PopulateTree(char* row, MainTreeNode** root, char* terminator){
    MainTreeNode* mainWordNode = NULL;
    MainTreeNode* subseqWordNode = NULL;

    float cumulativeFrequency = 0;

    char *token = strtok(row, ",");
    *root = insertMainNode(*root, token, &mainWordNode);
    if(is_TERMINATOR(token[0])){
        if(strchr(terminator, token[0]) == NULL)
            strcat(terminator, token);
    }
    while ((token = strtok(NULL, ",")) != NULL) {
        *root = insertMainNode(*root, token, &subseqWordNode);

        token = strtok(NULL, ",");
        if (!token) 
            break;

        if(is_TERMINATOR(token[0])){
            if(strchr(terminator, token[0]) == NULL)
                strcat(terminator, token);
        }
        cumulativeFrequency += (float) atof(token);

        mainWordNode->subtree = insertSubNode(mainWordNode->subtree, subseqWordNode, cumulativeFrequency, compareByCumulativeFreq);

    }
    mainWordNode->totalFreq = cumulativeFrequency;

    if(compareFloats(cumulativeFrequency, 1.0) == 1){
        return 1;
    }
    return 0;
}

MainTreeNode* searchMainTreeNode(MainTreeNode* root, const char* word) {
    if (root == NULL || strcmp(word, root->word) == 0) {
        return root;
    }

    if (strcmp(word, root->word) < 0) {
        return searchMainTreeNode(root->left, word);
    } else {
        return searchMainTreeNode(root->right, word);
    }
}

// Function to capitalize the first letter of a word
void capitalizeFirstLetter(char* buffer) {
        // Check if it's a multichar
        if(buffer[0]&128){
            // to lower
            buffer[1]-=32;
        } else {
            is_AlphaNumeric(&buffer[0], 0, 1);
        }
}

MainTreeNode* selectRandomNextNode(SubTreeNode* currentNode, float randValue){
    // If current node frequency == rand val --> return current
    if(compareFloats(currentNode->cumulativeFreq, randValue) == 0)
        return currentNode->mainNode;
    // If current node frequency > rand val & but there isn't a smaller node on the left --> return current
    if(compareFloats(currentNode->cumulativeFreq, randValue) > 0 && currentNode->left == NULL)
        return currentNode->mainNode;
    if(compareFloats(currentNode->cumulativeFreq, randValue) < 0 && currentNode->right == NULL)
        return currentNode->mainNode;
    // If current node frequency > rand val --> left
    if(compareFloats(currentNode->cumulativeFreq, randValue) > 0)
        return selectRandomNextNode(currentNode->left, randValue);
    // If current node frequency < rand val --> right
    if(compareFloats(currentNode->cumulativeFreq, randValue) < 0)
        return selectRandomNextNode(currentNode->right, randValue);
    return currentNode->mainNode;
}

int randomSenteceGen(MainTreeNode* root, int numWords, char* startWord, char* terminator,  FILE* out_fp, int fd, int multiproc){
    if (root == NULL) 
        return -1;

    srand(time(NULL));

    char buffer[62];
    MainTreeNode* firstNode = root;
    int terminatorSeen = 0;

    // Find the starting node in the main tree
    if (startWord != NULL) {
        firstNode = searchMainTreeNode(root, startWord);
        strcpy(buffer, firstNode->word);
        capitalizeFirstLetter(buffer);
        if(multiproc == 1){
            strcat(buffer, " ");
            size_t len_of_word = strlen(buffer) + 1;
            write(fd, &len_of_word,sizeof(size_t));
            write(fd, buffer, len_of_word);
        } else {
            fprintf(out_fp, "%s ", buffer);
        }
        numWords--;
    } 

    if(!firstNode | !startWord) {
        // Start with a random existing punctuation
        if(strlen(terminator) == 0){
            fprintf(stderr, "! . ? not found!\n");
            return -1;
        }
        
        char selectedTerm[2];
        selectedTerm[0] = terminator[rand() % strlen(terminator)];
        selectedTerm[1] = '\0';
        firstNode = searchMainTreeNode(root, selectedTerm);
    }

    for (int i = 0; i < numWords; i++) {
        if(is_TERMINATOR(firstNode->word[0]))
            terminatorSeen = 1;

        float randValue = (float)rand() / (float)RAND_MAX;
        firstNode = selectRandomNextNode(firstNode->subtree, randValue);
        strcpy(buffer, firstNode->word);
        if(terminatorSeen){
            capitalizeFirstLetter(buffer);
            if(multiproc == 1){
                strcat(buffer, " ");
                size_t len_of_word = strlen(buffer) + 1;
                write(fd, &len_of_word,sizeof(size_t));
                write(fd, buffer, len_of_word);
            } else {
                fprintf(out_fp, "%s ", buffer);
            }
            terminatorSeen = 0;

        } else {
            if(multiproc == 1){
                strcat(buffer, " ");
                size_t len_of_word = strlen(buffer) + 1;
                write(fd, &len_of_word,sizeof(size_t));
                write(fd, buffer, len_of_word);
            } else {
                fprintf(out_fp, "%s ", buffer);
            }
        }

    }
    return 0;
}