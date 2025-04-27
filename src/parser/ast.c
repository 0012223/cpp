/*
 * filename: ast.c
 * 
 * Purpose:
 * Implementation of the Abstract Syntax Tree (AST) for the ћ++ compiler.
 * This file contains functions to create, manipulate, and free AST nodes
 * that represent the structure of a ћ++ program.
 * 
 * Key Components:
 * - ast_create_program(): Create the root node of the AST
 * - ast_create_function(): Create a function declaration node
 * - ast_create_extern_declaration(): Create an external function declaration node
 * - ast_create_statement_block(): Create a block of statements (< >)
 * - ast_create_array_declaration(): Create an array declaration node
 * - ast_create_expression(): Create various expression nodes
 * - ast_free(): Release memory used by the AST
 * 
 * Interactions:
 * - Used by parser.c to build the program representation
 * - AST is consumed by semantic_analyzer.c and ir.c
 * 
 * Notes:
 * - All string data (identifiers, etc.) is stored as UTF-8
 * - Memory management is handled explicitly with creation/free functions
 * - Node types reflect ћ++ specific constructs like array declarations
 */

#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../utils/error.h"
#include "../utils/utf8.h"

//------------------------------------------------------------------------------
// Helper functions
//------------------------------------------------------------------------------

/**
 * Duplicate a string, allocating new memory
 */
static char* string_duplicate(const char* str) {
    if (!str) return NULL;
    
    size_t len = strlen(str);
    char* result = malloc(len + 1);
    if (!result) return NULL;
    
    memcpy(result, str, len + 1);
    return result;
}

/**
 * Check if a pointer is null and report an error if it is
 */
static bool check_null(void* ptr, const char* error_message, SourceLocation location) {
    if (ptr == NULL) {
        error_report(ERROR_INTERNAL, ERROR_FATAL, location.filename, location.line, location.column,
                     error_message, "This may be due to insufficient memory",
                     __FILE__, __LINE__);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
// Source location functions
//------------------------------------------------------------------------------

SourceLocation source_location_create(int line, int column, const char* filename) {
    SourceLocation location;
    location.line = line;
    location.column = column;
    location.filename = filename;
    return location;
}

//------------------------------------------------------------------------------
// Type creation functions
//------------------------------------------------------------------------------

TypeInfo* type_create_void(void) {
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) return NULL;
    
    type->category = TYPE_VOID;
    return type;
}

TypeInfo* type_create_bool(void) {
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) return NULL;
    
    type->category = TYPE_BOOL;
    return type;
}

TypeInfo* type_create_char(void) {
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) return NULL;
    
    type->category = TYPE_CHAR;
    return type;
}

TypeInfo* type_create_int(void) {
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) return NULL;
    
    type->category = TYPE_INT;
    return type;
}

TypeInfo* type_create_array(TypeInfo* element_type, int size) {
    if (!element_type) return NULL;
    
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) return NULL;
    
    type->category = TYPE_ARRAY;
    type->info.array.element_type = element_type;
    type->info.array.size = size;
    
    return type;
}

TypeInfo* type_create_function(TypeInfo* return_type, TypeInfo** param_types, int param_count) {
    if (!return_type) return NULL;
    
    TypeInfo* type = (TypeInfo*)malloc(sizeof(TypeInfo));
    if (!type) return NULL;
    
    type->category = TYPE_FUNCTION;
    type->info.function.return_type = return_type;
    
    if (param_count > 0 && param_types) {
        type->info.function.param_types = (TypeInfo**)malloc(param_count * sizeof(TypeInfo*));
        if (!type->info.function.param_types) {
            free(type);
            return NULL;
        }
        
        for (int i = 0; i < param_count; i++) {
            type->info.function.param_types[i] = param_types[i];
        }
        type->info.function.param_count = param_count;
    } else {
        type->info.function.param_types = NULL;
        type->info.function.param_count = 0;
    }
    
    return type;
}

void type_free(TypeInfo* type) {
    if (!type) return;
    
    switch (type->category) {
        case TYPE_ARRAY:
            type_free(type->info.array.element_type);
            break;
            
        case TYPE_FUNCTION:
            type_free(type->info.function.return_type);
            for (int i = 0; i < type->info.function.param_count; i++) {
                type_free(type->info.function.param_types[i]);
            }
            free(type->info.function.param_types);
            break;
            
        default:
            // Simple types don't need additional cleanup
            break;
    }
    
    free(type);
}

//------------------------------------------------------------------------------
// Node creation functions
//------------------------------------------------------------------------------

AstNode* ast_create_program(AstNode** declarations, int declaration_count, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate program node", location)) return NULL;
    
    node->type = AST_PROGRAM;
    node->location = location;
    node->type_info = NULL;
    
    if (declaration_count > 0 && declarations) {
        node->as.program.declarations = (AstNode**)malloc(declaration_count * sizeof(AstNode*));
        if (!check_null(node->as.program.declarations, "Failed to allocate declarations array", location)) {
            free(node);
            return NULL;
        }
        
        for (int i = 0; i < declaration_count; i++) {
            node->as.program.declarations[i] = declarations[i];
        }
        node->as.program.declaration_count = declaration_count;
    } else {
        node->as.program.declarations = NULL;
        node->as.program.declaration_count = 0;
    }
    
    return node;
}

