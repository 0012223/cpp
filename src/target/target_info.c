/*
 * filename: target_info.c
 * 
 * Purpose:
 * Implementation of target architecture detection and information for the ћ++ compiler.
 * This file provides functions to detect and query properties of the target
 * architecture that affect code generation.
 * 
 * Key Components:
 * - target_detect(): Automatically determine the current architecture
 * - target_init(): Initialize target information for a specific architecture
 * - target_get_word_size(): Return the word size (32 or 64 bits)
 * - target_get_register_names(): Get architecture-specific register names
 * - target_get_calling_convention(): Get function calling convention details
 * 
 * Interactions:
 * - Used by codegen modules to generate appropriate assembly code
 * - Used by IR generator to set word size constraints
 * 
 * Notes:
 * - Uses compile-time and runtime checks to determine architecture
 * - Handles differences between x86 (32-bit) and x86_64 (64-bit)
 * - Can be manually overridden for cross-compilation
 */

#include "target_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Static arrays of register names for different architectures */

// x86 (32-bit) general purpose registers
static const char* x86_registers[] = {
    "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp"
};

// x86 registers typically used for argument passing
static const char* x86_arg_registers[] = {
    // In cdecl calling convention, all arguments are passed on the stack
    // This is an empty array as there are no dedicated argument registers
};

// x86-64 general purpose registers
static const char* x86_64_registers[] = {
    "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

// x86-64 registers for argument passing in System V AMD64 ABI
static const char* x86_64_arg_registers[] = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9"
};

/**
 * Detect the architecture of the host system
 * 
 * Uses preprocessor definitions and other checks to determine
 * the current architecture.
 * 
 * @return The detected architecture
 */
TargetArchitecture target_detect(void) {
#if defined(__x86_64__) || defined(_M_X64)
    return TARGET_ARCH_X86_64;
#elif defined(__i386__) || defined(_M_IX86)
    return TARGET_ARCH_X86;
#else
    // Perform runtime detection if possible
    // For now, return unknown
    fprintf(stderr, "Warning: Unable to detect architecture at compile time\n");
    return TARGET_ARCH_UNKNOWN;
#endif
}

/**
 * Get the word size for an architecture in bytes
 * 
 * @param arch Target architecture
 * @return Word size in bytes (4 for x86, 8 for x86-64)
 */
int target_get_word_size(TargetArchitecture arch) {
    switch (arch) {
        case TARGET_ARCH_X86:
            return 4;  // 32 bits = 4 bytes
        case TARGET_ARCH_X86_64:
            return 8;  // 64 bits = 8 bytes
        default:
            // Default to 8 bytes for unknown architectures
            fprintf(stderr, "Warning: Unknown architecture, assuming 64-bit word size\n");
            return 8;
    }
}

/**
 * Get register information for a target architecture
 * 
 * @param arch Target architecture
 * @return RegisterInfo structure with register details
 */
RegisterInfo target_get_register_info(TargetArchitecture arch) {
    RegisterInfo info;
    
    switch (arch) {
        case TARGET_ARCH_X86:
            info.general_purpose = x86_registers;
            info.num_general_purpose = sizeof(x86_registers) / sizeof(x86_registers[0]);
            info.argument_passing = x86_arg_registers;
            info.num_argument_passing = sizeof(x86_arg_registers) / sizeof(x86_arg_registers[0]);
            info.return_value = "eax";
            break;
            
        case TARGET_ARCH_X86_64:
            info.general_purpose = x86_64_registers;
            info.num_general_purpose = sizeof(x86_64_registers) / sizeof(x86_64_registers[0]);
            info.argument_passing = x86_64_arg_registers;
            info.num_argument_passing = sizeof(x86_64_arg_registers) / sizeof(x86_64_arg_registers[0]);
            info.return_value = "rax";
            break;
            
        default:
            // Default to x86-64 for unknown architectures
            fprintf(stderr, "Warning: Unknown architecture, assuming x86-64 registers\n");
            info.general_purpose = x86_64_registers;
            info.num_general_purpose = sizeof(x86_64_registers) / sizeof(x86_64_registers[0]);
            info.argument_passing = x86_64_arg_registers;
            info.num_argument_passing = sizeof(x86_64_arg_registers) / sizeof(x86_64_arg_registers[0]);
            info.return_value = "rax";
            break;
    }
    
    return info;
}

