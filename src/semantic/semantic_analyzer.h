/*
 * filename: semantic_analyzer.h
 * 
 * Purpose:
 * Header file for the semantic analyzer of the ћ++ compiler.
 * Defines the interface for semantic checking and validation of the AST.
 * 
 * Key Components:
 * - SemanticAnalyzer struct: Maintains state during semantic analysis
 * - semantic_analyzer_init(): Initialize a new semantic analyzer
 * - analyze_program(): Entry point for analyzing a complete program
 * - check_function_declaration(): Validate function declarations
 * - check_statement(): Validate statements for semantic correctness
 * - check_expression(): Validate and annotate expressions
 * 
 * Interactions:
 * - Takes input from parser.c in the form of an AST
 * - Uses symbol_table.h to track identifiers
 * - Uses error.h for reporting semantic errors
 * 
 * Notes:
 * - All identifiers are WORD-sized (native machine word)
 * - Special handling for external declarations
 * - Validates parameter usage and array access
 */

#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

// Header content will be implemented later

#endif /* SEMANTIC_ANALYZER_H */