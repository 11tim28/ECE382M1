/*
    Name 1: Yu-Ting Cheng
    UTEID 1: yc35637
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND2, COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
/* MODIFY: you have to add all your new control signals */
    GATE_PSR,
    GATE_SP,
    LD_SSP,
    LD_USP,
    LD_PSR,
    SSPMUX1, SSPMUX0,
    PSR_PRMD,
    LD_VECT,
    GATE_VECT,
    ResetINV,
    ResetEXC,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetGate_PSR(int *x)      { return(x[GATE_PSR]); }
int GetGate_SP(int *x)       { return(x[GATE_SP]);}
int GetLD_SSP(int *x)        { return(x[LD_SSP]);}
int GetLD_USP(int *x)        { return(x[LD_USP]);}
int GetLD_PSR(int *x)        { return(x[LD_PSR]);}
int GetSSPMUX(int *x)        { return((x[SSPMUX1] << 1) + x[SSPMUX0]);}
int GetPSR_PRMD(int *x)      { return(x[PSR_PRMD]);}
int GetLD_VECT(int *x)       { return(x[LD_VECT]);}
int GetGate_VECT(int *x)     { return(x[GATE_VECT]); }
int GetResetINV(int *x)      { return(x[ResetINV]);}
int GetResetEXC(int *x)      { return(x[ResetEXC]);}



/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
int USP; /* User stack pointer */
int INT; /* Interrupt */
int EXC; /* Exception */
/* MODIFY: You may add system latches that are required by your implementation */

int PSR;
int VECT;

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("SSP          : 0x%0.4x\n", CURRENT_LATCHES.SSP);
    printf("USP          : 0x%0.4x\n", CURRENT_LATCHES.USP);
    printf("INT          : %d\n", CURRENT_LATCHES.INT);
    printf("EXC          : %d\n", CURRENT_LATCHES.EXC);
    printf("PSR          : 0x%0.4x\n", CURRENT_LATCHES.PSR);
    printf("INTV         : 0x%0.4x\n", CURRENT_LATCHES.INTV);
    printf("EXCV         : 0x%0.4x\n", CURRENT_LATCHES.EXCV);
    printf("VECT         : 0x%0.4x\n", CURRENT_LATCHES.VECT);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}
    

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 2]);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.USP = 0xFE00; /* Initial value of user stack pointer */
    CURRENT_LATCHES.PSR = 0x8002; // PSR[15] = 1, PSR[2:0] = 0b010 (NZP)
    CURRENT_LATCHES.INTV = 0x0001;

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/

/***************************************************************/
/* User defined variables                                      */
/***************************************************************/
int MEMORY_CYCLE_COUNT;
int DRIVER_MARMUX;
int DRIVER_PC;
int DRIVER_SHF;
int DRIVER_ALU;
int DRIVER_MDR;
int BUS_MUX;            // control signal for bus driver
int o_addr_adder;       // output of address adder
int o_LSHF1;            // output of LSHF1 
int o_ADDR2MUX;         // output of ADDR2MUX
int A, B;               // inputs to ALU
int SR1_OUT, SR2_OUT;   // output of registers


int sext(int x, int num_bits){
    if(((x >> (num_bits - 1)) & 0b1)){
        x |= (0xFFFF << num_bits);
    }
    return Low16bits(x);
}

void interrupt(){
    NEXT_LATCHES.INT = 1;
}

int exct_prot(int addr){
    if(Low16bits(addr) < 0x3000) return 1;
    else return 0;
}

int exct_unalign(int addr){
    if((Low16bits(addr) & 0x0001) == 0x0001) return 1;
    else return 0;
}



void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */
    // printf("Current State: %d\n", CURRENT_LATCHES.STATE_NUMBER);
    if(CYCLE_COUNT == 299) interrupt();
    // if(CYCLE_COUNT == 29) interrupt();


   /* !!! NOT SURE 1/0 !!!*/
    if(GetIRD(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.STATE_NUMBER = (CURRENT_LATCHES.IR >> 12) & 0x003F;
        if(NEXT_LATCHES.STATE_NUMBER == 0x000A || NEXT_LATCHES.STATE_NUMBER == 0x000B){
            NEXT_LATCHES.EXCV = 0x0004;
            NEXT_LATCHES.EXC = 1;
        }
    }
    else{
        NEXT_LATCHES.STATE_NUMBER = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
        int cond2 = (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) >> 2) & 0x0001;
        int cond1 = (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) >> 1) & 0x0001;
        int cond0 = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION) & 0x0001;
        NEXT_LATCHES.STATE_NUMBER |= ((cond2 & 0b1) & ~(cond1 & 0b1) & (cond0 & 0b1) & ((CURRENT_LATCHES.PSR >> 15) & 0x0001)) << 4;
        NEXT_LATCHES.STATE_NUMBER |= ((cond2 & 0b1) & ~(cond1 & 0b1) & ~(cond0 & 0b1) & CURRENT_LATCHES.INT) << 3;
        NEXT_LATCHES.STATE_NUMBER |= (~(cond2 & 0b1) & (cond1 & 0b1) & ~(cond0 & 0b1) & CURRENT_LATCHES.BEN) << 2;
        NEXT_LATCHES.STATE_NUMBER |= (~(cond2 & 0b1) & ~(cond1 & 0b1) & (cond0 & 0b1) & CURRENT_LATCHES.READY) << 1;
        NEXT_LATCHES.STATE_NUMBER |= (~(cond2 & 0b1) & (cond1 & 0b1) & (cond0 & 0b1) & ((CURRENT_LATCHES.IR >> 11) & 0x0001));
    }
    // Refer to control store to latch next microinstruction
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    return;

}


