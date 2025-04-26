/*
 * filename: error.c
 * 
 * Purpose:
 * Implementation of error handling utilities for the ћ++ compiler.
 * Provides functions to report, track, and display compilation errors.
 * 
 * Key Components:
 * - error_init(): Initialize the error tracking system
 * - error_report(): Log an error with position and message
 * - error_warning(): Report a non-fatal warning
 * - error_syntax(): Report a syntax error
 * - error_semantic(): Report a semantic error
 * - error_get_count(): Return the number of errors encountered
 * - error_print(): Print a formatted error message
 * - error_print_summary(): Print a summary of all errors after compilation
 * 
 * Interactions:
 * - Used by all compiler components to report issues
 * - Coordinates with main.c to handle error thresholds
 * 
 * Notes:
 * - Handles UTF-8 in error messages and file paths
 * - Maintains a list of all errors for final reporting
 * - Color-codes output when terminal supports it
 * - Can provide context from source code in error messages
 */

#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>

// ANSI color codes for terminal output
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"

// Define the maximum number of errors to track
#define MAX_ERRORS 500

// Static array to store errors
static Error error_list[MAX_ERRORS];
static int error_count = 0;
static bool use_colors = true;
static FILE* error_log_file = NULL;
static bool generate_log_file = false;

// Forward declarations
static void error_print(const Error* error);
static const char* error_type_to_string(ErrorType type);
static const char* error_severity_to_string(ErrorSeverity severity);
static const char* get_error_color(ErrorSeverity severity);
static void strip_path(const char* path, char* buffer, size_t size);
static char* safe_strdup(const char* str);

/**
 * Safe version of strdup that handles NULL
 */
static char* safe_strdup(const char* str) {
    if (str == NULL) return NULL;
    
    size_t len = strlen(str) + 1;
    char* new_str = malloc(len);
    if (new_str == NULL) return NULL;
    
    memcpy(new_str, str, len);
    return new_str;
}

/**
 * Initialize the error handling system
 */
bool error_init_with_log(bool create_log_file) {
    error_count = 0;
    generate_log_file = create_log_file;
    
    // Try to determine if terminal supports colors
    #ifdef _WIN32
        // On Windows, check if ANSICON or similar is used
        use_colors = (getenv("ANSICON") != NULL || getenv("ConEmuANSI") != NULL);
    #else
        // On Unix-like systems, check if TERM supports colors
        const char* term = getenv("TERM");
        use_colors = (term != NULL && strcmp(term, "dumb") != 0);
    #endif
    
    // Open error log file only if the flag is set
    if (generate_log_file) {
        time_t now = time(NULL);
        struct tm* tm_now = localtime(&now);
        char log_filename[100];
        strftime(log_filename, sizeof(log_filename), "ћпп_error_log_%Y%m%d_%H%M%S.txt", tm_now);
        
        error_log_file = fopen(log_filename, "w");
        if (error_log_file == NULL) {
            fprintf(stderr, "Warning: Could not create error log file\n");
            // Non-fatal, continue without log file
        } else {
            // Write log file header
            fprintf(error_log_file, "ћ++ Compiler Error Log\n");
            fprintf(error_log_file, "====================\n");
            fprintf(error_log_file, "Date: %s\n\n", ctime(&now));
        }
    }
    
    return true;
}

/**
 * Backward compatibility wrapper for error_init
 */
bool error_init(void) {
    // By default, don't generate log file
    return error_init_with_log(false);
}

/**
 * Clean up the error handling system
 */
void error_cleanup(void) {
    // Free all error strings
    for (int i = 0; i < error_count; i++) {
        free((void*)error_list[i].filename);
        free((void*)error_list[i].message);
        if (error_list[i].suggestion) free((void*)error_list[i].suggestion);
        free((void*)error_list[i].compiler_file);
    }
    
    // Close log file if it was opened
    if (error_log_file != NULL) {
        fclose(error_log_file);
        error_log_file = NULL;
    }
}

/**
 * Report an error
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
) {
    // Check if we have room for more errors
    if (error_count >= MAX_ERRORS) {
        // Can't record more errors, just print a message
        fprintf(stderr, "Too many errors, stopping error tracking.\n");
        return false;
    }
    
    // Create a new error
    Error* error = &error_list[error_count++];
    error->type = type;
    error->severity = severity;
    error->filename = safe_strdup(filename ? filename : "<unknown>");
    error->line = line;
    error->column = column;
    error->message = safe_strdup(message ? message : "<no message>");
    error->suggestion = suggestion ? safe_strdup(suggestion) : NULL;
    error->compiler_file = safe_strdup(compiler_file ? compiler_file : "<unknown>");
    error->compiler_line = compiler_line;
    
    // Print the error immediately
    error_print(error);
    
    // For fatal errors, exit immediately
    if (severity == ERROR_FATAL) {
        fprintf(stderr, "Fatal error encountered, stopping compilation.\n");
        error_cleanup();
        exit(EXIT_FAILURE);
    }
    
    return true;
}

/**
 * Get the number of errors of a certain severity
 */
int error_get_count(int severity) {
    if (severity < 0) {
        // Return total count of all errors
        return error_count;
    }
    
    // Count errors of a specific severity
    int count = 0;
    for (int i = 0; i < error_count; i++) {
        if ((int)error_list[i].severity == severity) {
            count++;
        }
    }
    
    return count;
}

/**
 * Print a summary of all errors encountered
 */
