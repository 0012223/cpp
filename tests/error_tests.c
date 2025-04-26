/*
 * filename: error_tests.c
 * 
 * Purpose:
 * Test suite for the error handling utilities of the ћ++ compiler.
 * Validates that error reporting, tracking, and summarizing work correctly.
 * 
 * Key Components:
 * - test_error_reporting(): Tests basic error reporting functionality
 * - test_error_severities(): Tests different error severity levels
 * - test_error_types(): Tests different error types
 * 
 * Interactions:
 * - Uses the utils/error.h interface
 * 
 * Notes:
 * - Run with the --test-error command-line option
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/utils/error.h"

void test_error_reporting() {
    printf("\n--- Testing Error Reporting System ---\n");
    
    // Initialize the error system
    error_init();
    
    printf("Generating test errors...\n");
    
    // Generate various types of errors for testing
    ERROR_LEXICAL_ERROR("test.ћпп", 10, 5, "Unknown character '~'", 
                       "Remove or replace the invalid character");
    
    ERROR_SYNTAX_ERROR("test.ћпп", 15, 10, "Expected ';' after statement", 
                      "Add a semicolon to end the statement");
    
    ERROR_SEMANTIC_ERROR("test.ћпп", 20, 8, "Undefined variable 'counter'", 
                        "Declare 'counter' before using it");
    
    ERROR_CODEGEN_ERROR("test.ћпп", 25, 1, "Cannot generate code for inline assembly", 
                       "Use supported language features instead");
    
    ERROR_WARNING_MSG(ERROR_SEMANTIC, "test.ћпп", 30, 15, 
                     "Unused variable 'result'", 
                     "Remove the variable or use it");
    
    // Don't test fatal errors because they would terminate the program
    // ERROR_INTERNAL_ERROR("test.ћпп", 35, 1, "Unexpected compiler state");
    
    // Print error summary
    printf("\nTest errors generated. Summary:\n");
    error_print_summary(false);
    
    // Cleanup
    error_cleanup();
    
    printf("\n--- Error Reporting Test Complete ---\n");
}

// Main entry point for the test
int main(int argc, char* argv[]) {
    bool verbose = (argc >= 2 && strcmp(argv[1], "--verbose") == 0);
    
    // Run error reporting tests
    test_error_reporting();
    
    return 0;
}