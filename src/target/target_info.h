/*
 * filename: target_info.h
 * 
 * Purpose:
 * Header file defining architecture-specific information for the ћ++ compiler.
 * Provides an interface for querying target architecture properties that
 * affect code generation.
 * 
 * Key Components:
 * - TargetArchitecture enum: Supported CPU architectures (x86, x86_64)
 * - TargetInfo struct: Contains architecture-specific properties
 * - target_detect(): Detect the current architecture automatically
 * - target_get_word_size(): Get the size of a word on the target architecture
 * - target_get_calling_convention(): Get target-specific calling convention info
 * 
 * Interactions:
 * - Used by code generator to adjust assembly output
 * - Used by IR module to set word size constraints
 * 
 * Notes:
 * - Abstracts architecture-specific details from the rest of the compiler
 * - Currently supports x86 (32-bit) and x86_64 (64-bit) architectures
 * - Enables cross-compilation by manually setting the target info
 */

#ifndef TARGET_INFO_H
#define TARGET_INFO_H

#include <stdbool.h>
#include <stdint.h>

// Supported target architectures
typedef enum {
    TARGET_ARCH_X86,     // 32-bit x86 architecture
    TARGET_ARCH_X86_64,  // 64-bit x86-64 architecture
    TARGET_ARCH_UNKNOWN  // Unknown or unsupported architecture
} TargetArchitecture;

// Endianness types
typedef enum {
    ENDIAN_LITTLE,       // Little-endian byte order
    ENDIAN_BIG           // Big-endian byte order
} Endianness;

// Calling convention types
typedef enum {
    CALLING_CONV_CDECL,      // Standard C calling convention (x86)
    CALLING_CONV_SYSTEMV_AMD64  // System V AMD64 ABI (x86-64)
} CallingConvention;

// Information about register availability
typedef struct {
    const char** general_purpose;  // Array of general-purpose register names
    int num_general_purpose;       // Number of general-purpose registers
    const char** argument_passing; // Registers used for argument passing (if applicable)
    int num_argument_passing;      // Number of argument passing registers
    const char* return_value;      // Register used for return values
} RegisterInfo;

// Information about a specific calling convention
typedef struct {
    CallingConvention type;        // Type of calling convention
    const char** argument_regs;    // Registers used for passing arguments (if any)
    int num_argument_regs;         // Number of registers used for arguments
    const char* return_reg;        // Register used for return value
    bool stack_cleanup_caller;     // true if caller cleans up stack (cdecl), false otherwise
    int alignment;                 // Stack alignment requirement in bytes
} CallingConventionInfo;

// Target architecture information
typedef struct {
    TargetArchitecture arch;       // Target architecture type
    int word_size;                 // Word size in bytes (4 for x86, 8 for x86-64)
    int pointer_size;              // Pointer size in bytes (usually same as word_size)
    int stack_alignment;           // Required stack alignment in bytes
    RegisterInfo registers;        // Available registers
    CallingConventionInfo calling_convention; // Default calling convention
    const char* asm_syntax;        // Assembly syntax (e.g., "intel", "att")
    bool big_endian;               // Endianness flag (true for big-endian, false for little-endian)
    Endianness endianness;         // Endianness (ENDIAN_LITTLE or ENDIAN_BIG)
} TargetInfo;

/**
 * Initialize target information for the current host architecture.
 * Detects the architecture automatically.
 * 
 * @return Initialized TargetInfo structure
 */
TargetInfo target_init(void);

/**
 * Initialize target information for a specific architecture.
 * 
 * @param arch The target architecture to use
 * @return Initialized TargetInfo structure
 */
TargetInfo target_init_arch(TargetArchitecture arch);

/**
 * Detect the current host architecture.
 * 
 * @return The detected architecture
 */
TargetArchitecture target_detect(void);

/**
 * Get the word size for an architecture in bytes.
 * 
 * @param arch The target architecture
 * @return Word size in bytes (4 for x86, 8 for x86-64)
 */
int target_get_word_size(TargetArchitecture arch);

/**
 * Get the calling convention information for a target architecture.
 * 
 * @param arch The target architecture
 * @return Calling convention information
 */
CallingConventionInfo target_get_calling_convention(TargetArchitecture arch);

/**
 * Get register information for a target architecture.
 * 
 * @param arch The target architecture
 * @return Register information including available registers
 */
RegisterInfo target_get_register_info(TargetArchitecture arch);

/**
 * Get a string representation of the architecture.
 * 
 * @param arch The target architecture
 * @return String representation (e.g., "x86", "x86-64")
 */
const char* target_architecture_to_string(TargetArchitecture arch);

#endif /* TARGET_INFO_H */