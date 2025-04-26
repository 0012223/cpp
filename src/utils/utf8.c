/*
 * filename: utf8.c
 * 
 * Purpose:
 * Implementation of UTF-8 string handling utilities for the ћ++ compiler.
 * Provides functions for working with UTF-8 encoded strings to support
 * the handling of UTF-8 identifiers and keywords in ћ++.
 * 
 * Key Components:
 * - utf8_decode(): Convert a UTF-8 byte sequence to a Unicode codepoint
 * - utf8_encode(): Convert a Unicode codepoint to a UTF-8 byte sequence
 * - utf8_strlen(): Count characters in a UTF-8 string
 * - utf8_strcmp(): Compare two UTF-8 strings properly
 * - utf8_is_alphabetic(): Check if a Unicode codepoint is a letter
 * - utf8_is_identifier_char(): Check if a codepoint can be used in an identifier
 * 
 * Interactions:
 * - Used by lexer.c for tokenizing and recognizing identifiers
 * - Used by symbol_table.c for handling identifier names
 * 
 * Notes:
 * - Handles full Unicode range
 * - Properly validates UTF-8 sequences
 * - Implements Unicode character classifications for identifiers
 */

#include "utf8.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Decode a UTF-8 character from a string.
 */
int utf8_decode(const char* str, utf8_char_t* character) {
    if (!str || !character) {
        return 0;
    }
    
    /* Clear the character structure */
    memset(character, 0, sizeof(utf8_char_t));
    
    /* Handle empty string */
    if (!str[0]) {
        return 0;
    }
    
    /* First byte */
    unsigned char first_byte = (unsigned char)str[0];
    character->bytes[0] = str[0];
    
    /* Determine the number of bytes in the character based on the first byte */
    int num_bytes;
    utf8_codepoint_t codepoint;
    
    if ((first_byte & 0x80) == 0) {
        /* 0xxxxxxx: Single-byte ASCII character */
        num_bytes = 1;
        codepoint = first_byte;
    } else if ((first_byte & 0xE0) == 0xC0) {
        /* 110xxxxx 10xxxxxx: Two-byte sequence */
        num_bytes = 2;
        codepoint = first_byte & 0x1F;
    } else if ((first_byte & 0xF0) == 0xE0) {
        /* 1110xxxx 10xxxxxx 10xxxxxx: Three-byte sequence */
        num_bytes = 3;
        codepoint = first_byte & 0x0F;
    } else if ((first_byte & 0xF8) == 0xF0) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx: Four-byte sequence */
        num_bytes = 4;
        codepoint = first_byte & 0x07;
    } else {
        /* Invalid first byte */
        return 0;
    }
    
    /* Check for string termination and read continuation bytes */
    for (int i = 1; i < num_bytes; i++) {
        /* Check for premature string termination */
        if (!str[i]) {
            return 0;
        }
        
        /* Check if this is a valid continuation byte (10xxxxxx) */
        unsigned char byte = (unsigned char)str[i];
        if (!UTF8_IS_CONTINUATION(byte)) {
            return 0;
        }
        
        /* Store the byte in the character structure */
        character->bytes[i] = str[i];
        
        /* Accumulate bits from the continuation byte */
        codepoint = (codepoint << 6) | (byte & 0x3F);
    }
    
    /* Set the character properties */
    character->len = num_bytes;
    character->codepoint = codepoint;
    
    /* Check for overlong encodings */
    if ((num_bytes == 2 && codepoint < 0x80) ||
        (num_bytes == 3 && codepoint < 0x800) ||
        (num_bytes == 4 && codepoint < 0x10000)) {
        return 0;
    }
    
    /* Check for invalid Unicode code points */
    if (codepoint > 0x10FFFF || 
        (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
        return 0;
    }
    
    return num_bytes;
}

/*
 * Encode a Unicode code point to UTF-8.
 */
