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
#include "keywords.h"
#include "../utils/error.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Maximum token length for error reporting
#define MAX_ERROR_TOKEN_LENGTH 50

// Forward declarations for internal lexer functions
static Token scan_token(LexerState* lexer);
static Token scan_identifier(LexerState* lexer);
static Token scan_number(LexerState* lexer);
static Token scan_string(LexerState* lexer);
static Token scan_character_literal(LexerState* lexer);
static bool is_identifier_start(int32_t c);
static bool is_identifier_part(int32_t c);
static int32_t advance(LexerState* lexer);
static int32_t advance_utf8(LexerState* lexer);
static int32_t peek(LexerState* lexer);
static int32_t peek_utf8_char(LexerState* lexer);
static int32_t peek_next(LexerState* lexer);
static bool match(LexerState* lexer, char expected);
static void skip_whitespace(LexerState* lexer);
static Token make_token(LexerState* lexer, TokenType type);
static Token error_token(LexerState* lexer, const char* message);

/**
 * Determines the number of bytes in a UTF-8 character based on the first byte
 *
 * @param first_byte The first byte of the UTF-8 character
 * @return The number of bytes in the UTF-8 character
 */
static int determine_utf8_bytes(uint8_t first_byte) {
    if ((first_byte & 0x80) == 0) {          // 0xxxxxxx
        return 1;
    } else if ((first_byte & 0xE0) == 0xC0) { // 110xxxxx
        return 2;
    } else if ((first_byte & 0xF0) == 0xE0) { // 1110xxxx
        return 3;
    } else if ((first_byte & 0xF8) == 0xF0) { // 11110xxx
        return 4;
    }
    // Invalid UTF-8 sequence, treat as a single byte
    return 1;
}

/**
 * Peeks at the next UTF-8 character without advancing the lexer position
 *
 * @param lexer The lexer state
 * @return The Unicode codepoint of the next character, or EOF
 */
static int32_t peek_utf8_char(LexerState* lexer) {
    if (lexer->current >= lexer->source_length) {
        return EOF;
    }
    
    uint8_t first_byte = (uint8_t)lexer->source[lexer->current];
    int num_bytes = determine_utf8_bytes(first_byte);
    
    // Check if we have enough bytes left
    if (lexer->current + num_bytes > lexer->source_length) {
        // Incomplete UTF-8 sequence at end of file
        return EOF;
    }
    
    int32_t codepoint = 0;
    
    // Decode the UTF-8 sequence into a Unicode codepoint
    if (num_bytes == 1) {
        codepoint = first_byte;
    } else if (num_bytes == 2) {
        codepoint = ((first_byte & 0x1F) << 6) | ((uint8_t)lexer->source[lexer->current + 1] & 0x3F);
    } else if (num_bytes == 3) {
        codepoint = ((first_byte & 0x0F) << 12) | 
                   (((uint8_t)lexer->source[lexer->current + 1] & 0x3F) << 6) | 
                   ((uint8_t)lexer->source[lexer->current + 2] & 0x3F);
    } else if (num_bytes == 4) {
        codepoint = ((first_byte & 0x07) << 18) | 
                   (((uint8_t)lexer->source[lexer->current + 1] & 0x3F) << 12) | 
                   (((uint8_t)lexer->source[lexer->current + 2] & 0x3F) << 6) | 
                   ((uint8_t)lexer->source[lexer->current + 3] & 0x3F);
    }
    
    return codepoint;
}

/**
 * Initialize a new lexer with the given source code
 *
 * @param source The source file to tokenize
 * @param filename The name of the source file (for error reporting)
 * @param target_info Target architecture information
 * @return A new lexer state, or NULL if initialization failed
 */
