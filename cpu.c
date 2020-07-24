#include <stdlib.h>
#include "cpu.h"
#include <stdio.h>
#include <string.h>
#define DATA_LEN 6

void cpu_load(struct cpu *cpu, char **arg_v, int arg_c)
{ 
  if (arg_c != 2) {
    printf(" Correct usage: ./files file_name.extension\n");
    exit(1);
  }

  FILE *fp;
  char lines[1024];
  int counter = 0;
  fp = fopen(arg_v[1], "r");
  if (fp == NULL) {
    printf("Error opening file\n");
    exit(1);
  }
  while(fgets(lines, 1024, fp) != NULL) {
    char *endptr;
    unsigned char val = strtoul(lines, &endptr, 2);
    if(lines == endptr) {
      printf("skipping: %s", lines);
      continue;
    }
    cpu->ram[counter] = val;
    counter++;
  }
  fclose(fp);
}

  int l_flag = 0;
  int e_flag = 0;
  int g_flag = 0;

/* void cpu_ram_read(struct cpu *cpu, int index) { */
/*   /1* return the value at the index *1/ */
/* } */

/* void cpu_ram_write(struct cpu *cpu, int value) { */
/*   /1* write it into the memory array *1/ */
/* /1* return the index? *1/ */
/* } */

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)

{ switch (op) {
    case ALU_MUL:
        cpu->registers[cpu->ram[cpu->pc + 1]] = cpu->registers[regA] * cpu->registers[regB];
      break;

    case ALU_CMP:
      if(cpu->registers[regA] < cpu->registers[regB]) {
        l_flag = 1;
      } else if (cpu->registers[regA] > cpu->registers[regB]) {
        g_flag = 1;
      } else {
        e_flag = 1;
      }
      break;

  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  unsigned int stack_pointer = sizeof(cpu->ram)/sizeof(cpu->ram[0]);
  int running = 1; // True until we get a HLT instruction
  unsigned char arg_1, arg_2, arg_3;
  while (running) {
    /* printf("Registers: \n[ "); */
    /* for (int i =0; i<8; i++) { */
    /*   printf("%d ", cpu->registers[i]); */
    /* } */
    /* printf("]\n"); */

    unsigned int command = cpu->ram[cpu->pc];
    int arg_count = command >> 6;


    switch(command) {
      case HLT:
        running = 0;
        break;

      case MUL:
        arg_1 = cpu->ram[cpu->pc + 1]; // register 0 instrcutions
        arg_2 = cpu->ram[cpu->pc + 2]; // register 1 
        /* arg_3 = cpu->ram[cpu->pc + 1]; // destination register for the answer */
        alu(cpu, ALU_MUL, arg_1, arg_2);
        /* cpu->registers[arg_3] = cpu->registers[arg_1] * cpu->registers[arg_2]; */
        cpu->pc += arg_count + 1;
        break;

      case PUSH:
        stack_pointer--;
        arg_1 = cpu->ram[cpu->pc + 1];
        cpu->ram[stack_pointer] = cpu->registers[arg_1];
        cpu->pc += arg_count + 1;
        break;

      case POP:
        arg_1 = stack_pointer; // this gets address of the top of the stack. In ram.
        arg_2 = cpu->ram[cpu->pc + 1]; // this is the registers where the pop goes.
        /* printf("sp value: %d\n", cpu->ram[stack_pointer]); */
        cpu->registers[arg_2] = cpu->ram[stack_pointer]; //this should set the register to the value the stack
        stack_pointer++; //increment stack pointer to reduce the height of the stack

       cpu->pc += arg_count + 1;
       break;

      case LDI:
         arg_1 = cpu->ram[cpu->pc + 1];
         arg_2 = cpu->ram[cpu->pc + 2];
         cpu->registers[arg_1] = arg_2;
         cpu->pc += arg_count + 1;
         break;

      case PRN:
       arg_3 = cpu->ram[cpu->pc + 1];
       printf("PRN: %d\n", cpu->registers[arg_3]);
       cpu->pc += arg_count + 1;
       break;

      case CALL:
        stack_pointer--; //decrement the stack
        arg_1 = cpu->pc + 2; //get the ADDRESS of  the  next instruction
        cpu->ram[stack_pointer] = arg_1; // put the next instruction on the stack
        arg_2 = cpu->ram[cpu->pc + 1]; // register value to get instructions from 
        cpu->pc = cpu->registers[arg_2];
        break;

       case ADD:
         arg_1 = cpu->ram[cpu->pc + 1]; //first register
         arg_2 = cpu->ram[cpu->pc + 2]; // second register
         arg_3 = cpu->ram[cpu->pc + 1];
         cpu->registers[arg_3] = cpu->registers[arg_1] + cpu->registers[arg_2];
         cpu->pc += arg_count + 1;
         break;

       case RET:
        arg_1 = stack_pointer; // this gets address of the top of the stack. In ram.
        cpu->pc = cpu->ram[stack_pointer];

        stack_pointer++; //increment stack pointer to reduce the height of the stack

         break;

       case CMP:
         arg_1 = cpu->ram[cpu->pc + 1]; //the register values 
         arg_2 = cpu->ram[cpu->pc + 2];
         alu(cpu, ALU_CMP, arg_1, arg_2);
         cpu->pc += arg_count + 1;
         break;

       case JEQ:
         if(e_flag == 1) {
           arg_1 = cpu->ram[cpu->pc + 1];// get the register value 
           cpu->pc = cpu->registers[arg_1];
         } else {
           cpu->pc += arg_count + 1;
         }
         break;

       case JNE:

         if(e_flag == 0) {
           arg_1 = cpu->ram[cpu->pc + 1];
           cpu->pc = cpu->registers[arg_1];
         } else {
           cpu->pc += arg_count + 1;
         }
         break;

       case JMP:
         arg_1 = cpu->ram[cpu->pc + 1];
         cpu->pc = cpu->registers[arg_1];
         /* printf("arg 1 in JMP should be 2: %d\n", arg_1); */
         break;


      default:
        printf("Unrecognized instructions: %d\n", command);
        exit(1);
    }
  }
}

void cpu_init(struct cpu *cpu)
{
  cpu->pc = 0;
  unsigned int n = 0;
  memset(cpu->ram, 0, n * sizeof(cpu->ram[0]));
  memset(cpu->registers, 0, n * sizeof(cpu->registers[0]));
  cpu->registers[7] = 0xf4;
}
