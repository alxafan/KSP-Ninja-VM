//
// Created by aafn on 15.04.24.
//
#include <string.h>
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

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))  // 0x00800000 enstpricht einer 1 beim 24. Bit    Muss bei negativen Zahlen durchgeführt werden, damit bei Berechnungen das Vorzeichen beachtet wird, sonst wird es als positive Zahl betrachtet

unsigned int stack[6];
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

void printInstruction(int IR) {
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
    unsigned int code1[] = {
        (PUSHC << 24) | IMMEDIATE(3),
        (PUSHC << 24) | IMMEDIATE(4),
        (ADD << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (PUSHC << 24) | IMMEDIATE(6),
        (SUB << 24),
        (MUL << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (WRCHR << 24),
        (HALT << 24)
    };
    unsigned int code2[] = {
        (PUSHC << 24) | IMMEDIATE(-2),
        (RDINT << 24),
        (MUL << 24),
        (PUSHC << 24) | IMMEDIATE(3),
        (ADD << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
    };
    unsigned int code3[] = {
        (RDCHR << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
    };
    unsigned int *pcode;

    {// command line arguments
    if (argc < 2) {
        printf("please provide a command line argument see --help for valid arguments");
        return -1;
    }

    if (strcmp(argv[1],"--prog1") == 0) pcode = code1;
    else if (strcmp(argv[1],"--prog2") == 0) pcode = code2;
    else if (strcmp(argv[1],"--prog3") == 0) pcode = code3;
    else if (strcmp(argv[1],"--version") == 0) {
        printf("Version 2.2");
        return 0;
    }
    else if (strcmp(argv[1],"--help") == 0) {
        printf("usage: ./Aufgabe1 [option] [option] ...\n--prog1\t\tselect program 1 to execute\n--prog2\t\tselect program 2 to execute\n--prog3\t\tselect program 3 to execute\n--version\tshow version and exit\n--help\t\tshow this help and exit\n");
        return 0;
    }
    else {
        printf("unknown command line argument, %s, try --help\n", argv[1]);
        return -2;
    }
    }
    
    // Determine the size of the selected code array
    int code_size;
    if (pcode == code1) {
        code_size = sizeof(code1) / sizeof(code1[0]);
    } else if (pcode == code2) {
        code_size = sizeof(code2) / sizeof(code2[0]);
    } else if (pcode == code3) {
        code_size = sizeof(code3) / sizeof(code3[0]);
    }

    printf("Ninja Virtual Machine started\n");

    unsigned int IR;
    int pc = 0;

    // Printing the assembled code
    for (int i = 0; i < code_size; i++) {
        printf("%03d:\t",i);
        printInstruction(pcode[i]);
        printf("\n");
    }

    while(!halt)
    {
        //Debugging
        //printf("%03d:\t",pc);
        IR = pcode[pc];
        pc = pc+1;
        execute(IR);
    }

    printf("Ninja Virtual Machine stopped\n");

    return 0;
}