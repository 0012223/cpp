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

#include <stdbool.h>
#include <stdint.h>
#include "../lexer/lexer.h"
#include "../utils/error.h"

// Forward declarations
typedef struct AstNode AstNode;
typedef struct TypeInfo TypeInfo;

// Location information for error reporting
typedef struct {
    int line;
    int column;
    const char* filename;
} SourceLocation;

// AST node types enum
typedef enum {
    // Program structure
    AST_PROGRAM,             // Top-level program node
    AST_FUNCTION_DECL,       // Function declaration
    AST_EXTERNAL_DECL,       // External function declaration
    
    // Statements
    AST_BLOCK,               // Block of statements (< >)
    AST_IF_STMT,             // If statement (with optional else)
    AST_WHILE_STMT,          // While loop
    AST_FOR_STMT,            // For loop
    AST_DO_WHILE_STMT,       // Do-while loop
    AST_RETURN_STMT,         // Return statement
    AST_BREAK_STMT,          // Break statement
    AST_VAR_DECL,            // Variable declaration
    AST_ARRAY_DECL,          // Array declaration
    AST_EXPR_STMT,           // Expression statement
    
    // Expressions
    AST_BINARY_EXPR,         // Binary operation
    AST_UNARY_EXPR,          // Unary operation
    AST_LITERAL_INT,         // Integer literal
    AST_LITERAL_CHAR,        // Character literal
    AST_LITERAL_STRING,      // String literal
    AST_LITERAL_BOOL,        // Boolean literal
    AST_IDENTIFIER,          // Variable or function reference
    AST_ARRAY_ACCESS,        // Array element access
    AST_CALL_EXPR,           // Function call
    AST_ASSIGNMENT,          // Assignment expression
    
    // Type nodes
    AST_TYPE
} AstNodeType;

// Basic type categories
typedef enum {
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_ARRAY,
    TYPE_FUNCTION
} TypeCategory;

// Type information structure
struct TypeInfo {
    TypeCategory category;
    // For array types
    union {
        struct {
            TypeInfo* element_type;
            int size;
        } array;
        // For function types
        struct {
            TypeInfo* return_type;
            TypeInfo** param_types;
            int param_count;
        } function;
    } info;
};

// Abstract syntax tree node
struct AstNode {
    AstNodeType type;
    SourceLocation location;
    TypeInfo* type_info;     // Type information (filled during semantic analysis)
    
    union {
        // Program node
        struct {
            AstNode** declarations;
            int declaration_count;
        } program;
        
        // Function declaration
        struct {
            char* name;
            AstNode** parameters;
            int parameter_count;
            AstNode* body;
            TypeInfo* return_type;
            bool is_external;
        } function_decl;
        
        // Variable declaration
        struct {
            char* name;
            AstNode* initializer;
            TypeInfo* var_type;
        } var_decl;
        
        // Array declaration
        struct {
            char* name;
            int size;
            AstNode** initializers;
            int initializer_count;
            TypeInfo* element_type;
        } array_decl;
        
        // Block statement
        struct {
            AstNode** statements;
            int statement_count;
        } block;
        
        // If statement
        struct {
            AstNode* condition;
            AstNode* then_branch;
            AstNode* else_branch;
        } if_stmt;
        
        // While statement
        struct {
            AstNode* condition;
            AstNode* body;
        } while_stmt;
        
        // For statement
        struct {
            AstNode* initializer;
            AstNode* condition;
            AstNode* increment;
            AstNode* body;
        } for_stmt;
        
        // Return statement
        struct {
            AstNode* value;
        } return_stmt;
        
        // Expression statement
        struct {
            AstNode* expression;
        } expr_stmt;
        
        // Binary expression
        struct {
            AstNode* left;
            AstNode* right;
            TokenType operator;
        } binary_expr;
        
        // Unary expression
        struct {
            AstNode* operand;
            TokenType operator;
            bool is_prefix;
        } unary_expr;
        
        // Literal expressions
        struct {
            intptr_t value;
        } int_literal;
        
        struct {
            int32_t value;
        } char_literal;
        
        struct {
            char* value;
        } string_literal;
        
        struct {
            bool value;
        } bool_literal;
        
        // Identifier reference
        struct {
            char* name;
        } identifier;
        
        // Array access
        struct {
            AstNode* array;
            AstNode* index;
        } array_access;
        
        // Function call
        struct {
            AstNode* callee;
            AstNode** arguments;
            int argument_count;
        } call_expr;
        
        // Assignment
        struct {
            AstNode* target;
            AstNode* value;
        } assignment;
        
        // Type node
        struct {
            TypeInfo* type_data;
        } type_node;
    } as;
};

