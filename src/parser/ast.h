/*
 * filename: ast.h
 * 
 * Purpose:
 * Header file defining the Abstract Syntax Tree (AST) structures for the ћ++ compiler.
 * This file contains all the node types and structures that represent the parsed
 * program structure in memory.
 * 
 * Key Components:
 * - AstNodeType enum: Defines all possible AST node types
 * - AstNode struct: Base structure for all AST nodes
 * - Specific node types (FunctionNode, StatementNode, ExpressionNode, etc.)
 * - ast_create_*(): Functions to create different types of AST nodes
 * - ast_free(): Function to free AST nodes and their children
 * 
 * Interactions:
 * - Created by parser.c during syntax analysis
 * - Used by semantic_analyzer.c for semantic checks
 * - Used by ir.c to generate intermediate representation
 * 
 * Notes:
 * - All string data in nodes (identifiers, etc.) is UTF-8 encoded
 * - AST reflects the structure of ћ++ with angle brackets for blocks
 * - Special nodes for array declarations and external function declarations
 */

#ifndef AST_H
#define AST_H

// Header content will be implemented later

#endif /* AST_H */