void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
    if(GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION) == 1 || GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        // State 16, 17, 25, 28, 29, 33
        MEMORY_CYCLE_COUNT++;
        if(MEMORY_CYCLE_COUNT == MEM_CYCLES - 1) NEXT_LATCHES.READY = 1;
        else if(MEMORY_CYCLE_COUNT == MEM_CYCLES){
            NEXT_LATCHES.READY = 0;
            MEMORY_CYCLE_COUNT = 0;
        }
    }
    else{
        // Other states
        MEMORY_CYCLE_COUNT = 0;
        NEXT_LATCHES.READY = 0;
    }
    return;


}



void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *             Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    
    if(GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 1;
    else if(GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 2;
    else if(GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 3;
    else if(GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 4;
    else if(GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 5;
    else if(GetGate_PSR(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 6;
    else if(GetGate_SP(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 7;
    else if(GetGate_VECT(CURRENT_LATCHES.MICROINSTRUCTION)) BUS_MUX = 8;
    else BUS_MUX = 0;
    return;

}


void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */      
   /* ALU */
    int sr1, sr2;
    if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) >= 2) sr1 = 6;
    else if(GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) sr1 = (CURRENT_LATCHES.IR >> 6) & 0x0007;
    else sr1 = (CURRENT_LATCHES.IR >> 9) & 0x0007;
    SR1_OUT = CURRENT_LATCHES.REGS[sr1];
    SR2_OUT = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x0007];
    A = SR1_OUT;
    B = (((CURRENT_LATCHES.IR >> 5) & 0x0001) == 1) ? sext((CURRENT_LATCHES.IR & 0x001F), 5) : SR2_OUT;

    /* ADDR2MUX */
    if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) o_ADDR2MUX = 0;
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) o_ADDR2MUX = sext((CURRENT_LATCHES.IR & 0x003F), 6);
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2) o_ADDR2MUX = sext((CURRENT_LATCHES.IR & 0x01FF), 9);
    else if(GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3) o_ADDR2MUX = sext((CURRENT_LATCHES.IR & 0x07FF), 11);
    o_LSHF1 = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION) ? Low16bits(o_ADDR2MUX << 1) : o_ADDR2MUX;
    o_addr_adder = Low16bits(o_LSHF1 + (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) ? SR1_OUT : CURRENT_LATCHES.PC));

    if(BUS_MUX == 1){ // MARMUX
        BUS = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION) ? o_addr_adder : ((CURRENT_LATCHES.IR & 0x00FF) << 1);
    }
    else if(BUS_MUX == 2) BUS = CURRENT_LATCHES.PC;
    else if(BUS_MUX == 3){ // SHF
        int amount4 = CURRENT_LATCHES.IR & 0X000F;
        if(((CURRENT_LATCHES.IR >> 4) & 0x0003) == 0x00000) BUS = Low16bits(SR1_OUT << amount4);
        else if(((CURRENT_LATCHES.IR >> 4) & 0x0003) == 0x0001){
            // Unsure bout dis //
                unsigned int temp = SR1_OUT;
                BUS = Low16bits(temp >> amount4);
        }
        else if(((CURRENT_LATCHES.IR >> 4) & 0x0003) == 0x0003) {
            // BUS = Low16bits(SR1_OUT >> amount4);
            // RSHFA
            if(SR1_OUT & 0x8000) BUS = Low16bits((SR1_OUT >> amount4) | 0xffff << (16 - amount4));
            else BUS = Low16bits(SR1_OUT >> amount4);
        }
    }
    else if(BUS_MUX == 4){ // ALU
        if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0) BUS = Low16bits(A + B);
        else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1) BUS = Low16bits(A & B);
        else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2) BUS = Low16bits(A ^ B);
        else if(GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 3) BUS = Low16bits(A);
    }
    else if(BUS_MUX == 5){ // MDR
        BUS = (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) ? CURRENT_LATCHES.MDR : 
            ((CURRENT_LATCHES.MAR & 0x0001) ? sext(CURRENT_LATCHES.MDR >> 8, 8) : sext(CURRENT_LATCHES.MDR & 0x00FF, 8));
    }
    else if(BUS_MUX == 6){ // PSR
        BUS = CURRENT_LATCHES.PSR;
    }
    else if(BUS_MUX == 7){ // SP
        if(Low16bits(CURRENT_LATCHES.PSR) & 0x8000) BUS = CURRENT_LATCHES.USP;
        else                                        BUS = CURRENT_LATCHES.SSP;
    }
    else if(BUS_MUX == 8){ // VECT
        // State 39
        BUS = 0x0200 + Low16bits(CURRENT_LATCHES.VECT << 1);
    }
    else BUS = 0;
    // printf("BUS_MUX: %d, BUS: 0x%.4x\n", BUS_MUX, BUS);
    return; 

}


