/*
 * filename: ast.c
 * 
 * Purpose:
 * Implementation of the Abstract Syntax Tree (AST) for the ћ++ compiler.
 * This file contains functions to create, manipulate, and free AST nodes
 * that represent the structure of a ћ++ program.
 * 
 * Key Components:
 * - ast_create_program(): Create the root node of the AST
 * - ast_create_function(): Create a function declaration node
 * - ast_create_extern_declaration(): Create an external function declaration node
 * - ast_create_statement_block(): Create a block of statements (< >)
 * - ast_create_array_declaration(): Create an array declaration node
 * - ast_create_expression(): Create various expression nodes
 * - ast_free(): Release memory used by the AST
 * 
 * Interactions:
 * - Used by parser.c to build the program representation
 * - AST is consumed by semantic_analyzer.c and ir.c
 * 
 * Notes:
 * - All string data (identifiers, etc.) is stored as UTF-8
 * - Memory management is handled explicitly with creation/free functions
 * - Node types reflect ћ++ specific constructs like array declarations
 */

#include "ast.h"
// Other includes will go here