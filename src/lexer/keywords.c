/*
 * filename: keywords.c
 * 
 * Purpose:
 * Defines and manages the keywords and their mappings in the ћ++ language.
 * This file maintains a table of all language keywords in UTF-8 format and
 * provides functions to check if an identifier matches a keyword.
 * 
 * Key Components:
 * - Keyword struct: Maps a keyword string to its token type
 * - keywords_table[]: Array of all language keywords
 * - is_keyword(): Checks if a string is a language keyword
 * - get_keyword_token(): Returns the token type for a keyword
 * 
 * Interactions:
 * - Used by lexer.c to identify keywords during tokenization
 * - Relies on utf8.h for string comparison with UTF-8 support
 * 
 * Notes:
 * - All keywords are stored in UTF-8 format
 * - Keywords include: екстерно (external), врати (return), низ (array), etc.
 * - Case sensitivity is enforced for keywords
 */

#include "lexer.h"
#include "../utils/utf8.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

// Keyword struct maps a keyword string to its token type
typedef struct {
    const char* word;    // UTF-8 keyword string
    TokenType type;      // Corresponding token type
} Keyword;

// Table of all keywords in the ћ++ language
static const Keyword keywords_table[] = {
    // Control flow keywords
    {"ако",             TOKEN_IF},
    {"иначе",           TOKEN_ELSE},
    {"док",             TOKEN_WHILE},
    {"за",              TOKEN_FOR},
    {"ради",            TOKEN_DO},
    {"прекини",         TOKEN_BREAK},
    {"врати",           TOKEN_RETURN},
    
    // Module/scope keywords
    {"екстерно",        TOKEN_EXTERNAL},
    
    // Boolean literals
    {"тачно",           TOKEN_TRUE},
    {"нетачно",         TOKEN_FALSE},
};

// Number of keywords in the table
static const int KEYWORDS_COUNT = sizeof(keywords_table) / sizeof(keywords_table[0]);

/**
 * Check if a string is a keyword in the ћ++ language
 * 
 * @param str The string to check
 * @return true if the string is a keyword, false otherwise
 */
bool is_keyword(const char* str) {
    if (!str) return false;
    
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (utf8_strcmp(str, keywords_table[i].word) == 0) {
            return true;
        }
    }
    
    return false;
}

/**
 * Get the token type for a keyword
 * 
 * @param str The keyword string
 * @return The token type if str is a keyword, or an error value otherwise
 */
TokenType get_keyword_token(const char* str) {
    if (!str) return TOKEN_KEYWORD_LAST; // Return sentinel value for error
    
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (utf8_strcmp(str, keywords_table[i].word) == 0) {
            return keywords_table[i].type;
        }
    }
    
    // Not a keyword, return sentinel value
    return TOKEN_KEYWORD_LAST;
}

/**
 * Get the keyword string for a token type
 * 
 * @param type The token type
 * @return The corresponding keyword string, or NULL if not a keyword token
 */
const char* get_keyword_string(TokenType type) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (keywords_table[i].type == type) {
            return keywords_table[i].word;
        }
    }
    
    return NULL; // Not a keyword token
}

/**
 * Print all keywords defined in the ћ++ language
 * 
 * Outputs all keywords from the keywords table to stdout,
 * useful for debugging and documentation purposes.
 */
void print_all_keywords(void) {
    printf("ћ++ language keywords (%d total):\n", KEYWORDS_COUNT);
    printf("--------------------------------\n");
    
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        printf("%d. %s\n", i + 1, keywords_table[i].word);
    }
}