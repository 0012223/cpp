/*
 * filename: error.h
 * 
 * Purpose:
 * Error handling utilities for the ћ++ compiler.
 * Provides structured error reporting with location information,
 * severity levels, and contextual suggestions.
 * 
 * Key Components:
 * - Error types (lexical, syntax, semantic, etc.)
 * - Error severity levels (warning, error, fatal)
 * - Macros for easy error reporting with source location
 * - Functions for error tracking and summarization
 * 
 * Interactions:
 * - Used by all compiler components to report issues
 * - Coordinates with main.c to handle error thresholds
 * 
 * Notes:
 * - Automatically captures compiler source location via __FILE__, __LINE__
 * - Supports colorized terminal output for better readability
 * - Can generate error log files for post-compilation analysis
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdbool.h>

// Error type classifications
typedef enum {
    ERROR_LEXICAL,    // Character/token level errors
    ERROR_SYNTAX,     // Grammar/parsing errors
    ERROR_SEMANTIC,   // Type checking/semantic analysis errors
    ERROR_CODEGEN,    // Code generation errors
    ERROR_IO,         // File I/O errors
    ERROR_INTERNAL    // Compiler internal errors
} ErrorType;

// Error severity levels
typedef enum {
    ERROR_WARNING,    // Non-fatal, compilation continues
    ERROR_ERROR,      // Error, compilation may continue to find more errors
    ERROR_FATAL       // Fatal error, compilation stops immediately
} ErrorSeverity;

// Error structure to track information about each error
typedef struct {
    ErrorType type;           // Type of error
    ErrorSeverity severity;   // Severity level
    const char* filename;     // Source file where error occurred
    int line;                 // Line number in source file
    int column;               // Column number in source file
    const char* message;      // Error message
    const char* suggestion;   // Optional suggestion for fixing the error
    const char* compiler_file;// Compiler source file reporting the error
    int compiler_line;        // Line in compiler where error was reported
} Error;

// Function prototypes

/**
 * Initialize the error handling system
 * @return True if initialization was successful
 */
bool error_init(void);

/**
 * Initialize the error handling system with optional log file generation
 * @param create_log_file If true, create a log file for errors
 * @return True if initialization was successful
 */
bool error_init_with_log(bool create_log_file);

/**
 * Clean up the error handling system and free any resources
 */
void error_cleanup(void);

/**
 * Report an error with all details
 * @param type Error type classification
 * @param severity Severity level of the error
 * @param filename Source file where error was detected
 * @param line Line number in source file
 * @param column Column number in source file
 * @param message Error message explaining the issue
 * @param suggestion Optional suggestion for fixing the error
 * @param compiler_file Compiler source file reporting the error
 * @param compiler_line Line in compiler where error was reported
 * @return True if error was successfully reported
 */
bool error_report(
    ErrorType type,
    ErrorSeverity severity,
    const char* filename,
    int line,
    int column,
    const char* message,
    const char* suggestion,
    const char* compiler_file,
    int compiler_line
);

/**
 * Get the number of errors of a given severity
 * @param severity Error severity level to count (-1 for all errors)
 * @return Number of errors of the specified severity
 */
int error_get_count(int severity);

/**
 * Print a summary of all errors encountered
 * @param verbose If true, print all errors again in detail
 */
void error_print_summary(bool verbose);

// Convenient macros for reporting different types of errors
// These automatically include compiler source location information

// General warning macro (type must be specified)
#define ERROR_WARNING_MSG(type, filename, line, column, message, suggestion) \
    error_report(type, ERROR_WARNING, filename, line, column, message, suggestion, __FILE__, __LINE__)

// General error macro (type must be specified)
#define ERROR_ERROR_MSG(type, filename, line, column, message, suggestion) \
    error_report(type, ERROR_ERROR, filename, line, column, message, suggestion, __FILE__, __LINE__)

// General fatal error macro (type must be specified)
#define ERROR_FATAL_MSG(type, filename, line, column, message, suggestion) \
    error_report(type, ERROR_FATAL, filename, line, column, message, suggestion, __FILE__, __LINE__)

// Specific error macros for common error types

// Lexical error (tokenization/character level)
#define ERROR_LEXICAL_ERROR(filename, line, column, message, suggestion) \
    ERROR_ERROR_MSG(ERROR_LEXICAL, filename, line, column, message, suggestion)

// Syntax error (parsing/grammar)
#define ERROR_SYNTAX_ERROR(filename, line, column, message, suggestion) \
    ERROR_ERROR_MSG(ERROR_SYNTAX, filename, line, column, message, suggestion)

// Semantic error (type checking, variable declarations, etc.)
#define ERROR_SEMANTIC_ERROR(filename, line, column, message, suggestion) \
    ERROR_ERROR_MSG(ERROR_SEMANTIC, filename, line, column, message, suggestion)

// Code generation error
#define ERROR_CODEGEN_ERROR(filename, line, column, message, suggestion) \
    ERROR_ERROR_MSG(ERROR_CODEGEN, filename, line, column, message, suggestion)

// I/O error (file not found, permissions, etc.)
#define ERROR_IO_ERROR(filename, line, column, message, suggestion) \
    ERROR_ERROR_MSG(ERROR_IO, filename, line, column, message, suggestion)

// Internal compiler error (compiler bug)
#define ERROR_INTERNAL_ERROR(filename, line, column, message) \
    ERROR_FATAL_MSG(ERROR_INTERNAL, filename, line, column, message, "Please report this bug to the ћ++ compiler team")

// Warning macros for common warning types
#define ERROR_LEXICAL_WARNING(filename, line, column, message, suggestion) \
    ERROR_WARNING_MSG(ERROR_LEXICAL, filename, line, column, message, suggestion)

#define ERROR_SYNTAX_WARNING(filename, line, column, message, suggestion) \
    ERROR_WARNING_MSG(ERROR_SYNTAX, filename, line, column, message, suggestion)

#define ERROR_SEMANTIC_WARNING(filename, line, column, message, suggestion) \
    ERROR_WARNING_MSG(ERROR_SEMANTIC, filename, line, column, message, suggestion)

#define ERROR_CODEGEN_WARNING(filename, line, column, message, suggestion) \
    ERROR_WARNING_MSG(ERROR_CODEGEN, filename, line, column, message, suggestion)

#endif /* ERROR_H */