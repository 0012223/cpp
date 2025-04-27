/*
 * filename: keywords.h
 * 
 * Purpose:
 * Header file for keyword definitions and functions in the ћ++ language.
 * Declares the interface for working with language keywords.
 * 
 * Key Components:
 * - is_keyword(): Function to check if a string is a language keyword
 * - get_keyword_token(): Function to get the token type for a keyword
 * - get_keyword_string(): Function to get the keyword string for a token type
 * 
 * Interactions:
 * - Used by lexer.c to identify keywords during tokenization
 */

#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stdbool.h>
#include "lexer.h" // Include lexer.h for TokenType definition

/**
 * Check if a string is a keyword in the ћ++ language
 * 
 * @param str The string to check
 * @return true if the string is a keyword, false otherwise
 */
bool is_keyword(const char* str);

/**
 * Get the token type for a keyword
 * 
 * @param str The keyword string
 * @return The token type if str is a keyword, or TOKEN_KEYWORD_LAST otherwise
 */
TokenType get_keyword_token(const char* str);

/**
 * Get the keyword string for a token type
 * 
 * @param type The token type
 * @return The corresponding keyword string, or NULL if not a keyword token
 */
const char* get_keyword_string(TokenType type);

/**
 * Print all keywords defined in the ћ++ language
 */
void print_all_keywords(void);

#endif /* KEYWORDS_H */