// Visitor pattern for AST traversal
typedef struct AstVisitor {
    void* context;
    
    // Callbacks for each node type
    bool (*visit_program)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_function_decl)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_var_decl)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_array_decl)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_block)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_if_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_while_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_for_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_do_while_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_return_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_break_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_expr_stmt)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_binary_expr)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_unary_expr)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_literal_int)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_literal_char)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_literal_string)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_literal_bool)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_identifier)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_array_access)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_call_expr)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_assignment)(struct AstVisitor* visitor, AstNode* node);
    bool (*visit_type)(struct AstVisitor* visitor, AstNode* node);
} AstVisitor;

// Function to apply visitor to an AST node
bool ast_accept(AstNode* node, AstVisitor* visitor);

//------------------------------------------------------------------------------
// Type creation functions
//------------------------------------------------------------------------------

/**
 * Create a void type
 *
 * @return A new void type
 */
TypeInfo* type_create_void(void);

/**
 * Create a boolean type
 *
 * @return A new boolean type
 */
TypeInfo* type_create_bool(void);

/**
 * Create a character type
 *
 * @return A new character type
 */
TypeInfo* type_create_char(void);

/**
 * Create an integer type
 *
 * @return A new integer type
 */
TypeInfo* type_create_int(void);

/**
 * Create an array type
 *
 * @param element_type Type of the array elements
 * @param size Size of the array, -1 for dynamic arrays
 * @return A new array type
 */
TypeInfo* type_create_array(TypeInfo* element_type, int size);

/**
 * Create a function type
 *
 * @param return_type Return type of the function
 * @param param_types Array of parameter types
 * @param param_count Number of parameters
 * @return A new function type
 */
TypeInfo* type_create_function(TypeInfo* return_type, TypeInfo** param_types, int param_count);

/**
 * Free a type information structure
 *
 * @param type The type to free
 */
void type_free(TypeInfo* type);

/**
 * Create a source location structure
 *
 * @param line Line number
 * @param column Column number
 * @param filename Source file name
 * @return A new source location structure
 */
SourceLocation source_location_create(int line, int column, const char* filename);

//------------------------------------------------------------------------------
// Node creation functions
//------------------------------------------------------------------------------

/**
 * Create a program node (the root of the AST)
 *
 * @param declarations Array of top-level declarations
 * @param declaration_count Number of declarations
 * @param location Source location information
 * @return A new program node
 */
AstNode* ast_create_program(AstNode** declarations, int declaration_count, SourceLocation location);

/**
 * Create a function declaration node
 *
 * @param name Function name (UTF-8 encoded)
 * @param parameters Array of parameter nodes
 * @param parameter_count Number of parameters
 * @param body Function body (block statement)
 * @param return_type Return type of the function
 * @param is_external Whether the function is an external declaration
 * @param location Source location information
 * @return A new function declaration node
 */
AstNode* ast_create_function_decl(char* name, AstNode** parameters, int parameter_count, 
                                 AstNode* body, TypeInfo* return_type, bool is_external,
                                 SourceLocation location);

/**
 * Create a variable declaration node
 *
 * @param name Variable name (UTF-8 encoded)
 * @param initializer Expression for the initial value (or NULL)
 * @param var_type Type of the variable
 * @param location Source location information
 * @return A new variable declaration node
 */
AstNode* ast_create_var_decl(char* name, AstNode* initializer, TypeInfo* var_type,
                            SourceLocation location);

/**
 * Create an array declaration node
 *
 * @param name Array name (UTF-8 encoded)
 * @param size Size of the array
 * @param initializers Array of initializer expressions
 * @param initializer_count Number of initializers
 * @param element_type Type of the array elements
 * @param location Source location information
 * @return A new array declaration node
 */
AstNode* ast_create_array_decl(char* name, int size, AstNode** initializers, 
                             int initializer_count, TypeInfo* element_type,
                             SourceLocation location);

/**
 * Create a block statement node
 *
 * @param statements Array of statement nodes
 * @param statement_count Number of statements
 * @param location Source location information
 * @return A new block statement node
 */
AstNode* ast_create_block(AstNode** statements, int statement_count, SourceLocation location);

/**
 * Create an if statement node
 *
 * @param condition Expression for the condition
 * @param then_branch Statement for the then branch
 * @param else_branch Statement for the else branch (or NULL)
 * @param location Source location information
 * @return A new if statement node
 */
AstNode* ast_create_if_stmt(AstNode* condition, AstNode* then_branch, AstNode* else_branch,
                           SourceLocation location);

/**
 * Create a while statement node
 *
 * @param condition Expression for the condition
 * @param body Statement for the loop body
 * @param location Source location information
 * @return A new while statement node
 */
AstNode* ast_create_while_stmt(AstNode* condition, AstNode* body, SourceLocation location);

