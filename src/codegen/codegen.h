/*
 * filename: codegen.h
 * 
 * Purpose:
 * Header file for the code generator component of the ћ++ compiler.
 * Defines the interface for translating IR into target-specific assembly code.
 * 
 * Key Components:
 * - CodeGenerator struct: Maintains state during code generation
 * - codegen_init(): Initialize a code generator for a specific target
 * - codegen_generate_program(): Entry point for generating assembly from IR
 * - codegen_generate_function(): Generate assembly for a function
 * - codegen_generate_prologue(): Generate function preamble (stack setup, etc.)
 * - codegen_generate_epilogue(): Generate function cleanup code
 * 
 * Interactions:
 * - Takes IR from ir.c/ir_optimizer.c as input
 * - Uses target_info.h for architecture-specific details
 * - Produces assembly output files
 * 
 * Notes:
 * - Architecture-agnostic interface with specific implementations
 *   for x86 and x86_64 in separate files
 * - Handles register allocation and calling conventions
 * - Manages stack frame layout for local variables and parameters
 */

#ifndef CODEGEN_H
#define CODEGEN_H

// Header content will be implemented later

#endif /* CODEGEN_H */