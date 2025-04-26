/*
 * filename: semantic_analyzer.c
 * 
 * Purpose:
 * Implementation of the semantic analyzer for the ћ++ compiler.
 * This file contains functions that validate the semantic correctness
 * of a ћ++ program after it has been parsed into an AST.
 * 
 * Key Components:
 * - semantic_analyzer_init(): Set up the analyzer with an AST and symbol table
 * - analyze_program(): Main entry point for program analysis
 * - check_function_declaration(): Validate function signatures and bodies
 * - check_external_declaration(): Validate external function declarations
 * - check_statement(): Validate different types of statements
 * - check_array_declaration(): Validate array size and initialization
 * - check_expression(): Validate expressions and function calls
 * 
 * Interactions:
 * - Operates on AST created by parser.c
 * - Uses symbol_table.h/symbol_table.c for identifier management
 * - Uses error.h for reporting semantic errors
 * 
 * Notes:
 * - All values are WORD-sized (architecture dependent)
 * - Special handling for the main function (главна)
 * - Validates array initialization and access boundaries
 */

#include "semantic_analyzer.h"
// Other includes will go here