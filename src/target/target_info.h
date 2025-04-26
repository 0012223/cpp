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

// Header content will be implemented later

#endif /* TARGET_INFO_H */