/**
 * Get calling convention information for a target architecture
 * 
 * @param arch Target architecture
 * @return CallingConventionInfo structure with calling convention details
 */
CallingConventionInfo target_get_calling_convention(TargetArchitecture arch) {
    CallingConventionInfo info;
    
    switch (arch) {
        case TARGET_ARCH_X86:
            info.type = CALLING_CONV_CDECL;
            info.argument_regs = NULL;  // All arguments on stack in cdecl
            info.num_argument_regs = 0;
            info.return_reg = "eax";
            info.stack_cleanup_caller = true;  // Caller cleans up stack in cdecl
            info.alignment = 4;  // 4-byte alignment
            break;
            
        case TARGET_ARCH_X86_64:
            info.type = CALLING_CONV_SYSTEMV_AMD64;
            info.argument_regs = x86_64_arg_registers;
            info.num_argument_regs = sizeof(x86_64_arg_registers) / sizeof(x86_64_arg_registers[0]);
            info.return_reg = "rax";
            info.stack_cleanup_caller = false;  // Callee cleans up in System V AMD64
            info.alignment = 16;  // 16-byte alignment required
            break;
            
        default:
            // Default to x86-64 for unknown architectures
            fprintf(stderr, "Warning: Unknown architecture, assuming x86-64 calling convention\n");
            info.type = CALLING_CONV_SYSTEMV_AMD64;
            info.argument_regs = x86_64_arg_registers;
            info.num_argument_regs = sizeof(x86_64_arg_registers) / sizeof(x86_64_arg_registers[0]);
            info.return_reg = "rax";
            info.stack_cleanup_caller = false;
            info.alignment = 16;
            break;
    }
    
    return info;
}

/**
 * Initialize target information for a specific architecture
 * 
 * @param arch Target architecture
 * @return TargetInfo structure with architecture details
 */
TargetInfo target_init_arch(TargetArchitecture arch) {
    TargetInfo info;
    
    // Initialize basic architecture information
    info.arch = arch;
    info.word_size = target_get_word_size(arch);
    info.pointer_size = info.word_size;  // Pointer size is typically the same as word size
    info.big_endian = false;  // x86 and x86-64 are little-endian
    
    // Get register and calling convention information
    info.registers = target_get_register_info(arch);
    info.calling_convention = target_get_calling_convention(arch);
    
    // Set stack alignment based on architecture
    switch (arch) {
        case TARGET_ARCH_X86:
            info.stack_alignment = 4;
            info.asm_syntax = "intel";
            break;
            
        case TARGET_ARCH_X86_64:
            info.stack_alignment = 16;  // System V AMD64 ABI requires 16-byte stack alignment
            info.asm_syntax = "intel";
            break;
            
        default:
            fprintf(stderr, "Warning: Unknown architecture, assuming x86-64 properties\n");
            info.stack_alignment = 16;
            info.asm_syntax = "intel";
            break;
    }
    
    return info;
}

/**
 * Initialize target information for the current host architecture
 * 
 * @return TargetInfo structure with current architecture details
 */
TargetInfo target_init(void) {
    // Detect architecture and initialize target info
    return target_init_arch(target_detect());
}

/**
 * Get a string representation of the architecture
 * 
 * @param arch Target architecture
 * @return String representation (e.g., "x86", "x86-64")
 */
const char* target_architecture_to_string(TargetArchitecture arch) {
    switch (arch) {
        case TARGET_ARCH_X86:
            return "x86";
        case TARGET_ARCH_X86_64:
            return "x86-64";
        default:
            return "unknown";
    }
}