AstNode* ast_create_function_decl(char* name, AstNode** parameters, int parameter_count, 
                                 AstNode* body, TypeInfo* return_type, bool is_external,
                                 SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate function declaration node", location)) return NULL;
    
    node->type = AST_FUNCTION_DECL;
    node->location = location;
    node->type_info = NULL;
    
    node->as.function_decl.name = string_duplicate(name);
    if (!check_null(node->as.function_decl.name, "Failed to allocate function name", location)) {
        free(node);
        return NULL;
    }
    
    node->as.function_decl.body = body;
    node->as.function_decl.return_type = return_type;
    node->as.function_decl.is_external = is_external;
    
    if (parameter_count > 0 && parameters) {
        node->as.function_decl.parameters = (AstNode**)malloc(parameter_count * sizeof(AstNode*));
        if (!check_null(node->as.function_decl.parameters, "Failed to allocate parameters array", location)) {
            free(node->as.function_decl.name);
            free(node);
            return NULL;
        }
        
        for (int i = 0; i < parameter_count; i++) {
            node->as.function_decl.parameters[i] = parameters[i];
        }
        node->as.function_decl.parameter_count = parameter_count;
    } else {
        node->as.function_decl.parameters = NULL;
        node->as.function_decl.parameter_count = 0;
    }
    
    return node;
}

AstNode* ast_create_var_decl(char* name, AstNode* initializer, TypeInfo* var_type,
                            SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate variable declaration node", location)) return NULL;
    
    node->type = AST_VAR_DECL;
    node->location = location;
    node->type_info = NULL;
    
    node->as.var_decl.name = string_duplicate(name);
    if (!check_null(node->as.var_decl.name, "Failed to allocate variable name", location)) {
        free(node);
        return NULL;
    }
    
    node->as.var_decl.initializer = initializer;
    node->as.var_decl.var_type = var_type;
    
    return node;
}

AstNode* ast_create_array_decl(char* name, int size, AstNode** initializers, 
                              int initializer_count, TypeInfo* element_type,
                              SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate array declaration node", location)) return NULL;
    
    node->type = AST_ARRAY_DECL;
    node->location = location;
    node->type_info = NULL;
    
    node->as.array_decl.name = string_duplicate(name);
    if (!check_null(node->as.array_decl.name, "Failed to allocate array name", location)) {
        free(node);
        return NULL;
    }
    
    node->as.array_decl.size = size;
    node->as.array_decl.element_type = element_type;
    
    if (initializer_count > 0 && initializers) {
        node->as.array_decl.initializers = (AstNode**)malloc(initializer_count * sizeof(AstNode*));
        if (!check_null(node->as.array_decl.initializers, "Failed to allocate initializers array", location)) {
            free(node->as.array_decl.name);
            free(node);
            return NULL;
        }
        
        for (int i = 0; i < initializer_count; i++) {
            node->as.array_decl.initializers[i] = initializers[i];
        }
        node->as.array_decl.initializer_count = initializer_count;
    } else {
        node->as.array_decl.initializers = NULL;
        node->as.array_decl.initializer_count = 0;
    }
    
    return node;
}

AstNode* ast_create_block(AstNode** statements, int statement_count, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate block node", location)) return NULL;
    
    node->type = AST_BLOCK;
    node->location = location;
    node->type_info = NULL;
    
    if (statement_count > 0 && statements) {
        node->as.block.statements = (AstNode**)malloc(statement_count * sizeof(AstNode*));
        if (!check_null(node->as.block.statements, "Failed to allocate statements array", location)) {
            free(node);
            return NULL;
        }
        
        for (int i = 0; i < statement_count; i++) {
            node->as.block.statements[i] = statements[i];
        }
        node->as.block.statement_count = statement_count;
    } else {
        node->as.block.statements = NULL;
        node->as.block.statement_count = 0;
    }
    
    return node;
}

AstNode* ast_create_if_stmt(AstNode* condition, AstNode* then_branch, AstNode* else_branch,
                           SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate if statement node", location)) return NULL;
    
    node->type = AST_IF_STMT;
    node->location = location;
    node->type_info = NULL;
    
    node->as.if_stmt.condition = condition;
    node->as.if_stmt.then_branch = then_branch;
    node->as.if_stmt.else_branch = else_branch;
    
    return node;
}

AstNode* ast_create_while_stmt(AstNode* condition, AstNode* body, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate while statement node", location)) return NULL;
    
    node->type = AST_WHILE_STMT;
    node->location = location;
    node->type_info = NULL;
    
    node->as.while_stmt.condition = condition;
    node->as.while_stmt.body = body;
    
    return node;
}

AstNode* ast_create_for_stmt(AstNode* initializer, AstNode* condition, AstNode* increment,
                            AstNode* body, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate for statement node", location)) return NULL;
    
    node->type = AST_FOR_STMT;
    node->location = location;
    node->type_info = NULL;
    
    node->as.for_stmt.initializer = initializer;
    node->as.for_stmt.condition = condition;
    node->as.for_stmt.increment = increment;
    node->as.for_stmt.body = body;
    
    return node;
}

AstNode* ast_create_do_while_stmt(AstNode* body, AstNode* condition, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate do-while statement node", location)) return NULL;
    
    node->type = AST_DO_WHILE_STMT;
    node->location = location;
    node->type_info = NULL;
    
    node->as.while_stmt.body = body;
    node->as.while_stmt.condition = condition;
    
    return node;
}

AstNode* ast_create_return_stmt(AstNode* value, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate return statement node", location)) return NULL;
    
    node->type = AST_RETURN_STMT;
    node->location = location;
    node->type_info = NULL;
    
    node->as.return_stmt.value = value;
    
    return node;
}

AstNode* ast_create_break_stmt(SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate break statement node", location)) return NULL;
    
    node->type = AST_BREAK_STMT;
    node->location = location;
    node->type_info = NULL;
    
    return node;
}

