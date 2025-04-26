/*
 * filename: x86_64.h
 * 
 * Purpose:
 * Header file for x86-64 specific code generation in the ћ++ compiler.
 * Defines constants, structures, and functions specific to generating
 * 64-bit x86 assembly code.
 * 
 * Key Components:
 * - X86_64_Register enum: Defines available registers on x86-64
 * - x86_64_codegen_init(): Initialize x86-64 specific code generator
 * - x86_64_get_register_name(): Convert register enum to string representation
 * - x86_64_calling_convention: Information about parameter passing and return values
 * 
 * Interactions:
 * - Used by codegen.c to implement architecture-specific code generation
 * - Uses target_info.h for general architecture information
 * 
 * Notes:
 * - Handles x86-64 specific registers (rax, rbx, rcx, etc.)
 * - Implements System V AMD64 ABI calling convention
 * - 64-bit word size for all operations
 */

#ifndef X86_64_H
#define X86_64_H

// Header content will be implemented later

#endif /* X86_64_H */