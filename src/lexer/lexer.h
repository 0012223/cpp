/*
 * filename: lexer.h
 * 
 * Purpose:
 * Header file defining the lexical analyzer interface for the ћ++ compiler.
 * This file contains token definitions, lexer state structures, and function
 * prototypes for tokenizing ћ++ source code with UTF-8 support.
 * 
 * Key Components:
 * - TokenType enum: Defines all possible token categories (identifiers, keywords, operators, etc.)
 * - Token struct: Represents a single lexical token with type, value, and position information
 * - Lexer struct: Maintains the state of the lexical analyzer during tokenization
 * - lexer_init(): Initialize a new lexer with source code
 * - lexer_next_token(): Extract the next token from the source
 * - lexer_peek_token(): Look ahead at the next token without consuming it
 * 
 * Interactions:
 * - Used by parser.c to obtain a stream of tokens for syntax analysis
 * - Utilizes utf8.h for proper UTF-8 character handling
 * - Uses error.h for reporting lexical errors
 * 
 * Notes:
 * - All tokens must handle UTF-8 encoded identifiers and keywords
 * - Character literals are stored as WORD-sized values (native machine word)
 * - Numeric literals are assumed to fit in a machine word
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../utils/utf8.h"
#include "../target/target_info.h"

// Token type definitions - combined with keywords.h TokenType
typedef enum {
    // Special tokens
    TOKEN_EOF,           // End of file
    TOKEN_ERROR,         // Error token
    
    // Identifiers and literals
    TOKEN_IDENTIFIER,    // Variable or function name
    TOKEN_NUMBER,        // Numeric literal
    TOKEN_CHAR_LITERAL,  // Character literal
    TOKEN_STRING,        // String literal
    
    // Operators
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_STAR,          // *
    TOKEN_SLASH,         // /
    TOKEN_PERCENT,       // %
    TOKEN_EQUALS,        // =
    TOKEN_DOUBLE_EQUALS, // ==
    TOKEN_NOT_EQUALS,    // !=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUALS,   // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUALS,// >=
    TOKEN_AND,           // &
    TOKEN_DOUBLE_AND,    // &&
    TOKEN_OR,            // |
    TOKEN_DOUBLE_OR,     // ||
    TOKEN_NOT,           // !
    TOKEN_CARET,         // ^
    TOKEN_TILDE,         // ~
    
    // Delimiters
    TOKEN_LEFT_PAREN,    // (
    TOKEN_RIGHT_PAREN,   // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_LEFT_ANGLE,    // < (when used as a brace)
    TOKEN_RIGHT_ANGLE,   // > (when used as a brace)
    TOKEN_SEMICOLON,     // ;
    TOKEN_COMMA,         // ,
    TOKEN_DOT,           // .
    TOKEN_COLON,         // :
    
    // Array-specific tokens
    TOKEN_ARRAY,         // низ
    
    // Keywords (imported from keywords.h)
    TOKEN_IF = 1000,     // ако
    TOKEN_ELSE,          // иначе
    TOKEN_WHILE,         // док
    TOKEN_FOR,           // за
    TOKEN_DO,            // ради
    TOKEN_BREAK,         // прекини
    TOKEN_RETURN,        // врати
    TOKEN_EXTERNAL,      // екстерно
    TOKEN_TRUE,          // тачно
    TOKEN_FALSE,         // нетачно
    
    TOKEN_KEYWORD_LAST   // Marker for the last keyword token
} TokenType;

// Token structure - represents a single lexical token
typedef struct {
    TokenType type;          // Type of the token
    const char* lexeme;      // The string of characters from source
    size_t lexeme_length;    // Length of the lexeme in bytes
    int line;                // Line number in source (1-based)
    int column;              // Column number in source (1-based)
    union {
        intptr_t int_value;  // Integer value for numeric literals
        char* string_value;  // String value (allocated)
        int32_t char_value;  // Character value for char literals (UTF-8 codepoint)
    } value;                 // Value associated with the token (for literals)
} Token;

// Forward declaration for LexerState to avoid circular dependency
typedef struct LexerState LexerState;

// Lexer state structure
struct LexerState {
    const char* source;      // Source code as a string
    const char* filename;    // Source filename for error reporting
    size_t source_length;    // Length of source in bytes
    size_t current;          // Current position in source
    size_t start;            // Start of the current lexeme
    int line;                // Current line number (1-based)
    int column;              // Current column number (1-based)
    int previous_column;     // Previous column, for tracking newlines
    Token current_token;     // Currently processed token
    Token next_token;        // Next token (for peek)
    bool has_next_token;     // Whether next_token has been filled
    TargetInfo target_info;  // Target architecture information (for word size)
};

/**
 * Initialize a new lexer with the given source code
 *
 * @param source The source code to tokenize
 * @param filename The name of the source file (for error reporting)
 * @param target_info Target architecture information
 * @return A new lexer state, or NULL if initialization failed
 */
LexerState* lexer_init(FILE* source, const char* filename, TargetInfo target_info);

/**
 * Clean up and free resources used by the lexer
 *
 * @param lexer The lexer state to clean up
 */
void lexer_cleanup(LexerState* lexer);

/**
 * Get the next token from the source code
 *
 * @param lexer The lexer state
 * @return The next token
 */
Token lexer_next_token(LexerState* lexer);

/**
 * Peek at the next token without consuming it
 *
 * @param lexer The lexer state
 * @return The next token, which will be returned again by the next call to lexer_next_token
 */
Token lexer_peek_token(LexerState* lexer);

/**
 * Get a string representation of a token type
 *
 * @param type The token type
 * @return A string representation of the token type
 */
const char* token_type_to_string(TokenType type);

/**
 * Print a token to stdout (for debugging)
 *
 * @param token The token to print
 */
void token_print(Token token);

/**
 * Print all tokens in the source code (for debugging)
 *
 * @param lexer The lexer state
 */
void lexer_print_all_tokens(LexerState* lexer);

#endif /* LEXER_H */