AstNode* ast_create_expr_stmt(AstNode* expression, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate expression statement node", location)) return NULL;
    
    node->type = AST_EXPR_STMT;
    node->location = location;
    node->type_info = NULL;
    
    node->as.expr_stmt.expression = expression;
    
    return node;
}

AstNode* ast_create_binary_expr(AstNode* left, TokenType operator, AstNode* right,
                               SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate binary expression node", location)) return NULL;
    
    node->type = AST_BINARY_EXPR;
    node->location = location;
    node->type_info = NULL;
    
    node->as.binary_expr.left = left;
    node->as.binary_expr.operator = operator;
    node->as.binary_expr.right = right;
    
    return node;
}

AstNode* ast_create_unary_expr(AstNode* operand, TokenType operator, bool is_prefix,
                              SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate unary expression node", location)) return NULL;
    
    node->type = AST_UNARY_EXPR;
    node->location = location;
    node->type_info = NULL;
    
    node->as.unary_expr.operand = operand;
    node->as.unary_expr.operator = operator;
    node->as.unary_expr.is_prefix = is_prefix;
    
    return node;
}

AstNode* ast_create_literal_int(intptr_t value, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate integer literal node", location)) return NULL;
    
    node->type = AST_LITERAL_INT;
    node->location = location;
    node->type_info = type_create_int();
    
    node->as.int_literal.value = value;
    
    return node;
}

AstNode* ast_create_literal_char(int32_t value, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate character literal node", location)) return NULL;
    
    node->type = AST_LITERAL_CHAR;
    node->location = location;
    node->type_info = type_create_char();
    
    node->as.char_literal.value = value;
    
    return node;
}

AstNode* ast_create_literal_string(char* value, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate string literal node", location)) return NULL;
    
    node->type = AST_LITERAL_STRING;
    node->location = location;
    node->type_info = NULL; // Will be set during semantic analysis
    
    node->as.string_literal.value = string_duplicate(value);
    if (!check_null(node->as.string_literal.value, "Failed to allocate string value", location)) {
        free(node);
        return NULL;
    }
    
    return node;
}

AstNode* ast_create_literal_bool(bool value, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate boolean literal node", location)) return NULL;
    
    node->type = AST_LITERAL_BOOL;
    node->location = location;
    node->type_info = type_create_bool();
    
    node->as.bool_literal.value = value;
    
    return node;
}

AstNode* ast_create_identifier(char* name, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate identifier node", location)) return NULL;
    
    node->type = AST_IDENTIFIER;
    node->location = location;
    node->type_info = NULL; // Will be set during semantic analysis
    
    node->as.identifier.name = string_duplicate(name);
    if (!check_null(node->as.identifier.name, "Failed to allocate identifier name", location)) {
        free(node);
        return NULL;
    }
    
    return node;
}

AstNode* ast_create_array_access(AstNode* array, AstNode* index, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate array access node", location)) return NULL;
    
    node->type = AST_ARRAY_ACCESS;
    node->location = location;
    node->type_info = NULL; // Will be set during semantic analysis
    
    node->as.array_access.array = array;
    node->as.array_access.index = index;
    
    return node;
}

AstNode* ast_create_call_expr(AstNode* callee, AstNode** arguments, int argument_count,
                             SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate function call node", location)) return NULL;
    
    node->type = AST_CALL_EXPR;
    node->location = location;
    node->type_info = NULL; // Will be set during semantic analysis
    
    node->as.call_expr.callee = callee;
    
    if (argument_count > 0 && arguments) {
        node->as.call_expr.arguments = (AstNode**)malloc(argument_count * sizeof(AstNode*));
        if (!check_null(node->as.call_expr.arguments, "Failed to allocate arguments array", location)) {
            free(node);
            return NULL;
        }
        
        for (int i = 0; i < argument_count; i++) {
            node->as.call_expr.arguments[i] = arguments[i];
        }
        node->as.call_expr.argument_count = argument_count;
    } else {
        node->as.call_expr.arguments = NULL;
        node->as.call_expr.argument_count = 0;
    }
    
    return node;
}

AstNode* ast_create_assignment(AstNode* target, AstNode* value, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate assignment node", location)) return NULL;
    
    node->type = AST_ASSIGNMENT;
    node->location = location;
    node->type_info = NULL; // Will be set during semantic analysis
    
    node->as.assignment.target = target;
    node->as.assignment.value = value;
    
    return node;
}

AstNode* ast_create_type(TypeInfo* type_data, SourceLocation location) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!check_null(node, "Failed to allocate type node", location)) return NULL;
    
    node->type = AST_TYPE;
    node->location = location;
    node->type_info = NULL;
    
    node->as.type_node.type_data = type_data;
    
    return node;
}

//------------------------------------------------------------------------------
// Memory management
//------------------------------------------------------------------------------

