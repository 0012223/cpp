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
// Other includes will go here