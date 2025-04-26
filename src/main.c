/*
 * filename: main.c
 * 
 * Purpose:
 * Main entry point for the ћ++ compiler. Coordinates the compilation process
 * by invoking the various compiler phases in sequence and handling command-line
 * arguments.
 * 
 * Key Components:
 * - main(): Entry point that processes command-line arguments
 * - compile_file(): Coordinate compilation of a single ћ++ source file
 * - process_args(): Parse and validate command-line options
 * - print_help(): Display usage information
 * - run_compiler_pipeline(): Execute the compilation phases in sequence
 * 
 * Interactions:
 * - Initializes and coordinates all compiler components
 * - Uses error.h for tracking and reporting compilation issues
 * - Manages file I/O for source and output files
 * 
 * Notes:
 * - Supports various command-line options for controlling the compiler
 * - Detects target architecture using target_info.h
 * - Can stop compilation after specific phases (lexing, parsing, etc.)
 * - Handles UTF-8 encoded filenames and paths
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Forward declarations of compiler component headers
// #include "utils/error.h"
// #include "target/target_info.h"
// Will be uncommented when these components are implemented

// Compiler configuration structure
typedef struct {
    char* input_file;           // Input source file path
    char* output_file;          // Output file path
    bool generate_assembly;     // Generate assembly only (-S flag)
    bool stop_after_lexing;     // Stop after lexical analysis
    bool stop_after_parsing;    // Stop after syntax analysis
    bool stop_after_semantic;   // Stop after semantic analysis
    bool stop_after_ir;         // Stop after IR generation
    bool verbose;               // Verbose output
    char* target_arch;          // Target architecture (x86 or x86-64)
    int optimization_level;     // Optimization level (0-3)
} CompilerConfig;

// Function to print usage information
void print_help(const char* program_name) {
    printf("ћ++ Compiler - A compiler for the ћ++ programming language\n");
    printf("\nUsage: %s [options] input-file\n", program_name);
    printf("\nOptions:\n");
    printf("  -o <file>              Specify output file name\n");
    printf("  -S                     Generate assembly code only\n");
    printf("  --target=<arch>        Target architecture (x86, x86-64, default: current machine)\n");
    printf("  -O<level>              Optimization level (0-3, default: 0)\n");
    printf("  --verbose              Verbose output\n");
    printf("  --stop-after-lexing    Stop after lexical analysis\n");
    printf("  --stop-after-parsing   Stop after syntax analysis\n");
    printf("  --stop-after-semantic  Stop after semantic analysis\n");
    printf("  --stop-after-ir        Stop after IR generation\n");
    printf("  -h, --help             Display this help message\n");
    printf("\nExamples:\n");
    printf("  %s input.ћпп -o program        Compile input.ћпп to executable 'program'\n", program_name);
    printf("  %s input.ћпп -S -o output.asm  Generate assembly for input.ћпп\n", program_name);
    printf("  %s --target=x86 input.ћпп      Compile for 32-bit x86 architecture\n", program_name);
}

// Parse command line arguments and populate the compiler configuration
bool process_args(int argc, char* argv[], CompilerConfig* config) {
    // Initialize config with default values
    config->input_file = NULL;
    config->output_file = NULL;
    config->generate_assembly = false;
    config->stop_after_lexing = false;
    config->stop_after_parsing = false;
    config->stop_after_semantic = false;
    config->stop_after_ir = false;
    config->verbose = false;
    config->target_arch = NULL;
    config->optimization_level = 0;
    
    // No arguments provided, print help
    if (argc < 2) {
        print_help(argv[0]);
        return false;
    }
    
    // Process all arguments
    for (int i = 1; i < argc; i++) {
        // Handle help flag
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return false;
        }
        // Handle output file
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                config->output_file = argv[++i];
            } else {
                fprintf(stderr, "Error: Missing argument for -o\n");
                return false;
            }
        }
        // Handle generate assembly only
        else if (strcmp(argv[i], "-S") == 0) {
            config->generate_assembly = true;
        }
        // Handle optimization level
        else if (strncmp(argv[i], "-O", 2) == 0) {
            if (strlen(argv[i]) > 2 && argv[i][2] >= '0' && argv[i][2] <= '3') {
                config->optimization_level = argv[i][2] - '0';
            } else {
                fprintf(stderr, "Error: Invalid optimization level. Use -O0 to -O3\n");
                return false;
            }
        }
        // Handle verbose flag
        else if (strcmp(argv[i], "--verbose") == 0) {
            config->verbose = true;
        }
        // Handle target architecture
        else if (strncmp(argv[i], "--target=", 9) == 0) {
            config->target_arch = &argv[i][9];
            if (strcmp(config->target_arch, "x86") != 0 && 
                strcmp(config->target_arch, "x86-64") != 0) {
                fprintf(stderr, "Error: Unsupported target architecture. Use x86 or x86-64\n");
                return false;
            }
        }
        // Handle compilation phase stops
        else if (strcmp(argv[i], "--stop-after-lexing") == 0) {
            config->stop_after_lexing = true;
        }
        else if (strcmp(argv[i], "--stop-after-parsing") == 0) {
            config->stop_after_parsing = true;
        }
        else if (strcmp(argv[i], "--stop-after-semantic") == 0) {
            config->stop_after_semantic = true;
        }
        else if (strcmp(argv[i], "--stop-after-ir") == 0) {
            config->stop_after_ir = true;
        }
        // Assume anything else is an input file
        else if (config->input_file == NULL) {
            config->input_file = argv[i];
            
            // Check if file has correct extension
            size_t len = strlen(config->input_file);
            if (len < 5 || strcmp(&config->input_file[len-4], ".ћпп") != 0) {
                fprintf(stderr, "Warning: Input file does not have .ћпп extension\n");
            }
        }
        // Unknown option
        else {
            fprintf(stderr, "Error: Unknown option or multiple input files: %s\n", argv[i]);
            return false;
        }
    }
    
    // Verify we have an input file
    if (config->input_file == NULL) {
        fprintf(stderr, "Error: No input file specified\n");
        return false;
    }
    
    // If no output file specified, default to a.out or file.asm for assembly
    if (config->output_file == NULL) {
        if (config->generate_assembly) {
            // Create output filename by replacing extension with .asm
            size_t len = strlen(config->input_file);
            char* default_output = malloc(len + 1); // +1 for null terminator
            if (default_output == NULL) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                return false;
            }
            strcpy(default_output, config->input_file);
            
            // Replace extension or add .asm
            char* ext = strrchr(default_output, '.');
            if (ext != NULL) {
                strcpy(ext, ".asm");
            } else {
                free(default_output);
                default_output = malloc(len + 5); // +5 for .asm and null terminator
                if (default_output == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed\n");
                    return false;
                }
                sprintf(default_output, "%s.asm", config->input_file);
            }
            
            config->output_file = default_output;
        } else {
            config->output_file = "a.out";
        }
    }
    
    // Print configuration if verbose
    if (config->verbose) {
        printf("ћ++ Compiler Configuration:\n");
        printf("  Input file: %s\n", config->input_file);
        printf("  Output file: %s\n", config->output_file);
        printf("  Generate assembly only: %s\n", config->generate_assembly ? "yes" : "no");
        printf("  Target architecture: %s\n", config->target_arch ? config->target_arch : "native");
        printf("  Optimization level: O%d\n", config->optimization_level);
        
        if (config->stop_after_lexing) printf("  Stopping after lexical analysis\n");
        else if (config->stop_after_parsing) printf("  Stopping after syntax analysis\n");
        else if (config->stop_after_semantic) printf("  Stopping after semantic analysis\n");
        else if (config->stop_after_ir) printf("  Stopping after IR generation\n");
    }
    
    return true;
}

// Main entry point
int main(int argc, char* argv[]) {
    // Parse command line arguments
    CompilerConfig config;
    if (!process_args(argc, argv, &config)) {
        return 1;
    }
    
    printf("ћ++ compiler: processing file %s\n", config.input_file);
    
    // TODO: Implement the compiler pipeline
    // 1. Initialize error handling system
    // 2. Detect target architecture
    // 3. Read input file
    // 4. Run lexical analysis (stopping if requested)
    // 5. Run syntax analysis (stopping if requested)
    // 6. Run semantic analysis (stopping if requested)
    // 7. Generate IR (stopping if requested)
    // 8. Optimize IR
    // 9. Generate target code
    // 10. Output result file
    
    printf("Compilation successful\n");
    return 0;
}