int utf8_encode(utf8_codepoint_t codepoint, utf8_char_t* character) {
    if (!character) {
        return 0;
    }
    
    /* Clear the character structure */
    memset(character, 0, sizeof(utf8_char_t));
    
    /* Store the code point */
    character->codepoint = codepoint;
    
    /* Determine the number of bytes needed */
    int num_bytes;
    
    if (codepoint < 0x80) {
        /* Single-byte encoding */
        num_bytes = 1;
        character->bytes[0] = (char)codepoint;
    } else if (codepoint < 0x800) {
        /* Two-byte encoding */
        num_bytes = 2;
        character->bytes[0] = (char)(0xC0 | (codepoint >> 6));
        character->bytes[1] = (char)(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
        /* Three-byte encoding */
        num_bytes = 3;
        character->bytes[0] = (char)(0xE0 | (codepoint >> 12));
        character->bytes[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        character->bytes[2] = (char)(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) {
        /* Four-byte encoding */
        num_bytes = 4;
        character->bytes[0] = (char)(0xF0 | (codepoint >> 18));
        character->bytes[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        character->bytes[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        character->bytes[3] = (char)(0x80 | (codepoint & 0x3F));
    } else {
        /* Invalid code point */
        return 0;
    }
    
    character->len = num_bytes;
    return num_bytes;
}

/*
 * Get the length of a UTF-8 string in characters (not bytes).
 */
size_t utf8_strlen(const char* str) {
    if (!str) {
        return 0;
    }
    
    size_t count = 0;
    utf8_char_t ch;
    const char* p = str;
    
    while (*p) {
        int bytes = utf8_decode(p, &ch);
        if (bytes == 0) {
            /* Invalid UTF-8 sequence, skip one byte and continue */
            p++;
        } else {
            count++;
            p += bytes;
        }
    }
    
    return count;
}

/*
 * Compare two UTF-8 strings.
 */
int utf8_strcmp(const char* str1, const char* str2) {
    if (!str1 && !str2) {
        return 0;
    }
    if (!str1) {
        return -1;
    }
    if (!str2) {
        return 1;
    }
    
    const char* p1 = str1;
    const char* p2 = str2;
    
    while (*p1 && *p2) {
        utf8_char_t ch1, ch2;
        int bytes1 = utf8_decode(p1, &ch1);
        int bytes2 = utf8_decode(p2, &ch2);
        
        /* Handle invalid UTF-8 sequences */
        if (bytes1 == 0) {
            p1++;
            continue;
        }
        if (bytes2 == 0) {
            p2++;
            continue;
        }
        
        /* Compare code points */
        if (ch1.codepoint != ch2.codepoint) {
            return ch1.codepoint - ch2.codepoint;
        }
        
        /* Move to next characters */
        p1 += bytes1;
        p2 += bytes2;
    }
    
    /* Check for end of strings */
    if (*p1) {
        return 1;
    }
    if (*p2) {
        return -1;
    }
    
    return 0;
}

/*
 * Check if a Unicode code point is alphabetic.
 * This is a simplified implementation that covers
 * basic Latin and Cyrillic ranges.
 */
bool utf8_is_alphabetic(utf8_codepoint_t codepoint) {
    /* Basic Latin (A-Z, a-z) */
    if ((codepoint >= 'A' && codepoint <= 'Z') ||
        (codepoint >= 'a' && codepoint <= 'z')) {
        return true;
    }
    
    /* Cyrillic (Ѐ-ԯ) */
    if (codepoint >= 0x0400 && codepoint <= 0x04FF) {
        return true;
    }
    
    /* Extended Cyrillic (Ѐ-ӿ) */
    if (codepoint >= 0x0400 && codepoint <= 0x04FF) {
        return true;
    }
    
    /* Serbian-specific characters (ћ, ђ, etc.) */
    if (codepoint == 0x045B || codepoint == 0x0452 ||
        codepoint == 0x045F || codepoint == 0x0458) {
        return true;
    }
    
    /* Additional special characters used in the ћ++ language */
    /* Add any other specific alphabetic characters needed */
    
    return false;
}

/*
 * Check if a Unicode code point is a digit.
 */
bool utf8_is_digit(utf8_codepoint_t codepoint) {
    /* ASCII digits (0-9) */
    return codepoint >= '0' && codepoint <= '9';
}

/*
 * Check if a Unicode code point is alphanumeric.
 */
bool utf8_is_alphanumeric(utf8_codepoint_t codepoint) {
    return utf8_is_alphabetic(codepoint) || utf8_is_digit(codepoint);
}

/*
 * Check if a Unicode code point is valid for an identifier in ћ++.
 */
bool utf8_is_identifier_char(utf8_codepoint_t codepoint, bool is_start) {
    /* Underscore is always valid in identifiers */
    if (codepoint == '_') {
        return true;
    }
    
    /* First character cannot be a digit */
    if (is_start && utf8_is_digit(codepoint)) {
        return false;
    }
    
    /* Alphabetic characters are always valid */
    if (utf8_is_alphabetic(codepoint)) {
        return true;
    }
    
    /* Digits are valid after the first character */
    if (!is_start && utf8_is_digit(codepoint)) {
        return true;
    }
    
    return false;
}

/*
 * Advance to the next UTF-8 character in a string.
 */
bool utf8_next_char(const char** str, utf8_char_t* character) {
    if (!str || !*str || !**str || !character) {
        return false;
    }
    
    int bytes = utf8_decode(*str, character);
    if (bytes == 0) {
        /* Invalid UTF-8 sequence, skip one byte */
        (*str)++;
        return utf8_next_char(str, character);
    }
    
    /* Move to the next character */
    *str += bytes;
    return true;
}