/*
 * filename: x86_64.c
 * 
 * Purpose:
 * Implementation of x86-64 specific code generation for the ћ++ compiler.
 * Contains functions to translate IR instructions into x86-64 assembly code.
 * 
 * Key Components:
 * - x86_64_codegen_init(): Initialize the x86-64 code generator
 * - x86_64_generate_instruction(): Convert IR instruction to x86-64 assembly
 * - x86_64_allocate_registers(): Perform register allocation
 * - x86_64_generate_function_prologue(): Set up stack frame and saved registers
 * - x86_64_generate_function_epilogue(): Clean up stack frame and restore registers
 * - x86_64_handle_function_call(): Implement function calling sequence
 * 
 * Interactions:
 * - Implements architecture-specific part of codegen.h interface
 * - Uses target_info.h for architecture information
 * 
 * Notes:
 * - Uses 64-bit registers (rax, rbx, etc.)
 * - Implements System V AMD64 calling convention
 * - Optimizes register usage based on instruction patterns
 * - Special handling for array access and operations
 */

#include "x86_64.h"
// Other includes will go here