LexerState* lexer_init(FILE* source, const char* filename, TargetInfo target_info) {
    if (!source) {
        error_report(ERROR_IO, ERROR_FATAL, filename, 0, 0,
                    "Failed to open source file",
                    "Check that the file exists and has read permissions",
                    __FILE__, __LINE__);
        return NULL;
    }
    
    // Determine file size
    fseek(source, 0, SEEK_END);
    long file_size = ftell(source);
    fseek(source, 0, SEEK_SET);
    
    if (file_size < 0) {
        error_report(ERROR_IO, ERROR_FATAL, filename, 0, 0,
                    "Failed to determine source file size",
                    "Check file permissions and disk space",
                    __FILE__, __LINE__);
        return NULL;
    }
    
    // Allocate memory for the source code
    char* source_code = (char*)malloc(file_size + 1);
    if (!source_code) {
        error_report(ERROR_INTERNAL, ERROR_FATAL, filename, 0, 0,
                    "Failed to allocate memory for source code",
                    "Try closing other applications to free memory",
                    __FILE__, __LINE__);
        return NULL;
    }
    
    // Read the file into memory
    size_t bytes_read = fread(source_code, 1, file_size, source);
    if (bytes_read != (size_t)file_size) {
        error_report(ERROR_IO, ERROR_FATAL, filename, 0, 0,
                    "Failed to read entire source file",
                    "Check file for corruption or disk errors",
                    __FILE__, __LINE__);
        free(source_code);
        return NULL;
    }
    
    // Null-terminate the source code
    source_code[file_size] = '\0';
    
    // Allocate memory for the lexer state
    LexerState* lexer = (LexerState*)malloc(sizeof(LexerState));
    if (!lexer) {
        error_report(ERROR_INTERNAL, ERROR_FATAL, filename, 0, 0,
                    "Failed to allocate memory for lexer state",
                    "Try closing other applications to free memory",
                    __FILE__, __LINE__);
        free(source_code);
        return NULL;
    }
    
    // Initialize the lexer state
    lexer->source = source_code;
    lexer->filename = filename;
    lexer->source_length = file_size;
    lexer->current = 0;
    lexer->start = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->previous_column = 1;
    lexer->has_next_token = false;
    lexer->target_info = target_info;
    
    return lexer;
}

/**
 * Clean up and free resources used by the lexer
 *
 * @param lexer The lexer state to clean up
 */
void lexer_cleanup(LexerState* lexer) {
    if (lexer) {
        if (lexer->source) {
            free((void*)lexer->source);
        }
        free(lexer);
    }
}

/**
 * Get the next token from the source code
 *
 * @param lexer The lexer state
 * @return The next token
 */
Token lexer_next_token(LexerState* lexer) {
    if (lexer->has_next_token) {
        lexer->has_next_token = false;
        return lexer->next_token;
    }
    
    skip_whitespace(lexer);
    
    lexer->start = lexer->current;
    
    if (lexer->current >= lexer->source_length) {
        return make_token(lexer, TOKEN_EOF);
    }
    
    Token token = scan_token(lexer);
    lexer->current_token = token;
    
    return token;
}

/**
 * Peek at the next token without consuming it
 *
 * @param lexer The lexer state
 * @return The next token, which will be returned again by lexer_next_token
 */
Token lexer_peek_token(LexerState* lexer) {
    if (lexer->has_next_token) {
        return lexer->next_token;
    }
    
    // Save current lexer state
    size_t saved_current = lexer->current;
    size_t saved_start = lexer->start;
    int saved_line = lexer->line;
    int saved_column = lexer->column;
    int saved_prev_column = lexer->previous_column;
    
    // Get the next token
    lexer->next_token = lexer_next_token(lexer);
    lexer->has_next_token = true;
    
    // Restore lexer state
    lexer->current = saved_current;
    lexer->start = saved_start;
    lexer->line = saved_line;
    lexer->column = saved_column;
    lexer->previous_column = saved_prev_column;
    
    return lexer->next_token;
}

/**
 * Advances the lexer by one UTF-8 character
 *
 * @param lexer The lexer state
 * @return The Unicode codepoint of the character that was consumed, or EOF
 */
static int32_t advance_utf8(LexerState* lexer) {
    if (lexer->current >= lexer->source_length) {
        return EOF;
    }
    
    int32_t codepoint = peek_utf8_char(lexer);
    uint8_t first_byte = (uint8_t)lexer->source[lexer->current];
    int num_bytes = determine_utf8_bytes(first_byte);
    
    // Advance by the appropriate number of bytes
    lexer->current += num_bytes;
    
    return codepoint;
}

/**
 * Scan the next token from the source code
 *
 * @param lexer The lexer state
 * @return The scanned token
 */
