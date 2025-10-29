/*
Name 1: Yu-Ting Cheng
UTEID 1: yc35637
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

FILE* infile    =   NULL;
FILE* outfile   =   NULL;

#define MAX_LINE_LENGTH 255
#define MAX_SYMBOLS 255

enum {DONE, OK, EMPTY_LINE};

int readAndParse(FILE *pInfile, char *pLine, char **pLabel, char **pOpcode, 
                 char **pArg1, char **pArg2, char **pArg3, char **pArg4);
int toNum(char *pStr);
int isOpcode(char *pPtr);
void writeOutFile(FILE *pOutfile, char *pLabel, char *pOpcode, 
                 char *pArg1, char *pArg2, char *pArg3, char *pArg4, unsigned short pc);

/* Assemble Private Function */
unsigned short assembleAdd(char *pArg1, char *pArg2, char *pArg3);
unsigned short assembleAnd(char *pArg1, char *pArg2, char *pArg3);
unsigned short assembleJmp(char *pArg1);
unsigned short assembleJsr(char *pArg1, unsigned short pc);
unsigned short assembleLdb(char *pArg1, char *pArg2, char *pArg3);
unsigned short assembleLdw(char *pArg1, char *pArg2, char *pArg3);
unsigned short assembleLea(char *pArg1, char *pArg2, unsigned short pc);
unsigned short assembleBr(char *pOpcode, char *pArg1, unsigned short pc);
unsigned short assembleXor(char *pArg1, char *pArg2, char *pArg3);
int regNumber(char *reg);

/* Symbol Table */
typedef struct {
    char label[13];
    int address;
} Symbol;
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;
void addSymbol(char *label, int address);
int getAddress(char *label);





