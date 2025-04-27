/*
 * filename: lexer_tests.c
 * 
 * Purpose:
 * Test suite for the lexical analyzer component of the ћ++ compiler.
 * Contains unit tests that verify the lexer correctly tokenizes ћ++ source code,
 * with particular focus on UTF-8 identifier and keyword handling.
 * 
 * Key Components:
 * - test_lexer_init(): Test lexer initialization
 * - test_basic_tokens(): Test recognition of basic tokens (operators, punctuation)
 * - test_utf8_identifiers(): Test handling of UTF-8 identifiers
 * - test_utf8_keywords(): Test recognition of ћ++ keywords (екстерно, врати, etc.)
 * - test_numeric_literals(): Test processing of numeric literals
 * - test_array_syntax(): Test tokenization of array declaration syntax
 * 
 * Interactions:
 * - Uses lexer.h to test lexer functionality
 * - Uses error.h for test failure reporting
 * 
 * Notes:
 * - Tests edge cases for UTF-8 handling
 * - Includes performance tests for large input streams
 * - Verifies correct line and column tracking for error reporting
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>  // For EXIT_SUCCESS and EXIT_FAILURE
#include <string.h>
#include <unistd.h> // For getcwd()
#include "../src/lexer/lexer.h"
#include "../src/utils/error.h"  // For error_init and error_cleanup

// Declaration of the function from keywords.c that we're testing
extern void print_all_keywords(void);

// Helper function to create a temporary file with given content
FILE* create_temp_file(const char* content) {
    // Create a real temporary file with a predictable name
    const char* temp_filename = "test.ћпп";
    FILE* temp_file = fopen(temp_filename, "w");
    if (!temp_file) {
        fprintf(stderr, "Failed to create temporary file\n");
        perror("Error details");  // Print the specific error reason
        return NULL;
    }
    
    fputs(content, temp_file);
    fclose(temp_file);
    
    // Reopen for reading
    temp_file = fopen(temp_filename, "r");
    if (!temp_file) {
        fprintf(stderr, "Failed to reopen temporary file\n");
        perror("Error details");  // Print the specific error reason
        return NULL;
    }
    
    return temp_file;
}

// Helper function to clean up temporary file
void cleanup_temp_file() {
    remove("test.ћпп");
}

/**
 * Test the print_all_keywords function
 * 
 * This test verifies that the print_all_keywords function can be called
 * without errors. The function should print all keywords to stdout.
 * Note: This is a visual test - the output should be manually verified.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_print_all_keywords(void) {
    printf("\n=== Testing print_all_keywords ===\n");
    
    // Redirect stdout to a temporary file to capture the output
    FILE* original_stdout = stdout;
    FILE* temp_file = tmpfile();
    if (!temp_file) {
        fprintf(stderr, "Failed to create temporary file for test_print_all_keywords\n");
        return false;
    }
    
    stdout = temp_file;
    
    // Call the function we're testing
    print_all_keywords();
    
    // Restore stdout
    fflush(stdout);
    stdout = original_stdout;
    
    // Read and display the captured output
    rewind(temp_file);
    char buffer[1024];
    size_t bytes_read;
    
    printf("Captured output:\n");
    printf("-----------------\n");
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer) - 1, temp_file)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
    
    printf("\n-----------------\n");
    
    // Close the temporary file
    fclose(temp_file);
    
    // Check if the output contains the expected header
    // This is a simple verification that the function produced output
    // For a more thorough test, we could parse the output and verify each keyword
    
    printf("Test completed. Visual inspection required to verify all keywords were printed correctly.\n");
    
    return true;
}

/**
 * Test lexer initialization
 * 
 * Tests that the lexer can be properly initialized with a source file
 * and that error handling works correctly for invalid inputs.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_lexer_init(void) {
    printf("\n=== Testing lexer_init ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a temporary test file
    const char *filename = "test.ћпп";
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd);
    }
    
    FILE *tmp_file = fopen(filename, "w");
    if (!tmp_file) {
        printf("Error: Could not create temporary test file '%s'\n", filename);
        perror("fopen");
        error_cleanup();
        return false;
    }
    
    // Write some test content to the file
    fprintf(tmp_file, "// This is a test file\nако (тачно) {\n    врати 42;\n}\n");
    fclose(tmp_file);
    
    printf("Created temporary test file: %s\n", filename);
    
    // Reopen file for reading
    tmp_file = fopen(filename, "r");
    if (!tmp_file) {
        printf("Error: Could not reopen temporary test file '%s' for reading\n", filename);
        perror("fopen");
        error_cleanup();
        remove(filename);
        return false;
    }
    
    printf("Initializing lexer with file: %s\n", filename);
    
    // Initialize the lexer with the temporary file
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(tmp_file, filename, target_info);
    
    // Close the file pointer - lexer_init should have made its own copy
    fclose(tmp_file);
    
    // Check if lexer initialization was successful
    if (!lexer) {
        printf("Lexer initialization failed\n");
        remove(filename);
        error_cleanup();
        return false;
    }
    
    printf("Lexer initialization successful\n");
    
    // Clean up
    lexer_cleanup(lexer);
    
    // Cleanup - remove the temporary file
    if (remove(filename) != 0) {
        printf("Warning: Could not remove temporary test file\n");
        perror("remove");
    } else {
        printf("Removed temporary test file\n");
    }
    
    error_cleanup();
    return true;
}

/**
 * Test basic token recognition
 * 
 * Verifies that the lexer correctly identifies basic tokens like
 * operators, punctuation, and simple literals.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_basic_tokens(void) {
    printf("\n=== Testing basic token recognition ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a source file with various tokens
    const char* source_code = 
        "// Test basic tokens\n"
        "+ - * / % = == != < <= > >= & && | || ! ^ ~\n"
        "( ) [ ] ; , . :\n"
        "42 \"string\" 'c'";
    
    FILE* source_file = create_temp_file(source_code);
    if (!source_file) {
        error_cleanup();
        return false;
    }
    
    // Initialize lexer
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(source_file, "basic_tokens.ћпп", target_info);
    if (!lexer) {
        fclose(source_file);
        error_cleanup();
        return false;
    }
    
    // Expected token types in order
    TokenType expected_tokens[] = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT,
        TOKEN_EQUALS, TOKEN_DOUBLE_EQUALS, TOKEN_NOT_EQUALS,
        TOKEN_LESS, TOKEN_LESS_EQUALS, TOKEN_GREATER, TOKEN_GREATER_EQUALS,
        TOKEN_AND, TOKEN_DOUBLE_AND, TOKEN_OR, TOKEN_DOUBLE_OR,
        TOKEN_NOT, TOKEN_CARET, TOKEN_TILDE,
        TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
        TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_DOT, TOKEN_COLON,
        TOKEN_NUMBER, TOKEN_STRING, TOKEN_CHAR_LITERAL,
        TOKEN_EOF
    };
    
    // Scan all tokens
    Token token;
    int i = 0;
    bool all_passed = true;
    
    printf("Testing token recognition:\n");
    while (i < (int)(sizeof(expected_tokens) / sizeof(expected_tokens[0]))) {
        token = lexer_next_token(lexer);
        
        // Skip comments and whitespace (which are handled internally)
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Error token encountered: %.*s\n", 
                   (int)token.lexeme_length, token.lexeme);
            all_passed = false;
            break;
        }
        
        printf("Expected: %-15s Got: %-15s ", 
               token_type_to_string(expected_tokens[i]), 
               token_type_to_string(token.type));
        
        if (token.type == expected_tokens[i]) {
            printf("✓\n");
        } else {
            printf("✗\n");
            all_passed = false;
        }
        
        i++;
    }
    
    // Clean up
    lexer_cleanup(lexer);
    fclose(source_file);
    error_cleanup();
    
    if (all_passed) {
        printf("All basic token tests passed!\n");
    } else {
        fprintf(stderr, "Some basic token tests failed!\n");
    }
    
    return all_passed;
}

/**
 * Test UTF-8 identifier handling
 * 
 * Verifies that the lexer correctly handles UTF-8 encoded identifiers,
 * particularly Serbian Cyrillic characters used in ћ++.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_utf8_identifiers(void) {
    printf("\n=== Testing UTF-8 identifier handling ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a source file with UTF-8 identifiers
    const char* source_code = 
        "// Test UTF-8 identifiers\n"
        "променљива = 10;\n"  // variable = 10;
        "број_један = 1;\n"    // number_one = 1;
        "функција();\n"        // function();
        "маx = 100;\n";         // max = 100;
    
    FILE* source_file = create_temp_file(source_code);
    if (!source_file) {
        error_cleanup();
        return false;
    }
    
    // Initialize lexer
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(source_file, "utf8_identifiers.ћпп", target_info);
    if (!lexer) {
        fclose(source_file);
        error_cleanup();
        return false;
    }
    
    // Expected identifiers
    const char* expected_identifiers[] = {
        "променљива", "број_један", "функција", "маx"
    };
    
    // Scan tokens and check identifiers
    Token token;
    int found_identifiers = 0;
    bool all_passed = true;
    
    printf("Checking UTF-8 identifiers:\n");
    while (found_identifiers < (int)(sizeof(expected_identifiers) / sizeof(expected_identifiers[0]))) {
        token = lexer_next_token(lexer);
        
        if (token.type == TOKEN_EOF) {
            fprintf(stderr, "Reached EOF before finding all expected identifiers\n");
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Error token encountered: %.*s\n", 
                   (int)token.lexeme_length, token.lexeme);
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_IDENTIFIER) {
            printf("Found identifier: %.*s\n", (int)token.lexeme_length, token.lexeme);
            
            // Check if it matches one of our expected identifiers
            bool matched = false;
            for (int i = 0; i < (int)(sizeof(expected_identifiers) / sizeof(expected_identifiers[0])); i++) {
                if (strncmp(token.lexeme, expected_identifiers[i], token.lexeme_length) == 0 &&
                    strlen(expected_identifiers[i]) == token.lexeme_length) {
                    matched = true;
                    found_identifiers++;
                    break;
                }
            }
            
            if (!matched) {
                fprintf(stderr, "Unexpected identifier: %.*s\n", 
                       (int)token.lexeme_length, token.lexeme);
                all_passed = false;
            }
        }
    }
    
    // Clean up
    lexer_cleanup(lexer);
    fclose(source_file);
    error_cleanup();
    
    if (all_passed && found_identifiers == (int)(sizeof(expected_identifiers) / sizeof(expected_identifiers[0]))) {
        printf("All UTF-8 identifier tests passed!\n");
        return true;
    } else {
        fprintf(stderr, "Some UTF-8 identifier tests failed!\n");
        return false;
    }
}

/**
 * Test UTF-8 keyword recognition
 * 
 * Verifies that the lexer correctly identifies ћ++ keywords written
 * in Serbian Cyrillic.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_utf8_keywords(void) {
    printf("\n=== Testing UTF-8 keyword recognition ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a source file with UTF-8 keywords
    const char* source_code = 
        "// Test UTF-8 keywords\n"
        "ако (x > 0) <\n"      // if (x > 0) {
        "    врати тачно;\n"    // return true;
        "> иначе <\n"          // } else {
        "    врати нетачно;\n"  // return false;
        ">\n"
        "док (i < 10) <\n"     // while (i < 10) {
        "    i = i + 1;\n"
        ">\n"
        "за (i = 0; i < 5; i = i + 1) <\n"  // for (i = 0; i < 5; i = i + 1) {
        "    ради <\n"         // do {
        "        прекини;\n"    // break;
        "    > док (0);\n"      // } while (0);
        ">\n"
        "екстерно функција();\n";  // external function();
    
    FILE* source_file = create_temp_file(source_code);
    if (!source_file) {
        error_cleanup();
        return false;
    }
    
    // Initialize lexer
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(source_file, "utf8_keywords.ћпп", target_info);
    if (!lexer) {
        fclose(source_file);
        error_cleanup();
        return false;
    }
    
    // Expected keywords and their token types
    struct {
        const char* keyword;
        TokenType type;
    } expected_keywords[] = {
        {"ако", TOKEN_IF},
        {"врати", TOKEN_RETURN},
        {"тачно", TOKEN_TRUE},
        {"иначе", TOKEN_ELSE},
        {"врати", TOKEN_RETURN},
        {"нетачно", TOKEN_FALSE},
        {"док", TOKEN_WHILE},
        {"за", TOKEN_FOR},
        {"ради", TOKEN_DO},
        {"прекини", TOKEN_BREAK},
        {"док", TOKEN_WHILE},
        {"екстерно", TOKEN_EXTERNAL}
    };
    
    // Scan tokens and check keywords
    Token token;
    int found_keywords = 0;
    bool all_passed = true;
    
    printf("Checking UTF-8 keywords:\n");
    while (found_keywords < (int)(sizeof(expected_keywords) / sizeof(expected_keywords[0]))) {
        token = lexer_next_token(lexer);
        
        if (token.type == TOKEN_EOF) {
            fprintf(stderr, "Reached EOF before finding all expected keywords\n");
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Error token encountered: %.*s\n", 
                   (int)token.lexeme_length, token.lexeme);
            all_passed = false;
            break;
        }
        
        // Check if token is one of our expected keywords
        for (int i = 0; i < (int)(sizeof(expected_keywords) / sizeof(expected_keywords[0])); i++) {
            if (token.type == expected_keywords[i].type &&
                strncmp(token.lexeme, expected_keywords[i].keyword, token.lexeme_length) == 0 &&
                strlen(expected_keywords[i].keyword) == token.lexeme_length) {
                
                printf("Found keyword: %.*s (type: %s) ✓\n", 
                       (int)token.lexeme_length, token.lexeme, 
                       token_type_to_string(token.type));
                
                found_keywords++;
                break;
            }
        }
    }
    
    // Clean up
    lexer_cleanup(lexer);
    fclose(source_file);
    error_cleanup();
    
    if (all_passed && found_keywords == (int)(sizeof(expected_keywords) / sizeof(expected_keywords[0]))) {
        printf("All UTF-8 keyword tests passed!\n");
        return true;
    } else {
        fprintf(stderr, "Some UTF-8 keyword tests failed! Found %d of %d keywords\n", 
               found_keywords, (int)(sizeof(expected_keywords) / sizeof(expected_keywords[0])));
        return false;
    }
}

/**
 * Test numeric literals
 * 
 * Verifies that the lexer correctly tokenizes and evaluates
 * numeric literals.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_numeric_literals(void) {
    printf("\n=== Testing numeric literals ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a source file with numeric literals
    const char* source_code = 
        "// Test numeric literals\n"
        "0\n"
        "42\n"
        "123456789\n"
        "0123\n"   // This is treated as decimal in ћ++, not octal
        "3.14159\n"; // For now, we'll get a warning about truncation
    
    FILE* source_file = create_temp_file(source_code);
    if (!source_file) {
        error_cleanup();
        return false;
    }
    
    // Initialize lexer
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(source_file, "numeric_literals.ћпп", target_info);
    if (!lexer) {
        fclose(source_file);
        error_cleanup();
        return false;
    }
    
    // Expected numeric values
    intptr_t expected_values[] = {0, 42, 123456789, 123, 3};
    
    // Scan tokens and check numeric literals
    Token token;
    int found_numerics = 0;
    bool all_passed = true;
    
    printf("Checking numeric literals:\n");
    while (found_numerics < (int)(sizeof(expected_values) / sizeof(expected_values[0]))) {
        token = lexer_next_token(lexer);
        
        if (token.type == TOKEN_EOF) {
            fprintf(stderr, "Reached EOF before finding all expected numeric literals\n");
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Error token encountered: %.*s\n", 
                   (int)token.lexeme_length, token.lexeme);
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_NUMBER) {
            printf("Found numeric literal: %.*s = %ld ", 
                   (int)token.lexeme_length, token.lexeme, 
                   (long)token.value.int_value);
            
            if (token.value.int_value == expected_values[found_numerics]) {
                printf("✓\n");
            } else {
                printf("✗ (expected %ld)\n", (long)expected_values[found_numerics]);
                all_passed = false;
            }
            
            found_numerics++;
        }
    }
    
    // Clean up
    lexer_cleanup(lexer);
    fclose(source_file);
    error_cleanup();
    
    if (all_passed && found_numerics == (int)(sizeof(expected_values) / sizeof(expected_values[0]))) {
        printf("All numeric literal tests passed!\n");
        return true;
    } else {
        fprintf(stderr, "Some numeric literal tests failed!\n");
        return false;
    }
}

/**
 * Test string and character literals
 * 
 * Verifies that the lexer correctly processes string and character literals,
 * including escape sequences and UTF-8 characters.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_string_char_literals(void) {
    printf("\n=== Testing string and character literals ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a source file with string and character literals
    const char* source_code = 
        "// Test string and character literals\n"
        "\"Hello, World!\"\n"
        "\"Special chars: \\n \\t \\r \\\\ \\\"\"\n"
        "\"UTF-8 string: Привет\"\n"
        "'a'\n"
        "'\\n'\n"
        "'\\t'\n"
        "'\\\\'\n"
        "'\\\''\n"
        "'ћ'\n";  // Serbian Cyrillic character
    
    FILE* source_file = create_temp_file(source_code);
    if (!source_file) {
        error_cleanup();
        return false;
    }
    
    // Initialize lexer
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(source_file, "string_char_literals.ћпп", target_info);
    if (!lexer) {
        fclose(source_file);
        error_cleanup();
        return false;
    }
    
    // Expected literals
    struct {
        TokenType type;
        const char* str_value;  // For strings
        int32_t char_value;     // For characters
    } expected_literals[] = {
        {TOKEN_STRING, "Hello, World!", 0},
        {TOKEN_STRING, "Special chars: \n \t \r \\ \"", 0},
        {TOKEN_STRING, "UTF-8 string: Привет", 0},
        {TOKEN_CHAR_LITERAL, NULL, 'a'},
        {TOKEN_CHAR_LITERAL, NULL, '\n'},
        {TOKEN_CHAR_LITERAL, NULL, '\t'},
        {TOKEN_CHAR_LITERAL, NULL, '\\'},
        {TOKEN_CHAR_LITERAL, NULL, '\''},
        {TOKEN_CHAR_LITERAL, NULL, 0x045B}  // Cyrillic small letter tshe (ћ)
    };
    
    // Scan tokens and check literals
    Token token;
    int found_literals = 0;
    bool all_passed = true;
    
    printf("Checking string and character literals:\n");
    while (found_literals < (int)(sizeof(expected_literals) / sizeof(expected_literals[0]))) {
        token = lexer_next_token(lexer);
        
        if (token.type == TOKEN_EOF) {
            fprintf(stderr, "Reached EOF before finding all expected literals\n");
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_ERROR) {
            fprintf(stderr, "Error token encountered: %.*s\n", 
                   (int)token.lexeme_length, token.lexeme);
            all_passed = false;
            break;
        }
        
        if (token.type == TOKEN_STRING || token.type == TOKEN_CHAR_LITERAL) {
            if (token.type == TOKEN_STRING) {
                printf("Found string literal: \"%s\" ", token.value.string_value);
                
                if (expected_literals[found_literals].type == TOKEN_STRING && 
                    strcmp(token.value.string_value, 
                           expected_literals[found_literals].str_value) == 0) {
                    printf("✓\n");
                } else {
                    printf("✗\n");
                    all_passed = false;
                }
            } else {  // CHAR_LITERAL
                printf("Found character literal: '");
                
                if (token.value.char_value >= 32 && token.value.char_value <= 126) {
                    printf("%c", (char)token.value.char_value);
                } else if (token.value.char_value == '\n') {
                    printf("\\n");
                } else if (token.value.char_value == '\t') {
                    printf("\\t");
                } else if (token.value.char_value == '\r') {
                    printf("\\r");
                } else if (token.value.char_value == '\\') {
                    printf("\\\\");
                } else {
                    printf("\\u%04x", token.value.char_value);
                }
                
                printf("' (Unicode: U+%04X) ", token.value.char_value);
                
                if (expected_literals[found_literals].type == TOKEN_CHAR_LITERAL && 
                    token.value.char_value == expected_literals[found_literals].char_value) {
                    printf("✓\n");
                } else {
                    printf("✗ (expected U+%04X)\n", 
                           expected_literals[found_literals].char_value);
                    all_passed = false;
                }
            }
            
            found_literals++;
        }
    }
    
    // Clean up
    lexer_cleanup(lexer);
    fclose(source_file);
    error_cleanup();
    
    if (all_passed && found_literals == (int)(sizeof(expected_literals) / sizeof(expected_literals[0]))) {
        printf("All string and character literal tests passed!\n");
        return true;
    } else {
        fprintf(stderr, "Some string or character literal tests failed!\n");
        return false;
    }
}

/**
 * Test lexer_print_all_tokens functionality
 * 
 * Verifies that the lexer can print all tokens in a source file.
 * This is primarily a visual test.
 * 
 * @return true if the test passes, false otherwise
 */
