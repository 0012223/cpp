/*
 * filename: parser.h
 * 
 * Purpose:
 * Header file for the syntactic analyzer of the ћ++ compiler.
 * Defines the parser interface for processing tokens into an abstract syntax tree.
 * 
 * Key Components:
 * - Parser struct: Maintains parser state during syntax analysis
 * - parser_init(): Initialize a new parser with a lexer
 * - parse_program(): Entry point for parsing a complete program
 * - parse_function_declaration(): Parse a function declaration
 * - parse_statement(): Parse a statement
 * - parse_expression(): Parse an expression
 * 
 * Interactions:
 * - Uses lexer.h to obtain tokens
 * - Creates AST nodes defined in ast.h
 * - Uses error.h for reporting syntax errors
 * 
 * Notes:
 * - Implements a recursive descent parser
 * - Handles angle brackets (< >) for code blocks
 * - Special handling for array declarations (низ:number: = _values_)
 */

#ifndef PARSER_H
#define PARSER_H

// Header content will be implemented later

#endif /* PARSER_H */