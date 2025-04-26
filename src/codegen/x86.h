/*
 * filename: x86.h
 * 
 * Purpose:
 * Header file for x86 (32-bit) specific code generation in the ћ++ compiler.
 * Defines constants, structures, and functions specific to generating
 * 32-bit x86 assembly code.
 * 
 * Key Components:
 * - X86_Register enum: Defines available registers on x86 (32-bit)
 * - x86_codegen_init(): Initialize x86 specific code generator
 * - x86_get_register_name(): Convert register enum to string representation
 * - x86_calling_convention: Information about parameter passing and return values
 * 
 * Interactions:
 * - Used by codegen.c to implement architecture-specific code generation
 * - Uses target_info.h for general architecture information
 * 
 * Notes:
 * - Handles x86 specific registers (eax, ebx, ecx, etc.)
 * - Implements cdecl calling convention
 * - 32-bit word size for all operations
 */

#ifndef X86_H
#define X86_H

// Header content will be implemented later

#endif /* X86_H */