static Token scan_token(LexerState* lexer) {
    int32_t c = advance(lexer);
    
    // Check for identifier start
    if (is_identifier_start(c)) {
        return scan_identifier(lexer);
    }
    
    // Check for number start
    if (isdigit(c)) {
        return scan_number(lexer);
    }
    
    // Handle various token types
    switch (c) {
        // Single-character tokens
        case '(': return make_token(lexer, TOKEN_LEFT_PAREN);
        case ')': return make_token(lexer, TOKEN_RIGHT_PAREN);
        case '[': return make_token(lexer, TOKEN_LEFT_BRACKET);
        case ']': return make_token(lexer, TOKEN_RIGHT_BRACKET);
        case ':': return make_token(lexer, TOKEN_COLON);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case '+': return make_token(lexer, TOKEN_PLUS);
        case '-': return make_token(lexer, TOKEN_MINUS);
        case '*': return make_token(lexer, TOKEN_STAR);
        case '/': return make_token(lexer, TOKEN_SLASH);
        case '%': return make_token(lexer, TOKEN_PERCENT);
        case '^': return make_token(lexer, TOKEN_CARET);
        case '~': return make_token(lexer, TOKEN_TILDE);
        
        // One or two character tokens
        case '!':
            return make_token(lexer, match(lexer, '=') ? TOKEN_NOT_EQUALS : TOKEN_NOT);
        case '=':
            return make_token(lexer, match(lexer, '=') ? TOKEN_DOUBLE_EQUALS : TOKEN_EQUALS);
        case '<':
            // Could be <, <=, or opening angle bracket (instead of {)
            return make_token(lexer, match(lexer, '=') ? TOKEN_LESS_EQUALS : TOKEN_LESS);
        case '>':
            // Could be >, >=, or closing angle bracket (instead of })
            return make_token(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUALS : TOKEN_GREATER);
        case '&':
            return make_token(lexer, match(lexer, '&') ? TOKEN_DOUBLE_AND : TOKEN_AND);
        case '|':
            return make_token(lexer, match(lexer, '|') ? TOKEN_DOUBLE_OR : TOKEN_OR);
            
        // String literals
        case '"': return scan_string(lexer);
        
        // Character literals
        case '\'': return scan_character_literal(lexer);
    }
    
    return error_token(lexer, "Unexpected character");
}

/**
 * Scan an identifier token
 *
 * @param lexer The lexer state
 * @return The scanned identifier token
 */
static Token scan_identifier(LexerState* lexer) {
    // Continue reading as long as we find valid identifier characters
    while (is_identifier_part(peek_utf8_char(lexer))) {
        advance(lexer);
    }
    
    // Extract the identifier string
    size_t length = lexer->current - lexer->start;
    char* identifier = (char*)malloc(length + 1);
    if (!identifier) {
        return error_token(lexer, "Memory allocation failed");
    }
    
    memcpy(identifier, lexer->source + lexer->start, length);
    identifier[length] = '\0';
    
    // Check if it's a keyword
    TokenType type = TOKEN_IDENTIFIER;
    if (is_keyword(identifier)) {
        type = get_keyword_token(identifier);
        free(identifier);
    } else {
        // If not a keyword, keep the identifier string
        Token token = make_token(lexer, type);
        token.value.string_value = identifier;
        return token;
    }
    
    return make_token(lexer, type);
}

/**
 * Scan a number token
 *
 * @param lexer The lexer state
 * @return The scanned number token
 */
static Token scan_number(LexerState* lexer) {
    // Continue reading as long as we find digits
    while (isdigit(peek(lexer))) {
        advance(lexer);
    }
    
    // Look for a decimal point followed by at least one digit
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        // Consume the '.'
        advance(lexer);
        
        // Consume digits after the decimal
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
        
        // For now, we're treating all numbers as integers
        // This would be where we handle floating-point values in the future
        error_report(ERROR_LEXICAL, ERROR_WARNING, lexer->filename, lexer->line, lexer->column,
                    "Floating-point numbers are not fully supported yet",
                    "Truncating to integer value",
                    __FILE__, __LINE__);
    }
    
    // Parse the number
    char number_str[64] = {0}; // Large enough for any reasonable number
    size_t length = lexer->current - lexer->start;
    if (length >= sizeof(number_str)) {
        return error_token(lexer, "Number too large");
    }
    
    strncpy(number_str, lexer->source + lexer->start, length);
    number_str[length] = '\0';
    
    // Convert to integer
    intptr_t value = strtol(number_str, NULL, 10);
    
    // Create the token
    Token token = make_token(lexer, TOKEN_NUMBER);
    token.value.int_value = value;
    
    return token;
}

