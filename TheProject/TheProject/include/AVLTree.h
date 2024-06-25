#ifndef AVLTREE_H
#define AVLTREE_H

// Define the structure for nodes in the main AVL tree
typedef struct MainTreeNode {
    char *word; // The word stored in this node
    int frequency; // The frequency of the word
    float totalFreq; // The total frequency (may represent cumulative frequency or another metric)
    struct MainTreeNode *left; // Pointer to the left child
    struct MainTreeNode *right; // Pointer to the right child
    int height; // Height of the node in the tree
    struct SubTreeNode *subtree; // Pointer to the subtree
} MainTreeNode;

// Define the structure for nodes in the subtree
typedef struct SubTreeNode {
    MainTreeNode *mainNode; // Pointer to the corresponding main node
    int frequency; // Frequency of the word in the subtree context
    float cumulativeFreq; // Cumulative frequency in the subtree
    struct SubTreeNode *left; // Pointer to the left child
    struct SubTreeNode *right; // Pointer to the right child
    int height; // Height of the node in the subtree
} SubTreeNode;

// Function prototypes

// Create a key from a word (possibly for hashing or comparison purposes)
char* createKey(char* word);

// Create a new main tree node with the given word
MainTreeNode* createMainNode(char *word);

// Create a new subtree node linked to the given main node
SubTreeNode* createSubNode(MainTreeNode *mainNode);

// Get the height of a main tree node
int heightMain(MainTreeNode *node);

// Get the height of a subtree node
int heightSub(SubTreeNode *node);

// Return the maximum of two integers
int max(int a, int b);

// Perform a right rotation on the main tree node y and return the new root
MainTreeNode* rightRotateMain(MainTreeNode *y);

// Perform a left rotation on the main tree node x and return the new root
MainTreeNode* leftRotateMain(MainTreeNode *x);

// Perform a right rotation on the subtree node y and return the new root
SubTreeNode* rightRotateSub(SubTreeNode *y);

// Perform a left rotation on the subtree node x and return the new root
SubTreeNode* leftRotateSub(SubTreeNode *x);

// Insert a new word into the main tree, return the new root, and set the insertedNode pointer
MainTreeNode* insertMainNode(MainTreeNode *node, char *word, MainTreeNode **insertedNode);

// Compare two floats, return 0 if equal, -1 if a < b, 1 if a > b
int compareFloats(float a, float b);

// Compare two subtree nodes by their pointers
int compareByPointer(SubTreeNode *a, SubTreeNode *b);

// Compare two subtree nodes by their cumulative frequency
int compareByCumulativeFreq(SubTreeNode *a, SubTreeNode *b);

// Insert a main tree node into a subtree, return the new root of the subtree
SubTreeNode* insertSubNode(SubTreeNode *node, MainTreeNode *mainNode, float cumulativeFreq, int (*compare)(SubTreeNode*, SubTreeNode*));

// Generate a frequency table from the main tree and write to a file
void generateFrequencyTable(MainTreeNode *root, FILE* out_fp, int fd, int multiproc);

// Write the subtree to a file
void writeSubTree(SubTreeNode *root, int mainNodeFrequency, FILE* out_fp, int fd, int multiproc);

// Free all nodes in the main tree
void freeMainTree(MainTreeNode *node);

// Free all nodes in the subtree
void freeSubTree(SubTreeNode *node);

// Print the main tree in order
void printInOrder(MainTreeNode *root);

// Print the subtree in order
void printSubTree(SubTreeNode *root);

// Convert a frequency to a string without rounding
int float_no_rounding(char* freq, float frequency);

// Populate the tree with data from a row, update the root, and set a termination flag
int PopulateTree(char* row, MainTreeNode** root, char* termnatorSeen);

// Generate a random sentence from the tree
int randomSenteceGen(MainTreeNode* root, int numWords, char* startWord, char* termnatorSeen, FILE* out_fp, int fd, int multiproc);

// Select a random next node in the subtree based on a random value
MainTreeNode* selectRandomNextNode(SubTreeNode* currentNode, float randValue);

// Capitalize the first letter of a word
void capitalizeFirstLetter(char* word);

// Search for a main tree node by word
MainTreeNode* searchMainTreeNode(MainTreeNode* root, const char* word);

#endif
