//
// Created by aafn on 15.04.24.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define HALT                   0
#define PUSHC                  1
#define ADD                    2
#define SUB                    3
#define MUL                    4
#define DIV                    5
#define MOD                    6
#define RDINT                  7
#define WRINT                  8
#define RDCHR                  9
#define WRCHR                  10
#define PUSHG                  11
#define POPG                   12
#define ASF                    13
#define RSF                    14
#define PUSHL                  15
#define POPL                   16

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))  // 0x00800000 enstpricht einer 1 beim 24. Bit    Muss bei negativen Zahlen durchgeführt werden, damit bei Berechnungen das Vorzeichen beachtet wird, sonst wird es als positive Zahl betrachtet

unsigned int stack[10000];
int sp = 0;
int halt = 0;

void execute(unsigned int IR) {
    switch(IR>>24) {
    case 0:
        halt = 1;
        break;
    case 1: // TODO: negative numbers
        stack[sp++] = SIGN_EXTEND(IMMEDIATE(IR));
        break;
    case 2:
        stack[sp-2] = stack[sp-2] + stack[sp-1];
        stack[sp-1] = 0;
        sp--;
        break;
    case 3:
        stack[sp-2] = stack[sp-2] - stack[sp-1];
        stack[sp-1] = 0;
        sp--;
        break;
    case 4:
        stack[sp-2] = stack[sp-2] * stack[sp-1];
        stack[sp-1] = 0;
        sp--;
        break;
    case 5:
        stack[sp-2] = stack[sp-2] / stack[sp-1];
        stack[sp-1] = 0;
        sp--;
        break;
    case 6:
        stack[sp-2] = stack[sp-2] % stack[sp-1];
        stack[sp-1] = 0;
        sp--;
        break;
    case 7:
        scanf("%d", (int *) &stack[sp++]);
        break;
    case 8:
        printf("%d", stack[--sp]);
        stack[sp] = 0;
        break;
    case 9:
        scanf("%c", (char *) &stack[sp++]);
        break;
    case 10:
        printf("%c", stack[--sp]);
        stack[sp] = 0;
        break;
    }

    // Debugging
    //for (int i = 0; i<10; i++) printf("%d ",stack[i]);
    //printf("\tsp: %d\n",sp);
}

void printInstruction(unsigned int IR) {
    switch(IR>>24) {
    case 0:
        printf("halt");
        break;
    case 1:
        printf("pushc\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 2:
        printf("add");
        break;
    case 3:
        printf("sub");
        break;
    case 4:
        printf("mul");
        break;
    case 5:
        printf("div");
        break;
    case 6:
        printf("mod");
        break;
    case 7:
        printf("rdint");
        break;
    case 8:
        printf("wrint");
        break;
    case 9:
        printf("rdchr");
        break;
    case 10:
        printf("wrchr");
        break;
    }
}

int main(int argc, char *argv[]) {
    int version = 2;
    unsigned int *pcode;
    int instrNum;
    int staticNum;
    int fileVersion;
    FILE *fp;

    if (argc != 2) {
        printf("please provide a single command line argument see --help for valid arguments\n");
        exit(-1);
    }
    else if (strcmp(argv[1],"--version\n") == 0) {
        printf("Version: %d", version);
        exit(0);
    }
    else if (strcmp(argv[1],"--help") == 0) {
        printf("usage: ./Aufgabe2 [option] \n Options:\n [prog].bin \t to execute a ninja binary file \n--version \t show version and exit\n --help\t\tshow this help and exit\n");
        exit(0);
    }
    else {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            printf("%s could not be opened or is an invalid argument given, check the filename or see --help for valid arguments\n", argv[1]);
            exit(-1);
        }

        char *fileformat = (char*) malloc(4);
        if (fileformat == NULL) {
            printf("memory could not be allocated\n");
            exit(-1);
        }
        if (fread(fileformat, 1, 4, fp) != 4) {
            printf("file contents could not be read properly\n");
            exit(-1);
        }

        // "Too much Voodoo"
        // taking the adress of fileformat pointer and adding +1 (C knows that it is char so it adds +4 in the background) to get to the next char
        if (*fileformat != 'N' || *(fileformat+1) != 'J' || *(fileformat+2) != 'B' || *(fileformat+3) != 'F') {
            printf("given fileformat is not correct, make sure to pass a ninja binary file as the argument\n");
            exit(-1);
        }
        free(fileformat);

        if (fread(&fileVersion, 4, 1, fp) != 1) {
            printf("file-version could not be read\n");
            exit(-1);
        }
        if (fileVersion != version) {
            printf("the file-version doesn't match the vm-version\n");
            exit(-1);
        }
        if (fread(&instrNum, 4, 1, fp) != 1) {
            printf("number of instructions could not be read\n");
            exit(-1);
        }

        //TODO: static variables
        if (fread(&staticNum, 4, 1, fp) != 1) {
            printf("number of static variables could not be read\n");
            exit(-1);
        }

        pcode = (unsigned int *) malloc(instrNum * 4);
        if (pcode == NULL) {
            printf("memory could not be allocated\n");
            exit(-1);
        }
        
        if(fread(pcode, 4, instrNum, fp) != instrNum) {
            printf("program-code could not be read\n");
            exit(-1);
        }
        
        fclose(fp);
    }

    printf("Ninja Virtual Machine started\n");

    unsigned int IR;
    int pc = 0;

    // Printing the assembled code
    for (int i = 0; i < instrNum-1; i++) {
        printf("%03d:\t",i);
        printInstruction((unsigned int) *(pcode+i));
        printf("\n");
    }

    while(!halt)
    {
        //Debugging
        //printf("%03d:\t",pc);
        IR = (unsigned int) *(pcode + pc);
        pc = pc+1;
        execute(IR);
    }
    
    printf("Ninja Virtual Machine stopped\n");

    exit(0);
}