void latch_datapath_values() {

  /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   */       



   /* IR NEXT */
   if(GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.IR = BUS;
   }

   /* PC NEXT */
   if(GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
        else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) NEXT_LATCHES.PC = BUS;
        else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2) NEXT_LATCHES.PC = o_addr_adder;
        else if(GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3) NEXT_LATCHES.PC = CURRENT_LATCHES.PC - 2;
   }

   /* NZP */
   if(GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(BUS == 0){
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if(BUS & 0x8000){
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else{
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
   }

   /* BEN */
   if(GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)){
        int ir11 = (CURRENT_LATCHES.IR >> 11) & 0x0001;
        int ir10 = (CURRENT_LATCHES.IR >> 10) & 0x0001;
        int ir9  = (CURRENT_LATCHES.IR >> 9) & 0x0001;
        NEXT_LATCHES.BEN = (ir11 & CURRENT_LATCHES.N) | (ir10 & CURRENT_LATCHES.Z) | (ir9 & CURRENT_LATCHES.P);
   }

   

   /* Register */
   if(GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)){
        int dr;
        if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) >= 2) dr = 6;
        else if(GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) dr = 7;
        else dr = (CURRENT_LATCHES.IR >> 9) & 0x0007;
        // printf("DR: %d\n", dr);
        NEXT_LATCHES.REGS[dr] = BUS;
   }

   /* MAR */
   if(GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)){
        //////////////////////////////////////////////////////////* Lab 4 *////////////////////////////////////////////////////////
        if((BUS < 0x3000) && (Low16bits(CURRENT_LATCHES.PSR) >> 15) == 0x0001 && ((CURRENT_LATCHES.IR & 0xF000) >> 12) != 0b001111){
            NEXT_LATCHES.STATE_NUMBER = 0x000A;
            memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
            NEXT_LATCHES.EXCV = 0x0002;
            NEXT_LATCHES.EXC = 1;
        }
        else if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1 && (BUS & 0x0001)){
            NEXT_LATCHES.STATE_NUMBER = 0x000A;
            memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
            NEXT_LATCHES.EXCV = 0x0003;
            NEXT_LATCHES.EXC = 1;
        }
        else{
            NEXT_LATCHES.MAR = BUS;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   }

   /* MDR */
   if(GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)){
            // LOAD
            NEXT_LATCHES.MDR = (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) + MEMORY[CURRENT_LATCHES.MAR >> 1][0];
        }
        else{
            // STORE
            if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
                // STW  
                NEXT_LATCHES.MDR = BUS;
            }
            else{
                // STB
                if((CURRENT_LATCHES.MAR & 0x0001) == 1) NEXT_LATCHES.MDR = Low16bits((BUS & 0x00FF) << 8);
                else NEXT_LATCHES.MDR = Low16bits(BUS & 0x00FF);
            }
        }
   }

   /* STW, STB*/
   if(GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
        if(GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1){
            // STW
            MEMORY[CURRENT_LATCHES.MAR >> 1][0] = CURRENT_LATCHES.MDR & 0x00FF;
            MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (CURRENT_LATCHES.MDR >> 8) & 0x00FF;
        }
        else{
            // STB
            if((CURRENT_LATCHES.MAR & 0x0001) == 1){
                MEMORY[CURRENT_LATCHES.MAR >> 1][1] = (CURRENT_LATCHES.MDR >> 8) & 0x00FF;
            }
            else{
                MEMORY[CURRENT_LATCHES.MAR >> 1][0] = CURRENT_LATCHES.MDR & 0x00FF;
            }
        }
   }

   //////////////////////////////////////////////////////////////
   ////////////////////// Lab 4 /////////////////////////////////
   //////////////////////////////////////////////////////////////

   // SSP
   if(GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0) NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP;
        else if(GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1) NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP - 2;
        else if(GetSSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2) NEXT_LATCHES.SSP = CURRENT_LATCHES.SSP + 2;
   }

   // USP
   if(GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.USP = SR1_OUT; // State 56

   // PSR
   if(GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION)){
    if(GetPSR_PRMD(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.PSR = Low16bits(CURRENT_LATCHES.PSR) & 0x7FFF; // set PSR[15] = 0 (State 56)
    else NEXT_LATCHES.PSR = BUS; // State 47
   }

   // VECT
   if(GetLD_VECT(CURRENT_LATCHES.MICROINSTRUCTION)){
    if(CURRENT_LATCHES.EXC == 1){
        NEXT_LATCHES.VECT = CURRENT_LATCHES.EXCV;
        // NEXT_LATCHES.EXC = 0;
    }
    else{
        // State 41
        NEXT_LATCHES.VECT = CURRENT_LATCHES.INTV;
        // NEXT_LATCHES.INT = 0;
    }
   }

   // Reset INT and EXC
   if(GetResetINV(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.INT = 0;
   if(GetResetEXC(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.EXC = 0;



}