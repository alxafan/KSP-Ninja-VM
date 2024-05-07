//
// Created by aafn on 15.04.24.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))  // 0x00800000 enstpricht einer 1 beim 24. Bit    Muss bei negativen Zahlen durchgeführt werden, damit bei Berechnungen das Vorzeichen beachtet wird, sonst wird es als positive Zahl betrachtet

unsigned int *stack;
unsigned int *pcode;
int *sda;
int staticsCount;
int instrCount;
int sp = 0;
int fp = 0;
int halt = 0;
int pc = 0;

int version = 3;


void execute(unsigned int IR) {
    switch(IR>>24) {
    case 0:
        halt = 1;
        break;
    case 1:
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
    case 11:
        stack[sp++] = sda[SIGN_EXTEND(IMMEDIATE(IR))];
        break;
    case 12:
        sda[SIGN_EXTEND(IMMEDIATE(IR))] = stack[--sp];
        stack[sp] = 0;
        break;
    case 13:
        stack[sp++] = fp;
        fp = sp;
        sp = sp + SIGN_EXTEND(IMMEDIATE(IR));
        break;
    case 14:
        sp = fp;
        fp = stack[--sp];
        break;
    case 15:
        stack[sp++] = stack[fp + SIGN_EXTEND(IMMEDIATE(IR))];
        break;
    case 16:
        stack[fp + SIGN_EXTEND(IMMEDIATE(IR))] = stack[--sp];
        stack[sp] = 0;
        break;
    case 17:
        sp--;
        if (stack[sp-1] == stack[sp]) stack[sp-1] = 1;
        else stack[sp-1] = 0;
        stack[sp] = 0;
        break;
    case 18:
        sp--;
        if (stack[sp-1] != stack[sp]) stack[sp-1] = 1;
        else stack[sp-1] = 0;
        stack[sp] = 0;
        break;
    case 19:
        sp--;
        if (stack[sp-1] < stack[sp]) stack[sp-1] = 1;
        else stack[sp-1] = 0;
        stack[sp] = 0;
        break;
    case 20:
        sp--;
        if (stack[sp-1] <= stack[sp]) stack[sp-1] = 1;
        else stack[sp-1] = 0;
        stack[sp] = 0;
        break;
    case 21:
        sp--;
        if (stack[sp-1] > stack[sp]) stack[sp-1] = 1;
        else stack[sp-1] = 0;
        stack[sp] = 0;
        break;
    case 22:
        sp--;
        if (stack[sp-1] >= stack[sp]) stack[sp-1] = 1;
        else stack[sp-1] = 0;
        stack[sp] = 0;
        break;
    case 23:
        pc = SIGN_EXTEND(IMMEDIATE(IR));
        break;
    case 24:
        sp--;
        if (stack[sp] == 0) pc = SIGN_EXTEND(IMMEDIATE(IR));
        stack[sp] = 0;
        break;
    case 25:
        sp--;
        if (stack[sp] == 1) pc = SIGN_EXTEND(IMMEDIATE(IR));
        stack[sp] = 0;
        break;
    }
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
    case 11:
        printf("pushg\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 12:
        printf("popg\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 13:
        printf("asf\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 14:
        printf("rsf");
        break;
    case 15:
        printf("pushl\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 16:
        printf("popl");
        break;
    case 17:
        printf("eq");
        break;
    case 18:
        printf("ne");
        break;
    case 19:
        printf("lt");
        break;
    case 20:
        printf("le");
        break;
    case 21:
        printf("gt");
        break;
    case 22:
        printf("ge");
        break;
    case 23:
        printf("jmp\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 24:
        printf("brf\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 25:
        printf("brt\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    }
    printf("\n");
}

void initializeProgram(char prog[]) {
    int fileVersion;
    
    FILE *fp;

    // open file
    fp = fopen(prog, "r");
    if (fp == NULL) {
        printf("%s could not be opened or is an invalid argument given, check the filename or see --help for valid arguments\n", prog);
        exit(-1);
    }

    // check for njbf tag in file
    char *fileformat = (char*) malloc(4);
    if (fileformat == NULL) {
        printf("memory could not be allocated\n");
        exit(-1);
    }
    if (fread(fileformat, 1, 4, fp) != 4) {
        printf("file contents could not be read properly\n");
        exit(-1);
    }

    if (fileformat[0] != 'N' || fileformat[1] != 'J' || fileformat[2] != 'B' || fileformat[3] != 'F') {
        printf("given fileformat is not correct, make sure to pass a ninja binary file as the argument\n");
        exit(-1);
    }
    free(fileformat);

    // check for correct version of njvm
    if (fread(&fileVersion, 4, 1, fp) != 1) {
        printf("file-version could not be read\n");
        exit(-1);
    }
    if (fileVersion != version) {
        printf("the file-version doesn't match the vm-version\n");
        exit(-1);
    }

    // determine number of instructions and global variables
    if (fread(&instrCount, 4, 1, fp) != 1) {
        printf("number of instructions could not be read\n");
        exit(-1);
    }
    if (fread(&staticsCount, 4, 1, fp) != 1) {
        printf("number of static variables could not be read\n");
        exit(-1);
    }

    // initialize the Static Data Area
    sda = (int*) malloc(staticsCount);

    // initialize the program code and read from file
    pcode = (unsigned int *) malloc(instrCount * 4);
    if (pcode == NULL) {
        printf("memory could not be allocated\n");
        exit(-1);
    }
    
    if(fread(pcode, 4, instrCount, fp) != instrCount) {
        printf("program-code could not be read\n");
        exit(-1);
    }

    // close file
    fclose(fp);

    printf("Ninja Virtual Machine started\n");
} 

int main(int argc, char *argv[]) {
    stack = (unsigned int*) malloc(10000);
    unsigned int IR;
    int debug = 0;

    if (strcmp(argv[1],"--debug") == 0 && argc == 3) debug = 1;
    else if (argc != 2) {
        printf("please provide a single command line argument see --help for valid arguments\n");
        exit(-1);
    }
    else if (strcmp(argv[1],"--version\n") == 0) {
        printf("Version: %d", version);
        exit(0);
    }
    else if (strcmp(argv[1],"--help") == 0) {
        printf("usage: ./Aufgabe3 [option] \n Options:\n [prog].bin \t to execute a ninja binary file \n--debug [prog].bin \t to execute a ninja binary file with the debugger \n--version \t show version and exit\n --help\t\tshow this help and exit\n");
        exit(0);
    }
    
    if (debug == 1) {
        int cmdAccepted = 0;
        char command[255];
        initializeProgram(argv[2]);

        while(!halt){
            printf("next instruction:\t");
            printInstruction((unsigned int) *(pcode+pc));
            printf("\n");

            do {
                cmdAccepted = 1;
                scanf("%s", command);

                if (strcmp(command, "show_stack") == 0) {
                    printf("stack: \n");
                    for (int i = 0; i < sp; i++) {
                        printf("%d\n", stack[i]);
                    }
                }
                else if (strcmp(command, "show_static") == 0) {
                    for (int i = 0; i < staticsCount; i++) {
                        printf("%d\n",sda[i]);
                    }
                }
                else if (strcmp(command, "list_program") == 0) {
                    for (int i = 0; i < instrCount-1; i++) {
                    printf("%03d:\t",i);
                    printInstruction((unsigned int) *(pcode+i));
                    }
                }
                else if (strcmp(command, "execute_next") == 0) {
                    IR = (unsigned int) *(pcode + pc);
                    pc = pc+1;
                    execute(IR);
                }
                else if (strcmp(command, "execute_until_eof") == 0){
                    while(!halt) {
                        IR = (unsigned int) *(pcode + pc);
                        pc = pc+1;
                        execute(IR);
                    }
                }
                else if (strcmp(command, "help") == 0){
                    printf("show_stack, show_static, list_program, execute_next, execute_until_eof, exit\n");
                }
                else if (strcmp(command, "exit") == 0){
                    halt = 1;
                }
                else {
                    printf("unknown command: %s. type \"help\" to show valid debugger commands\n", command);
                    cmdAccepted = 0;
                }
                printf("\n");
            } while (cmdAccepted == 0);
        }
    }
    else {
        initializeProgram(argv[1]);
        while(!halt) {
            IR = (unsigned int) *(pcode + pc);
            pc = pc+1;
            execute(IR);
        }
    }

    printf("Ninja Virtual Machine stopped\n");
    exit(0);
}