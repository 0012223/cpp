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
// Other includes will go here