int main(int argc, char* argv[]){

    /* open the source file*/
    infile  =   fopen(argv[1], "r");
    outfile =   fopen(argv[2], "w");

    if(!infile){
        printf("Error: Cannot open file %s\n", argv[1]);
        exit(4);
    }
    if(!outfile){
        printf("Error: Cannot open file %s\n", argv[2]);
        exit(4);
    }

    /* First Pass: Record Labels */
    char lLine[MAX_LINE_LENGTH+1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet;
    unsigned short pc = 0;
    do{
        lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if(lRet != DONE && lRet != EMPTY_LINE){
            // printf("%s %s %s %s %s %s\n", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
            pc += 2;
            if(strcmp(lOpcode, ".orig") == 0){
                pc = toNum(lArg1);
            }
            //if(strlen(lLabel) > 0){
	    if(isalnum(lLabel[0])){
                // Add label to symbol table
		//printf("strlen: %d\n", strlen(lLabel));
                addSymbol(lLabel, pc);
                // printf("Added Symbol: %s at address %x\n", lLabel, pc);
            }
            // printf("PC: %x\n", pc);
        }
    } while(lRet != DONE);
    // for(int i=0; i<symbolCount; i++){
    //     printf("Symbol: %s at address %x\n", symbolTable[i].label, symbolTable[i].address);
    // } 

    fclose(infile);
    infile  =   fopen(argv[1], "r");

    /* Read and parse*/
    pc = 0;
    do{
        lRet = readAndParse(infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4);
        if(lRet != DONE && lRet != EMPTY_LINE){
            pc += 2;
            if(strcmp(lOpcode, ".orig") == 0){
                pc = toNum(lArg1);
            }
            // printf("%s %s %s %s %s %s\n", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
            writeOutFile(outfile, lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4, pc);
        }
    } while(lRet != DONE);

    /* Close files*/
    fclose(infile);
    fclose(outfile);
}



int readAndParse(FILE *pInfile, char *pLine, char **pLabel, char **pOpcode, 
                 char **pArg1, char **pArg2, char **pArg3, char **pArg4)
{
    char *lRet, *lPtr;
    int i;
    if(!fgets(pLine, MAX_LINE_LENGTH, pInfile))
        return(DONE);
    /* convert entire line to lowercase*/
    for(i=0; i<strlen(pLine); i++)
        pLine[i] = tolower(pLine[i]);
    
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments*/
    lPtr = pLine;

    while(*lPtr != ';' && *lPtr != '\0' && *lPtr != '\n')
        lPtr++;
    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
    *pLabel = lPtr;
    //printf("Label %s isalnum(): %d\n", *pLabel, isalnum(*pLabel[0]));
    if( !( lPtr = strtok( NULL, "\t\n\r ," ) ) ) return( OK );
    }
        *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n\r ," ) ) ) return( OK );
    
        *pArg1 = lPtr;
    
    if( !( lPtr = strtok( NULL, "\t\n\r ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n\r ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n\r ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}

int isOpcode(char *lPtr){
    if(!strcmp(lPtr, "add") || !strcmp(lPtr, "and") || !strcmp(lPtr, "xor") || !strcmp(lPtr, "halt") || !strcmp(lPtr, "jmp") 
    || !strcmp(lPtr, "jsr") || !strcmp(lPtr, "jsrr")|| !strcmp(lPtr, "ldb")|| !strcmp(lPtr, "ldw")  || !strcmp(lPtr, "lea")
    || !strcmp(lPtr, "nop") || !strcmp(lPtr, "not") || !strcmp(lPtr, "ret")|| !strcmp(lPtr, "lshf") || !strcmp(lPtr, "rshfl")
    || !strcmp(lPtr, "rshfa")|| !strcmp(lPtr, "rti")|| !strcmp(lPtr, "stb")|| !strcmp(lPtr, "stw")  || !strcmp(lPtr, "trap")
    || !strcmp(lPtr, "brn") || !strcmp(lPtr, "brz") || !strcmp(lPtr, "brp")|| !strcmp(lPtr, "br") || !strcmp(lPtr, "brzp")
    || !strcmp(lPtr, "brnp")|| !strcmp(lPtr, "brnz")|| !strcmp(lPtr, "brnzp")){
        return 1;
    }
    else return -1;
}

int toNum(char *pStr){
    char *t_ptr;
    char *orig_pStr;
    int t_length, k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if(*pStr == '#'){  /* decimal*/
        pStr++;
        if(*pStr == '-'){  /*negative*/
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        // printf("t_length: %d\n", t_length);
        // printf("t_ptr: %s\n", t_ptr);
        for(k=0; k<t_length; k++){
            if(!isdigit(*t_ptr)){
                // printf("*t_ptr: %c, k: %d\n", *t_ptr, k);
                // printf("%d\n", *t_ptr);
                printf("Error: invalid decimal operand, %s\n", orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if(lNeg) lNum = -lNum;
        return lNum;
    }
    else if(*pStr == 'x'){  /* hex*/
        pStr++;
        if(*pStr == '-'){
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0; k<t_length; k++){
            if(!isxdigit(*t_ptr)){
                printf("Error: invalid hex operand, %s\n", orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);  /* convert hex string into integer*/
        lNum = (lNumLong > INT_MAX) ? INT_MAX : lNumLong;
        if(lNeg) lNum = -lNum;
        return lNum;
    }
    else{
        printf("Error: valid operand, %s\n", orig_pStr);
        exit(4);
    }
}

void writeOutFile(FILE *pOutfile, char *pLabel, char *pOpcode, char *pArg1, char *pArg2, char *pArg3, char *pArg4, unsigned short pc){
    unsigned short instr = 0;

    /* pseudo-op */
    if(strcmp(pOpcode, ".orig") == 0) instr = toNum(pArg1);
    if(strcmp(pOpcode, ".fill") == 0) instr = toNum(pArg1);
    if(strcmp(pOpcode, ".end") == 0) return;

    /* ADD */
    if(strcmp(pOpcode, "add") == 0) instr = assembleAdd(pArg1, pArg2, pArg3);

    /* AND */
    if(strcmp(pOpcode, "and") == 0) instr = assembleAnd(pArg1, pArg2, pArg3);

    /* BR */
    if(strcmp(pOpcode, "br") == 0 || strcmp(pOpcode, "brnzp") == 0 || strcmp(pOpcode, "brn") == 0
    || strcmp(pOpcode, "brz") == 0 || strcmp(pOpcode, "brp") == 0 || strcmp(pOpcode, "brzp") == 0
    || strcmp(pOpcode, "brnz") == 0 || strcmp(pOpcode, "brnp") == 0) instr = assembleBr(pOpcode, pArg1, pc);

    /* HALT */
    if(strcmp(pOpcode, "halt") == 0) instr = 0xF025;

    /* JMP */
    if(strcmp(pOpcode, "jmp") == 0) instr = assembleJmp(pArg1);

    /* JSR */
    if(strcmp(pOpcode, "jsr") == 0) instr = assembleJsr(pArg1, pc);

    /* JSRR */
    if(strcmp(pOpcode, "jsrr") == 0) instr = 0x4000 | (regNumber(pArg1) << 6);

    /* LDB */
    if(strcmp(pOpcode, "ldb") == 0) instr = assembleLdb(pArg1, pArg2, pArg3);

    /* LDW */
    if(strcmp(pOpcode, "ldw") == 0) instr = assembleLdw(pArg1, pArg2, pArg3);

    /* LEA */
    if(strcmp(pOpcode, "lea") == 0) instr = assembleLea(pArg1, pArg2, pc);

    /* NOP */
    if(strcmp(pOpcode, "nop") == 0) instr = 0x0000;

    /* NOT */
    if(strcmp(pOpcode, "not") == 0) instr = 0x903F | (regNumber(pArg1) << 9) | (regNumber(pArg2) << 6);

    /* RET */
    if(strcmp(pOpcode, "ret") == 0) instr = assembleJmp("r7");

    /* LSHT */
    if(strcmp(pOpcode, "lshf") == 0) instr = 0xD000 | (regNumber(pArg1) << 9) | (regNumber(pArg2) << 6) | (toNum(pArg3) & 0xF);

    /* RSHFL */
    if(strcmp(pOpcode, "rshfl") == 0) instr = 0xD010 | (regNumber(pArg1) << 9) | (regNumber(pArg2) << 6) | (toNum(pArg3) & 0xF);

    /* RSHFA */
    if(strcmp(pOpcode, "rshfa") == 0) instr = 0xD030 | (regNumber(pArg1) << 9) | (regNumber(pArg2) << 6) | (toNum(pArg3) & 0xF);

    /* RTI */
    if(strcmp(pOpcode, "rti") == 0) instr = 0x8000;
    
    /* STB */
    if(strcmp(pOpcode, "stb") == 0) instr = 0x3000 | (regNumber(pArg1) << 9) | (regNumber(pArg2) << 6) | (toNum(pArg3) & 0x3F);

    /* STW */
    if(strcmp(pOpcode, "stw") == 0) instr = 0x7000 | (regNumber(pArg1) << 9) | (regNumber(pArg2) << 6) | (toNum(pArg3) & 0x3F);

    /* TRAP */
    if(strcmp(pOpcode, "trap") == 0) instr = 0xF000 | (toNum(pArg1) & 0xFF);

    /* XOR */
    if(strcmp(pOpcode, "xor") == 0) instr = assembleXor(pArg1, pArg2, pArg3);


    /* write outfile */
    fprintf(pOutfile, "0x%.4X\n", instr);
}

int regNumber(char *reg){
    if(reg[0] == 'R' || reg[0] == 'r'){
        return atoi(&reg[1]);
    }
    return -1;
}




void addSymbol(char *label, int addr){
    strcpy(symbolTable[symbolCount].label, label);
    symbolTable[symbolCount].address = addr;
    symbolCount++;
}

int getAddress(char *label){
    for(int i = 0; i < symbolCount; i++){
        if(strcmp(symbolTable[i].label, label) == 0){
            return symbolTable[i].address;
        }
    }
    // printf("Label not found: %s", label);
    // for(int i=0; label[i] != '\0'; i++){
    //     printf("[%d]", (unsigned char)label[i]);
    // }
    // printf("\n");
    return -1; // Not found
}








unsigned short assembleAdd(char *pArg1, char *pArg2, char *pArg3){
    unsigned short instr = 0;
    instr |= (0x1 << 12); 
    int dr  = regNumber(pArg1);
    int sr1 = regNumber(pArg2);

    instr |= (dr & 0x7) << 9;  
    instr |= (sr1 & 0x7) << 6; 

    if(pArg3[0] == 'R' || pArg3[0] == 'r'){
        int sr2 = regNumber(pArg3);
        instr |= (sr2 & 0x7);
    }
    else{
        int imm5 = toNum(pArg3);
        instr |= (1 << 5); 
        instr |= (imm5 & 0x1F); 
    }
    return instr;
}

unsigned short assembleAnd(char *pArg1, char *pArg2, char *pArg3){
    unsigned short instr = 0;
    instr |= (0x5 << 12); 
    int dr  = regNumber(pArg1);
    int sr1 = regNumber(pArg2);
    instr |= (dr & 0x7) << 9; 
    instr |= (sr1 & 0x7) << 6; 
    if(pArg3[0] == 'R' || pArg3[0] == 'r'){
        int sr2 = regNumber(pArg3);
        instr |= (sr2 & 0x7);
    }
    else{
        int imm5 = toNum(pArg3);
        instr |= (1 << 5); 
        instr |= (imm5 & 0x1F); 
    }
    return instr;
}

unsigned short assembleJmp(char *pArg1){
    unsigned short instr = 0;
    instr |= (0xC << 12); 
    int baseR = regNumber(pArg1);
    instr |= (baseR & 0x7) << 6; 
    return instr;
}

unsigned short assembleJsr(char *pArg1, unsigned short pc){
    unsigned short instr = 0;
    instr |= (0x4 << 12); 

    int labelAddr = getAddress(pArg1);
    if(labelAddr == -1){
        printf("Error: Undefined label %s\n", pArg1);
        exit(4);
    }

    int offset11 = labelAddr - (pc+2);
    offset11 /= 2;
    instr |= (1 << 11); 
    instr |= (offset11 & 0x7FF); 

    return instr;
}

unsigned short assembleLdb(char *pArg1, char *pArg2, char *pArg3){
    unsigned short instr = 0;
    instr |= (0x2 << 12); 
    int dr  = regNumber(pArg1);
    int baseR = regNumber(pArg2);
    int offset6 = toNum(pArg3);
    instr |= (dr & 0x7) << 9; 
    instr |= (baseR & 0x7) << 6; 
    instr |= (offset6 & 0x3F); 
    return instr;
}

unsigned short assembleLdw(char *pArg1, char *pArg2, char *pArg3){
    unsigned short instr = 0;
    instr |= (0x6 << 12); 
    int dr  = regNumber(pArg1);
    int baseR = regNumber(pArg2);
    int offset6 = toNum(pArg3);
    instr |= (dr & 0x7) << 9; 
    instr |= (baseR & 0x7) << 6; 
    instr |= (offset6 & 0x3F); 
    return instr;
}

unsigned short assembleLea(char *pArg1, char *pArg2, unsigned short pc){
    unsigned short instr = 0;
    instr |= (0xE << 12); 
    int dr  = regNumber(pArg1);
    instr |= (dr & 0x7) << 9; 

    int labelAddr = getAddress(pArg2);
    if(labelAddr == -1){
        printf("Error: Undefined label %s\n", pArg2);
        exit(4);
    }

    int offset9 = labelAddr - (pc+2);
    offset9 /= 2;
    instr |= (offset9 & 0x1FF); 

    return instr;
}

unsigned short assembleBr(char *pOpcode, char *pArg1, unsigned short pc){
    unsigned short instr = 0;
    instr |= (0x0 << 12); 

    if(strcmp(pOpcode, "br") == 0) instr |= (0x7 << 9);
    else{
        if(strchr(pOpcode, 'n')) instr |= (1 << 11); 
        if(strchr(pOpcode, 'z')) instr |= (1 << 10); 
        if(strchr(pOpcode, 'p')) instr |= (1 << 9);  
    }

    int labelAddr = getAddress(pArg1);
    if(labelAddr == -1){
        printf("Error: Undefined label %s\n", pArg1);
        exit(4);
    }

    int offset9 = labelAddr - (pc+2);
    offset9 /= 2;
    instr |= (offset9 & 0x1FF); 

    return instr;
}

unsigned short assembleXor(char *pArg1, char *pArg2, char *pArg3){
    unsigned short instr = 0;
    instr |= (0x9 << 12); 
    int dr  = regNumber(pArg1);
    int sr1 = regNumber(pArg2);
    instr |= (dr & 0x7) << 9; 
    instr |= (sr1 & 0x7) << 6; 
    if(pArg3[0] == 'R' || pArg3[0] == 'r'){
        int sr2 = regNumber(pArg3);
        instr |= (sr2 & 0x7);
    }
    else{
        int imm5 = toNum(pArg3);
        instr |= (1 << 5); 
        instr |= (imm5 & 0x1F); 
    }
    return instr;
}
