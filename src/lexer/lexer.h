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

// Header content will be implemented later

#endif /* LEXER_H */