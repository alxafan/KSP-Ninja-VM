//
// Created by aafn on 15.04.24.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <support.h>
#include <bigint.h>

#define intValue(s) (s.u.number)
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))  // 0x00800000 enstpricht einer 1 beim 24. Bit    Muss bei negativen Zahlen durchgeführt werden, damit bei Berechnungen das Vorzeichen beachtet wird, sonst wird es als positive Zahl betrachtet
#define STACK_SIZE 10000

#define MSB         (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) (((objRef)->size & MSB) == 0)

#define GET_SIZE(objRef) ((objRef)->size & ~MSB)

#define GET_REFS(objRef) ((ObjRef*)(objRef)->data)

int version = 7;

typedef struct {
  unsigned int size;    /* byte count of payload data */
  unsigned char data[1]; /* payload data, size as needed */
}*ObjRef;


typedef struct{
    int isObjRef;   /* slot used for object reference ? */
    union{
        ObjRef objRef;  /* used if isObjRef=1 */
        int number; /* used if isObjRef=0 */
    }u;
}StackSlot;


StackSlot stack[STACK_SIZE];
unsigned int *pcode;
ObjRef *sda;

int staticsCount;
int instrCount;

int halt = 0;

int sp = 0;
int pc = 0;
int fp = 0;
ObjRef rvr = NULL;


/*
 * This routine is called in case a fatal error has occurred.
 * It should print the error message and terminate the program.
 */
void fatalError(char *msg) {
  printf("Fatal error: %s\n", msg);
  exit(1);
}

/*
 * This function is called whenever a new primitive object with
 * a certain amount of internal memory is needed. It should return
 * an object reference to a regular object, which contains a freely
 * usable memory area of at least the requested size (measured in
 * bytes). The memory area need not be initialized in any way.
 *
 * Note that this function may move all objects in memory at will
 * (due to, e.g., garbage collection), as long as the pointers in
 * the global "bip" structure point to the correct objects when
 * the function returns.
 */
