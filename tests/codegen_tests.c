/*
 * filename: codegen_tests.c
 * 
 * Purpose:
 * Test suite for the code generation component of the ћ++ compiler.
 * Contains tests that verify the compiler correctly translates ћ++ programs
 * into valid x86/x86-64 assembly code that can be assembled and executed.
 * 
 * Key Components:
 * - test_codegen_init(): Test code generator initialization
 * - test_expression_codegen(): Test code generation for expressions
 * - test_control_flow_codegen(): Test generation of control flow structures
 * - test_function_calls(): Test generation of function call sequences
 * - test_array_operations(): Test generation of array access operations
 * - test_architecture_detection(): Test target architecture detection
 * 
 * Interactions:
 * - Uses codegen.h and architecture-specific modules (x86.h/x86_64.h)
 * - Uses ir.h to test the full IR-to-assembly pipeline
 * - Uses target_info.h for architecture information
 * 
 * Notes:
 * - Tests both 32-bit and 64-bit code generation when possible
 * - Verifies correct register allocation and calling conventions
 * - Includes end-to-end tests that compile, assemble, and run code
 */

// Test file content will be implemented later