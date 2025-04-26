/*
 * filename: error.h
 * 
 * Purpose:
 * Header file for error handling utilities in the ћ++ compiler.
 * Defines interfaces for reporting and handling different types of errors
 * that can occur during compilation.
 * 
 * Key Components:
 * - ErrorType enum: Categorizes errors (lexical, syntax, semantic, code generation)
 * - ErrorSeverity enum: Levels of severity (warning, error, fatal)
 * - Error struct: Contains information about a specific error
 * - error_init(): Initialize the error handling system
 * - error_report(): Report an error with location and message
 * - error_get_count(): Get the number of errors encountered
 * - error_print_summary(): Print a summary of all errors
 * 
 * Interactions:
 * - Used by all compiler components to report errors
 * - Coordinates with main.c to determine when to abort compilation
 * 
 * Notes:
 * - Supports UTF-8 in error messages and file paths
 * - Tracks file, line, and column positions for precise error reporting
 * - Can provide suggestions for some common errors
 */

#ifndef ERROR_H
#define ERROR_H

// Header content will be implemented later

#endif /* ERROR_H */