/**
 * Scan a string token
 *
 * @param lexer The lexer state
 * @return The scanned string token
 */
static Token scan_string(LexerState* lexer) {
    // Buffer to build the string, starting with a reasonable size
    size_t buffer_size = 128;
    size_t buffer_pos = 0;
    char* buffer = (char*)malloc(buffer_size);
    
    if (!buffer) {
        return error_token(lexer, "Memory allocation failed");
    }
    
    // Track line and column for error reporting
    int start_line = lexer->line;
    int start_column = lexer->column - 1; // Adjust for the opening quote
    
    // Read characters until we reach the closing quote
    while (peek(lexer) != '"' && lexer->current < lexer->source_length) {
        int32_t c = advance(lexer);
        
        // Handle escape sequences
        if (c == '\\') {
            int32_t next = advance(lexer);
            switch (next) {
                case '\"': c = '\"'; break;
                case '\\': c = '\\'; break;
                case 'r': c = '\r'; break;
                case 't': c = '\t'; break;
                case '0': c = '\0'; break;
                case 'n': c = '\n'; break;
                case 'u': {
                    // Unicode escape sequences \uXXXX
                    char hex[5] = {0};
                    for (int i = 0; i < 4; i++) {
                        if (lexer->current >= lexer->source_length || 
                            !isxdigit(peek(lexer))) {
                            free(buffer);
                            error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                                        "Invalid Unicode escape sequence",
                                        "Unicode escape must be in the form \\uXXXX",
                                        __FILE__, __LINE__);
                            return error_token(lexer, "Invalid Unicode escape");
                        }
                        hex[i] = peek(lexer);
                        advance(lexer);
                    }
                    
                    // Convert hex to codepoint
                    c = (int32_t)strtol(hex, NULL, 16);
                    break;
                }
                case 'x': {
                    // Hex escape sequences \xXX
                    char hex[3] = {0};
                    for (int i = 0; i < 2; i++) {
                        if (lexer->current >= lexer->source_length || 
                            !isxdigit(peek(lexer))) {
                            free(buffer);
                            error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                                        "Invalid hex escape sequence",
                                        "Hex escape must be in the form \\xXX",
                                        __FILE__, __LINE__);
                            return error_token(lexer, "Invalid hex escape");
                        }
                        hex[i] = peek(lexer);
                        advance(lexer);
                    }
                    
                    // Convert hex to codepoint
                    c = (int32_t)strtol(hex, NULL, 16);
                    break;
                }
                case 'b': c = '\b'; break; // Backspace
                case 'f': c = '\f'; break; // Form feed
                case 'v': c = '\v'; break; // Vertical tab
                case 'a': c = '\a'; break; // Bell
                default: {
                    free(buffer);
                    char error_msg[64];
                    snprintf(error_msg, sizeof(error_msg), "Invalid escape sequence '\\%c'", (char)next);
                    error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, lexer->line, lexer->column,
                                error_msg, "Use a valid escape sequence (\\n, \\t, etc.)",
                                __FILE__, __LINE__);
                    return error_token(lexer, error_msg);
                }
            }
        }
        
        // Expand buffer if needed
        if (buffer_pos >= buffer_size - 1) { // -1 for null terminator
            buffer_size *= 2;
            char* new_buffer = (char*)realloc(buffer, buffer_size);
            if (!new_buffer) {
                free(buffer);
                return error_token(lexer, "Memory allocation failed");
            }
            buffer = new_buffer;
        }
        
        // Store the character in the buffer (as a UTF-8 sequence)
        if (c < 128) {
            // ASCII character
            buffer[buffer_pos++] = (char)c;
        } else {
            // UTF-8 character - need to encode it
            utf8_char_t utf8_char;
            int bytes = utf8_encode(c, &utf8_char);
            
            // Make sure we have room
            if (buffer_pos + bytes >= buffer_size) {
                buffer_size *= 2;
                char* new_buffer = (char*)realloc(buffer, buffer_size);
                if (!new_buffer) {
                    free(buffer);
                    return error_token(lexer, "Memory allocation failed");
                }
                buffer = new_buffer;
            }
            
            // Copy the UTF-8 bytes
            for (int i = 0; i < bytes; i++) {
                buffer[buffer_pos++] = utf8_char.bytes[i];
            }
        }
    }
    
    // Check if we reached the end of the string
    if (lexer->current >= lexer->source_length || peek(lexer) != '"') {
        free(buffer);
        error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                    "Unterminated string literal",
                    "Add closing double quote",
                    __FILE__, __LINE__);
        return error_token(lexer, "Unterminated string");
    }
    
    // Consume the closing quote
    advance(lexer);
    
    // Null-terminate the string
    buffer[buffer_pos] = '\0';
    
    // Create the token
    Token token = make_token(lexer, TOKEN_STRING);
    token.value.string_value = buffer;
    
    return token;
}

