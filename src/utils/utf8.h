/*
 * filename: utf8.h
 * 
 * Purpose:
 * Header file for UTF-8 string handling utilities in the ћ++ compiler.
 * Provides functions for working with UTF-8 encoded strings, which is
 * essential for handling identifiers and keywords in the ћ++ language.
 * 
 * Key Components:
 * - utf8_char struct: Represents a single UTF-8 character
 * - utf8_decode(): Decode a UTF-8 codepoint from a byte sequence
 * - utf8_encode(): Encode a codepoint into a UTF-8 byte sequence
 * - utf8_strlen(): Count characters (not bytes) in a UTF-8 string
 * - utf8_strcmp(): Compare two UTF-8 strings
 * - utf8_is_alphabetic(): Check if a UTF-8 character is a letter
 * 
 * Interactions:
 * - Used by lexer.c for tokenizing UTF-8 identifiers and keywords
 * - Used by symbol_table.c for comparing identifier names
 * 
 * Notes:
 * - Handles the full Unicode range for identifiers and keywords
 * - Implements proper character-level operations on UTF-8 strings
 * - Provides utility functions for the rest of the compiler
 */

#ifndef UTF8_H
#define UTF8_H

// Header content will be implemented later

#endif /* UTF8_H */