void ast_free(AstNode* node) {
    if (!node) return;
    
    // Free node-specific data
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->as.program.declaration_count; i++) {
                ast_free(node->as.program.declarations[i]);
            }
            free(node->as.program.declarations);
            break;
            
        case AST_FUNCTION_DECL:
            free(node->as.function_decl.name);
            for (int i = 0; i < node->as.function_decl.parameter_count; i++) {
                ast_free(node->as.function_decl.parameters[i]);
            }
            free(node->as.function_decl.parameters);
            ast_free(node->as.function_decl.body);
            type_free(node->as.function_decl.return_type);
            break;
            
        case AST_VAR_DECL:
            free(node->as.var_decl.name);
            ast_free(node->as.var_decl.initializer);
            type_free(node->as.var_decl.var_type);
            break;
            
        case AST_ARRAY_DECL:
            free(node->as.array_decl.name);
            for (int i = 0; i < node->as.array_decl.initializer_count; i++) {
                ast_free(node->as.array_decl.initializers[i]);
            }
            free(node->as.array_decl.initializers);
            type_free(node->as.array_decl.element_type);
            break;
            
        case AST_BLOCK:
            for (int i = 0; i < node->as.block.statement_count; i++) {
                ast_free(node->as.block.statements[i]);
            }
            free(node->as.block.statements);
            break;
            
        case AST_IF_STMT:
            ast_free(node->as.if_stmt.condition);
            ast_free(node->as.if_stmt.then_branch);
            ast_free(node->as.if_stmt.else_branch);
            break;
            
        case AST_WHILE_STMT:
        case AST_DO_WHILE_STMT:
            ast_free(node->as.while_stmt.condition);
            ast_free(node->as.while_stmt.body);
            break;
            
        case AST_FOR_STMT:
            ast_free(node->as.for_stmt.initializer);
            ast_free(node->as.for_stmt.condition);
            ast_free(node->as.for_stmt.increment);
            ast_free(node->as.for_stmt.body);
            break;
            
        case AST_RETURN_STMT:
            ast_free(node->as.return_stmt.value);
            break;
            
        case AST_EXPR_STMT:
            ast_free(node->as.expr_stmt.expression);
            break;
            
        case AST_BINARY_EXPR:
            ast_free(node->as.binary_expr.left);
            ast_free(node->as.binary_expr.right);
            break;
            
        case AST_UNARY_EXPR:
            ast_free(node->as.unary_expr.operand);
            break;
            
        case AST_LITERAL_STRING:
            free(node->as.string_literal.value);
            break;
            
        case AST_IDENTIFIER:
            free(node->as.identifier.name);
            break;
            
        case AST_ARRAY_ACCESS:
            ast_free(node->as.array_access.array);
            ast_free(node->as.array_access.index);
            break;
            
        case AST_CALL_EXPR:
            ast_free(node->as.call_expr.callee);
            for (int i = 0; i < node->as.call_expr.argument_count; i++) {
                ast_free(node->as.call_expr.arguments[i]);
            }
            free(node->as.call_expr.arguments);
            break;
            
        case AST_ASSIGNMENT:
            ast_free(node->as.assignment.target);
            ast_free(node->as.assignment.value);
            break;
            
        case AST_TYPE:
            type_free(node->as.type_node.type_data);
            break;
            
        default:
            // Other node types don't need special cleanup
            break;
    }
    
    // Free type info if it exists
    if (node->type_info) {
        type_free(node->type_info);
    }
    
    // Free the node itself
    free(node);
}

//------------------------------------------------------------------------------
// AST visitor pattern
//------------------------------------------------------------------------------

