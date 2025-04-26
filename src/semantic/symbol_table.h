/*
 * filename: symbol_table.h
 * 
 * Purpose:
 * Header file for the symbol table implementation in the ћ++ compiler.
 * Defines the interface for tracking identifiers and their properties
 * throughout the compilation process.
 * 
 * Key Components:
 * - SymbolType enum: Categorizes symbols (variable, array, function, extern)
 * - Symbol struct: Represents a single symbol with name and properties
 * - SymbolTable struct: Contains all symbols with scope management
 * - symbol_table_init(): Create a new symbol table
 * - symbol_table_enter_scope(): Create a new nested scope
 * - symbol_table_exit_scope(): Return to the parent scope
 * - symbol_table_add(): Add a symbol to the current scope
 * - symbol_table_lookup(): Find a symbol by name in current or parent scopes
 * 
 * Interactions:
 * - Used by semantic_analyzer.c to track and validate identifiers
 * - Used by ir.c for identifier lookups during IR generation
 * 
 * Notes:
 * - Supports UTF-8 identifiers for symbol names
 * - Implements scope nesting for proper variable shadowing
 * - All identifiers use WORD-sized storage (native machine word)
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// Header content will be implemented later

#endif /* SYMBOL_TABLE_H */