/**
 * Scan a character literal token
 *
 * @param lexer The lexer state
 * @return The scanned character token
 */
static Token scan_character_literal(LexerState* lexer) {
    // Track line and column for error reporting
    int start_line = lexer->line;
    int start_column = lexer->column - 1; // Adjust for the opening quote
    
    // Read the character
    int32_t c;
    
    // Handle escape sequences
    if (peek(lexer) == '\\') {
        advance(lexer); // Consume the backslash
        
        switch (peek(lexer)) {
            case '\'': c = '\''; advance(lexer); break;
            case '\\': c = '\\'; advance(lexer); break;
            case 'n': c = '\n'; advance(lexer); break;
            case 'r': c = '\r'; advance(lexer); break;
            case 't': c = '\t'; advance(lexer); break;
            case '0': c = '\0'; advance(lexer); break;
            case 'u': {
                // Unicode escape sequences \uXXXX
                advance(lexer); // Consume 'u'
                
                // Read 4 hex digits
                char hex[5] = {0};
                for (int i = 0; i < 4; i++) {
                    if (lexer->current >= lexer->source_length || 
                        !isxdigit(peek(lexer))) {
                        error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                                    "Invalid Unicode escape sequence",
                                    "Unicode escape must be in the form \\uXXXX",
                                    __FILE__, __LINE__);
                        return error_token(lexer, "Invalid Unicode escape");
                    }
                    hex[i] = peek(lexer);
                    advance(lexer);
                }
                
                // Convert hex to codepoint
                c = (int32_t)strtol(hex, NULL, 16);
                break;
            }
            default: {
                char error_msg[64];
                snprintf(error_msg, sizeof(error_msg), "Invalid escape sequence '\\%c'", (char)peek(lexer));
                error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, lexer->line, lexer->column,
                            error_msg, "Use a valid escape sequence (\\n, \\t, etc.)",
                            __FILE__, __LINE__);
                return error_token(lexer, error_msg);
            }
        }
    } else {
        // Regular character
        c = advance(lexer);
        
        // Handle UTF-8 multi-byte sequences
        if ((c & 0x80) != 0) {
            // This is a multi-byte UTF-8 character
            char first_byte = (char)c;
            int32_t codepoint;
            
            int bytes_to_read = determine_utf8_bytes(first_byte);
            if (bytes_to_read <= 0) {
                error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                            "Invalid UTF-8 character",
                            "Character must be valid UTF-8",
                            __FILE__, __LINE__);
                return error_token(lexer, "Invalid UTF-8 character");
            }
            
            // Build the UTF-8 sequence
            char utf8_char[5] = {first_byte, 0, 0, 0, 0};
            
            // Read the additional bytes
            for (int i = 1; i < bytes_to_read; i++) {
                if (lexer->current >= lexer->source_length) {
                    error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                                "Incomplete UTF-8 character",
                                "Character must be complete UTF-8 sequence",
                                __FILE__, __LINE__);
                    return error_token(lexer, "Incomplete UTF-8 character");
                }
                
                int32_t next_byte = advance(lexer);
                if ((next_byte & 0xC0) != 0x80) {
                    error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                                "Invalid UTF-8 continuation byte",
                                "Character must be valid UTF-8",
                                __FILE__, __LINE__);
                    return error_token(lexer, "Invalid UTF-8 continuation byte");
                }
                
                utf8_char[i] = (char)next_byte;
            }
            
            // Decode the UTF-8 sequence to a codepoint
            utf8_char_t character;
            if (utf8_decode(utf8_char, &character) == 0) {
                // Decoding failed
                error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                            "Invalid UTF-8 character",
                            "Character must be valid UTF-8",
                            __FILE__, __LINE__);
                return error_token(lexer, "Invalid UTF-8 character");
            }
            
            c = character.codepoint;
        }
    }
    
    // Verify closing quote
    if (peek(lexer) != '\'') {
        error_report(ERROR_LEXICAL, ERROR_ERROR, lexer->filename, start_line, start_column,
                    "Unterminated character literal",
                    "Add closing single quote",
                    __FILE__, __LINE__);
        return error_token(lexer, "Unterminated character literal");
    }
    
    advance(lexer); // Consume closing quote
    
    // Create the token
    Token token = make_token(lexer, TOKEN_CHAR_LITERAL);
    token.value.char_value = c;
    
    return token;
}