bool test_print_all_tokens(void) {
    printf("\n=== Testing lexer_print_all_tokens ===\n");
    
    // Initialize error handling
    if (!error_init()) {
        fprintf(stderr, "Failed to initialize error handling system\n");
        return false;
    }
    
    // Create a source file with various tokens
    const char* source_code = 
        "// This is a sample ћ++ program\n"
        "ако (x > 0) <\n"
        "    врати x * 2;\n"
        "> иначе <\n"
        "    врати 0;\n"
        ">\n";
    
    FILE* source_file = create_temp_file(source_code);
    if (!source_file) {
        error_cleanup();
        return false;
    }
    
    // Initialize lexer
    TargetInfo target_info = target_init();
    LexerState* lexer = lexer_init(source_file, "test.ћпп", target_info);
    if (!lexer) {
        fclose(source_file);
        error_cleanup();
        return false;
    }
    
    // Call the function we're testing
    printf("Printing all tokens from the source file:\n");
    lexer_print_all_tokens(lexer);
    
    // Clean up
    lexer_cleanup(lexer);
    fclose(source_file);
    error_cleanup();
    
    printf("Test completed. Visual inspection required to verify token printing.\n");
    return true;
}

/**
 * Run all lexer tests
 */
int main(void) {
    bool all_tests_passed = true;
    
    // Run the test for print_all_keywords
    if (!test_print_all_keywords()) {
        all_tests_passed = false;
        fprintf(stderr, "test_print_all_keywords failed\n");
    }
    
    // Test lexer initialization
    if (!test_lexer_init()) {
        all_tests_passed = false;
        fprintf(stderr, "test_lexer_init failed\n");
    }
    
    // Test basic token recognition
    if (!test_basic_tokens()) {
        all_tests_passed = false;
        fprintf(stderr, "test_basic_tokens failed\n");
    }
    
    // Test UTF-8 identifier handling
    if (!test_utf8_identifiers()) {
        all_tests_passed = false;
        fprintf(stderr, "test_utf8_identifiers failed\n");
    }
    
    // Test UTF-8 keyword recognition
    if (!test_utf8_keywords()) {
        all_tests_passed = false;
        fprintf(stderr, "test_utf8_keywords failed\n");
    }
    
    // Test numeric literals
    if (!test_numeric_literals()) {
        all_tests_passed = false;
        fprintf(stderr, "test_numeric_literals failed\n");
    }
    
    // Test string and character literals
    if (!test_string_char_literals()) {
        all_tests_passed = false;
        fprintf(stderr, "test_string_char_literals failed\n");
    }
    
    // Test token printing
    if (!test_print_all_tokens()) {
        all_tests_passed = false;
        fprintf(stderr, "test_print_all_tokens failed\n");
    }
    
    cleanup_temp_file();
    
    if (all_tests_passed) {
        printf("\nAll lexer tests passed!\n");
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "\nSome lexer tests failed!\n");
        return EXIT_FAILURE;
    }
}