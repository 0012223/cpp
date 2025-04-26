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
// Other includes will go here