/**
 * Get a string representation of a token type
 *
 * @param type The token type
 * @return A string representation of the token type
 */
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_CHAR_LITERAL: return "CHAR_LITERAL";
        case TOKEN_STRING: return "STRING";
        case TOKEN_PLUS: return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_STAR: return "*";
        case TOKEN_SLASH: return "/";
        case TOKEN_PERCENT: return "%";
        case TOKEN_EQUALS: return "=";
        case TOKEN_DOUBLE_EQUALS: return "==";
        case TOKEN_NOT_EQUALS: return "!=";
        case TOKEN_LESS: return "<";
        case TOKEN_LESS_EQUALS: return "<=";
        case TOKEN_GREATER: return ">";
        case TOKEN_GREATER_EQUALS: return ">=";
        case TOKEN_AND: return "&";
        case TOKEN_DOUBLE_AND: return "&&";
        case TOKEN_OR: return "|";
        case TOKEN_DOUBLE_OR: return "||";
        case TOKEN_NOT: return "!";
        case TOKEN_CARET: return "^";
        case TOKEN_TILDE: return "~";
        case TOKEN_LEFT_PAREN: return "(";
        case TOKEN_RIGHT_PAREN: return ")";
        case TOKEN_LEFT_BRACKET: return "[";
        case TOKEN_RIGHT_BRACKET: return "]";
        case TOKEN_LEFT_ANGLE: return "<";
        case TOKEN_RIGHT_ANGLE: return ">";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_COMMA: return ",";
        case TOKEN_DOT: return ".";
        case TOKEN_COLON: return ":";
        case TOKEN_ARRAY: return "ARRAY";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_DO: return "DO";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_EXTERNAL: return "EXTERNAL";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        default: return "UNKNOWN";
    }
}

/**
 * Print token information for debugging
 *
 * @param token The token to print
 */
void token_print(Token token) {
    printf("Token { type: %s, lexeme: \"", token_type_to_string(token.type));

    // Print the lexeme, preserving UTF-8 sequences
    if (token.lexeme) {
        size_t i = 0;
        while (i < token.lexeme_length) {
            unsigned char c = (unsigned char)token.lexeme[i];
            if (c == '\n') {
                printf("\\n");
                i++;
            } else if (c == '\t') {
                printf("\\t");
                i++;
            } else if (c == '\r') {
                printf("\\r");
                i++;
            } else if (c < 0x20 && c != '\n' && c != '\t' && c != '\r') {
                printf("\\x%02x", c);
                i++;
            } else if (c < 0x80) {
                putchar(c);
                i++;
            } else {
                // UTF-8 multi-byte sequence: print as-is
                int num_bytes = determine_utf8_bytes(c);
                for (int j = 0; j < num_bytes && i + j < token.lexeme_length; j++) {
                    putchar((unsigned char)token.lexeme[i + j]);
                }
                i += num_bytes;
            }
        }
    }

    printf("\", line: %d, col: %d", token.line, token.column);

    // Print token-specific values
    switch (token.type) {
        case TOKEN_NUMBER:
            printf(", value: %ld", (long)token.value.int_value);
            break;
        case TOKEN_STRING:
            printf(", value: \"%s\"", token.value.string_value ? token.value.string_value : "");
            break;
        case TOKEN_CHAR_LITERAL:
            if (token.value.char_value >= 32 && token.value.char_value <= 126) {
                printf(", value: '%c'", (char)token.value.char_value);
            } else {
                printf(", value: '\\u%04x'", token.value.char_value);
            }
            break;
        default:
            break;
    }

    printf(" }\n");
}

