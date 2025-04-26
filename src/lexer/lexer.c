/*
 * filename: lexer.c
 * 
 * Purpose:
 * Implementation of the lexical analyzer for the ћ++ compiler.
 * This file contains the implementation of functions declared in lexer.h,
 * responsible for breaking down the source code into tokens with UTF-8 support.
 * 
 * Key Components:
 * - lexer_init(): Initialize lexer state with input source
 * - lexer_next_token(): Main token extraction function
 * - is_identifier_start(): Checks if a UTF-8 character can start an identifier
 * - is_identifier_part(): Checks if a UTF-8 character can be part of an identifier
 * - scan_identifier(): Extracts an identifier or keyword
 * - scan_number(): Extracts a numeric literal
 * - scan_character_literal(): Extracts a character literal
 * 
 * Interactions:
 * - Uses utf8.h for UTF-8 character processing
 * - Uses keywords.c to identify language keywords
 * - Uses error.h to report lexical errors
 * 
 * Notes:
 * - Handles UTF-8 encoded identifiers and keywords
 * - All numeric values are treated as WORD-sized (architecture dependent)
 * - Special handling for angle brackets (< >) which replace curly braces
 * - Special handling for array syntax (низ:number: = _values_)
 */

#include "lexer.h"
// Other includes will go here