<div align="center">

# ћ++ Compiler

<img src="https://img.shields.io/badge/language-C-blue.svg" alt="Language C">
<img src="https://img.shields.io/badge/architecture-x86%20%7C%20x86--64-orange.svg" alt="Architecture">
<img src="https://img.shields.io/badge/license-MIT-green.svg" alt="License">

A compiler for the ћ++ programming language, a C-like language with UTF-8 support for identifiers and keywords.

</div>

## 📋 Language Overview

ћ++ is a simple programming language with the following features:

- C-like syntax with UTF-8 support for identifiers and keywords
- All values are WORD-sized (64-bit on x86-64, 32-bit on x86 architectures)
- No explicit type distinctions - all variables use the native machine word size
- Uses angle brackets `< >` instead of curly braces `{ }` for blocks
- Special array syntax: `низ:size: name = _value1, value2, ..._;`
- External function declarations with `екстерно` keyword
- Return statements use `врати` keyword
- Main function is declared as `главна()`

<details>
<summary><b>Sample Program</b> (Click to expand)</summary>

```
// Example program
екстерна putchar(порука);

низ:3: бројеви = _1, 2, 3_;

сума(низ:н: аргументи) <
    резултат = 0;
    и = 0;
    
    док (и < н) <
        резултат = резултат + аргументи:и:;
        и = и + 1;
    >
    
    врати резултат;
>

главна() <
    збир = сума(бројеви);
    испиши(збир);
    врати 0;
>
```

</details>

## 🏗️ Project Structure

<table>
<tr>
<td width="50%" valign="top">

The compiler is organized into the following components:

- **Lexical Analysis**: UTF-8 aware tokenizer for ћ++ source code
- **Syntax Analysis**: Parser that constructs an Abstract Syntax Tree (AST)
- **Semantic Analysis**: Type checking and symbol table management
- **Intermediate Representation**: Architecture-independent code representation
- **Code Generation**: Translates IR to x86 or x86-64 assembly code

</td>
<td width="50%" valign="top">

Directory structure:

```
ћпп/
├── src/               # Source code
│   ├── lexer/         # Lexical analyzer
│   ├── parser/        # Syntax analyzer
│   ├── semantic/      # Semantic analyzer
│   ├── ir/            # IR generation
│   ├── codegen/       # Code generator
│   ├── target/        # Target arch info
│   ├── utils/         # Utilities
│   └── main.c         # Compiler driver
├── examples/          # Example programs
├── tests/             # Test suite
├── Makefile           # Build system
└── README.md          # This file
```

</td>
</tr>
</table>

## 🛠️ Build Instructions

### Prerequisites

<ul style="list-style-type: none; padding-left: 20px;">
<li>✅ GCC or Clang compiler</li>
<li>✅ GNU Make</li>
<li>✅ NASM (for assembling generated code)</li>
</ul>

### Compiling the Compiler

```bash
# Build in debug mode
make debug

# Build in release mode
make release

# Run tests
make test

# Clean build artifacts
make clean
```

## 📖 Usage

```bash
# Basic usage
./ћпп input.ћпп -o output

# Generate assembly only
./ћпп input.ћпп -S -o output.asm

# Specify target architecture (default is current machine)
./ћпп input.ћпп --target=x86 -o output

# Get help
./ћпп --help
```

## 🧪 Examples

See the `examples/` directory for sample ћ++ programs, including:

- `main.ћпп`: A simple program demonstrating array operations
- `main2.ћпп`: Another example program

## 🗺️ Development Roadmap

<div style="display: flex; justify-content: space-between; flex-wrap: wrap;">
<div style="flex: 0 0 48%; margin-bottom: 10px; padding: 10px; border: 1px solid #ccc; border-radius: 5px;">
<b>Phase 1</b>: Lexer and Parser implementation with UTF-8 support
</div>
<div style="flex: 0 0 48%; margin-bottom: 10px; padding: 10px; border: 1px solid #ccc; border-radius: 5px;">
<b>Phase 2</b>: Semantic analysis and symbol table implementation
</div>
<div style="flex: 0 0 48%; margin-bottom: 10px; padding: 10px; border: 1px solid #ccc; border-radius: 5px;">
<b>Phase 3</b>: IR generation
</div>
<div style="flex: 0 0 48%; margin-bottom: 10px; padding: 10px; border: 1px solid #ccc; border-radius: 5px;">
<b>Phase 4</b>: Code generation for x86-64
</div>
<div style="flex: 0 0 48%; margin-bottom: 10px; padding: 10px; border: 1px solid #ccc; border-radius: 5px;">
<b>Phase 5</b>: Code generation for x86 (32-bit)
</div>
<div style="flex: 0 0 48%; margin-bottom: 10px; padding: 10px; border: 1px solid #ccc; border-radius: 5px;">
<b>Phase 6</b>: Optimizations and extended features
</div>
</div>

## 👥 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests to ensure they pass
5. Submit a pull request

<hr>

<div align="center">
<p>This project is released under the MIT License.</p>
<p>© 2025 ћ++ Compiler Project</p>
</div>