bool ast_accept(AstNode* node, AstVisitor* visitor) {
    if (!node || !visitor) return true;
    
    switch (node->type) {
        case AST_PROGRAM:
            if (visitor->visit_program && !visitor->visit_program(visitor, node)) return false;
            for (int i = 0; i < node->as.program.declaration_count; i++) {
                if (!ast_accept(node->as.program.declarations[i], visitor)) return false;
            }
            break;
            
        case AST_FUNCTION_DECL:
            if (visitor->visit_function_decl && !visitor->visit_function_decl(visitor, node)) return false;
            for (int i = 0; i < node->as.function_decl.parameter_count; i++) {
                if (!ast_accept(node->as.function_decl.parameters[i], visitor)) return false;
            }
            if (node->as.function_decl.body) {
                if (!ast_accept(node->as.function_decl.body, visitor)) return false;
            }
            break;
            
        case AST_VAR_DECL:
            if (visitor->visit_var_decl && !visitor->visit_var_decl(visitor, node)) return false;
            if (node->as.var_decl.initializer) {
                if (!ast_accept(node->as.var_decl.initializer, visitor)) return false;
            }
            break;
            
        case AST_ARRAY_DECL:
            if (visitor->visit_array_decl && !visitor->visit_array_decl(visitor, node)) return false;
            for (int i = 0; i < node->as.array_decl.initializer_count; i++) {
                if (!ast_accept(node->as.array_decl.initializers[i], visitor)) return false;
            }
            break;
            
        case AST_BLOCK:
            if (visitor->visit_block && !visitor->visit_block(visitor, node)) return false;
            for (int i = 0; i < node->as.block.statement_count; i++) {
                if (!ast_accept(node->as.block.statements[i], visitor)) return false;
            }
            break;
            
        case AST_IF_STMT:
            if (visitor->visit_if_stmt && !visitor->visit_if_stmt(visitor, node)) return false;
            if (!ast_accept(node->as.if_stmt.condition, visitor)) return false;
            if (!ast_accept(node->as.if_stmt.then_branch, visitor)) return false;
            if (node->as.if_stmt.else_branch) {
                if (!ast_accept(node->as.if_stmt.else_branch, visitor)) return false;
            }
            break;
            
        case AST_WHILE_STMT:
            if (visitor->visit_while_stmt && !visitor->visit_while_stmt(visitor, node)) return false;
            if (!ast_accept(node->as.while_stmt.condition, visitor)) return false;
            if (!ast_accept(node->as.while_stmt.body, visitor)) return false;
            break;
            
        case AST_DO_WHILE_STMT:
            if (visitor->visit_do_while_stmt && !visitor->visit_do_while_stmt(visitor, node)) return false;
            if (!ast_accept(node->as.while_stmt.body, visitor)) return false;
            if (!ast_accept(node->as.while_stmt.condition, visitor)) return false;
            break;
            
        case AST_FOR_STMT:
            if (visitor->visit_for_stmt && !visitor->visit_for_stmt(visitor, node)) return false;
            if (node->as.for_stmt.initializer) {
                if (!ast_accept(node->as.for_stmt.initializer, visitor)) return false;
            }
            if (node->as.for_stmt.condition) {
                if (!ast_accept(node->as.for_stmt.condition, visitor)) return false;
            }
            if (node->as.for_stmt.increment) {
                if (!ast_accept(node->as.for_stmt.increment, visitor)) return false;
            }
            if (!ast_accept(node->as.for_stmt.body, visitor)) return false;
            break;
            
        case AST_RETURN_STMT:
            if (visitor->visit_return_stmt && !visitor->visit_return_stmt(visitor, node)) return false;
            if (node->as.return_stmt.value) {
                if (!ast_accept(node->as.return_stmt.value, visitor)) return false;
            }
            break;
            
        case AST_BREAK_STMT:
            if (visitor->visit_break_stmt && !visitor->visit_break_stmt(visitor, node)) return false;
            break;
            
        case AST_EXPR_STMT:
            if (visitor->visit_expr_stmt && !visitor->visit_expr_stmt(visitor, node)) return false;
            if (!ast_accept(node->as.expr_stmt.expression, visitor)) return false;
            break;
            
        case AST_BINARY_EXPR:
            if (visitor->visit_binary_expr && !visitor->visit_binary_expr(visitor, node)) return false;
            if (!ast_accept(node->as.binary_expr.left, visitor)) return false;
            if (!ast_accept(node->as.binary_expr.right, visitor)) return false;
            break;
            
        case AST_UNARY_EXPR:
            if (visitor->visit_unary_expr && !visitor->visit_unary_expr(visitor, node)) return false;
            if (!ast_accept(node->as.unary_expr.operand, visitor)) return false;
            break;
            
        case AST_LITERAL_INT:
            if (visitor->visit_literal_int && !visitor->visit_literal_int(visitor, node)) return false;
            break;
            
        case AST_LITERAL_CHAR:
            if (visitor->visit_literal_char && !visitor->visit_literal_char(visitor, node)) return false;
            break;
            
        case AST_LITERAL_STRING:
            if (visitor->visit_literal_string && !visitor->visit_literal_string(visitor, node)) return false;
            break;
            
        case AST_LITERAL_BOOL:
            if (visitor->visit_literal_bool && !visitor->visit_literal_bool(visitor, node)) return false;
            break;
            
        case AST_IDENTIFIER:
            if (visitor->visit_identifier && !visitor->visit_identifier(visitor, node)) return false;
            break;
            
        case AST_ARRAY_ACCESS:
            if (visitor->visit_array_access && !visitor->visit_array_access(visitor, node)) return false;
            if (!ast_accept(node->as.array_access.array, visitor)) return false;
            if (!ast_accept(node->as.array_access.index, visitor)) return false;
            break;
            
        case AST_CALL_EXPR:
            if (visitor->visit_call_expr && !visitor->visit_call_expr(visitor, node)) return false;
            if (!ast_accept(node->as.call_expr.callee, visitor)) return false;
            for (int i = 0; i < node->as.call_expr.argument_count; i++) {
                if (!ast_accept(node->as.call_expr.arguments[i], visitor)) return false;
            }
            break;
            
        case AST_ASSIGNMENT:
            if (visitor->visit_assignment && !visitor->visit_assignment(visitor, node)) return false;
            if (!ast_accept(node->as.assignment.target, visitor)) return false;
            if (!ast_accept(node->as.assignment.value, visitor)) return false;
            break;
            
        case AST_TYPE:
            if (visitor->visit_type && !visitor->visit_type(visitor, node)) return false;
            break;
            
        default:
            // Unknown node type
            return false;
    }
    
    return true;
}

//------------------------------------------------------------------------------
// AST printing for debugging
//------------------------------------------------------------------------------

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static const char* get_node_type_name(AstNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "Program";
        case AST_FUNCTION_DECL: return "FunctionDecl";
        case AST_VAR_DECL: return "VarDecl";
        case AST_ARRAY_DECL: return "ArrayDecl";
        case AST_BLOCK: return "Block";
        case AST_IF_STMT: return "IfStmt";
        case AST_WHILE_STMT: return "WhileStmt";
        case AST_FOR_STMT: return "ForStmt";
        case AST_DO_WHILE_STMT: return "DoWhileStmt";
        case AST_RETURN_STMT: return "ReturnStmt";
        case AST_BREAK_STMT: return "BreakStmt";
        case AST_EXPR_STMT: return "ExprStmt";
        case AST_BINARY_EXPR: return "BinaryExpr";
        case AST_UNARY_EXPR: return "UnaryExpr";
        case AST_LITERAL_INT: return "IntLiteral";
        case AST_LITERAL_CHAR: return "CharLiteral";
        case AST_LITERAL_STRING: return "StringLiteral";
        case AST_LITERAL_BOOL: return "BoolLiteral";
        case AST_IDENTIFIER: return "Identifier";
        case AST_ARRAY_ACCESS: return "ArrayAccess";
        case AST_CALL_EXPR: return "CallExpr";
        case AST_ASSIGNMENT: return "Assignment";
        case AST_TYPE: return "Type";
        default: return "Unknown";
    }
}

