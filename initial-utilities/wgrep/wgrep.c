#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // for (int i = 0; i < argc; i++) {
    //     printf("lm_debug: argv[%d]: %s\n", i, argv[i]);
    // }
    if (argc == 1 && strcmp(argv[0], "./wgrep") == 0) {
        puts("wgrep: searchterm [file ...]");
        exit(EXIT_FAILURE);
    }
    if (argc < 2) {
        fprintf(stderr, "Usage: %s searchterm [file ...] ...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *searchterm = argv[1];
    if (argc == 2) {
        // 借助 ChatGPT 找到识别管道的方法。
        // 先检查标准输入是否连接到终端（通过isatty函数和fileno(stdin)）。
        // 如果是，说明没有管道输入.
        if (isatty(fileno(stdin))) {
            fprintf(stderr, "Usage: %s searchterm [file ...]2 \n", argv[0]);
            exit(EXIT_FAILURE);
        }
        while ((nread = getline(&line, &len, stdin)) != -1) {
            if (strstr(line, searchterm) != NULL) {
                fwrite(line, nread, 1, stdout);
            }
        }
    }

    for (int i = 2; i < argc; i++) {
        stream = fopen(argv[i], "r");
        if (stream == NULL) {
            // perror("fopen");
            puts("wgrep: cannot open file");
            exit(EXIT_FAILURE);
        }

        while ((nread = getline(&line, &len, stream)) != -1) {
            if (strstr(line, searchterm) != NULL) {
                fwrite(line, nread, 1, stdout);
            }
        }
    }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}

/* 
❯ gcc -o wgrep wgrep.c -Wall
❯ bash -x test-wgrep.sh

# 管道获取是难点。
❯ cat tests/4.in | ./wgrep words
lm_debug: argv[0]: ./wgrep
lm_debug: argv[1]: words

通过 ChatGPT 知道了如何识别管道，然后成功 AC。
 */