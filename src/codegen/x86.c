/*
 * filename: x86.c
 * 
 * Purpose:
 * Implementation of x86 (32-bit) specific code generation for the ћ++ compiler.
 * Contains functions to translate IR instructions into x86 assembly code.
 * 
 * Key Components:
 * - x86_codegen_init(): Initialize the x86 code generator
 * - x86_generate_instruction(): Convert IR instruction to x86 assembly
 * - x86_allocate_registers(): Perform register allocation
 * - x86_generate_function_prologue(): Set up stack frame and saved registers
 * - x86_generate_function_epilogue(): Clean up stack frame and restore registers
 * - x86_handle_function_call(): Implement function calling sequence
 * 
 * Interactions:
 * - Implements architecture-specific part of codegen.h interface
 * - Uses target_info.h for architecture information
 * 
 * Notes:
 * - Uses 32-bit registers (eax, ebx, etc.)
 * - Implements cdecl calling convention (parameters pushed on stack)
 * - Optimizes register usage based on instruction patterns
 * - Special handling for array access and operations
 */

#include "x86.h"
// Other includes will go here