static const char* get_type_name(TypeInfo* type) {
    if (!type) return "unknown";
    
    switch (type->category) {
        case TYPE_VOID: return "void";
        case TYPE_BOOL: return "bool";
        case TYPE_CHAR: return "char";
        case TYPE_INT: return "int";
        case TYPE_ARRAY: return "array";
        case TYPE_FUNCTION: return "function";
        default: return "unknown";
    }
}

void ast_print(AstNode* node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }
    
    print_indent(indent);
    printf("%s", get_node_type_name(node->type));
    
    // Print node-specific information
    switch (node->type) {
        case AST_PROGRAM:
            printf(" (declarations: %d)\n", node->as.program.declaration_count);
            for (int i = 0; i < node->as.program.declaration_count; i++) {
                ast_print(node->as.program.declarations[i], indent + 1);
            }
            break;
            
        case AST_FUNCTION_DECL:
            printf(" (name: %s, params: %d, external: %s)\n", 
                  node->as.function_decl.name, 
                  node->as.function_decl.parameter_count,
                  node->as.function_decl.is_external ? "true" : "false");
            
            print_indent(indent + 1);
            printf("Return type: %s\n", get_type_name(node->as.function_decl.return_type));
            
            for (int i = 0; i < node->as.function_decl.parameter_count; i++) {
                print_indent(indent + 1);
                printf("Parameter %d:\n", i);
                ast_print(node->as.function_decl.parameters[i], indent + 2);
            }
            
            if (node->as.function_decl.body) {
                print_indent(indent + 1);
                printf("Body:\n");
                ast_print(node->as.function_decl.body, indent + 2);
            }
            break;
            
        case AST_VAR_DECL:
            printf(" (name: %s)\n", node->as.var_decl.name);
            
            print_indent(indent + 1);
            printf("Type: %s\n", get_type_name(node->as.var_decl.var_type));
            
            if (node->as.var_decl.initializer) {
                print_indent(indent + 1);
                printf("Initializer:\n");
                ast_print(node->as.var_decl.initializer, indent + 2);
            }
            break;
            
        case AST_ARRAY_DECL:
            printf(" (name: %s, size: %d, initializers: %d)\n", 
                  node->as.array_decl.name, 
                  node->as.array_decl.size,
                  node->as.array_decl.initializer_count);
            
            print_indent(indent + 1);
            printf("Element type: %s\n", get_type_name(node->as.array_decl.element_type));
            
            for (int i = 0; i < node->as.array_decl.initializer_count; i++) {
                print_indent(indent + 1);
                printf("Initializer %d:\n", i);
                ast_print(node->as.array_decl.initializers[i], indent + 2);
            }
            break;
            
        case AST_BLOCK:
            printf(" (statements: %d)\n", node->as.block.statement_count);
            for (int i = 0; i < node->as.block.statement_count; i++) {
                ast_print(node->as.block.statements[i], indent + 1);
            }
            break;
            
        case AST_IF_STMT:
            printf("\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->as.if_stmt.condition, indent + 2);
            
            print_indent(indent + 1);
            printf("Then branch:\n");
            ast_print(node->as.if_stmt.then_branch, indent + 2);
            
            if (node->as.if_stmt.else_branch) {
                print_indent(indent + 1);
                printf("Else branch:\n");
                ast_print(node->as.if_stmt.else_branch, indent + 2);
            }
            break;
            
        case AST_WHILE_STMT:
            printf("\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->as.while_stmt.condition, indent + 2);
            
            print_indent(indent + 1);
            printf("Body:\n");
            ast_print(node->as.while_stmt.body, indent + 2);
            break;
            
        case AST_DO_WHILE_STMT:
            printf("\n");
            print_indent(indent + 1);
            printf("Body:\n");
            ast_print(node->as.while_stmt.body, indent + 2);
            
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->as.while_stmt.condition, indent + 2);
            break;
            
        case AST_FOR_STMT:
            printf("\n");
            if (node->as.for_stmt.initializer) {
                print_indent(indent + 1);
                printf("Initializer:\n");
                ast_print(node->as.for_stmt.initializer, indent + 2);
            }
            
            if (node->as.for_stmt.condition) {
                print_indent(indent + 1);
                printf("Condition:\n");
                ast_print(node->as.for_stmt.condition, indent + 2);
            }
            
            if (node->as.for_stmt.increment) {
                print_indent(indent + 1);
                printf("Increment:\n");
                ast_print(node->as.for_stmt.increment, indent + 2);
            }
            
            print_indent(indent + 1);
            printf("Body:\n");
            ast_print(node->as.for_stmt.body, indent + 2);
            break;
            
        case AST_RETURN_STMT:
            printf("\n");
            if (node->as.return_stmt.value) {
                print_indent(indent + 1);
                printf("Value:\n");
                ast_print(node->as.return_stmt.value, indent + 2);
            }
            break;
            
        case AST_BREAK_STMT:
            printf("\n");
            break;
            
        case AST_EXPR_STMT:
            printf("\n");
            ast_print(node->as.expr_stmt.expression, indent + 1);
            break;
            
        case AST_BINARY_EXPR:
            printf(" (operator: %s)\n", token_type_to_string(node->as.binary_expr.operator));
            
            print_indent(indent + 1);
            printf("Left:\n");
            ast_print(node->as.binary_expr.left, indent + 2);
            
            print_indent(indent + 1);
            printf("Right:\n");
            ast_print(node->as.binary_expr.right, indent + 2);
            break;
            
        case AST_UNARY_EXPR:
            printf(" (operator: %s, prefix: %s)\n", 
                  token_type_to_string(node->as.unary_expr.operator),
                  node->as.unary_expr.is_prefix ? "true" : "false");
            
            print_indent(indent + 1);
            printf("Operand:\n");
            ast_print(node->as.unary_expr.operand, indent + 2);
            break;
            
        case AST_LITERAL_INT:
            printf(" (value: %ld)\n", (long)node->as.int_literal.value);
            break;
            
        case AST_LITERAL_CHAR:
            if (node->as.char_literal.value >= 32 && node->as.char_literal.value <= 126) {
                printf(" (value: '%c')\n", (char)node->as.char_literal.value);
            } else {
                printf(" (value: '\\u%04x')\n", node->as.char_literal.value);
            }
            break;
            
        case AST_LITERAL_STRING:
            printf(" (value: \"%s\")\n", node->as.string_literal.value ? node->as.string_literal.value : "");
            break;
            
        case AST_LITERAL_BOOL:
            printf(" (value: %s)\n", node->as.bool_literal.value ? "true" : "false");
            break;
            
        case AST_IDENTIFIER:
            printf(" (name: %s)\n", node->as.identifier.name);
            break;
            
        case AST_ARRAY_ACCESS:
            printf("\n");
            print_indent(indent + 1);
            printf("Array:\n");
            ast_print(node->as.array_access.array, indent + 2);
            
            print_indent(indent + 1);
            printf("Index:\n");
            ast_print(node->as.array_access.index, indent + 2);
            break;
            
        case AST_CALL_EXPR:
            printf(" (arguments: %d)\n", node->as.call_expr.argument_count);
            
            print_indent(indent + 1);
            printf("Callee:\n");
            ast_print(node->as.call_expr.callee, indent + 2);
            
            for (int i = 0; i < node->as.call_expr.argument_count; i++) {
                print_indent(indent + 1);
                printf("Argument %d:\n", i);
                ast_print(node->as.call_expr.arguments[i], indent + 2);
            }
            break;
            
        case AST_ASSIGNMENT:
            printf("\n");
            print_indent(indent + 1);
            printf("Target:\n");
            ast_print(node->as.assignment.target, indent + 2);
            
            print_indent(indent + 1);
            printf("Value:\n");
            ast_print(node->as.assignment.value, indent + 2);
            break;
            
        case AST_TYPE:
            printf(" (type: %s)\n", get_type_name(node->as.type_node.type_data));
            break;
            
        default:
            printf(" (unknown node type)\n");
            break;
    }
}

