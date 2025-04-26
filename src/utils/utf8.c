/*
 * filename: utf8.c
 * 
 * Purpose:
 * Implementation of UTF-8 string handling utilities for the ћ++ compiler.
 * Provides functions for working with UTF-8 encoded strings to support
 * the handling of UTF-8 identifiers and keywords in ћ++.
 * 
 * Key Components:
 * - utf8_decode(): Convert a UTF-8 byte sequence to a Unicode codepoint
 * - utf8_encode(): Convert a Unicode codepoint to a UTF-8 byte sequence
 * - utf8_strlen(): Count characters in a UTF-8 string
 * - utf8_strcmp(): Compare two UTF-8 strings properly
 * - utf8_is_alphabetic(): Check if a Unicode codepoint is a letter
 * - utf8_is_identifier_char(): Check if a codepoint can be used in an identifier
 * 
 * Interactions:
 * - Used by lexer.c for tokenizing and recognizing identifiers
 * - Used by symbol_table.c for handling identifier names
 * 
 * Notes:
 * - Handles full Unicode range
 * - Properly validates UTF-8 sequences
 * - Implements Unicode character classifications for identifiers
 */

#include "utf8.h"
// Other includes will go here