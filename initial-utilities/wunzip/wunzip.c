#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    FILE *stream;

    // for (int i = 0; i < argc; i++) {
    //     printf("lm_debug: argv[%d]: %s\n", i, argv[i]);
    // }
    if (argc == 1) {
        puts("wunzip: file1 [file2 ...]");
        exit(EXIT_FAILURE);
    }

    int cnt = 0;
    char cur_c = '\0';
    for (int i = 1; i < argc; i++) {
        stream = fopen(argv[i], "r");
        if (stream == NULL) {
            puts("wunzip: cannot open file");
            exit(EXIT_FAILURE);
        }

        while ((fread(&cnt, sizeof(cnt), 1, stream)) &&
               fread(&cur_c, sizeof(cur_c), 1, stream)) {
            // printf("lm_debug: cnt:%d cur_c:%c\n", cnt, cur_c);
            // 系统会进行 batch write, 不用担心，因为没有 fsync, fclose。
            for (int i = 0; i < cnt; i++) {
                if (fwrite(&cur_c, sizeof(cur_c), 1, stdout) < 1) {
                    perror("fwrite error.");
                    exit(EXIT_FAILURE);
                }
            }
            // 下面是把 cur_c 地址开始的 cnt*sizeof(char) 个字节写到了 stdout
            // if (fwrite(&cur_c, sizeof(cur_c), cnt, stdout) < cnt) {
            //     perror("fwrite error.");
            //     exit(EXIT_FAILURE);
            // }
        }
    }

    fclose(stream);
    exit(EXIT_SUCCESS);
}

/* 
❯ gcc -o wunzip wunzip.c -Wall
❯ bash -x test-wunzip.sh
+ [[ -x wunzip ]]
+ ../../tester/run-tests.sh
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed
 */