Collecting workspace information# ћ++ Language Specification

## 1. Introduction

ћ++ is a minimalist programming language inspired by C but with unique design decisions that simplify its typing system and enhance international programmability through UTF-8 support. This document specifies the syntax, semantics, and core features of the ћ++ language.

## 2. Fundamental Concepts

### 2.1 WORD-Based Type System

Unlike C's variety of types, ћ++ uses a single native type:

- All variables are a single WORD in size (architecture-dependent):
  - 64-bit on x86-64 architectures
  - 32-bit on x86 architectures
  
- No explicit type distinctions (no `int`, `char`, `float`, etc.)
- No type declarations are required or permitted

### 2.2 Character Set

- Source code is encoded in UTF-8
- Identifiers support the full Unicode character set
- Keywords are written in Serbian Cyrillic

## 3. Lexical Elements

### 3.1 Identifiers

- Can use any alphabetic Unicode characters, digits, and underscores
- Must start with an alphabetic character or underscore
- Case-sensitive
- No length limit

### 3.2 Keywords

ћ++ keywords are written in Serbian Cyrillic:

| ћ++ Keyword | Meaning | C Equivalent |
|-------------|---------|--------------|
| `ако`       | if      | `if`         |
| `иначе`     | else    | `else`       |
| `док`       | while   | `while`      |
| `за`        | for     | `for`        |
| `ради`      | do      | `do`         |
| `прекини`   | break   | `break`      |
| `врати`     | return  | `return`     |
| `екстерно`  | external| `extern`     |
| `тачно`     | true    | `true`       |
| `нетачно`   | false   | `false`      |

### 3.3 Literals

- **Numeric literals**: Direct integer values (e.g., `42`, `100`)
- **Character literals**: Single UTF-8 characters enclosed in single quotes (e.g., `'a'`, `'ћ'`)
  - Escape sequences: `\n`, `\t`, `\r`, `\\`, `\'`, `\"`, `\uXXXX` (for Unicode)
- **String literals**: UTF-8 character sequences enclosed in double quotes (e.g., `"Hello"`, `"Здраво"`)
- **Boolean literals**: `тачно` (true) and `нетачно` (false)

### 3.4 Operators

Similar to C with identical semantics:
- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Relational: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`, `~`
- Assignment: `=`
- Pointer: `*` (dereference), `&` (address-of)

### 3.5 Delimiters

- Statement terminator: `;`
- Code blocks: `< >` (instead of C's `{ }`)
- Parameter lists and expressions: `( )`
- Array access: `:index:`

## 4. Program Structure

### 4.1 Functions

Function definitions:
```
функција(параметар1, параметар2) <
    // function body
    врати результат;
>
```

- No return type declarations (all returns are WORD-sized)
- No parameter type declarations
- Main function must be named `главна()`

### 4.2 External Declarations

For declaring functions from external libraries:
```
екстерно име_функције(параметар1, параметар2);
```

### 4.3 Variable Declarations

Variables are implicitly declared on first assignment:
```
променљива = 42;  // Declaration and initialization
```

### 4.4 Arrays

Special syntax for arrays:
```
име_низа:size: = _value1, value2, ..._;
```

Examples:
```
бројеви:4: = _1, 2, 3, 4_;  // Array of 4 elements
низ:10: = _0, 0, 0, 0, 0, 0, 0, 0, 0, 0_;
```

Array access:
```
бројеви:0:  // Access first element (instead of C's array[0])
```

Array parameter syntax:
```
функција(низ::, н) < ... >  // Array parameter and size parameter
```

## 5. Statements

### 5.1 Expression Statements

Any expression followed by a semicolon:
```
x = 5;
функција(аргумент);
```

### 5.2 Compound Statements

Series of statements enclosed in angle brackets:
```
<
    statement1;
    statement2;
    statement3;
>
```

### 5.3 Conditional Statements

```
ако (expression) <
    // then branch
> иначе <
    // else branch
>
```

### 5.4 Loop Statements

While loop:
```
док (expression) <
    // body
>
```

For loop:
```
за (initializer; condition; increment) <
    // body
>
```

Do-while loop:
```
ради <
    // body
> док (expression);
```

### 5.5 Jump Statements

Break:
```
прекини;
```

Return:
```
врати expression;
```

## 6. Expressions

### 6.1 Primary Expressions

- Identifiers
- Literals
- Parenthesized expressions

### 6.2 Operators

Operator precedence and associativity follow C conventions.

### 6.3 Assignment

```
identifier = expression;
```

### 6.4 Function Calls

```
функција(аргумент1, аргумент2);
```

### 6.5 Array Indexing

```
низ:index:
```

### 6.6 Pointer Operations

```
pointer = &variable;  // Address-of operator
value = *pointer;     // Dereference operator
```

## 7. Memory Model

- All scalar variables occupy one WORD
- Arrays are contiguous blocks of WORDs
- No struct, union, or other composite types
- Pointers are represented as WORDs containing memory addresses

## 8. Examples

### Example 1: Hello World
```
екстерно putchar;

главна() <
    putchar('H');
    putchar('e');
    putchar('l');
    putchar('l');
    putchar('o');
    putchar('\n');
    
    врати 0;
>
```

### Example 2: Sum of Array
```
екстерно испиши;

сума(низ::, н) <
    резултат = 0;
    и = 0;
    
    док (и < н) <
        резултат = резултат + низ:и:;
        и = и + 1;
    >
    
    врати резултат;
>

главна() <
    бројеви:4: = _1, 2, 3, 4_;
    збир = сума(бројеви, 4);
    испиши(збир);
    врати 0;
>
```

## 9. Key Differences from C

1. **Type System**: Single WORD-sized values only, no type declarations
2. **Block Delimiters**: Angle brackets `< >` instead of curly braces `{ }`
3. **Array Syntax**: Uses `низ:size:` declaration and `:index:` access instead of `[]`
4. **Keywords**: Serbian Cyrillic instead of English
5. **UTF-8 Support**: Identifiers can use any Unicode alphabetic characters
6. **Main Function**: Named `главна()` instead of `main()`
7. **No Type Qualifiers**: No `const`, `volatile`, etc.
8. **No Preprocessor**: No `#include`, `#define`, etc.
9. **Simplified Initialization**: No complex initialization syntax
10. **No Structs or Unions**: No composite data types

## 10. Implementation-Defined Behavior

- WORD size is determined by the target architecture
- Stack size limits
- Maximum array dimensions
- Maximum identifier length

This specification defines the core of the ћ++ language. The implementation may provide additional library functions through external declarations.