//------------------------------------------------------------------------------
// AST cloning
//------------------------------------------------------------------------------

static AstNode** clone_node_array(AstNode** nodes, int count) {
    if (!nodes || count <= 0) return NULL;
    
    AstNode** cloned = (AstNode**)malloc(count * sizeof(AstNode*));
    if (!cloned) return NULL;
    
    for (int i = 0; i < count; i++) {
        cloned[i] = ast_clone(nodes[i]);
        if (!cloned[i]) {
            // Cleanup previously cloned nodes on failure
            for (int j = 0; j < i; j++) {
                ast_free(cloned[j]);
            }
            free(cloned);
            return NULL;
        }
    }
    
    return cloned;
}

static TypeInfo* clone_type(TypeInfo* type) {
    if (!type) return NULL;
    
    TypeInfo* cloned = NULL;
    
    switch (type->category) {
        case TYPE_VOID:
            cloned = type_create_void();
            break;
            
        case TYPE_BOOL:
            cloned = type_create_bool();
            break;
            
        case TYPE_CHAR:
            cloned = type_create_char();
            break;
            
        case TYPE_INT:
            cloned = type_create_int();
            break;
            
        case TYPE_ARRAY: {
            TypeInfo* element_type = clone_type(type->info.array.element_type);
            if (!element_type) return NULL;
            
            cloned = type_create_array(element_type, type->info.array.size);
            if (!cloned) {
                type_free(element_type);
            }
            break;
        }
            
        case TYPE_FUNCTION: {
            TypeInfo* return_type = clone_type(type->info.function.return_type);
            if (!return_type) return NULL;
            
            TypeInfo** param_types = NULL;
            if (type->info.function.param_count > 0 && type->info.function.param_types) {
                param_types = (TypeInfo**)malloc(type->info.function.param_count * sizeof(TypeInfo*));
                if (!param_types) {
                    type_free(return_type);
                    return NULL;
                }
                
                for (int i = 0; i < type->info.function.param_count; i++) {
                    param_types[i] = clone_type(type->info.function.param_types[i]);
                    if (!param_types[i]) {
                        // Cleanup on failure
                        for (int j = 0; j < i; j++) {
                            type_free(param_types[j]);
                        }
                        free(param_types);
                        type_free(return_type);
                        return NULL;
                    }
                }
            }
            
            cloned = type_create_function(return_type, param_types, type->info.function.param_count);
            if (!cloned) {
                for (int i = 0; i < type->info.function.param_count; i++) {
                    type_free(param_types[i]);
                }
                free(param_types);
                type_free(return_type);
            }
            break;
        }
    }
    
    return cloned;
}

