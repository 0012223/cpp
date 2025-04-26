/*
 * filename: ir.c
 * 
 * Purpose:
 * Implementation of the Intermediate Representation (IR) generator for the ћ++ compiler.
 * This file contains functions that translate the AST into a three-address code
 * representation that's easier to optimize and translate to assembly.
 * 
 * Key Components:
 * - ir_generate_program(): Entry point for IR generation from an AST
 * - ir_generate_function(): Generate IR for a function definition
 * - ir_generate_statement(): Generate IR for statements (conditionals, loops, etc.)
 * - ir_generate_expression(): Generate IR for expressions with temporary variables
 * - ir_create_basic_block(): Create and manage basic blocks for control flow
 * - ir_optimize(): Apply basic optimizations to the generated IR
 * 
 * Interactions:
 * - Takes input from the semantic analyzer's validated AST
 * - Uses symbol_table.h for variable information
 * - Output is used by codegen components for target-specific code generation
 * 
 * Notes:
 * - All operations are on WORD-sized values (architecture dependent)
 * - Uses SSA (Static Single Assignment) form for better optimization potential
 * - Special handling for array operations and function calls
 */

#include "ir.h"
// Other includes will go here