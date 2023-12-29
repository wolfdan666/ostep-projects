/*
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 2048

int main(int argc, char** argv) {
    if (argc < 2) {
        puts("No args get. please ./wcat file_name_arg");
        return -1;
    }
    
    char* fname = argv[1];
    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        printf("open file: %s err, fd = %d\n", fname, fd);
        return -1;
    }

    char buf[MAX_BUF];
    // read a line err...
    // `man getline` and get a solution
    while (read(fd, buf, MAX_BUF)) {
        puts(buf);
    }

    return 0;
}
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    if (argc == 1 && strcmp(argv[0], "./wcat") == 0) {
        exit(EXIT_SUCCESS);
    }
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file> ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        stream = fopen(argv[i], "r");
        if (stream == NULL) {
            // perror("fopen");
            puts("wcat: cannot open file");
            exit(EXIT_FAILURE);
        }

        while ((nread = getline(&line, &len, stream)) != -1) {
            // printf("Retrieved line of length %zu:\n", nread);
            fwrite(line, nread, 1, stdout);
        }
    }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}

/* 

[root@us-arm wcat]# gcc -o wcat wcat.c -Wall 
[root@us-arm wcat]# bash -x test-wcat.sh 
 */