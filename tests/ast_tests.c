/*
 * filename: ast_tests.c
 * 
 * Purpose:
 * Test suite for the Abstract Syntax Tree (AST) module of the ћ++ compiler.
 * This file contains tests to verify the functionality of AST creation, manipulation,
 * and traversal operations.
 * 
 * Key Components:
 * - test_ast_creation(): Test basic AST node creation
 * - test_ast_cloning(): Test AST deep cloning functionality
 * - test_ast_visitor(): Test AST visitor pattern
 * - test_ast_with_lexer(): Test integration with lexer for complete parsing flow
 * 
 * Interactions:
 * - Uses lexer.h for token creation and management
 * - Uses ast.h for AST operations
 * 
 * Notes:
 * - All tests validate memory management behavior
 * - UTF-8 encoded identifiers are specifically tested
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../src/parser/ast.h"
#include "../src/lexer/lexer.h"
#include "../src/target/target_info.h"
#include "../src/utils/error.h"

// Custom assert macro that prints a message
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("ASSERTION FAILED: %s\n", message); \
            printf("  in %s at line %d\n", __FILE__, __LINE__); \
            exit(1); \
        } \
    } while (0)

// Test AST node creation
void test_ast_creation() {
    printf("Testing AST node creation...\n");
    
    // Create a source location
    SourceLocation loc = source_location_create(1, 1, "test.ћпп");
    
    // Create an integer literal
    AstNode* int_literal = ast_create_literal_int(42, loc);
    TEST_ASSERT(int_literal != NULL, "Failed to create integer literal node");
    TEST_ASSERT(int_literal->type == AST_LITERAL_INT, "Wrong node type for integer literal");
    TEST_ASSERT(int_literal->as.int_literal.value == 42, "Wrong value for integer literal");
    
    // Create an identifier
    AstNode* id_node = ast_create_identifier("тест", loc);
    TEST_ASSERT(id_node != NULL, "Failed to create identifier node");
    TEST_ASSERT(id_node->type == AST_IDENTIFIER, "Wrong node type for identifier");
    TEST_ASSERT(strcmp(id_node->as.identifier.name, "тест") == 0, "Wrong name for identifier");
    
    // Create a binary expression
    AstNode* bin_expr = ast_create_binary_expr(int_literal, TOKEN_PLUS, id_node, loc);
    TEST_ASSERT(bin_expr != NULL, "Failed to create binary expression node");
    TEST_ASSERT(bin_expr->type == AST_BINARY_EXPR, "Wrong node type for binary expression");
    TEST_ASSERT(bin_expr->as.binary_expr.operator == TOKEN_PLUS, "Wrong operator for binary expression");
    TEST_ASSERT(bin_expr->as.binary_expr.left == int_literal, "Wrong left operand for binary expression");
    TEST_ASSERT(bin_expr->as.binary_expr.right == id_node, "Wrong right operand for binary expression");
    
    // Create a statement block
    AstNode* stmt = ast_create_expr_stmt(bin_expr, loc);
    AstNode* stmts[1] = { stmt };
    AstNode* block = ast_create_block(stmts, 1, loc);
    TEST_ASSERT(block != NULL, "Failed to create block node");
    TEST_ASSERT(block->type == AST_BLOCK, "Wrong node type for block");
    TEST_ASSERT(block->as.block.statement_count == 1, "Wrong statement count for block");
    TEST_ASSERT(block->as.block.statements[0] == stmt, "Wrong statement in block");
    
    // Free the AST
    ast_free(block); // This should free the entire tree
    
    printf("AST node creation tests passed!\n\n");
}

// Test AST cloning
void test_ast_cloning() {
    printf("Testing AST cloning...\n");
    
    SourceLocation loc = source_location_create(1, 1, "test.ћпп");
    
    // Create a simple expression tree: 42 + "тест"
    AstNode* int_literal = ast_create_literal_int(42, loc);
    AstNode* id_node = ast_create_identifier("тест", loc);
    AstNode* bin_expr = ast_create_binary_expr(int_literal, TOKEN_PLUS, id_node, loc);
    
    // Clone the expression
    AstNode* cloned_expr = ast_clone(bin_expr);
    TEST_ASSERT(cloned_expr != NULL, "Failed to clone binary expression");
    TEST_ASSERT(cloned_expr->type == AST_BINARY_EXPR, "Wrong node type in cloned expression");
    TEST_ASSERT(cloned_expr->as.binary_expr.operator == TOKEN_PLUS, "Wrong operator in cloned expression");
    
    // Verify that the clone is a deep copy
    TEST_ASSERT(cloned_expr->as.binary_expr.left != int_literal, "Clone should not share the same left operand instance");
    TEST_ASSERT(cloned_expr->as.binary_expr.right != id_node, "Clone should not share the same right operand instance");
    
    // Verify values in the cloned tree
    TEST_ASSERT(cloned_expr->as.binary_expr.left->type == AST_LITERAL_INT, "Wrong left operand type in clone");
    TEST_ASSERT(cloned_expr->as.binary_expr.left->as.int_literal.value == 42, "Wrong left operand value in clone");
    TEST_ASSERT(cloned_expr->as.binary_expr.right->type == AST_IDENTIFIER, "Wrong right operand type in clone");
    TEST_ASSERT(strcmp(cloned_expr->as.binary_expr.right->as.identifier.name, "тест") == 0, "Wrong right operand name in clone");
    
    // Free both trees
    ast_free(bin_expr);
    ast_free(cloned_expr);
    
    printf("AST cloning tests passed!\n\n");
}

// Visitor to count nodes of different types
typedef struct {
    int total_nodes;
    int expr_nodes;
    int literal_nodes;
    int identifier_nodes;
} NodeCounter;

bool visitor_count_nodes(AstVisitor* visitor, AstNode* node) {
    NodeCounter* counter = (NodeCounter*)visitor->context;
    counter->total_nodes++;
    
    switch (node->type) {
        case AST_BINARY_EXPR:
        case AST_UNARY_EXPR:
            counter->expr_nodes++;
            break;
            
        case AST_LITERAL_INT:
        case AST_LITERAL_CHAR:
        case AST_LITERAL_STRING:
        case AST_LITERAL_BOOL:
            counter->literal_nodes++;
            break;
            
        case AST_IDENTIFIER:
            counter->identifier_nodes++;
            break;
            
        default:
            break;
    }
    
    return true; // Continue traversal
}

// Test AST visitor pattern
void test_ast_visitor() {
    printf("Testing AST visitor pattern...\n");
    
    SourceLocation loc = source_location_create(1, 1, "test.ћпп");
    
    // Create a simple expression: (40 + x) * 2
    AstNode* int_literal1 = ast_create_literal_int(40, loc);
    AstNode* id_node = ast_create_identifier("x", loc);
    AstNode* add_expr = ast_create_binary_expr(int_literal1, TOKEN_PLUS, id_node, loc);
    AstNode* int_literal2 = ast_create_literal_int(2, loc);
    AstNode* mul_expr = ast_create_binary_expr(add_expr, TOKEN_STAR, int_literal2, loc);
    
    // Create a visitor to count nodes
    NodeCounter counter = {0, 0, 0, 0};
    AstVisitor visitor = {0};
    visitor.context = &counter;
    visitor.visit_binary_expr = visitor_count_nodes;
    visitor.visit_unary_expr = visitor_count_nodes;
    visitor.visit_literal_int = visitor_count_nodes;
    visitor.visit_literal_char = visitor_count_nodes;
    visitor.visit_literal_string = visitor_count_nodes;
    visitor.visit_literal_bool = visitor_count_nodes;
    visitor.visit_identifier = visitor_count_nodes;
    
    // Apply the visitor
    bool result = ast_accept(mul_expr, &visitor);
    TEST_ASSERT(result, "Visitor traversal should succeed");
    
    // Verify node counts
    TEST_ASSERT(counter.total_nodes == 5, "Expected 5 total nodes");
    TEST_ASSERT(counter.expr_nodes == 2, "Expected 2 expression nodes");
    TEST_ASSERT(counter.literal_nodes == 2, "Expected 2 literal nodes");
    TEST_ASSERT(counter.identifier_nodes == 1, "Expected 1 identifier node");
    
    // Free the tree
    ast_free(mul_expr);
    
    printf("AST visitor tests passed!\n\n");
}

// Test AST with lexer integration
void test_ast_with_lexer() {
    printf("Testing AST with lexer integration...\n");
    
    // Sample code to tokenize
    const char* source_code = "x = 42 + y;";
    
    // Create a temporary file with the source code
    FILE* temp_file = tmpfile();
    TEST_ASSERT(temp_file != NULL, "Failed to create temporary file");
    fputs(source_code, temp_file);
    rewind(temp_file);
    
    // Initialize target info
    TargetInfo target_info;
    target_info.word_size = 8; // 64-bit
    target_info.endianness = ENDIAN_LITTLE;
    
    // Initialize lexer
    LexerState* lexer = lexer_init(temp_file, "test.ћпп", target_info);
    TEST_ASSERT(lexer != NULL, "Failed to initialize lexer");
    
    // Get tokens and build a simple AST manually
    SourceLocation loc = source_location_create(1, 1, "test.ћпп");
    
    // Create nodes for the expression "x = 42 + y"
    Token id_token1 = lexer_next_token(lexer); // x
    TEST_ASSERT(id_token1.type == TOKEN_IDENTIFIER, "Expected identifier token");
    AstNode* id_node1 = ast_create_identifier(id_token1.value.string_value, loc);
    
    Token equals_token = lexer_next_token(lexer); // =
    TEST_ASSERT(equals_token.type == TOKEN_EQUALS, "Expected equals token");
    
    Token num_token = lexer_next_token(lexer); // 42
    TEST_ASSERT(num_token.type == TOKEN_NUMBER, "Expected number token");
    AstNode* num_node = ast_create_literal_int(num_token.value.int_value, loc);
    
    Token plus_token = lexer_next_token(lexer); // +
    TEST_ASSERT(plus_token.type == TOKEN_PLUS, "Expected plus token");
    
    Token id_token2 = lexer_next_token(lexer); // y
    TEST_ASSERT(id_token2.type == TOKEN_IDENTIFIER, "Expected identifier token");
    AstNode* id_node2 = ast_create_identifier(id_token2.value.string_value, loc);
    
    Token semicolon_token = lexer_next_token(lexer); // ;
    TEST_ASSERT(semicolon_token.type == TOKEN_SEMICOLON, "Expected semicolon token");
    
    // Build the AST: x = (42 + y);
    AstNode* bin_expr = ast_create_binary_expr(num_node, TOKEN_PLUS, id_node2, loc);
    AstNode* assign_expr = ast_create_assignment(id_node1, bin_expr, loc);
    AstNode* stmt = ast_create_expr_stmt(assign_expr, loc);
    
    // Print the AST
    printf("AST for \"%s\":\n", source_code);
    ast_print(stmt, 0);
    
    // Free resources
    ast_free(stmt);
    lexer_cleanup(lexer);
    fclose(temp_file);
    
    printf("AST with lexer tests passed!\n\n");
}

// Test for nested control structures
void test_nested_control_structures() {
    printf("Testing nested control structures in AST...\n");
    
    SourceLocation loc = source_location_create(1, 1, "test.ћпп");
    
    // Create a for loop containing an if-else with a while loop in the else branch:
    // for (i = 0; i < 10; i = i + 1) {
    //     if (i % 2 == 0) {
    //         x = i;
    //     } else {
    //         while (j < i) {
    //             j = j + 1;
    //         }
    //     }
    // }
    
    // Variables i, j, x
    AstNode* i_var = ast_create_identifier("i", loc);
    AstNode* j_var = ast_create_identifier("j", loc);
    AstNode* x_var = ast_create_identifier("x", loc);
    
    // i = 0 (initializer)
    AstNode* zero = ast_create_literal_int(0, loc);
    AstNode* init = ast_create_assignment(i_var, zero, loc);
    
    // i < 10 (condition)
    AstNode* ten = ast_create_literal_int(10, loc);
    AstNode* cond = ast_create_binary_expr(ast_clone(i_var), TOKEN_LESS, ten, loc);
    
    // i = i + 1 (increment)
    AstNode* one = ast_create_literal_int(1, loc);
    AstNode* i_plus_one = ast_create_binary_expr(ast_clone(i_var), TOKEN_PLUS, one, loc);
    AstNode* incr = ast_create_assignment(ast_clone(i_var), i_plus_one, loc);
    
    // i % 2 == 0 (if condition)
    AstNode* two = ast_create_literal_int(2, loc);
    AstNode* i_mod_two = ast_create_binary_expr(ast_clone(i_var), TOKEN_PERCENT, two, loc);
    AstNode* if_cond = ast_create_binary_expr(i_mod_two, TOKEN_DOUBLE_EQUALS, ast_clone(zero), loc);
    
    // x = i (then branch)
    AstNode* x_equals_i = ast_create_assignment(ast_clone(x_var), ast_clone(i_var), loc);
    AstNode* then_stmt = ast_create_expr_stmt(x_equals_i, loc);
    
    // j < i (while condition)
    AstNode* j_lt_i = ast_create_binary_expr(ast_clone(j_var), TOKEN_LESS, ast_clone(i_var), loc);
    
    // j = j + 1 (while body)
    AstNode* j_plus_one = ast_create_binary_expr(ast_clone(j_var), TOKEN_PLUS, ast_clone(one), loc);
    AstNode* j_incr = ast_create_assignment(ast_clone(j_var), j_plus_one, loc);
    AstNode* while_body = ast_create_expr_stmt(j_incr, loc);
    
    // Construct the while loop
    AstNode* while_loop = ast_create_while_stmt(j_lt_i, while_body, loc);
    
    // Construct the if-else statement
    AstNode* if_stmt = ast_create_if_stmt(if_cond, then_stmt, while_loop, loc);
    
    // Construct the for loop
    AstNode* for_loop = ast_create_for_stmt(init, cond, incr, if_stmt, loc);
    
    // Print the AST
    printf("AST for nested control structures:\n");
    ast_print(for_loop, 0);
    
    // Make sure we can clone this complex structure
    AstNode* cloned_ast = ast_clone(for_loop);
    TEST_ASSERT(cloned_ast != NULL, "Failed to clone complex nested control structure");
    
    // Check that the cloned AST has the right structure
    TEST_ASSERT(cloned_ast->type == AST_FOR_STMT, "Cloned AST should be a for statement");
    TEST_ASSERT(cloned_ast->as.for_stmt.body->type == AST_IF_STMT, "For loop body should be an if statement");
    TEST_ASSERT(cloned_ast->as.for_stmt.body->as.if_stmt.else_branch->type == AST_WHILE_STMT, 
               "If statement else branch should be a while loop");
    
    // Free the AST
    ast_free(for_loop);
    ast_free(cloned_ast);
    
    printf("Nested control structures test passed!\n\n");
}

// Test for external function declarations
void test_external_function_declarations() {
    printf("Testing external function declarations in AST...\n");
    
    SourceLocation loc = source_location_create(1, 1, "test.ћпп");
    
    // Create an external function declaration:
    // екстерно printf(format, ...);
    
    // Parameter declaration for "format"
    TypeInfo* char_type = type_create_char();
    TypeInfo* string_type = type_create_array(char_type, -1); // -1 for dynamic size
    AstNode* format_param = ast_create_var_decl("format", NULL, string_type, loc);
    
    // Set up parameter array
    AstNode* params[1] = { format_param };
    
    // Create a function declaration for printf
    TypeInfo* int_type = type_create_int();
    AstNode* printf_decl = ast_create_function_decl(
        "printf",
        params,
        1,          // 1 parameter
        NULL,       // No body for external function
        int_type,   // Return type is int
        true,       // is_external = true
        loc
    );
    
    // Create a program node with the printf declaration
    AstNode* declarations[1] = { printf_decl };
    AstNode* program = ast_create_program(declarations, 1, loc);
    
    // Print the AST
    printf("AST for external function declaration:\n");
    ast_print(program, 0);
    
    // Check that we can correctly identify the external function
    TEST_ASSERT(program->type == AST_PROGRAM, "Root node should be a program");
    TEST_ASSERT(program->as.program.declaration_count == 1, "Program should have one declaration");
    TEST_ASSERT(program->as.program.declarations[0]->type == AST_FUNCTION_DECL, 
               "Declaration should be a function declaration");
    TEST_ASSERT(program->as.program.declarations[0]->as.function_decl.is_external, 
               "Function should be marked as external");
    TEST_ASSERT(strcmp(program->as.program.declarations[0]->as.function_decl.name, "printf") == 0,
               "Function name should be 'printf'");
    
    // Free the AST
    ast_free(program);
    
    printf("External function declarations test passed!\n\n");
}

int main() {
    printf("=== AST Tests ===\n\n");
    
    // Run the tests
    test_ast_creation();
    test_ast_cloning();
    test_ast_visitor();
    test_ast_with_lexer();
    test_nested_control_structures();
    test_external_function_declarations();
    
    printf("All AST tests passed successfully!\n");
    return 0;
}