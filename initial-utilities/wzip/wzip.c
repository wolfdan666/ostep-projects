/*
// can't limit cnt,pre_c in one line! 
// because in that case: 1. multi file need write tmp file 
// 2. multi line `'\n'` will not compress in one line!

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* tmpFile = "tmpFile";

int main(int argc, char *argv[]) {
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // for (int i = 0; i < argc; i++) {
    //     printf("lm_debug: argv[%d]: %s\n", i, argv[i]);
    // }
    if (argc == 1 && strcmp(argv[0], "./wzip") == 0) {
        puts("wzip: file1 [file2 ...]");
        exit(EXIT_FAILURE);
    }

    // for multi file, write to a tmp file firstly.
    if (argc > 2) {
        FILE * tmpStream = fopen(tmpFile, "w");
        for (int i = 1; i < argc; i++) {
            stream = fopen(argv[i], "r");
            if (stream == NULL) {
                puts("wzip: cannot open file");
                exit(EXIT_FAILURE);
            }

            while ((nread = getline(&line, &len, stream)) != -1) {
                fwrite(line, nread, 1, tmpStream);
            }
        }
        // must close to persist
        fclose(tmpStream);
    }

    stream = fopen(argc == 2 ? argv[1] : tmpFile, "r");
    if (stream == NULL) {
        puts("wzip: cannot open file");
        exit(EXIT_FAILURE);
    }

    while ((nread = getline(&line, &len, stream)) != -1) {
        if (nread == 0) {
            puts("lm_debug: read size 0!");
            continue;
        }
        // can't limit cnt,pre_c in one line! 
        // because in that case: 1. multi file need write tmp file 
        // 2. multi line `'\n'` will not compress in one line!
        int cnt = 1;
        char pre_c = line[0];
        // i == nread , line[nread] = '\0', will write finish
        for (int i = 1; i <= nread; i++) {
            if (line[i] == pre_c) {
                cnt++;
                continue;
            }
            fwrite(&cnt, sizeof(cnt), 1, stdout);
            fwrite(&pre_c, sizeof(pre_c), 1, stdout);
            cnt = 1;
            pre_c = line[i];
        }
    }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}
*/

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
    if (argc == 1) {
        puts("wzip: file1 [file2 ...]");
        exit(EXIT_FAILURE);
    }

    // can't limit cnt,pre_c in one line!
    // because in that case: 1. multi file need write tmp file
    // 2. multi line `'\n'` will not compress in one line!
    int cnt = 0;
    char pre_c = '\0';
    for (int i = 1; i < argc; i++) {
        stream = fopen(argv[i], "r");
        if (stream == NULL) {
            puts("wzip: cannot open file");
            exit(EXIT_FAILURE);
        }

        while ((nread = getline(&line, &len, stream)) != -1) {
            // can't limit cnt,pre_c in one line!
            // because in that case: 1. multi file need write tmp file
            // 2. multi line `'\n'` will not compress in one line!
            for (int i = 0; i < nread; i++) {
                if (line[i] == pre_c) {
                    cnt++;
                    continue;
                }
                // 这里也很重要，防止一开始就写
                if (pre_c != '\0') {
                    fwrite(&cnt, sizeof(cnt), 1, stdout);
                    fwrite(&pre_c, sizeof(pre_c), 1, stdout);
                }
                cnt = 1;
                pre_c = line[i];
            }
        }
    }
    fwrite(&cnt, sizeof(cnt), 1, stdout);
    fwrite(&pre_c, sizeof(pre_c), 1, stdout);

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}

/* 
// 因为测试6过不了，然后找不到问题，于是看了一下别人的答案
// other's ans
// 1. 别人用的多文件全局的 cnt 和 pre_c 
// 2. 别人用的是 fread 读一个字符，而不是 getline
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("wzip: file1 [file2 ...]\n");
        exit(EXIT_FAILURE);
    }

    FILE *file;
    char character[2] = "";
    char previous_character[2] = "";
    int char_occurrences = 0;

    for (int i = 1; i < argc; i++) {
        char *file_name = argv[i];
        if ((file = fopen(file_name, "r")) == NULL) {
            perror("wzip: cannot open file");
            exit(EXIT_FAILURE);
        }

        while (fread(&character, 1, 1, file)) {
            if (strcmp(character, previous_character) == 0) {
                char_occurrences++;
            } else {
                if (previous_character[0] != '\0') {
                    fwrite(&char_occurrences, 4, 1, stdout);
                    fwrite(previous_character, 1, 1, stdout);
                }
                char_occurrences = 1;
                strcpy(previous_character, character);
            }
        }
        fclose(file);
    }

    fwrite(&char_occurrences, 4, 1, stdout);
    fwrite(previous_character, 1, 1, stdout);
}
*/
/* 
❯ gcc -o wzip wzip.c -Wall
❯ bash -x test-wzip.sh
+ [[ -x wzip ]]
+ ../../tester/run-tests.sh
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed

就是不能把 cnt 和 pre_c 限制在同一行..
可以用 getline的...
 */