/*
 * filename: ir.h
 * 
 * Purpose:
 * Header file for the Intermediate Representation (IR) in the ћ++ compiler.
 * Defines the structures and interfaces for translating the AST into a lower-level
 * representation that's closer to machine code but still architecture-independent.
 * 
 * Key Components:
 * - IrOpcode enum: Defines all possible IR instruction types
 * - IrOperand struct: Represents operands (variables, constants, etc.)
 * - IrInstruction struct: Represents a single IR instruction
 * - IrBasicBlock struct: Contains a sequence of IR instructions
 * - IrFunction struct: Represents a function in IR form
 * - IrProgram struct: Top-level container for all IR components
 * - ir_generate(): Convert an AST into IR representation
 * 
 * Interactions:
 * - Takes input from the semantic analyzer's validated AST
 * - Output is used by codegen.h for target code generation
 * - Uses symbol_table.h for variable information
 * 
 * Notes:
 * - All values are WORD-sized (architecture dependent)
 * - Uses three-address code format for instructions
 * - Basic blocks enable future control-flow optimizations
 */

#ifndef IR_H
#define IR_H

// Header content will be implemented later

#endif /* IR_H */