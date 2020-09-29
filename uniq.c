/*
 * Author: Duc Tran, Lebohang McCallum
 * File: uniq.c
 * Purpose: Count unique adjacent line. Similar to Linux uniq command.
 * Usage: 
 * uniq filename: will print unique line if there are two adjacent identical line.
 * Argument:
 * -c: print count of line for both unique and none unique.
 * -d: print only line that is repeated.
 * -i: ignore case in string.
 * Language:  C
 */

#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];

void
uniq(int fd, char *name, int cflag, int dflag, int iflag)
{
    //Allocate prev and curr memory
    char *prev = (char*) malloc(1024*sizeof(char));
    char *curr = (char*) malloc(1024*sizeof(char));

    //Initialize a null value at index 0
    prev[0] = 0;
    curr[0] = 0;
    int count = 0;
    int repeat_count = 1;
    int line_count = 0;
    int i, n;

    while((n = read(fd, buf, sizeof(buf))) > 0) {
        for (i = 0; i <= n; i++) {
            if ((buf[i] == '\n') || (buf[i] == '\0')) {
                curr[count] = '\n';
                curr[count+1] = 0;
                int compare = strcmp(prev, curr);
                
                if ((line_count != 0) && (compare != 0)) {
                    if (repeat_count > 0) {
                        if (cflag) {
                            printf(1, "%d ", repeat_count);
                        }
                        if (dflag) {
                            if (repeat_count > 1) {
                                printf(1, "%s", prev);
                            }
                        } else {
                            printf(1, "%s", prev);
                        }
                    }
                    repeat_count = 1;
                } else if ((line_count != 0) && (compare == 0)) {
                    repeat_count++;
                }
                if (buf[i] == '\0') {
                    if (repeat_count > 0) {
                        if (cflag) {
                            if (strcmp(curr, "\n") == 0) {
                                repeat_count--;
                            }
                            if (repeat_count > 0) {
                                printf(1, "%d ", repeat_count);
                            }
                        }
                        if (dflag) {
                            if (repeat_count > 1) {
                                printf(1, "%s", curr);
                            }
                        } else {
                            printf(1, "%s", curr);
                        }
                    }
                    break;
                    repeat_count = 1;
                }
                strcpy(prev, curr);
                line_count++;
                curr = (char*) malloc(20*sizeof(char));
                count = 0;
                } else {
                    if (iflag) {
                        int ascii = (int) buf[i];
                        if ((ascii >= 65) && (ascii <= 90)) {
                            ascii += 32;
                            curr[count] = (char) ascii;
                        } else {
                            curr[count] = buf[i];
                        }
                    } else {
                        curr[count] = buf[i];
                    }
                    count++;
                }
            }
        }
        if(n < 0){
        printf(1, "uniq: read error\n");
        exit();
    }
}

int
main(int argc, char *argv[])
{
    int fd, i;
    int cflag = 0;
    int dflag = 0;
    int iflag = 0;
   
    for(i = 1; i < argc; i++){
        char *arg = argv[i];
        if (strcmp(arg, "-c") == 0) {
            cflag = 1;
        } else if (strcmp(arg, "-d") == 0) {
            dflag = 1;
        } else if (strcmp(arg, "-i") == 0) {
            iflag = 1;
        }
    }

    if ((cflag == 1) && (dflag == 1)) {
        printf(1, "-c and -d will not appear at the same time.\n");
        exit();
    }

    if(argc <= 1){
        uniq(0, "", cflag, dflag, iflag);
        exit();
    }

    //check if argument piped so last arg is none or "-c" etc:
    if(strcmp(argv[argc-1], "-c") == 0 
        || strcmp(argv[argc-1], "-d") == 0
        || strcmp(argv[argc-1], "-i") == 0) {
        uniq(0, "", cflag, dflag, iflag);
        exit();
    }

    if((fd = open(argv[argc-1], 0)) < 0){
        printf(1, "uniq: cannot open %s\n", argv[argc-1]);
        exit();
    }

    uniq(fd, argv[argc-1], cflag, dflag, iflag);
    close(fd);
    exit();
}
