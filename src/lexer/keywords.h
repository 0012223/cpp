/*
 * filename: keywords.h
 * 
 * Purpose:
 * Header file for keyword definitions and functions in the ћ++ language.
 * Declares the interface for working with language keywords.
 * 
 * Key Components:
 * - TokenType enum: Defines token types for keywords
 * - is_keyword(): Function to check if a string is a language keyword
 * - get_keyword_token(): Function to get the token type for a keyword
 * 
 * Interactions:
 * - Used by lexer.c to identify keywords during tokenization
 */

#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <stdbool.h>

// Define Token Types
typedef enum {
    // Control flow keywords
    TOKEN_IF,            // ако
    TOKEN_ELSE,          // иначе
    TOKEN_WHILE,         // док
    TOKEN_FOR,           // за
    TOKEN_DO,            // ради
    TOKEN_BREAK,         // прекини
    TOKEN_RETURN,        // врати
    
    // Module/scope keywords
    TOKEN_EXTERNAL,      // екстерно
    
    // Boolean literals
    TOKEN_TRUE,          // тачно
    TOKEN_FALSE,         // нетачно
    
    TOKEN_KEYWORD_LAST   // Marker for the last keyword token
} TokenType;

// Function to check if a string is a keyword
bool is_keyword(const char* str);

/**
 * Get the token type for a keyword
 * 
 * @param str The keyword string
 * @return The token type if str is a keyword, or TOKEN_KEYWORD_LAST otherwise
 */
TokenType get_keyword_token(const char* str);

#endif /* KEYWORDS_H */