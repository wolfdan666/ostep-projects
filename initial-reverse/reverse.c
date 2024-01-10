// 确定思路后，感觉主要是查api，对自己提升不大，直接把 README 丢进 ChatGPT,看思路是否正确。
// 看了GPT的答案，发现自己对 README 中说的输入很大有误解，
// 1. 因为README 后面说了可以malloc，自己理解大到了超过内存...问题不大。就用malloc版。
// 2. README 提到了用链表，GPT想到了头插法，很不错
// 以后可以考虑简单功能让GPT快速试错，然后大功能自己把握，这样提升效率，一般面试也是面算法题，
// 所以这种工程学习可以这样提高效率。 -- 自己针对GPT做修改就好。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct Node {
    char *line;
    struct Node *next;
} Node;

// Function prototypes
void reverseAndPrint(Node *head, FILE* out);
void freeList(Node *head);
// Function to get the inode number of a file
int getInode(const char *filename);

int main(int argc, char *argv[]) {
    FILE *inputFile, *outputFile;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    Node *head = NULL;

    // Check the number of command-line arguments
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Open input file or use stdin if no input file is provided
    if (argc >= 2) {
        inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    } else {
        inputFile = stdin;
    }

    // early check to aviod some process.
    // Check if the input file and output file are the same using inode numbers
    if (argc == 3) {
        int inode1 = getInode(argv[1]);
        int inode2 = getInode(argv[2]);
        if (inode1 != -1 && inode2 != -1 && inode1 == inode2) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
        }
    }

    // Read input file and store each line in a linked list
    while ((read = getline(&line, &len, inputFile)) != -1) {
        Node *newNode = (Node *)malloc(sizeof(Node));
        if (newNode == NULL) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }

        newNode->line = line;
        newNode->next = head;
        head = newNode;

        line = NULL; // Reset line pointer for the next iteration
    }

    // Close input file if it is not stdin
    if (inputFile != stdin) {
        fclose(inputFile);
    }

    // If the user specified an output file, open it; otherwise, use stdout
    if (argc == 3) {
        outputFile = fopen(argv[2], "w");
        if (outputFile == NULL) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            freeList(head);
            exit(1);
        }
    } else {
        outputFile = stdout;
    }

    // Reverse the linked list and print the result
    // GPT haven't the outputFile args, add it.
    reverseAndPrint(head, outputFile);

    // Close output file if it is not stdout
    if (outputFile != stdout) {
        fclose(outputFile);
    }

    // Free allocated memory
    freeList(head);

    return 0;
}

// Function to reverse the linked list and print the result
void reverseAndPrint(Node *head, FILE* out) {
    Node *current = head;
    while (current != NULL) {
        fprintf(out, "%s", current->line);
        current = current->next;
    }
}

// Function to free the allocated memory for the linked list
void freeList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->line);
        free(current);
        current = next;
    }
}

// Function to get the inode number of a file
int getInode(const char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) == -1) {
        fprintf(stderr, "reverse: cannot open file '%s'\n", filename);
        // perror("stat");
        return -1;
    }
    return fileStat.st_ino;
}

/* 
❯ gcc -o reverse reverse.c -Wall
❯ bash -x test-reverse.sh

发现GPT有很多err的输出没有好好读 README...问题不大。

17:39 发现示例5不能只判断文件名，要硬链接判断:
input and output are actually the same file (hardlinked)

修改完还要记得清空`tests-out/6.err` 文件，否则脚本没有自动清空。

[root@us-arm initial-reverse]# bash -x test-reverse.sh
+ [[ -x reverse ]]
+ ../tester/run-tests.sh
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed
test 7: passed
 */