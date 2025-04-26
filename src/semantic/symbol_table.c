/*
 * filename: symbol_table.c
 * 
 * Purpose:
 * Implementation of the symbol table for the ћ++ compiler.
 * This file provides functions for tracking and looking up identifiers
 * with proper scope handling.
 * 
 * Key Components:
 * - symbol_table_init(): Create and initialize a new symbol table
 * - symbol_table_free(): Free all memory used by the symbol table
 * - symbol_table_enter_scope(): Create a new scope level
 * - symbol_table_exit_scope(): Exit current scope and return to parent
 * - symbol_table_add(): Add a new symbol to the current scope
 * - symbol_table_lookup(): Find a symbol by name across accessible scopes
 * - symbol_table_get_current_scope(): Get the current scope level
 * 
 * Interactions:
 * - Used by semantic_analyzer.c during program analysis
 * - Used by ir.c during intermediate code generation
 * 
 * Notes:
 * - Implements a linked hash table for efficient lookups
 * - Handles UTF-8 symbol names with proper string comparison
 * - Maintains scope hierarchy for correct symbol resolution
 */

#include "symbol_table.h"
// Other includes will go here