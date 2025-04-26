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

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Unicode code point type */
typedef uint32_t utf8_codepoint_t;

/* Maximum number of bytes in a UTF-8 character */
#define UTF8_MAX_BYTES 4

/* Represents a UTF-8 character with its byte sequence and length */
typedef struct {
    char bytes[UTF8_MAX_BYTES];  /* UTF-8 byte sequence */
    int len;                     /* Length in bytes (1-4) */
    utf8_codepoint_t codepoint;  /* Unicode code point */
} utf8_char_t;

/*
 * Decode a UTF-8 character from a string.
 * 
 * Parameters:
 *   str: Pointer to a UTF-8 encoded string
 *   character: Output parameter that will be filled with the decoded character
 * 
 * Returns:
 *   Number of bytes consumed (1-4), or 0 on error
 */
int utf8_decode(const char* str, utf8_char_t* character);

/*
 * Encode a Unicode code point to UTF-8.
 * 
 * Parameters:
 *   codepoint: Unicode code point to encode
 *   character: Output parameter that will be filled with the encoded character
 * 
 * Returns:
 *   Number of bytes written (1-4), or 0 on error
 */
int utf8_encode(utf8_codepoint_t codepoint, utf8_char_t* character);

/*
 * Get the length of a UTF-8 string in characters (not bytes).
 * 
 * Parameters:
 *   str: UTF-8 encoded string
 * 
 * Returns:
 *   Number of characters in the string
 */
size_t utf8_strlen(const char* str);

/*
 * Compare two UTF-8 strings.
 * 
 * Parameters:
 *   str1: First UTF-8 string
 *   str2: Second UTF-8 string
 * 
 * Returns:
 *   < 0 if str1 < str2
 *   = 0 if str1 = str2
 *   > 0 if str1 > str2
 */
int utf8_strcmp(const char* str1, const char* str2);

/*
 * Check if a Unicode code point is alphabetic.
 * 
 * Parameters:
 *   codepoint: Unicode code point to check
 * 
 * Returns:
 *   true if the character is alphabetic, false otherwise
 */
bool utf8_is_alphabetic(utf8_codepoint_t codepoint);

/*
 * Check if a Unicode code point is a digit.
 * 
 * Parameters:
 *   codepoint: Unicode code point to check
 * 
 * Returns:
 *   true if the character is a digit, false otherwise
 */
bool utf8_is_digit(utf8_codepoint_t codepoint);

/*
 * Check if a Unicode code point is alphanumeric.
 * 
 * Parameters:
 *   codepoint: Unicode code point to check
 * 
 * Returns:
 *   true if the character is alphanumeric, false otherwise
 */
bool utf8_is_alphanumeric(utf8_codepoint_t codepoint);

/*
 * Check if a Unicode code point is valid for an identifier in ћ++.
 * 
 * Parameters:
 *   codepoint: Unicode code point to check
 *   is_start: true if checking for the first character of an identifier
 * 
 * Returns:
 *   true if the character is valid for an identifier, false otherwise
 */
bool utf8_is_identifier_char(utf8_codepoint_t codepoint, bool is_start);

/*
 * Advance to the next UTF-8 character in a string.
 * 
 * Parameters:
 *   str: Pointer to a pointer to the current position in a UTF-8 string
 *   character: Output parameter that will be filled with the next character
 * 
 * Returns:
 *   true if a character was read, false on end of string or error
 */
bool utf8_next_char(const char** str, utf8_char_t* character);

/*
 * Check if a byte is a UTF-8 continuation byte (10xxxxxx)
 */
#define UTF8_IS_CONTINUATION(byte) (((byte) & 0xC0) == 0x80)

#endif /* UTF8_H */