/**
 * Print all tokens in the source code for debugging
 *
 * @param lexer The lexer state
 */
void lexer_print_all_tokens(LexerState* lexer) {
    // Save lexer state
    size_t saved_current = lexer->current;
    size_t saved_start = lexer->start;
    int saved_line = lexer->line;
    int saved_column = lexer->column;
    int saved_prev_column = lexer->previous_column;
    bool saved_has_next = lexer->has_next_token;
    
    // Reset lexer
    lexer->current = 0;
    lexer->start = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->previous_column = 1;
    lexer->has_next_token = false;
    
    // Print all tokens
    printf("All tokens in file: %s\n", lexer->filename);
    printf("----------------------------------------\n");
    
    Token token;
    int token_count = 0;
    do {
        token = lexer_next_token(lexer);
        printf("%d: ", ++token_count);
        token_print(token);
        
        if (token.type == TOKEN_ERROR) {
            printf("Lexical error encountered, stopping token printing\n");
            break;
        }
    } while (token.type != TOKEN_EOF);
    
    printf("----------------------------------------\n");
    printf("Total tokens: %d\n", token_count);
    
    // Restore lexer state
    lexer->current = saved_current;
    lexer->start = saved_start;
    lexer->line = saved_line;
    lexer->column = saved_column;
    lexer->previous_column = saved_prev_column;
    lexer->has_next_token = saved_has_next;
}

/**
 * Check if a character can start an identifier
 *
 * @param c The character to check
 * @return true if the character can start an identifier
 */
static bool is_identifier_start(int32_t c) {
    // ASCII letters and underscore
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        return true;
    }
    
    // For non-ASCII, use our proper UTF-8 utilities
    if (c > 127) {
        return utf8_is_identifier_char((utf8_codepoint_t)c, true);  // true indicates this is a start character
    }
    
    return false;
}

/**
 * Check if a character can be part of an identifier
 *
 * @param c The character to check
 * @return true if the character can be part of an identifier
 */
static bool is_identifier_part(int32_t c) {
    // ASCII letters, digits, underscore
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
        (c >= '0' && c <= '9') || c == '_') {
        return true;
    }
    
    // For non-ASCII, use our proper UTF-8 utilities
    if (c > 127) {
        return utf8_is_identifier_char((utf8_codepoint_t)c, false);  // false indicates this is not a start character
    }
    
    return false;
}

/**
 * Advance to the next character in the source and return the current one
 * For UTF-8 characters, this will read the full character and return its codepoint
 *
 * @param lexer The lexer state
 * @return The current character codepoint
 */
