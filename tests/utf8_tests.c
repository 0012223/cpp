/*
 * filename: utf8_tests.c
 * 
 * Purpose:
 * Test suite for the UTF-8 string handling utilities of the ћ++ compiler.
 * Contains tests that verify the correct operation of the UTF-8 encoding,
 * decoding, character classification, and string operations.
 * 
 * Key Components:
 * - test_utf8_utilities(): Main test function
 * - Tests for utf8_decode(), utf8_encode(), utf8_strlen()
 * - Tests for utf8_strcmp()
 * - Tests for character classification functions
 * 
 * Interactions:
 * - Uses the utils/utf8.h interface
 * 
 * Notes:
 * - Run with the --test-utf8 command-line option
 * - Can show verbose output with the --verbose flag
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../src/utils/utf8.h"

// Test the UTF-8 utility functions
void test_utf8_utilities(bool verbose) {
    printf("\n--- Testing UTF-8 Utilities ---\n");
    
    // Test cases for UTF-8 encoding and decoding
    struct {
        const char* text;
        size_t expected_length;
        utf8_codepoint_t first_codepoint;
    } test_cases[] = {
        {"Hello", 5, 'H'},                        // ASCII 
        {"ћ++", 3, 0x045B},                       // Serbian letter ћ + ASCII
        {"главна", 6, 0x0433},                    // Cyrillic
        {"низ:4:", 6, 0x043D},                    // Cyrillic with punctuation
        {"екстерно", 8, 0x0435},                  // Cyrillic 
        {"врати", 5, 0x0432},                     // Cyrillic
        {"абвгдђежзијклљмнњопрстћуфхцчџш", 30, 0x0430}, // Cyrillic alphabet
        {"_variable_1", 11, '_'},                 // ASCII with underscore
        {"한국어", 3, 0xD55C},                    // Korean (multi-byte)
        {"😊", 1, 0x1F60A},                       // Emoji (4 bytes)
        {"", 0, 0}                                // Empty string
    };
    
    int tests_passed = 0;
    int tests_failed = 0;
    
    // Run test cases
    for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        const char* text = test_cases[i].text;
        size_t expected_length = test_cases[i].expected_length;
        utf8_codepoint_t expected_first_codepoint = test_cases[i].first_codepoint;
        
        // Test utf8_strlen
        size_t length = utf8_strlen(text);
        bool length_test = (length == expected_length);
        
        // Test utf8_decode for the first character
        utf8_char_t first_char;
        int bytes = 0;
        if (*text) {
            bytes = utf8_decode(text, &first_char);
        }
        bool decode_test = (bytes > 0 && first_char.codepoint == expected_first_codepoint);
        
        // For non-empty strings, test utf8_encode by encoding the first character
        bool encode_test = true;
        if (*text) {
            utf8_char_t encoded_char;
            int encoded_bytes = utf8_encode(expected_first_codepoint, &encoded_char);
            encode_test = (encoded_bytes == bytes && 
                          memcmp(encoded_char.bytes, first_char.bytes, bytes) == 0);
        }
        
        // Check if all tests passed
        bool all_passed = length_test && (text[0] == '\0' || (decode_test && encode_test));
        
        if (all_passed) {
            tests_passed++;
        } else {
            tests_failed++;
        }
        
        // Print results
        if (verbose || !all_passed) {
            printf("Test %zu: \"%s\"\n", i + 1, text);
            printf("  String length: %zu characters %s\n", 
                  length, length_test ? "✓" : "✗");
            
            if (*text) {
                printf("  First character: U+%04X %s\n", 
                      (unsigned int)first_char.codepoint, decode_test ? "✓" : "✗");
                printf("  Encode test: %s\n", encode_test ? "✓" : "✗");
            }
            
            if (!all_passed) {
                printf("  FAILED!\n");
            }
            printf("\n");
        }
    }
    
    // Test utf8_strcmp
    printf("Testing UTF-8 string comparison:\n");
    const char* str1 = "главна";
    const char* str2 = "главна";
    const char* str3 = "врати";
    
    bool strcmp_test1 = (utf8_strcmp(str1, str2) == 0);
    bool strcmp_test2 = (utf8_strcmp(str1, str3) != 0);
    
    printf("  \"%s\" == \"%s\": %s\n", str1, str2, strcmp_test1 ? "✓" : "✗");
    printf("  \"%s\" != \"%s\": %s\n", str1, str3, strcmp_test2 ? "✓" : "✗");
    
    if (strcmp_test1 && strcmp_test2) {
        tests_passed++;
    } else {
        tests_failed++;
    }
    
    // Test character classification
    printf("\nTesting character classification:\n");
    struct {
        utf8_codepoint_t codepoint;
        const char* description;
        bool should_be_alphabetic;
        bool should_be_identifier_start;
    } char_tests[] = {
        {'a', "Latin lowercase a", true, true},
        {'Z', "Latin uppercase Z", true, true},
        {'0', "Digit 0", false, false},
        {'_', "Underscore", false, true},
        {0x0433, "Cyrillic г", true, true},
        {0x045B, "Serbian ћ", true, true}
    };
    
    for (size_t i = 0; i < sizeof(char_tests) / sizeof(char_tests[0]); i++) {
        utf8_codepoint_t cp = char_tests[i].codepoint;
        bool is_alpha = utf8_is_alphabetic(cp);
        bool is_id_start = utf8_is_identifier_char(cp, true);
        
        bool alpha_test = (is_alpha == char_tests[i].should_be_alphabetic);
        bool id_start_test = (is_id_start == char_tests[i].should_be_identifier_start);
        
        printf("  U+%04X (%s):\n", (unsigned int)cp, char_tests[i].description);
        printf("    Alphabetic: %s %s\n", 
              is_alpha ? "yes" : "no", alpha_test ? "✓" : "✗");
        printf("    Identifier start: %s %s\n", 
              is_id_start ? "yes" : "no", id_start_test ? "✓" : "✗");
        
        if (alpha_test && id_start_test) {
            tests_passed++;
        } else {
            tests_failed++;
        }
    }
    
    // Print summary
    printf("\nUTF-8 Tests: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("----------------------------\n\n");
}

// Main entry point for the test
int main(int argc, char* argv[]) {
    bool verbose = (argc >= 2 && strcmp(argv[1], "--verbose") == 0);
    test_utf8_utilities(verbose);
    return 0;
}