/**
 * Create a for statement node
 *
 * @param initializer Initialization expression (or NULL)
 * @param condition Condition expression (or NULL)
 * @param increment Increment expression (or NULL)
 * @param body Statement for the loop body
 * @param location Source location information
 * @return A new for statement node
 */
AstNode* ast_create_for_stmt(AstNode* initializer, AstNode* condition, AstNode* increment,
                            AstNode* body, SourceLocation location);

/**
 * Create a do-while statement node
 *
 * @param body Statement for the loop body
 * @param condition Expression for the condition
 * @param location Source location information
 * @return A new do-while statement node
 */
AstNode* ast_create_do_while_stmt(AstNode* body, AstNode* condition, SourceLocation location);

/**
 * Create a return statement node
 *
 * @param value Expression for the return value (or NULL for void return)
 * @param location Source location information
 * @return A new return statement node
 */
AstNode* ast_create_return_stmt(AstNode* value, SourceLocation location);

/**
 * Create a break statement node
 *
 * @param location Source location information
 * @return A new break statement node
 */
AstNode* ast_create_break_stmt(SourceLocation location);

/**
 * Create an expression statement node
 *
 * @param expression The expression to execute
 * @param location Source location information
 * @return A new expression statement node
 */
AstNode* ast_create_expr_stmt(AstNode* expression, SourceLocation location);

/**
 * Create a binary expression node
 *
 * @param left Left operand
 * @param operator Binary operator
 * @param right Right operand
 * @param location Source location information
 * @return A new binary expression node
 */
AstNode* ast_create_binary_expr(AstNode* left, TokenType operator, AstNode* right,
                               SourceLocation location);

/**
 * Create a unary expression node
 *
 * @param operand Operand expression
 * @param operator Unary operator
 * @param is_prefix Whether the operator is a prefix (true) or postfix (false)
 * @param location Source location information
 * @return A new unary expression node
 */
AstNode* ast_create_unary_expr(AstNode* operand, TokenType operator, bool is_prefix,
                              SourceLocation location);

/**
 * Create an integer literal node
 *
 * @param value Integer value
 * @param location Source location information
 * @return A new integer literal node
 */
AstNode* ast_create_literal_int(intptr_t value, SourceLocation location);

/**
 * Create a character literal node
 *
 * @param value Character value (as a UTF-8 codepoint)
 * @param location Source location information
 * @return A new character literal node
 */
AstNode* ast_create_literal_char(int32_t value, SourceLocation location);

/**
 * Create a string literal node
 *
 * @param value String value (UTF-8 encoded)
 * @param location Source location information
 * @return A new string literal node
 */
AstNode* ast_create_literal_string(char* value, SourceLocation location);

/**
 * Create a boolean literal node
 *
 * @param value Boolean value
 * @param location Source location information
 * @return A new boolean literal node
 */
AstNode* ast_create_literal_bool(bool value, SourceLocation location);

/**
 * Create an identifier node
 *
 * @param name Identifier name (UTF-8 encoded)
 * @param location Source location information
 * @return A new identifier node
 */
AstNode* ast_create_identifier(char* name, SourceLocation location);

/**
 * Create an array access node
 *
 * @param array Expression for the array
 * @param index Expression for the index
 * @param location Source location information
 * @return A new array access node
 */
AstNode* ast_create_array_access(AstNode* array, AstNode* index, SourceLocation location);

/**
 * Create a function call node
 *
 * @param callee Expression for the function to call
 * @param arguments Array of argument expressions
 * @param argument_count Number of arguments
 * @param location Source location information
 * @return A new function call node
 */
AstNode* ast_create_call_expr(AstNode* callee, AstNode** arguments, int argument_count,
                             SourceLocation location);

/**
 * Create an assignment node
 *
 * @param target Expression for the assignment target
 * @param value Expression for the value to assign
 * @param location Source location information
 * @return A new assignment node
 */
AstNode* ast_create_assignment(AstNode* target, AstNode* value, SourceLocation location);

/**
 * Create a type node
 *
 * @param type_data Type information
 * @param location Source location information
 * @return A new type node
 */
AstNode* ast_create_type(TypeInfo* type_data, SourceLocation location);

/**
 * Free an AST node and all its children recursively
 *
 * @param node The node to free
 */
void ast_free(AstNode* node);

/**
 * Print an AST node for debugging
 *
 * @param node The node to print
 * @param indent Indentation level
 */
void ast_print(AstNode* node, int indent);

/**
 * Create a deep clone of an AST node and all its children
 *
 * @param node The node to clone
 * @return A new node that is a deep copy of the original
 */
AstNode* ast_clone(AstNode* node);

#endif /* AST_H */