AstNode* ast_clone(AstNode* node) {
    if (!node) return NULL;
    
    AstNode* cloned = NULL;
    
    switch (node->type) {
        case AST_PROGRAM: {
            AstNode** decls = clone_node_array(node->as.program.declarations, 
                                             node->as.program.declaration_count);
            cloned = ast_create_program(decls, node->as.program.declaration_count, node->location);
            break;
        }
            
        case AST_FUNCTION_DECL: {
            AstNode** params = clone_node_array(node->as.function_decl.parameters,
                                              node->as.function_decl.parameter_count);
            AstNode* body = ast_clone(node->as.function_decl.body);
            TypeInfo* return_type = clone_type(node->as.function_decl.return_type);
            
            cloned = ast_create_function_decl(node->as.function_decl.name, params,
                                            node->as.function_decl.parameter_count, body,
                                            return_type, node->as.function_decl.is_external,
                                            node->location);
            break;
        }
            
        case AST_VAR_DECL: {
            AstNode* init = ast_clone(node->as.var_decl.initializer);
            TypeInfo* type = clone_type(node->as.var_decl.var_type);
            
            cloned = ast_create_var_decl(node->as.var_decl.name, init, type, node->location);
            break;
        }
            
        case AST_ARRAY_DECL: {
            AstNode** inits = clone_node_array(node->as.array_decl.initializers,
                                             node->as.array_decl.initializer_count);
            TypeInfo* element_type = clone_type(node->as.array_decl.element_type);
            
            cloned = ast_create_array_decl(node->as.array_decl.name, node->as.array_decl.size,
                                         inits, node->as.array_decl.initializer_count,
                                         element_type, node->location);
            break;
        }
            
        case AST_BLOCK: {
            AstNode** statements = clone_node_array(node->as.block.statements,
                                                  node->as.block.statement_count);
            
            cloned = ast_create_block(statements, node->as.block.statement_count, node->location);
            break;
        }
            
        case AST_IF_STMT: {
            AstNode* condition = ast_clone(node->as.if_stmt.condition);
            AstNode* then_branch = ast_clone(node->as.if_stmt.then_branch);
            AstNode* else_branch = ast_clone(node->as.if_stmt.else_branch);
            
            cloned = ast_create_if_stmt(condition, then_branch, else_branch, node->location);
            break;
        }
            
        case AST_WHILE_STMT: {
            AstNode* condition = ast_clone(node->as.while_stmt.condition);
            AstNode* body = ast_clone(node->as.while_stmt.body);
            
            cloned = ast_create_while_stmt(condition, body, node->location);
            break;
        }
            
        case AST_DO_WHILE_STMT: {
            AstNode* body = ast_clone(node->as.while_stmt.body);
            AstNode* condition = ast_clone(node->as.while_stmt.condition);
            
            cloned = ast_create_do_while_stmt(body, condition, node->location);
            break;
        }
            
        case AST_FOR_STMT: {
            AstNode* init = ast_clone(node->as.for_stmt.initializer);
            AstNode* condition = ast_clone(node->as.for_stmt.condition);
            AstNode* increment = ast_clone(node->as.for_stmt.increment);
            AstNode* body = ast_clone(node->as.for_stmt.body);
            
            cloned = ast_create_for_stmt(init, condition, increment, body, node->location);
            break;
        }
            
        case AST_RETURN_STMT: {
            AstNode* value = ast_clone(node->as.return_stmt.value);
            
            cloned = ast_create_return_stmt(value, node->location);
            break;
        }
            
        case AST_BREAK_STMT: {
            cloned = ast_create_break_stmt(node->location);
            break;
        }
            
        case AST_EXPR_STMT: {
            AstNode* expr = ast_clone(node->as.expr_stmt.expression);
            
            cloned = ast_create_expr_stmt(expr, node->location);
            break;
        }
            
        case AST_BINARY_EXPR: {
            AstNode* left = ast_clone(node->as.binary_expr.left);
            AstNode* right = ast_clone(node->as.binary_expr.right);
            
            cloned = ast_create_binary_expr(left, node->as.binary_expr.operator, right, node->location);
            break;
        }
            
        case AST_UNARY_EXPR: {
            AstNode* operand = ast_clone(node->as.unary_expr.operand);
            
            cloned = ast_create_unary_expr(operand, node->as.unary_expr.operator,
                                         node->as.unary_expr.is_prefix, node->location);
            break;
        }
            
        case AST_LITERAL_INT: {
            cloned = ast_create_literal_int(node->as.int_literal.value, node->location);
            break;
        }
            
        case AST_LITERAL_CHAR: {
            cloned = ast_create_literal_char(node->as.char_literal.value, node->location);
            break;
        }
            
        case AST_LITERAL_STRING: {
            cloned = ast_create_literal_string(node->as.string_literal.value, node->location);
            break;
        }
            
        case AST_LITERAL_BOOL: {
            cloned = ast_create_literal_bool(node->as.bool_literal.value, node->location);
            break;
        }
            
        case AST_IDENTIFIER: {
            cloned = ast_create_identifier(node->as.identifier.name, node->location);
            break;
        }
            
        case AST_ARRAY_ACCESS: {
            AstNode* array = ast_clone(node->as.array_access.array);
            AstNode* index = ast_clone(node->as.array_access.index);
            
            cloned = ast_create_array_access(array, index, node->location);
            break;
        }
            
        case AST_CALL_EXPR: {
            AstNode* callee = ast_clone(node->as.call_expr.callee);
            AstNode** args = clone_node_array(node->as.call_expr.arguments,
                                            node->as.call_expr.argument_count);
            
            cloned = ast_create_call_expr(callee, args, node->as.call_expr.argument_count, node->location);
            break;
        }
            
        case AST_ASSIGNMENT: {
            AstNode* target = ast_clone(node->as.assignment.target);
            AstNode* value = ast_clone(node->as.assignment.value);
            
            cloned = ast_create_assignment(target, value, node->location);
            break;
        }
            
        case AST_TYPE: {
            TypeInfo* type_data = clone_type(node->as.type_node.type_data);
            
            cloned = ast_create_type(type_data, node->location);
            break;
        }
    }
    
    // Copy type info if any
    if (cloned && node->type_info) {
        cloned->type_info = clone_type(node->type_info);
    }
    
    return cloned;
}