void error_print_summary(bool verbose) {
    // Count errors by severity
    int warnings = error_get_count(ERROR_WARNING);
    int errors = error_get_count(ERROR_ERROR);
    int fatals = error_get_count(ERROR_FATAL);
    
    // Print summary to stderr
    fprintf(stderr, "\n=== Compilation Summary ===\n");
    fprintf(stderr, "Total issues: %d\n", error_count);
    fprintf(stderr, "  Warnings: %d\n", warnings);
    fprintf(stderr, "  Errors:   %d\n", errors);
    fprintf(stderr, "  Fatal:    %d\n", fatals);
    
    // Also log to file if available
    if (error_log_file != NULL) {
        fprintf(error_log_file, "\n=== Compilation Summary ===\n");
        fprintf(error_log_file, "Total issues: %d\n", error_count);
        fprintf(error_log_file, "  Warnings: %d\n", warnings);
        fprintf(error_log_file, "  Errors:   %d\n", errors);
        fprintf(error_log_file, "  Fatal:    %d\n", fatals);
    }
    
    // If verbose, print every error again
    if (verbose && error_count > 0) {
        fprintf(stderr, "\n=== Error Details ===\n");
        for (int i = 0; i < error_count; i++) {
            error_print(&error_list[i]);
        }
    }
}

/**
 * Print a formatted error message
 */
static void error_print(const Error* error) {
    if (!error) return;
    
    // Get string representations
    const char* severity_str = error_severity_to_string(error->severity);
    const char* type_str = error_type_to_string(error->type);
    
    // Create a stripped version of the filename
    char short_filename[256];
    strip_path(error->filename, short_filename, sizeof(short_filename));
    
    // Create a stripped version of the compiler file
    char short_compiler_file[256];
    strip_path(error->compiler_file, short_compiler_file, sizeof(short_compiler_file));
    
    // Print to stderr with or without colors
    if (use_colors) {
        const char* color = get_error_color(error->severity);
        
        fprintf(stderr, "%s%s%s [%s] in %s:%d:%d: %s%s%s\n",
                COLOR_BOLD, color, severity_str, type_str,
                short_filename, error->line, error->column,
                error->message, COLOR_RESET, COLOR_RESET);
        
        // Print suggestion if available
        if (error->suggestion) {
            fprintf(stderr, "  %sSuggestion:%s %s\n", 
                   COLOR_CYAN, COLOR_RESET, error->suggestion);
        }
        
        // Print compiler source location in debug builds
        #ifdef DEBUG
        fprintf(stderr, "  %sReported from:%s %s:%d\n",
               COLOR_BLUE, COLOR_RESET, short_compiler_file, error->compiler_line);
        #endif
    } else {
        fprintf(stderr, "%s [%s] in %s:%d:%d: %s\n",
                severity_str, type_str,
                short_filename, error->line, error->column,
                error->message);
        
        // Print suggestion if available
        if (error->suggestion) {
            fprintf(stderr, "  Suggestion: %s\n", error->suggestion);
        }
        
        // Print compiler source location in debug builds
        #ifdef DEBUG
        fprintf(stderr, "  Reported from: %s:%d\n",
               short_compiler_file, error->compiler_line);
        #endif
    }
    
    // Log to file if available
    if (error_log_file != NULL) {
        fprintf(error_log_file, "%s [%s] in %s:%d:%d: %s\n",
                severity_str, type_str,
                error->filename, error->line, error->column,
                error->message);
        
        if (error->suggestion) {
            fprintf(error_log_file, "  Suggestion: %s\n", error->suggestion);
        }
        
        fprintf(error_log_file, "  Reported from: %s:%d\n",
               error->compiler_file, error->compiler_line);
    }
}

/**
 * Convert error type to string
 */
static const char* error_type_to_string(ErrorType type) {
    switch (type) {
        case ERROR_LEXICAL:   return "Lexical";
        case ERROR_SYNTAX:    return "Syntax";
        case ERROR_SEMANTIC:  return "Semantic";
        case ERROR_CODEGEN:   return "CodeGen";
        case ERROR_IO:        return "IO";
        case ERROR_INTERNAL:  return "Internal";
        default:              return "Unknown";
    }
}

/**
 * Convert error severity to string
 */
static const char* error_severity_to_string(ErrorSeverity severity) {
    switch (severity) {
        case ERROR_WARNING:   return "Warning";
        case ERROR_ERROR:     return "Error";
        case ERROR_FATAL:     return "Fatal Error";
        default:              return "Unknown";
    }
}

/**
 * Get color code for an error severity
 */
static const char* get_error_color(ErrorSeverity severity) {
    switch (severity) {
        case ERROR_WARNING:   return COLOR_YELLOW;
        case ERROR_ERROR:     return COLOR_RED;
        case ERROR_FATAL:     return COLOR_RED;
        default:              return COLOR_RESET;
    }
}

/**
 * Strip directory path from a filename
 */
static void strip_path(const char* path, char* buffer, size_t size) {
    if (!path || !buffer || size == 0) return;
    
    // Find the last separator
    const char* last_sep = strrchr(path, '/');
    #ifdef _WIN32
    const char* last_sep_win = strrchr(path, '\\');
    if (last_sep_win > last_sep) last_sep = last_sep_win;
    #endif
    
    // Copy the filename part
    if (last_sep) {
        strncpy(buffer, last_sep + 1, size - 1);
    } else {
        strncpy(buffer, path, size - 1);
    }
    
    buffer[size - 1] = '\0'; // Ensure null termination
}