void * newPrimObject(int dataSize) {
  ObjRef bigObjRef;

  bigObjRef = malloc(sizeof(unsigned int) +
                  dataSize * sizeof(unsigned char));
  if (bigObjRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  bigObjRef->size = dataSize;
  return bigObjRef;
}

void * getPrimObjectDataPointer(void * obj){
    ObjRef oo = ((ObjRef)  (obj));
    return oo->data;
}

ObjRef newCompoundObject(int numOfRefs) {
    ObjRef compound;

    compound = malloc(sizeof(unsigned int) + numOfRefs * sizeof(ObjRef*));
    if (compound == NULL) {
    fatalError("newCompoundObject got no memory");
    }
    compound->size = numOfRefs | MSB;
    for (int i = 0; i < numOfRefs; i++) {
        GET_REFS(compound)[i] = NULL;
    }
    return compound;
}

void clearStackSlot(StackSlot *slot) {
    memset(slot, 0, sizeof(StackSlot));
}

void checkIfObject(StackSlot *slot) {
    if (slot->isObjRef == 0) fatalError("Object reference expected");
}

void execute(unsigned int IR) {
    switch(IR>>24) {
    case 0:     // HALT
        halt = 1;
        break;
    case 1:     // PUSHC
        stack[sp].isObjRef = 1;
        bigFromInt(SIGN_EXTEND(IMMEDIATE(IR)));
        stack[sp++].u.objRef = bip.res;
        break;
    case 2:     // ADD
        bip.op1 = stack[sp-2].u.objRef;
        bip.op2 = stack[sp-1].u.objRef;
        bigAdd();
        stack[sp-2].u.objRef = bip.res;
        clearStackSlot(&stack[sp-1]);
        sp--;
        break;
    case 3:     // SUB
        bip.op1 = stack[sp-2].u.objRef;
        bip.op2 = stack[sp-1].u.objRef;
        bigSub();
        stack[sp-2].u.objRef = bip.res;
        clearStackSlot(&stack[sp-1]);
        sp--;
        break;
    case 4:     // MUL
        bip.op1 = stack[sp-2].u.objRef;
        bip.op2 = stack[sp-1].u.objRef;
        bigMul();
        stack[sp-2].u.objRef = bip.res;
        clearStackSlot(&stack[sp-1]);
        sp--;
        break;
    case 5:     // DIV
        bip.op1 = stack[sp-2].u.objRef;
        bip.op2 = stack[sp-1].u.objRef;
        bigDiv();
        stack[sp-2].u.objRef = bip.res;
        clearStackSlot(&stack[sp-1]);
        sp--;
        break;
    case 6:     // MOD
        bip.op1 = stack[sp-2].u.objRef;
        bip.op2 = stack[sp-1].u.objRef;
        bigDiv();
        stack[sp-2].u.objRef = bip.rem;
        clearStackSlot(&stack[sp-1]);
        sp--;
        break;
    case 7:     // RDINT
        stack[sp].isObjRef = 1;
        bigRead(stdin);
        stack[sp++].u.objRef = bip.res;
        break;
    case 8:     // WRINT
        bip.op1 = stack[--sp].u.objRef;
        bigPrint(stdout);
        clearStackSlot(&stack[sp]);
        break;
    case 9:     // RDCHR
        stack[sp].isObjRef = 1;
        bigRead(stdin);
        stack[sp++].u.objRef = bip.res;
        break;
    case 10:    // WRCHR
        bip.op1 = stack[--sp].u.objRef;
        printf("%c", bigToInt());
        clearStackSlot(&stack[sp]);
        break;
    case 11:    // PUSHG
        stack[sp].isObjRef = 1;
        stack[sp].u.objRef = sda[SIGN_EXTEND(IMMEDIATE(IR))];
        sp++;
        break;
    case 12:   // POPG
        sp--;
        sda[SIGN_EXTEND(IMMEDIATE(IR))] = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);
        break;
    case 13:    // ASF
        stack[sp].isObjRef = 0;
        stack[sp++].u.number = fp;
        for (int i = 0; i < SIGN_EXTEND(IMMEDIATE(IR)); i++) stack[sp+i].u.objRef = NULL;
        fp = sp;
        sp = sp + SIGN_EXTEND(IMMEDIATE(IR));
        break;
    case 14:    // RSF
        sp = fp;
        fp = stack[--sp].u.number;
        break;
    case 15:    // PUSHL
        stack[sp].isObjRef = 1;
        stack[sp].u.objRef = stack[fp + SIGN_EXTEND(IMMEDIATE(IR))].u.objRef;
        sp++;
        break;
    case 16:    // POPL
        // broken????
        sp--;
        stack[fp + SIGN_EXTEND(IMMEDIATE(IR))].isObjRef = 1;
        stack[fp + SIGN_EXTEND(IMMEDIATE(IR))].u.objRef = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);
        break;
    case 17:    // EQ
        sp--;
        bip.op1 = stack[sp-1].u.objRef;
        bip.op2 = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);

        if (bigCmp() == 0) bigFromInt(1);
        else bigFromInt(0);
        stack[sp-1].u.objRef = bip.res;
        break;
    case 18:    // NE
        sp--;
        bip.op1 = stack[sp-1].u.objRef;
        bip.op2 = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);

        if (bigCmp() != 0) bigFromInt(1);
        else bigFromInt(0);
        stack[sp-1].u.objRef = bip.res;
        break;
    case 19:    // LT
        sp--;
        bip.op1 = stack[sp-1].u.objRef;
        bip.op2 = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);

        if (bigCmp() < 0) bigFromInt(1);
        else bigFromInt(0);
        stack[sp-1].u.objRef = bip.res;
        break;
    case 20:    // LE
        sp--;
        bip.op1 = stack[sp-1].u.objRef;
        bip.op2 = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);

        if (bigCmp() <= 0) bigFromInt(1);
        else bigFromInt(0);
        stack[sp-1].u.objRef = bip.res;
        break;
    case 21:    // GT
        sp--;
        bip.op1 = stack[sp-1].u.objRef;
        bip.op2 = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);

        if (bigCmp() > 0) bigFromInt(1);
        else bigFromInt(0);
        stack[sp-1].u.objRef = bip.res;
        break;
    case 22:    // GE
        sp--;
        bip.op1 = stack[sp-1].u.objRef;
        bip.op2 = stack[sp].u.objRef;
        clearStackSlot(&stack[sp]);

        if (bigCmp() >= 0) bigFromInt(1);
        else bigFromInt(0);
        stack[sp-1].u.objRef = bip.res;
        break;
    case 23:    // JMP
        pc = SIGN_EXTEND(IMMEDIATE(IR));
        break;
    case 24:    // BRF
        sp--;
        bip.op1 = stack[sp].u.objRef;
        if (bigToInt() == 0) pc = SIGN_EXTEND(IMMEDIATE(IR));
        clearStackSlot(&stack[sp]);
        break;
    case 25:    // BRT
        sp--;
        bip.op1 = stack[sp].u.objRef;
        if (bigToInt() == 1) pc = SIGN_EXTEND(IMMEDIATE(IR));
        clearStackSlot(&stack[sp]);
        break;
    case 26:    // CALL
        stack[sp].isObjRef = 0;
        stack[sp].u.number = pc;
        sp++;
        pc = SIGN_EXTEND(IMMEDIATE(IR));
        break;
    case 27:    // RET
        pc = stack[--sp].u.number;
        clearStackSlot(&stack[sp]);
        break;
    case 28:    // DROP
        for (int i = 0; i < SIGN_EXTEND(IMMEDIATE(IR)); i++) clearStackSlot(&stack[--sp]);
        break;
    case 29:    // PUSHR
        stack[sp].isObjRef = 1;
        stack[sp++].u.objRef = rvr;
        break;
    case 30:    // POPR
        rvr = stack[--sp].u.objRef;
        clearStackSlot(&stack[sp]);
        break;
    case 31:    // DUP
        stack[sp].isObjRef = 1;
        stack[sp].u.objRef = stack[sp-1].u.objRef;
        sp++;
        break;
    case 32:    // NEW


        //
        // TODO: CONSIDER UTILIZING IS_PRIM MORE
        //


        stack[sp].isObjRef = 1;
        stack[sp].u.objRef = newCompoundObject(SIGN_EXTEND(IMMEDIATE(IR)));
        sp++;
        break;
    case 33:    // GETF
        //TODO: check for inbounds
        stack[sp-1].u.objRef = GET_REFS(stack[sp-1].u.objRef)[SIGN_EXTEND(IMMEDIATE(IR))];
        break;
    case 34:    // PUTF
        GET_REFS(stack[sp-2].u.objRef)[SIGN_EXTEND(IMMEDIATE(IR))] = stack[sp-1].u.objRef;
        clearStackSlot(&stack[--sp]);
        clearStackSlot(&stack[--sp]);
        break;
    case 35:    // NEWA
        // number_elements is already objRef, so no need to set isObjRef
        bip.op1 = stack[sp-1].u.objRef;
        stack[sp-1].u.objRef = newCompoundObject(bigToInt());
        break;
    case 36:    // GETFA
        bip.op1 = stack[sp-1].u.objRef;
        if (GET_SIZE(stack[sp-2].u.objRef) < bigToInt()) fatalError("Index out of bounds");
        bip.op1 = stack[sp-1].u.objRef;
        stack[sp-2].u.objRef = GET_REFS(stack[sp-2].u.objRef)[bigToInt()];
        clearStackSlot(&stack[--sp]);
        break;
    case 37:    // PUTFA
        bip.op1 = stack[sp-2].u.objRef;
        if (GET_SIZE(stack[sp-3].u.objRef) < bigToInt()) fatalError("Index out of bounds");
        bip.op1 = stack[sp-2].u.objRef;
        GET_REFS(stack[sp-3].u.objRef)[bigToInt()] = stack[sp-1].u.objRef;
        clearStackSlot(&stack[--sp]);
        clearStackSlot(&stack[--sp]);
        clearStackSlot(&stack[--sp]);
        break;
    case 38:    // GETSZ
    //TODO: Consider making more of these tests for the case that someone writes in assembly?
        checkIfObject(&stack[sp-1]);
        if (IS_PRIM(stack[sp-1].u.objRef)) {
            bigFromInt(-1);
        }
        else {
            bigFromInt(GET_SIZE(stack[sp-1].u.objRef));
        }
        stack[sp-1].u.objRef = bip.res;
        break;
    case 39:    // PUSHN
        stack[sp].isObjRef = 1;
        stack[sp++].u.objRef = NULL;
        break;
    case 40:    // REFEQ
        if (stack[sp-2].u.objRef == stack[sp-1].u.objRef) {
            bigFromInt(1);
        } else {
            bigFromInt(0);
        }
        stack[sp-2].u.objRef = bip.res;
        clearStackSlot(&stack[--sp]);
        break;
    case 41:    // REFNE
        if (stack[sp-2].u.objRef != stack[sp-1].u.objRef) {
            bigFromInt(1);
        } else {
            bigFromInt(0);
        }
        stack[sp-2].u.objRef = bip.res;
        clearStackSlot(&stack[--sp]);
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
        printf("popl\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
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
    case 26:
        printf("call\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 27:
        printf("ret");
        break;
    case 28:
        printf("drop\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 29:
        printf("pushr");
        break;
    case 30:
        printf("popr");
        break;
    case 31:
        printf("dup");
        break;
    case 32:
        printf("new\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 33:
        printf("getf\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 34:
        printf("putf\t%d", SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case 35:
        printf("newa");
        break;
    case 36:
        printf("getfa");
        break;
    case 37:
        printf("putfa");
        break;
    case 38:
        printf("getsz");
        break;
    case 39:
        printf("pushn");
        break;
    case 40:
        printf("refeq");
        break;
    case 41:
        printf("refne");
        break;
    }
    printf("\n");
}

// Helper function to print a message with a border around it for the debugger
// Currently not used
void printWithBorder(const char *format, ...) {
    char buffer[256];
    va_list args;

    // Start variadic argument list
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    int length = strlen(buffer);
    int borderLength = length + 20; // 10 spaces padding on each side

    // Upper Border
    for (int i = 0; i < borderLength; i++) {
        printf("*");
    }
    printf("\n");

    // Message
    printf("*     %s     *\n", buffer);

    // Lower Border
    for (int i = 0; i < borderLength; i++) {
        printf("*");
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
    sda = (ObjRef*) malloc(staticsCount * sizeof(ObjRef));

    if (sda == NULL) {
        printf("memory could not be allocated\n");
        exit(-1);
    }

    // set all static data area entries to NULL
    for (int i = 0; i < staticsCount; i++) sda[i] = NULL;

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
    
    //TODO: fix printing of bigints 

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

                if (strcmp(command, "stack") == 0) {    // very jankey, but not a lot of effort for almost full functionality, maybe improve debugger later
                    for (int i = 0; i < sp; i++) {
                            if (stack[i].isObjRef == 1) {
                                if (stack[i].u.objRef == NULL) printf("NULL\n");
                                else {
                                    if (IS_PRIM(stack[i].u.objRef)) {
                                        printf("Object: %p, value:", (void *) stack[i].u.objRef);
                                        bigDump(stdout, stack[i].u.objRef);
                                        printf("\n");
                                    } else printf("Compound-Object: %p\n", (void *) stack[i].u.objRef);
                                }
                            }
                            else printf("%d\n", stack[i].u.number);
                      }
                }
                else if (strcmp(command, "pointer") == 0) {
                    printf("sp: %d\npc: %d\nfp: %d\nrvr:", sp, pc, fp);
                    if (rvr != NULL) {
                        bigDump(stdout, rvr);
                        printf("\n");
                    }
                    else printf("NULL\n");
                }
                else if (strcmp(command, "static") == 0) {
                    for (int i = 0; i < staticsCount; i++) {
                        if (sda[i] == NULL) printf("%d: Empty\n", i);
                        else printf("Object: %p, value:", (void *) sda[i]);
                        bigDump(stdout, sda[i]);
                        printf("\n");
                    }
                }
                else if (strcmp(command, "program") == 0) {
                    for (int i = 0; i < instrCount-1; i++) {
                    printf("%03d:\t",i);
                    printInstruction((unsigned int) *(pcode+i));
                    }
                }
                else if (strcmp(command, "next") == 0) {
                    IR = (unsigned int) *(pcode + pc);
                    pc = pc+1;
                    execute(IR);
                }
                else if (strcmp(command, "eof") == 0){  // copied printing from "stack"
                    while(!halt) {
                        IR = (unsigned int) *(pcode + pc);
                        pc = pc+1;
                        printf("--------------------\n");
                        printInstruction(IR);
                        printf("--------------------\n");
                        for (int i = 0; i < sp; i++) {
                            if (stack[i].isObjRef == 1) {
                                if (stack[i].u.objRef == NULL) printf("NULL\n");
                                else {
                                    if (IS_PRIM(stack[i].u.objRef)) {
                                        printf("Object: %p, value:", (void *) stack[i].u.objRef);
                                        bigDump(stdout, stack[i].u.objRef);
                                        printf("\n");
                                    } else printf("Compound-Object: %p\n", (void *) stack[i].u.objRef);
                                }
                            }
                            else printf("%d\n", stack[i].u.number);
                      }
                       execute(IR);
                    }
                }
                else if (strcmp(command, "help") == 0){
                    printf("stack, pointer, static, program, next, eof, exit\n");
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