static int32_t advance(LexerState* lexer) {
    if (lexer->current >= lexer->source_length) {
        return '\0';
    }
    
    unsigned char first_byte = (unsigned char)lexer->source[lexer->current++];
    
    // Update column count
    lexer->previous_column = lexer->column++;
    
    // If we see a newline, increment the line count
    if (first_byte == '\n') {
        lexer->line++;
        lexer->column = 1;
        return '\n';
    }
    
    // ASCII character (0-127)
    if ((first_byte & 0x80) == 0) {
        return first_byte;
    }
    
    // Multi-byte UTF-8 sequence
    int bytes_to_read = determine_utf8_bytes(first_byte);
    if (bytes_to_read <= 0) {
        // Invalid UTF-8 leading byte
        return first_byte;
    }
    
    // Already consumed the first byte, now read the remaining bytes
    char utf8_char[5] = {(char)first_byte, 0, 0, 0, 0};
    
    // Read the additional bytes
    for (int i = 1; i < bytes_to_read; i++) {
        if (lexer->current >= lexer->source_length) {
            // Incomplete UTF-8 sequence
            return first_byte;
        }
        
        unsigned char next_byte = (unsigned char)lexer->source[lexer->current++];
        lexer->column++; // Update column for each byte
        
        if ((next_byte & 0xC0) != 0x80) {
            // Invalid continuation byte
            // Rewind the lexer position
            lexer->current--;
            lexer->column--;
            return first_byte;
        }
        
        utf8_char[i] = (char)next_byte;
    }
    
    // Decode the UTF-8 sequence to get the codepoint
    utf8_char_t character;
    if (utf8_decode(utf8_char, &character) == 0) {
        // Decoding failed
        return first_byte;
    }
    
    return character.codepoint;
}

/**
 * Look at the current character without advancing
 *
 * @param lexer The lexer state
 * @return The current character
 */
static int32_t peek(LexerState* lexer) {
    if (lexer->current >= lexer->source_length) {
        return '\0';
    }
    
    return lexer->source[lexer->current];
}

/**
 * Look at the next character without advancing
 *
 * @param lexer The lexer state
 * @return The next character
 */
static int32_t peek_next(LexerState* lexer) {
    if (lexer->current + 1 >= lexer->source_length) {
        return '\0';
    }
    
    return lexer->source[lexer->current + 1];
}

/**
 * Check if the current character matches the expected one
 * If it matches, advance to the next character
 *
 * @param lexer The lexer state
 * @param expected The expected character
 * @return true if the current character matches the expected one
 */
static bool match(LexerState* lexer, char expected) {
    if (lexer->current >= lexer->source_length) {
        return false;
    }
    
    if (lexer->source[lexer->current] != expected) {
        return false;
    }
    
    lexer->current++;
    lexer->column++;
    
    return true;
}

/**
 * Skip whitespace and comments
 *
 * @param lexer The lexer state
 */
static void skip_whitespace(LexerState* lexer) {
    while (lexer->current < lexer->source_length) {
        char c = peek(lexer);
        
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance(lexer);
                break;
                
            case '\n':
                advance(lexer);
                break;
                
            case '/':
                if (peek_next(lexer) == '/') {
                    // Single-line comment
                    while (peek(lexer) != '\n' && peek(lexer) != '\0') {
                        advance(lexer);
                    }
                } else if (peek_next(lexer) == '*') {
                    // Multi-line comment
                    advance(lexer); // Consume '/'
                    advance(lexer); // Consume '*'
                    
                    // Find the closing */
                    while (lexer->current < lexer->source_length) {
                        if (peek(lexer) == '*' && peek_next(lexer) == '/') {
                            advance(lexer); // Consume '*'
                            advance(lexer); // Consume '/'
                            break;
                        }
                        
                        advance(lexer);
                    }
                    
                    if (lexer->current >= lexer->source_length) {
                        error_report(ERROR_LEXICAL, ERROR_WARNING, lexer->filename, lexer->line, lexer->column,
                                   "Unterminated multi-line comment",
                                   "Add */ to close the comment",
                                   __FILE__, __LINE__);
                    }
                } else {
                    // Not a comment, must be a division operator
                    return;
                }
                break;
                
            default:
                return;
        }
    }
}

/**
 * Create a token of the given type
 *
 * @param lexer The lexer state
 * @param type The token type
 * @return The created token
 */
static Token make_token(LexerState* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.lexeme = lexer->source + lexer->start;
    token.lexeme_length = lexer->current - lexer->start;
    token.line = lexer->line;
    token.column = lexer->column - (int)token.lexeme_length;
    
    // Initialize value to zero
    token.value.int_value = 0;
    
    return token;
}

/**
 * Create an error token with the given message
 *
 * @param lexer The lexer state
 * @param message The error message
 * @return The error token
 */
static Token error_token(LexerState* lexer, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.lexeme = message;
    token.lexeme_length = strlen(message);
    token.line = lexer->line;
    token.column = lexer->column;
    token.value.int_value = 0;
    
    return token;
}